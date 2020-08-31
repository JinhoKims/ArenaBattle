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
	void BindGameState(class AABGameState* GameState); // ��� ȭ�� �Լ�
	
protected:
	virtual void NativeConstruct() override; // GamplayWidget�� Construct�� �������̵�

private:
	TWeakObjectPtr<class AABGameState> CurrentGameState; // �÷��� ��� ȭ��
};
