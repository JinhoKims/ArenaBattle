// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameState.h"

AABGameState::AABGameState()
{
	TotalGameScore = 0; // 통합 점수 획득
	bGameCleared = false;
}

int32 AABGameState::GetTotalGameScore() const
{
	return TotalGameScore;
}

void AABGameState::AddGameScore()
{
	TotalGameScore++; // 점수 추가
}

void AABGameState::SetGameCleared()
{
	bGameCleared = true; // 클리어 조건 확보
}

bool AABGameState::IsGameCleared() const
{
	return bGameCleared;
}
