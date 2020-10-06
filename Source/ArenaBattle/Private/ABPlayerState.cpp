// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerState.h"
#include "ABGameInstance.h"
#include "ABSaveGame.h" // UABSaveGame을 캐스팅하기 위해 include

AABPlayerState::AABPlayerState()
{
    CharacterLevel = 1;
    GameScore = 0;
    Exp = 0;
    GameHighScore = 0;
    SaveSlotName = TEXT("Player1"); // 세이브 슬롯 이름
    CharacterIndex = 0;
}

int32 AABPlayerState::GetGameScore() const
{
    return GameScore;
}

int32 AABPlayerState::GetCharacterLevel() const
{
    return CharacterLevel;
}

void AABPlayerState::InitPlayerData() // 로그인 시 동작(필수)
{
    auto ABSaveGame = Cast<UABSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
    if (nullptr == ABSaveGame)
    {
        ABSaveGame = GetMutableDefault<UABSaveGame>(); // 저장 데이터 블러오기
    }

    SetPlayerName(ABSaveGame->PlayerName); // APlayerState에 있는 기본제공 PlayerName 변수 초기화 (닉네임)
    SetCharacterLevel(ABSaveGame->Level); // 세이브된 데이터 로드
    GameScore = 0;
    GameHighScore = ABSaveGame->HighScore;
    Exp = ABSaveGame->Exp;
    CharacterIndex = ABSaveGame->CharacterIndex;
    SavePlayerData(); // 최초 플레이어 데이터를 생성한 후 바로 저장
}

float AABPlayerState::GetExpratio() const // 경험치 퍼센트화
{
    if (CurrentStatData->NextExp <= KINDA_SMALL_NUMBER)
        return 0.0f;

    float Result = (float)Exp / (float)CurrentStatData->NextExp;
    ABLOG_Long(Warning, TEXT("Ratio : %f, Current : %d, Next : %d"), Result, Exp, CurrentStatData->NextExp);
    return Result; // 0.x 으로 수치화하여 넘기기
}

bool AABPlayerState::AddExp(int32 IncomeExp) 
{
    if (CurrentStatData->NextExp == -1)
    {
        return false;
    }

    bool DidLevelUp = false;
    Exp = Exp + IncomeExp; // 경험치 획득
    if (Exp >= CurrentStatData->NextExp)
    {
        Exp -= CurrentStatData->NextExp;
        SetCharacterLevel(CharacterLevel + 1);
        DidLevelUp = true;
    }

    OnPlayerStateChanged.Broadcast();
    SavePlayerData(); // 경험치 획득 시 자동 저장
    return DidLevelUp;
}

void AABPlayerState::AddGameScore()
{
    GameScore++; // 개인 점수 획득
    if (GameScore >= GameHighScore) // 오른쪽 빨간색 점수는 역대 최고 스코어를 뜻함 
    {
        GameHighScore = GameScore; // 역대 최고 스코어가 현재 스코어와 동일할경우 역대 스코어도 갱신함
    }
    OnPlayerStateChanged.Broadcast(); // 플레이어 UI에 동기화 
    SavePlayerData(); // 자동 저장
}

int32 AABPlayerState::GetGameHighScore() const
{
    return GameHighScore;
}

void AABPlayerState::SavePlayerData() // 게임 저장
{
    UABSaveGame* NewPlayerData = NewObject<UABSaveGame>(); // 동적할당 (언리얼실행환경에서 가비지 컬렉터가 마무리)
    NewPlayerData->PlayerName = GetPlayerName(); // 현재 플레이어 이름을 UABSaveGame Data에 저장
    NewPlayerData->Level = CharacterLevel;
    NewPlayerData->Exp = Exp;
    NewPlayerData->HighScore = GameHighScore;
    NewPlayerData->CharacterIndex = CharacterIndex;

    if (!UGameplayStatics::SaveGameToSlot(NewPlayerData, SaveSlotName, 0)) // SaveSlotName(1)에 세이브 데이터 저장
    {
        ABLOG_Long(Error, TEXT("SaveGame Error!"));
    }
}

int32 AABPlayerState::GetCharacterIndex() const
{
    return CharacterIndex;
}


void AABPlayerState::SetCharacterLevel(int32 NewCharacterLevel)
{
    auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
    ABCHECK(nullptr != ABGameInstance);

    CurrentStatData = ABGameInstance->GetABCharacterData(NewCharacterLevel); // 플레이어 레벨 업데이트
    ABCHECK(nullptr != CurrentStatData);

    CharacterLevel = NewCharacterLevel;
}
