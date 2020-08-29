// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameState.h"

AABGameState::AABGameState()
{
	TotalGameScore = 0; // ÅëÇÕ Á¡¼ö È¹µæ
}

int32 AABGameState::GetTotalGameScore() const
{
	return TotalGameScore;
}

void AABGameState::AddGameScore()
{
	TotalGameScore++;
}
