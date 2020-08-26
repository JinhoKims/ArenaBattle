// Fill out your copyright notice in the Description page of Project Settings.


#include "ABSection.h"
#include "ABCharacter.h"
#include "ABItemBox.h"

// Sets default values
AABSection::AABSection()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	RootComponent = Mesh;
	bNoBattle = false;
	EnemySpawnTime = 2.0f;
	ItemBoxSpawnTime = 5.0f;

	FString AssetPath = TEXT("StaticMesh'/Game/Book/StaticMesh/SM_SQUARE.SM_SQUARE'"); // 애셋 경로 저장변수
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_SQUARE(*AssetPath);
	if (SM_SQUARE.Succeeded())
	{
		Mesh->SetStaticMesh(SM_SQUARE.Object); // 에셋의 메시 설정(지형)
	}
	else
	{
		ABLOG_Long(Error, TEXT("Failed to load staticmesh asset. : %s"), *AssetPath);
	}

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER")); // 중앙의 박스 트리거 생성 (플레이어 감지용)
	Trigger->SetBoxExtent(FVector(775.0f, 775.0f, 300.0f)); // 중앙 박스 범위 설정(맵을 꽉 채우게 설정)
	Trigger->SetupAttachment(RootComponent);
	Trigger->SetRelativeLocation(FVector(0.0f, 0.0f, 250.0f));
	Trigger->SetCollisionProfileName(TEXT("ABTrigger")); // 중앙 콜리전 프리셋 설정
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnTriggerBeginOverlap); // 컴포넌트의 델리게이트에 OnTriggerBeginOverlap() 함수를 바인딩 해준다. Ready 상태에서 Battle 상태로 전이 역할

	FString GateAssetPath = TEXT("StaticMesh'/Game/Book/StaticMesh/SM_GATE.SM_GATE'");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_GATE(*GateAssetPath); // 게이트 스태틱메시
	if (!SM_GATE.Succeeded())
	{
		ABLOG_Long(Error, TEXT("Failed to load staticmesh asset. : %s"), *GateAssetPath);
	}
	static FName GateSockets[] = { {TEXT("+XGate")}, {TEXT("-XGate")}, {TEXT("+YGate")}, {TEXT("-YGate")} };
	for (FName GateSocket : GateSockets) // 동서남북 4번 반복
	{
		ABCHECK(Mesh->DoesSocketExist(GateSocket));
		UStaticMeshComponent* NewGate = CreateDefaultSubobject<UStaticMeshComponent>(*GateSocket.ToString()); // 게이트 메시 컴포넌트 생성 
		NewGate->SetStaticMesh(SM_GATE.Object); // 메시 삽입
		NewGate->SetupAttachment(RootComponent, GateSocket);
		NewGate->SetRelativeLocation(FVector(0.0f, -80.5f, 0.0f)); // 컴포넌트 좌표 설정 
		GateMeshes.Add(NewGate);

		UBoxComponent* NewGateTrigger = CreateDefaultSubobject<UBoxComponent>(*GateSocket.ToString().Append(TEXT("Trigger"))); // 게이트 콜리전 설정
		NewGateTrigger->SetBoxExtent(FVector(100.0f, 100.0f, 300.0f)); // 콜리전 영역
		NewGateTrigger->SetupAttachment(RootComponent, GateSocket);
		NewGateTrigger->SetRelativeLocation(FVector(70.0f, 0.0f, 250.0f));
		NewGateTrigger->SetCollisionProfileName(TEXT("ABTrigger")); // 콜리전 적용
		GateTriggers.Add(NewGateTrigger);
		
		NewGateTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnGateTriggerBeginOverlap); // 컴포넌트의 델리게이트에 OnTriggerBeginOverlap() 함수를 바인딩 해준다. 해당 함수는 검사 후 섹션(액터)를 생성하는 역할을 한다.
		NewGateTrigger->ComponentTags.Add(GateSocket); // 어떤 문인지 구분할 수 있도록 태그를 추가한다.(x+ ~ y-) + 트리거는 총 4개가 존재하지만, 해당 트리거에서 호출하는 델리게이트는 공통의 하나의 함수(GateBeginOverlap)를 호출한다.
	} 

	
}

// Called when the game starts or when spawned
void AABSection::BeginPlay()
{
	Super::BeginPlay();
	// Beginplay()는 에디터가 아닌 인게임에서 섹션 초기 상태를 COMPLETE로 설정함.
	// OnConstruction()가 에디터 상에서 설정!
	SetState(bNoBattle ? ESectionState::COMPLETE : ESectionState::READY); // 첫 시작은 COMPLETE 스테이트에서 시작 (플레이어가 스테이지를 고를 수 있게)
}

