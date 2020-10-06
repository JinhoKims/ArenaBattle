// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameMode.h"
#include "ABCharacter.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"
#include "ABGameState.h"

AABGameMode::AABGameMode() {
	DefaultPawnClass = AABCharacter::StaticClass();
	PlayerControllerClass = AABPlayerController::StaticClass();
	PlayerStateClass = AABPlayerState::StaticClass(); // PlayerState 클래스를 AB플레이어 스테이트로 동적할당하여 스텟정보를 가져올 수 있다.
	GameStateClass = AABGameState::StaticClass(); // GameState 클래스를 가져온다
	ScoreToClear = 10; // 게임 승리 조건 설정
}

void AABGameMode::PostInitializeComponents() // 게임 초기화
{
	Super::PostInitializeComponents();
	ABLOG_Short(Error);
	ABGameState = Cast<AABGameState>(GameState); // 승리 조건을 가져옴
}

void AABGameMode::PostLogin(APlayerController* NewPlayer) {
	ABLOG_Long(Warning, TEXT("PostLogin Begin"));
	Super::PostLogin(NewPlayer);
	auto ABPlayerState = Cast<AABPlayerState>(NewPlayer->PlayerState); // 플레이어 컨트롤러의 정보를 가져옴
	ABCHECK(nullptr != ABPlayerState);
	ABPlayerState->InitPlayerData(); // PostLogin()에서 플레이어 스텟을 초기화한다.
	ABLOG_Long(Warning, TEXT("PostLogin End"));
}

void AABGameMode::AddScore(AABPlayerController* ScoredPlayer) // (플레이어 점수)와 (통합 점수)를 획득하는 함수를 각각 호출시킨다.
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++) // 게임에 참여 중인 플레이어 컨트롤러의 목록은 GetPlayerControllerIterator()를 통해 얻을 수 있다.
	{ // 존재하는 플레이어 중 점수를 획득할 플레이어를 산출한다.
		const auto ABPlayerController = Cast<AABPlayerController>(It->Get()); // 개별 컨트롤러로 캐스팅
		if ((nullptr != ABPlayerController) && (ScoredPlayer == ABPlayerController)) // 플컨이 null이 아니고 플레이어 중 점수를 획든한 플레이어와 일치하는 경우 
		{
			ABPlayerController->AddGameScore(); // 해당 컨트롤러에게 점수 추가
			break; // 반복문 종료
		}
	}
	ABGameState->AddGameScore(); // 통합 점수도 획득

	if (GetScore() >= ScoreToClear) // 현재 점수가 클리어 목표보다 높을 경우
	{
		ABGameState->SetGameCleared(); // 클리어 활성화

		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++) // 월드의 모든 폰 대상
		{
			(*It)->TurnOff(); // 폰의 활동을 멈춤
		}

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++) // 월드의 모든 플레이어 컨트롤러 대상
		{
			const auto ABPlayerController = Cast<AABPlayerController>(It->Get()); // 플레이어 컨트롤러 캐스팅
			if (nullptr != ABPlayerController)
			{
				ABPlayerController->ShowResultUI(); // 결과 화면 출력
			}
		}
	}
}

int32 AABGameMode::GetScore() const
{
	return ABGameState->GetTotalGameScore();
}
