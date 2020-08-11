// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterStatComponent.h"
#include "ABGameInstance.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	Level = 1;
}


// Called when the game starts
void UABCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UABCharacterStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UABCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
	SetNewLevel(Level);
}

void UABCharacterStatComponent::SetNewLevel(int32 NewLevel) // 레벨 설정 
{
	auto ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())); // include GameInstance.h가 필요

	ABCHECK(nullptr != ABGameInstance);
	CurrentStatData = ABGameInstance->GetABCharacterData(NewLevel);
	if (nullptr != CurrentStatData)
	{
		Level = NewLevel;
		SetHP(CurrentStatData->MaxHP);
		CurrentHP = CurrentStatData->MaxHP;
	}
	else
	{
		ABLOG_Long(Error, TEXT("Level (%d) data dosen't exist"), NewLevel);
	}
}

void UABCharacterStatComponent::SetDamage(float NewDamage) // 공격(데미지) 받기
{
	ABCHECK(nullptr != CurrentStatData);
	SetHP(FMath::Clamp<float>(CurrentHP - NewDamage, 0.0f, CurrentStatData->MaxHP));
}

void UABCharacterStatComponent::SetHP(float NewHP)
{
	CurrentHP = NewHP;
	OnHPChanged.Broadcast(); // 캐릭터 hp가 변경되면 델리게이트 브로드캐스팅
	if (CurrentHP < KINDA_SMALL_NUMBER) // hp가 변경되던 중 피가 0 이하가 될 경우 Zero 델리게이트 호출 
	{
		CurrentHP = 0.0f;
		OnHPIsZero.Broadcast(); 
	}
}

float UABCharacterStatComponent::GetAttack() // 공격 하기(데미지 전달)
{
	ABCHECK(nullptr != CurrentStatData, 0.0f);
	return CurrentStatData->Attack; // 테이블에 저장된 레벨별 공격데미지를 전달
}

float UABCharacterStatComponent::GetHPRatio()
{
	ABCHECK(nullptr != CurrentStatData, 0.0f);

	return (CurrentStatData->MaxHP < KINDA_SMALL_NUMBER) ? 0.0f : (CurrentHP / CurrentStatData->MaxHP);
}

