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
    SetPlayerName(TEXT("Destiny")); // APlayerState에 있는 기본제공 PlayerName 변수 초기화 (닉네임)
    SetCharacterLevel(5);
    GameScore = 0;
    Exp = 0;
}

float AABPlayerState::GetExpratio() const // 경험치통 출력
{
    if (CurrentStatData->NextExp <= KINDA_SMALL_NUMBER)
        return 0.0f;

    float Result = (float)Exp / (float)CurrentStatData->NextExp;
    ABLOG_Long(Warning, TEXT("Ratio : %f, Current : %d, Next : %d"), Result, Exp, CurrentStatData->NextExp);
    return Result; // 경험치 게이지 넘기기
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
    GameScore++; // 개인 점수 획득
    OnPlayerStateChanged.Broadcast(); // 플레이어 UI에 동기화 
}


void AABPlayerState::SetCharacterLevel(int32 NewCharacterLevel)
{
    auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
    ABCHECK(nullptr != ABGameInstance);

    CurrentStatData = ABGameInstance->GetABCharacterData(NewCharacterLevel); // 레벨 업데이트
    ABCHECK(nullptr != CurrentStatData);

    CharacterLevel = NewCharacterLevel;
}
