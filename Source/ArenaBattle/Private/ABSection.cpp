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

	FString AssetPath = TEXT("StaticMesh'/Game/Book/StaticMesh/SM_SQUARE.SM_SQUARE'"); // �ּ� ��� ���庯��
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_SQUARE(*AssetPath);
	if (SM_SQUARE.Succeeded())
	{
		Mesh->SetStaticMesh(SM_SQUARE.Object); // ������ �޽� ����(����)
	}
	else
	{
		ABLOG_Long(Error, TEXT("Failed to load staticmesh asset. : %s"), *AssetPath);
	}

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER")); // �߾��� �ڽ� Ʈ���� ���� (�÷��̾� ������)
	Trigger->SetBoxExtent(FVector(775.0f, 775.0f, 300.0f)); // �߾� �ڽ� ���� ����(���� �� ä��� ����)
	Trigger->SetupAttachment(RootComponent);
	Trigger->SetRelativeLocation(FVector(0.0f, 0.0f, 250.0f));
	Trigger->SetCollisionProfileName(TEXT("ABTrigger")); // �߾� �ݸ��� ������ ����
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnTriggerBeginOverlap); // ������Ʈ�� ��������Ʈ�� OnTriggerBeginOverlap() �Լ��� ���ε� ���ش�. Ready ���¿��� Battle ���·� ���� ����

	FString GateAssetPath = TEXT("StaticMesh'/Game/Book/StaticMesh/SM_GATE.SM_GATE'");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_GATE(*GateAssetPath); // ����Ʈ ����ƽ�޽�
	if (!SM_GATE.Succeeded())
	{
		ABLOG_Long(Error, TEXT("Failed to load staticmesh asset. : %s"), *GateAssetPath);
	}
	static FName GateSockets[] = { {TEXT("+XGate")}, {TEXT("-XGate")}, {TEXT("+YGate")}, {TEXT("-YGate")} };
	for (FName GateSocket : GateSockets) // �������� 4�� �ݺ�
	{
		ABCHECK(Mesh->DoesSocketExist(GateSocket));
		UStaticMeshComponent* NewGate = CreateDefaultSubobject<UStaticMeshComponent>(*GateSocket.ToString()); // ����Ʈ �޽� ������Ʈ ���� 
		NewGate->SetStaticMesh(SM_GATE.Object); // �޽� ����
		NewGate->SetupAttachment(RootComponent, GateSocket);
		NewGate->SetRelativeLocation(FVector(0.0f, -80.5f, 0.0f)); // ������Ʈ ��ǥ ���� 
		GateMeshes.Add(NewGate);

		UBoxComponent* NewGateTrigger = CreateDefaultSubobject<UBoxComponent>(*GateSocket.ToString().Append(TEXT("Trigger"))); // ����Ʈ �ݸ��� ����
		NewGateTrigger->SetBoxExtent(FVector(100.0f, 100.0f, 300.0f)); // �ݸ��� ����
		NewGateTrigger->SetupAttachment(RootComponent, GateSocket);
		NewGateTrigger->SetRelativeLocation(FVector(70.0f, 0.0f, 250.0f));
		NewGateTrigger->SetCollisionProfileName(TEXT("ABTrigger")); // �ݸ��� ����
		GateTriggers.Add(NewGateTrigger);
		
		NewGateTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnGateTriggerBeginOverlap); // ������Ʈ�� ��������Ʈ�� OnTriggerBeginOverlap() �Լ��� ���ε� ���ش�. �ش� �Լ��� �˻� �� ����(����)�� �����ϴ� ������ �Ѵ�.
		NewGateTrigger->ComponentTags.Add(GateSocket); // � ������ ������ �� �ֵ��� �±׸� �߰��Ѵ�.(x+ ~ y-) + Ʈ���Ŵ� �� 4���� ����������, �ش� Ʈ���ſ��� ȣ���ϴ� ��������Ʈ�� ������ �ϳ��� �Լ�(GateBeginOverlap)�� ȣ���Ѵ�.
	} 

	
}

// Called when the game starts or when spawned
void AABSection::BeginPlay()
{
	Super::BeginPlay();
	// Beginplay()�� �����Ͱ� �ƴ� �ΰ��ӿ��� ���� �ʱ� ���¸� COMPLETE�� ������.
	// OnConstruction()�� ������ �󿡼� ����!
	SetState(bNoBattle ? ESectionState::COMPLETE : ESectionState::READY); // ù ������ COMPLETE ������Ʈ���� ���� (�÷��̾ ���������� �� �� �ְ�)
}

