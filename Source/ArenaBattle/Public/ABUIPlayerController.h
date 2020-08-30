// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/PlayerController.h"
#include "ABUIPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABUIPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadwrite, Category = UI)
	TSubclassOf<class UUserWidget> UIWidgetClass; // 띄울 UI 위젯, 에디터에서 위젯 값을 설정할 수 있다. 에디터에서 간략한 범위로 표시되기 위해 TSubclassOf를 사용한다.

	UPROPERTY()
	class UUserWidget * UIWidgetInstance; // UI 위젯에 대한 조작용 인스턴스
	
};
