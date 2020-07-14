// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerController.h"

void AABPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABLOG_Short(Warning);
}

void AABPlayerController::OnPossess(APawn * aPawn)
{
	ABLOG_Short(Warning); // ��Ʈ�ѷ� �� ����
	Super::OnPossess(aPawn);
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	ABLOG_Short(Warning);
}