void AABSection::SetState(ESectionState NewState) // ���� ���� ������Ʈ �ӽ�
{
	switch (NewState)
	{
	case AABSection::ESectionState::READY: // ���� ���� ������Ʈ, ���� ������� ����ϴٰ� �߾� Ʈ���� �ڽ��� �÷��̾� ������ �����ϸ� ���� ������Ʈ�� ����
	{
		Trigger->SetCollisionProfileName(TEXT("ABTrigger"));
		for (UBoxComponent* GateTrigger : GateTriggers)
		{
			GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
		}
		OperateGates(true);
		break;
	}
	case AABSection::ESectionState::BATTLE: // ���� ������Ʈ, ���� �ݰ�(false) NPC�� ������� ��ȯ
	{
		Trigger->SetCollisionProfileName(TEXT("NoCollision"));
		for (UBoxComponent* GateTrigger : GateTriggers)
		{
			GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
		}
		OperateGates(false);
		GetWorld()->GetTimerManager().SetTimer(SpawnNPCTimerHandle, FTimerDelegate::CreateUObject(this, &AABSection::OnNPCSpawn), EnemySpawnTime, false); // NPC ���� ����(��������Ʈ�� ����Լ� ���ε�)
		GetWorld()->GetTimerManager().SetTimer(SpawnItemBoxTimerHandle, FTimerDelegate::CreateLambda([this]()->void { // �����۹ڽ� ���� ����(���ٷ� ��ü)
			FVector2D RandXY = FMath::RandPointInCircle(600.0f); // ���� �� ������ �������� ����(������ �ڽ�)
			GetWorld()->SpawnActor<AABItemBox>(GetActorLocation() + FVector(RandXY, 30.0f), FRotator::ZeroRotator);
		}), ItemBoxSpawnTime, false);
		break;
	}
	case AABSection::ESectionState::COMPLETE: // ���� ���� ���� ������ ��ġ�� �� 4���� Ʈ���� ����Ʈ�� �÷��̾ �����ϸ� �̵��� ���� �������� ���ο� ����(��������) ��ȯ
	{
		Trigger->SetCollisionProfileName(TEXT("NoCollision"));
		for (UBoxComponent* GateTrigger : GateTriggers)
		{
			GateTrigger->SetCollisionProfileName(TEXT("ABTrigger")); // ���� ����ϰ� ��
		}
		OperateGates(true);
		break;
	}
	}
	CurrentState = NewState;
}

void AABSection::OperateGates(bool bOpen) // ������ �Լ� 
{
	for (UStaticMeshComponent* Gate : GateMeshes)
	{
		Gate->SetRelativeRotation(bOpen ? FRotator(0.0f, -90.0f, 0.0f) : FRotator::ZeroRotator); // ���� ������
	}
}

void AABSection::OnNPCSpawn()
{
	GetWorld()->SpawnActor<AABCharacter>(GetActorLocation() + FVector::UpVector * 88.0f, FRotator::ZeroRotator); // ������ ����(NPC) ��ǥ ���� �� ����
}

void AABSection::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{ // ���� ���´��̾�׷� ���� �Լ� 
	if (CurrentState == ESectionState::READY)
	{
		SetState(ESectionState::BATTLE); // �÷��̰Ű� ���ο� ���ǿ� ������ �� Ready���� Battle�� �ٲ۴�.
	}
}

void AABSection::OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{ // ���� Ȯ�� �Լ�
	ABCHECK(OverlappedComponent->ComponentTags.Num() == 1);
	FName ComponentTag = OverlappedComponent->ComponentTags[0];
	FName SocketName = FName(*ComponentTag.ToString().Left(2));
	if (!Mesh->DoesSocketExist(SocketName))
		return;

	FVector NewLocation = Mesh->GetSocketLocation(SocketName); // ���ο� �޽� ���� ��ǥ�� ����(��)�� ���� 
	TArray<FOverlapResult> OverlapResult;
	FCollisionQueryParams CollisionQueryParm(NAME_None, false, this);
	FCollisionObjectQueryParams ObjectQueryParm(FCollisionObjectQueryParams::InitType::AllObjects);
	bool bResult = GetWorld()->OverlapMultiByObjectType( // �ش� �� �տ� �̹� ������ ���������� �־����� Ȯ��
		OverlapResult,
		NewLocation,
		FQuat::Identity,
		ObjectQueryParm,
		FCollisionShape::MakeSphere(775.0f),
		CollisionQueryParm
	);
	if (!bResult) // �ƴ϶��
	{
		auto NewSection = GetWorld()->SpawnActor<AABSection>(NewLocation, FRotator::ZeroRotator); // ����(����) ����
	}
	else // ���� �̹� �����Ǿ��ٸ� �α� ���
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
{ // OnConstruction()�� ���� ���� ������ �ƴ� <������ ����> ȭ�鿡�� �⺻ ���ǻ��¸� COMPLETE�� ������
	Super::OnConstruction(Transform); // �ش� �Լ��� ���Ϳ� �����Ͱ� �����Ǵ� �������̵� �� OnConstruction �Լ��̴�. 
	// �����Ϳ��� bNoBattle�� ���� üũ(��)�� �� ���, �����Ϳ��� ���� ������ ���� ���� COMPLETE�� �����ȴ�. (���� ������)
	SetState(bNoBattle ? ESectionState::COMPLETE : ESectionState::READY);
}