void AABSection::SetState(ESectionState NewState) // 게임 로직 스테이트 머신
{
	switch (NewState)
	{
	case AABSection::ESectionState::READY: // 엑터 시작 스테이트, 문을 열어놓고 대기하다가 중앙 트리거 박스로 플레이어 진입을 감지하면 다음 스테이트로 전이
	{
		Trigger->SetCollisionProfileName(TEXT("ABTrigger"));
		for (UBoxComponent* GateTrigger : GateTriggers)
		{
			GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
		}
		OperateGates(true);
		break;
	}
	case AABSection::ESectionState::BATTLE: // 전투 스테이트, 문을 닫고(false) NPC와 무기상자 소환
	{
		Trigger->SetCollisionProfileName(TEXT("NoCollision"));
		for (UBoxComponent* GateTrigger : GateTriggers)
		{
			GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
		}
		OperateGates(false);
		GetWorld()->GetTimerManager().SetTimer(SpawnNPCTimerHandle, FTimerDelegate::CreateUObject(this, &AABSection::OnNPCSpawn), EnemySpawnTime, false); // NPC 스폰 설정(델리게이트에 멤버함수 바인딩)
		GetWorld()->GetTimerManager().SetTimer(SpawnItemBoxTimerHandle, FTimerDelegate::CreateLambda([this]()->void { // 아이템박스 스폰 설정(람다로 대체)
			FVector2D RandXY = FMath::RandPointInCircle(600.0f); // 섹션 내 영역을 무작위로 설정(아이템 박스)
			GetWorld()->SpawnActor<AABItemBox>(GetActorLocation() + FVector(RandXY, 30.0f), FRotator::ZeroRotator);
		}), ItemBoxSpawnTime, false);
		break;
	}
	case AABSection::ESectionState::COMPLETE: // 닫힌 문을 열고 문마다 배치한 총 4개의 트리거 게이트로 플레이어를 감지하면 이동한 문의 방향으로 새로운 섹션(스테이지) 소환
	{
		Trigger->SetCollisionProfileName(TEXT("NoCollision"));
		for (UBoxComponent* GateTrigger : GateTriggers)
		{
			GateTrigger->SetCollisionProfileName(TEXT("ABTrigger")); // 문을 통과하게 함
		}
		OperateGates(true);
		break;
	}
	}
	CurrentState = NewState;
}

void AABSection::OperateGates(bool bOpen) // 문여는 함수 
{
	for (UStaticMeshComponent* Gate : GateMeshes)
	{
		Gate->SetRelativeRotation(bOpen ? FRotator(0.0f, -90.0f, 0.0f) : FRotator::ZeroRotator); // 문을 열어줌
	}
}

void AABSection::OnNPCSpawn()
{
	GetWorld()->SpawnActor<AABCharacter>(GetActorLocation() + FVector::UpVector * 88.0f, FRotator::ZeroRotator); // 스폰될 액터(NPC) 좌표 설정 및 생성
}

void AABSection::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{ // 섹션 상태다이어그램 진행 함수 
	if (CurrentState == ESectionState::READY)
	{
		SetState(ESectionState::BATTLE); // 플레이거가 새로운 섹션에 진입할 시 Ready에서 Battle로 바꾼다.
	}
}

void AABSection::OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{ // 섹션 확장 함수
	ABCHECK(OverlappedComponent->ComponentTags.Num() == 1);
	FName ComponentTag = OverlappedComponent->ComponentTags[0];
	FName SocketName = FName(*ComponentTag.ToString().Left(2));
	if (!Mesh->DoesSocketExist(SocketName))
		return;

	FVector NewLocation = Mesh->GetSocketLocation(SocketName); // 새로운 메시 생성 좌표는 소켓(문)로 부터 
	TArray<FOverlapResult> OverlapResult;
	FCollisionQueryParams CollisionQueryParm(NAME_None, false, this);
	FCollisionObjectQueryParams ObjectQueryParm(FCollisionObjectQueryParams::InitType::AllObjects);
	bool bResult = GetWorld()->OverlapMultiByObjectType( // 해당 문 앞에 이미 색션이 생성된적이 있었는지 확인
		OverlapResult,
		NewLocation,
		FQuat::Identity,
		ObjectQueryParm,
		FCollisionShape::MakeSphere(775.0f),
		CollisionQueryParm
	);
	if (!bResult) // 아니라면
	{
		auto NewSection = GetWorld()->SpawnActor<AABSection>(NewLocation, FRotator::ZeroRotator); // 섹션(액터) 스폰
	}
	else // 전에 이미 생성되었다면 로그 출력
	{
		ABLOG_Long(Warning, TEXT("New section area is not empty.")); 
	}
}

// Called every frame
void AABSection::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AABSection::OnConstruction(const FTransform& Transform)
{ // OnConstruction()는 게임 시작 지점이 아닌 <에디터 설정> 화면에서 기본 섹션상태를 COMPLETE로 설정함
	Super::OnConstruction(Transform); // 해당 함수는 엑터와 에디터가 연동되는 오버라이딩 된 OnConstruction 함수이다. 
	// 에디터에서 bNoBattle의 값이 체크(참)이 될 경우, 에디터에선 섹션 엑터의 상태 값이 COMPLETE로 설정된다. (문이 열린다)
	SetState(bNoBattle ? ESectionState::COMPLETE : ESectionState::READY);
}

