// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerState.h"
#include "ABGameInstance.h"

AABPlayerState::AABPlayerState()
{
    CharacterLevel = 1;
    GameScore = 0;
    Exp = 0;
}

int32 AABPlayerState::GetGameScore() const
{
    return GameScore;
}

int32 AABPlayerState::GetCharacterLevel() const
{
    return CharacterLevel;
}

void AABPlayerState::InitPlayerData()
{
    SetPlayerName(TEXT("Destiny")); // APlayerState�� �ִ� �⺻���� PlayerName ���� �ʱ�ȭ (�г���)
    SetCharacterLevel(5);
    GameScore = 0;
    Exp = 0;
}

float AABPlayerState::GetExpratio() const // ����ġ�� ���
{
    if (CurrentStatData->NextExp <= KINDA_SMALL_NUMBER)
        return 0.0f;

    float Result = (float)Exp / (float)CurrentStatData->NextExp;
    ABLOG_Long(Warning, TEXT("Ratio : %f, Current : %d, Next : %d"), Result, Exp, CurrentStatData->NextExp);
    return Result; // ����ġ ������ �ѱ��
}

bool AABPlayerState::AddExp(int32 IncomeExp) 
{
    if (CurrentStatData->NextExp == -1)
    {
        return false;
    }

    bool DidLevelUp = false;
    Exp = Exp + IncomeExp;
    if (Exp >= CurrentStatData->NextExp)
    {
        Exp -= CurrentStatData->NextExp;
        SetCharacterLevel(CharacterLevel + 1);
        DidLevelUp = true;
    }

    OnPlayerStateChanged.Broadcast();
    return DidLevelUp;
}

void AABPlayerState::AddGameScore()
{
    GameScore++; // ���� ���� ȹ��
    OnPlayerStateChanged.Broadcast(); // �÷��̾� UI�� ����ȭ 
}


void AABPlayerState::SetCharacterLevel(int32 NewCharacterLevel)
{
    auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
    ABCHECK(nullptr != ABGameInstance);

    CurrentStatData = ABGameInstance->GetABCharacterData(NewCharacterLevel); // ���� ������Ʈ
    ABCHECK(nullptr != CurrentStatData);

    CharacterLevel = NewCharacterLevel;
}
