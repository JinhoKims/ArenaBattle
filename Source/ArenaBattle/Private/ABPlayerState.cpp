// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerState.h"
#include "ABGameInstance.h"
#include "ABSaveGame.h" // UABSaveGame�� ĳ�����ϱ� ���� include

AABPlayerState::AABPlayerState()
{
    CharacterLevel = 1;
    GameScore = 0;
    Exp = 0;
    GameHighScore = 0;
    SaveSlotName = TEXT("Player1"); // ���̺� ���� �̸�
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
    auto ABSaveGame = Cast<UABSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
    if (nullptr == ABSaveGame)
    {
        ABSaveGame = GetMutableDefault<UABSaveGame>();
    }

    SetPlayerName(ABSaveGame->PlayerName); // APlayerState�� �ִ� �⺻���� PlayerName ���� �ʱ�ȭ (�г���)
    SetCharacterLevel(ABSaveGame->Level); // ���̺�� ������ �ε�
    GameScore = 0;
    GameHighScore = ABSaveGame->HighScore;
    Exp = ABSaveGame->Exp;
    SavePlayerData(); // ���� �÷��̾� �����͸� ������ �� �ٷ� ����
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
    SavePlayerData(); // ����ġ ȹ�� �� �ڵ� ����
    return DidLevelUp;
}

void AABPlayerState::AddGameScore()
{
    GameScore++; // ���� ���� ȹ��
    if (GameScore >= GameHighScore) // ������ ������ ������ ���� �ְ� ���ھ ���� 
    {
        GameHighScore = GameScore; // ���� �ְ� ���ھ ���� ���ھ�� �����Ұ�� ���� ���ھ ������
    }
    OnPlayerStateChanged.Broadcast(); // �÷��̾� UI�� ����ȭ 
    SavePlayerData(); // �ڵ� ����
}

int32 AABPlayerState::GetGameHighScore() const
{
    return GameHighScore;
}

void AABPlayerState::SavePlayerData()
{
    UABSaveGame* NewPlayerData = NewObject<UABSaveGame>(); // �����Ҵ� (�𸮾����ȯ�濡�� ������ �÷��Ͱ� ������)
    NewPlayerData->PlayerName = GetPlayerName(); // ���� �÷��̾� �̸��� UABSaveGame Data�� ����
    NewPlayerData->Level = CharacterLevel;
    NewPlayerData->Exp = Exp;
    NewPlayerData->HighScore = GameHighScore;

    if (!UGameplayStatics::SaveGameToSlot(NewPlayerData, SaveSlotName, 0)) // SaveSlotName(1)�� ���̺� ������ ����
    {
        ABLOG_Long(Error, TEXT("SaveGame Error!"));
    }
}


void AABPlayerState::SetCharacterLevel(int32 NewCharacterLevel)
{
    auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
    ABCHECK(nullptr != ABGameInstance);

    CurrentStatData = ABGameInstance->GetABCharacterData(NewCharacterLevel); // ���� ������Ʈ
    ABCHECK(nullptr != CurrentStatData);

    CharacterLevel = NewCharacterLevel;
}
