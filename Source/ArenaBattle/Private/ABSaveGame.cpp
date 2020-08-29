// Fill out your copyright notice in the Description page of Project Settings.


#include "ABSaveGame.h"

UABSaveGame::UABSaveGame()
{
	Level = 1;
	Exp = 0;
	PlayerName = TEXT("Guest");
	HighScore = 0;
} // 세이브 데이터(변수)는 ABPlayerState에서 관리한다.