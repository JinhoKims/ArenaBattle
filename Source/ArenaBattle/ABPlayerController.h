// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/PlayerController.h"
#include "ABPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void PostInitializeComponents() override; // 폰과 플레이어 컨트롤러(액터)가 생성되는 시점
	virtual void OnPossess(APawn * aPawn) override; // 빙의(Possess)를 진행하는 시점
protected:
	virtual void BeginPlay() override;
	
};
