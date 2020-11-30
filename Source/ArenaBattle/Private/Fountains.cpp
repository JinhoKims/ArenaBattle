// Fill out your copyright notice in the Description page of Project Settings.


#include "Fountains.h"

// Sets default values
AFountains::AFountains()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY")); // 동적할당
	Water = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WATER"));
	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("LIGHT"));
	Splash = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SPLASH"));
	Movement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("MOVEMENT"));

	RootComponent = Body; // Body를 루트 컴포넌트로 지정
	Water->SetupAttachment(Body); // Water 컴포넌트는 이제 Body 컴포넌트에 귀속(자식)
	Light->SetupAttachment(Body);
	Splash->SetupAttachment(Body);

	// 객체 상대 포지셔닝
	Water->SetRelativeLocation(FVector(0.0f, 0.0f, 135.0f));
	Light->SetRelativeLocation(FVector(0.0f, 0.0f, 195.0f));
	Splash->SetRelativeLocation(FVector(0.0f, 0.0f, 195.0f));

	// 애셋의 지정(텍스처 붙이기) 생성자가 호출될 때마다 변수를 초기화해줄 필요는 없기에 static으로 구성한다.
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SM_BODY(TEXT("/Game/InfinityBladeGrassLands/Environments/Plains/Env_Plains_Ruins/StaticMesh/SM_Plains_Castle_Fountain_01.SM_Plains_Castle_Fountain_01"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SM_WATER(TEXT("StaticMesh'/Game/InfinityBladeGrassLands/Effects/FX_Meshes/Env/SM_Plains_Fountain_02.SM_Plains_Fountain_02'"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem>PS_SPLASH(TEXT("ParticleSystem'/Game/InfinityBladeGrassLands/Effects/FX_Ambient/Water/P_Water_Fountain_Splash_Base_01.P_Water_Fountain_Splash_Base_01'"));

	// ※ 컴포넌트 : 메시(데이터)를 담는 객체 / 메시 : 데이터
	if (SM_BODY.Succeeded())
		Body->SetStaticMesh(SM_BODY.Object); // 성공하면 메시 셋
	if (SM_WATER.Succeeded())
		Water->SetStaticMesh(SM_WATER.Object);
	if (PS_SPLASH.Succeeded())
		Splash->SetTemplate(PS_SPLASH.Object);


	// 객체/값 유형 초기화
	RotateSpeed = 30.0f;
	Movement->RotationRate = FRotator(0.0f, RotateSpeed, 0.0f); // 회전 기능


}

// Called when the game starts or when spawned
void AFountains::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFountains::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

