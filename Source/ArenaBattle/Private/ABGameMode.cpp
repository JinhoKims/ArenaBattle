// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameMode.h"
#include "ABCharacter.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"

AABGameMode::AABGameMode() {
	ABLOG_Long(Error, TEXT("AABGameMode Costructor"));
	DefaultPawnClass = AABCharacter::StaticClass();
	PlayerControllerClass = AABPlayerController::StaticClass();
	PlayerStateClass = AABPlayerState::StaticClass(); // PlayerState 클래스를 AB플레이어 스테이트로 동적할당하여 스텟정보를 가져올 수 있다.
}

void AABGameMode::PostLogin(APlayerController* NewPlayer) {
	ABLOG_Long(Warning, TEXT("PostLogin Begin"));
	Super::PostLogin(NewPlayer);
	auto ABPlayerState = Cast<AABPlayerState>(NewPlayer->PlayerState);
	ABCHECK(nullptr != ABPlayerState);
	ABPlayerState->InitPlayerData(); // PostLogin()에서 플레이어 스텟을 초기화한다.
	ABLOG_Long(Warning, TEXT("PostLogin End"));
}