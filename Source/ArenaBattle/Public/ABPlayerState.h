// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/PlayerState.h"
#include "ABPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnPlayerStateChangeDelegate);

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AABPlayerState(); // �÷��̾� ������ ������ ���� ����
	FOnPlayerStateChangeDelegate OnPlayerStateChanged; // ���� ����ȭ�� ���� ��������Ʈ
	int32 GetGameScore() const;
	int32 GetCharacterLevel() const;
	float GetExpratio() const;
	void InitPlayerData();
	bool AddExp(int32 IncomeExp);
	void AddGameScore();
	int32 GetGameHighScore() const;
	FString SaveSlotName; // ���̺� ������ 1���� ����
	void SavePlayerData();
	int32 GetCharacterIndex() const;

protected:
	UPROPERTY(Transient)
	int32 GameScore;

	UPROPERTY(Transient)
	int32 CharacterLevel;

	UPROPERTY(Transient)
	int32 Exp;

	UPROPERTY(Transient)
	int32 GameHighScore; // ���� �ְ� ���ھ�(������)

	UPROPERTY(Transient)
	int32 CharacterIndex;

private:
	void SetCharacterLevel(int32 NewCharacterLevel);
	struct FABCharacterData* CurrentStatData;
	
};
