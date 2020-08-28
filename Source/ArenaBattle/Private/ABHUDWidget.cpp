// Fill out your copyright notice in the Description page of Project Settings.


#include "ABHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "ABCharacterStatComponent.h"
#include "ABPlayerState.h"


void UABHUDWidget::BindCharacterStat(UABCharacterStatComponent* CharacterStat)
{
	ABCHECK(nullptr != CharacterStat);
	CurrentCharacterStat = CharacterStat;
	CharacterStat->OnHPChanged.AddUObject(this, &UABHUDWidget::UpdateCharacterStat); // 델리게이트에 HP가 변경될 때마다 UpdateCharacterStat()를 호출하도록 등록 (HP 변수값이 바뀔때마다 UI의 바도 업데이트됨)
}

void UABHUDWidget::BindPlayerState(AABPlayerState* PlayerState)
{
	ABCHECK(nullptr != PlayerState);
	CurrentPlayerState = PlayerState;
	PlayerState->OnPlayerStateChanged.AddUObject(this, &UABHUDWidget::UpdatePlayerState); // 델리게이트에 스텟이 변경될 때마다 UpdatePlayerState()를 호출하도록 (등록)
}


void UABHUDWidget::NativeConstruct()
{
	Super::NativeConstruct(); // 블프의 변수(위젯)과 클래스의 변수를 연동
	HPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("pbHP"))); // HPBar(변수) 연동 bpHP(블프)
	ExpBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("pbExp")));
	PlayerName = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtPlayerName")));
	PlayerLevel = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtLevel")));
	CurrentScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtCurrentScore"))); // 현재 점수 UI
	HighScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtHighScore"))); // 최대 점수 UI
}

void UABHUDWidget::UpdateCharacterStat() // 델리게이트에 등록할 함수
{
	ABCHECK(CurrentCharacterStat.IsValid());
	HPBar->SetPercent(CurrentCharacterStat->GetHPRatio()); // UI의 HP바 갱신
}

void UABHUDWidget::UpdatePlayerState() // 델리게이트에 등록할 함수
{
	ABCHECK(CurrentPlayerState.IsValid());
	ExpBar->SetPercent(CurrentPlayerState->GetExpratio());
	PlayerName->SetText(FText::FromString(CurrentPlayerState->GetPlayerName()));
	PlayerLevel->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetCharacterLevel()))); // 정확하지 않게 건드리면 에러나니, UI랑 꼭 일치시켜야함!
	CurrentScore->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetGameScore())));
}
