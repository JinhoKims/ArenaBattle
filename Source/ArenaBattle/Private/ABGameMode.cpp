// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameMode.h"
#include "ABCharacter.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"

AABGameMode::AABGameMode() {
	ABLOG_Long(Error, TEXT("AABGameMode Costructor"));
	DefaultPawnClass = AABCharacter::StaticClass();
	PlayerControllerClass = AABPlayerController::StaticClass();
	PlayerStateClass = AABPlayerState::StaticClass(); // PlayerState Ŭ������ AB�÷��̾� ������Ʈ�� �����Ҵ��Ͽ� ���������� ������ �� �ִ�.
}

void AABGameMode::PostLogin(APlayerController* NewPlayer) {
	ABLOG_Long(Warning, TEXT("PostLogin Begin"));
	Super::PostLogin(NewPlayer);
	auto ABPlayerState = Cast<AABPlayerState>(NewPlayer->PlayerState);
	ABCHECK(nullptr != ABPlayerState);
	ABPlayerState->InitPlayerData(); // PostLogin()���� �÷��̾� ������ �ʱ�ȭ�Ѵ�.
	ABLOG_Long(Warning, TEXT("PostLogin End"));
}