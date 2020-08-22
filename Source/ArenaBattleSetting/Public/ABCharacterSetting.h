// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ABCharacterSetting.generated.h"

/**
 * 
 */
UCLASS(config=ArenaBattle)
class ARENABATTLESETTING_API UABCharacterSetting : public UObject
{
	GENERATED_BODY()
	
public:
	UABCharacterSetting();

	UPROPERTY(config) // ini 파일을 불러오도록 하는 메크로
	TArray<FSoftObjectPath> CharacterAssets; // ini 파일 데이터 배열
};
