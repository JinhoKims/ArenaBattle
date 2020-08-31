// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameplayResultWidget.h"
#include "Components/TextBlock.h"
#include "ABGameState.h"

void UABGameplayResultWidget::BindGameState(AABGameState * GameState)
{
	ABCHECK(nullptr != GameState);
	CurrentGameState = GameState; // ���� ����(���)
}

void UABGameplayResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ABCHECK(CurrentGameState.IsValid());

	auto Result = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtResult"))); // ��� �ؽ�Ʈ ���� ĳ����
	ABCHECK(nullptr != Result);
	Result->SetText(FText::FromString(CurrentGameState->IsGameCleared() ? TEXT("Mission Complete") : TEXT("Mission Failed"))); // ��� ȭ���� �ؽ�Ʈ �ڽ��� ��ǥ �޼����� �¸� �Ǵ� �й� ���

	auto TotalScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtTotalScore"))); // ���� �ؽ�Ʈ ���� ĳ����
	ABCHECK(nullptr != TotalScore);
	TotalScore->SetText(FText::FromString(FString::FromInt(CurrentGameState->GetTotalGameScore()))); // ���� ���� ���
}
