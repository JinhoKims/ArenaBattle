// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ABGamplayWidget.h"
#include "ABGameplayResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABGameplayResultWidget : public UABGamplayWidget
{
	GENERATED_BODY()
	
public:
	void BindGameState(class AABGameState* GameState); // 결과 화면 함수
	
protected:
	virtual void NativeConstruct() override; // GamplayWidget의 Construct를 오버라이드

private:
	TWeakObjectPtr<class AABGameState> CurrentGameState; // 플레이 결과 화면
};
