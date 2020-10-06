// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameMode.h"
#include "ABCharacter.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"
#include "ABGameState.h"

AABGameMode::AABGameMode() {
	DefaultPawnClass = AABCharacter::StaticClass();
	PlayerControllerClass = AABPlayerController::StaticClass();
	PlayerStateClass = AABPlayerState::StaticClass(); // PlayerState Ŭ������ AB�÷��̾� ������Ʈ�� �����Ҵ��Ͽ� ���������� ������ �� �ִ�.
	GameStateClass = AABGameState::StaticClass(); // GameState Ŭ������ �����´�
	ScoreToClear = 10; // ���� �¸� ���� ����
}

void AABGameMode::PostInitializeComponents() // ���� �ʱ�ȭ
{
	Super::PostInitializeComponents();
	ABLOG_Short(Error);
	ABGameState = Cast<AABGameState>(GameState); // �¸� ������ ������
}

void AABGameMode::PostLogin(APlayerController* NewPlayer) {
	ABLOG_Long(Warning, TEXT("PostLogin Begin"));
	Super::PostLogin(NewPlayer);
	auto ABPlayerState = Cast<AABPlayerState>(NewPlayer->PlayerState); // �÷��̾� ��Ʈ�ѷ��� ������ ������
	ABCHECK(nullptr != ABPlayerState);
	ABPlayerState->InitPlayerData(); // PostLogin()���� �÷��̾� ������ �ʱ�ȭ�Ѵ�.
	ABLOG_Long(Warning, TEXT("PostLogin End"));
}

void AABGameMode::AddScore(AABPlayerController* ScoredPlayer) // (�÷��̾� ����)�� (���� ����)�� ȹ���ϴ� �Լ��� ���� ȣ���Ų��.
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++) // ���ӿ� ���� ���� �÷��̾� ��Ʈ�ѷ��� ����� GetPlayerControllerIterator()�� ���� ���� �� �ִ�.
	{ // �����ϴ� �÷��̾� �� ������ ȹ���� �÷��̾ �����Ѵ�.
		const auto ABPlayerController = Cast<AABPlayerController>(It->Get()); // ���� ��Ʈ�ѷ��� ĳ����
		if ((nullptr != ABPlayerController) && (ScoredPlayer == ABPlayerController)) // ������ null�� �ƴϰ� �÷��̾� �� ������ ȹ���� �÷��̾�� ��ġ�ϴ� ��� 
		{
			ABPlayerController->AddGameScore(); // �ش� ��Ʈ�ѷ����� ���� �߰�
			break; // �ݺ��� ����
		}
	}
	ABGameState->AddGameScore(); // ���� ������ ȹ��

	if (GetScore() >= ScoreToClear) // ���� ������ Ŭ���� ��ǥ���� ���� ���
	{
		ABGameState->SetGameCleared(); // Ŭ���� Ȱ��ȭ

		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++) // ������ ��� �� ���
		{
			(*It)->TurnOff(); // ���� Ȱ���� ����
		}

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++) // ������ ��� �÷��̾� ��Ʈ�ѷ� ���
		{
			const auto ABPlayerController = Cast<AABPlayerController>(It->Get()); // �÷��̾� ��Ʈ�ѷ� ĳ����
			if (nullptr != ABPlayerController)
			{
				ABPlayerController->ShowResultUI(); // ��� ȭ�� ���
			}
		}
	}
}

int32 AABGameMode::GetScore() const
{
	return ABGameState->GetTotalGameScore();
}
