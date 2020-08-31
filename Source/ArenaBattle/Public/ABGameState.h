// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/GameStateBase.h"
#include "ABGameState.generated.h"

/**
 * 종합 게임 점수를 관리하는 클래스
 */
UCLASS()
class ARENABATTLE_API AABGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AABGameState();
	int32 GetTotalGameScore() const;
	void AddGameScore();
	void SetGameCleared(); // 클리어 달성 활성화
	bool IsGameCleared() const; // 달성여부 반환

private:
	UPROPERTY(Transient)
	int32 TotalGameScore;

	UPROPERTY(Transient)
	bool bGameCleared; // 게임 클리어 조건 달성여부
};
