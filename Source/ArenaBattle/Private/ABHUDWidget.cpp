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
	CharacterStat->OnHPChanged.AddUObject(this, &UABHUDWidget::UpdateCharacterStat); // ��������Ʈ�� HP�� ����� ������ UpdateCharacterStat()�� ȣ���ϵ��� ��� (HP �������� �ٲ𶧸��� UI�� �ٵ� ������Ʈ��)
}

void UABHUDWidget::BindPlayerState(AABPlayerState* PlayerState)
{
	ABCHECK(nullptr != PlayerState);
	CurrentPlayerState = PlayerState;
	PlayerState->OnPlayerStateChanged.AddUObject(this, &UABHUDWidget::UpdatePlayerState); // ��������Ʈ�� ������ ����� ������ UpdatePlayerState()�� ȣ���ϵ��� (���)
}


void UABHUDWidget::NativeConstruct()
{
	Super::NativeConstruct(); // ������ ����(����)�� Ŭ������ ������ ����
	HPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("pbHP"))); // HPBar(����) ���� bpHP(����)
	ExpBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("pbExp")));
	PlayerName = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtPlayerName")));
	PlayerLevel = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtLevel")));
	CurrentScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtCurrentScore"))); // ���� ���� UI
	HighScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtHighScore"))); // �ִ� ���� UI
}

void UABHUDWidget::UpdateCharacterStat() // ��������Ʈ�� ����� �Լ�
{
	ABCHECK(CurrentCharacterStat.IsValid());
	HPBar->SetPercent(CurrentCharacterStat->GetHPRatio()); // UI�� HP�� ����
}

void UABHUDWidget::UpdatePlayerState() // ��������Ʈ�� ����� �Լ�
{
	ABCHECK(CurrentPlayerState.IsValid());
	ExpBar->SetPercent(CurrentPlayerState->GetExpratio());
	PlayerName->SetText(FText::FromString(CurrentPlayerState->GetPlayerName()));
	PlayerLevel->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetCharacterLevel()))); // ��Ȯ���� �ʰ� �ǵ帮�� ��������, UI�� �� ��ġ���Ѿ���!
	CurrentScore->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetGameScore())));
}
