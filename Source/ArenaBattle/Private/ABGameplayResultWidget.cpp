// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameplayResultWidget.h"
#include "Components/TextBlock.h"
#include "ABGameState.h"

void UABGameplayResultWidget::BindGameState(AABGameState * GameState)
{
	ABCHECK(nullptr != GameState);
	CurrentGameState = GameState; // 게임 정보(결과)
}

void UABGameplayResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ABCHECK(CurrentGameState.IsValid());

	auto Result = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtResult"))); // 결과 텍스트 상자 캐스팅
	ABCHECK(nullptr != Result);
	Result->SetText(FText::FromString(CurrentGameState->IsGameCleared() ? TEXT("Mission Complete") : TEXT("Mission Failed"))); // 결과 화면의 텍스트 박스에 목표 달성여부 승리 또는 패배 출력

	auto TotalScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtTotalScore"))); // 점수 텍스트 상자 캐스팅
	ABCHECK(nullptr != TotalScore);
	TotalScore->SetText(FText::FromString(FString::FromInt(CurrentGameState->GetTotalGameScore()))); // 종합 점수 출력
}
