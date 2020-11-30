// Fill out your copyright notice in the Description page of Project Settings.


#include "Fountains.h"

// Sets default values
AFountains::AFountains()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY")); // �����Ҵ�
	Water = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WATER"));
	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("LIGHT"));
	Splash = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SPLASH"));
	Movement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("MOVEMENT"));

	RootComponent = Body; // Body�� ��Ʈ ������Ʈ�� ����
	Water->SetupAttachment(Body); // Water ������Ʈ�� ���� Body ������Ʈ�� �ͼ�(�ڽ�)
	Light->SetupAttachment(Body);
	Splash->SetupAttachment(Body);

	// ��ü ��� �����Ŵ�
	Water->SetRelativeLocation(FVector(0.0f, 0.0f, 135.0f));
	Light->SetRelativeLocation(FVector(0.0f, 0.0f, 195.0f));
	Splash->SetRelativeLocation(FVector(0.0f, 0.0f, 195.0f));

	// �ּ��� ����(�ؽ�ó ���̱�) �����ڰ� ȣ��� ������ ������ �ʱ�ȭ���� �ʿ�� ���⿡ static���� �����Ѵ�.
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SM_BODY(TEXT("/Game/InfinityBladeGrassLands/Environments/Plains/Env_Plains_Ruins/StaticMesh/SM_Plains_Castle_Fountain_01.SM_Plains_Castle_Fountain_01"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SM_WATER(TEXT("StaticMesh'/Game/InfinityBladeGrassLands/Effects/FX_Meshes/Env/SM_Plains_Fountain_02.SM_Plains_Fountain_02'"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem>PS_SPLASH(TEXT("ParticleSystem'/Game/InfinityBladeGrassLands/Effects/FX_Ambient/Water/P_Water_Fountain_Splash_Base_01.P_Water_Fountain_Splash_Base_01'"));

	// �� ������Ʈ : �޽�(������)�� ��� ��ü / �޽� : ������
	if (SM_BODY.Succeeded())
		Body->SetStaticMesh(SM_BODY.Object); // �����ϸ� �޽� ��
	if (SM_WATER.Succeeded())
		Water->SetStaticMesh(SM_WATER.Object);
	if (PS_SPLASH.Succeeded())
		Splash->SetTemplate(PS_SPLASH.Object);


	// ��ü/�� ���� �ʱ�ȭ
	RotateSpeed = 30.0f;
	Movement->RotationRate = FRotator(0.0f, RotateSpeed, 0.0f); // ȸ�� ���


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

