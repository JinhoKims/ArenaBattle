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
	AABPlayerController();
	virtual void PostInitializeComponents() override; // ���� �÷��̾� ��Ʈ�ѷ�(����)�� �����Ǵ� ����
	virtual void OnPossess(APawn * aPawn) override; // ����(Possess)�� �����ϴ� ����
	class UABHUDWidget* GetHUDWidget() const;
	void NPCKill(class AABCharacter* KilledNPC) const;
	void AddGameScore() const;
	void ChangeInputMode(bool bGameMode = true);
	void ShowResultUI(); // ���� ��� ȭ��

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override; // ����Ű ���ε�
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UABHUDWidget> HUDWidgetClass; // �����Ϳ��� UI����(���ҽ�)�� ������ ���ø� Ŭ����
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UABGamplayWidget> MenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UABGameplayResultWidget> ResultWidgetClass;
	
private:
	UPROPERTY()
	class UABHUDWidget* HUDWidget; // �ν��Ͻ�ȭ�� ���� Ŭ������

	UPROPERTY()
	class AABPlayerState* ABPlayerState;
	
	UPROPERTY()
	class UABGamplayWidget* MenuWidget;

	UPROPERTY()
	class UABGameplayResultWidget * ResultWidget; // ���� Ŭ���� : (AddToViewport())�� ȭ�鿡 ����� �� �ִ�.

	FInputModeGameOnly GameInputMode;
	FInputModeUIOnly UIInputMode;
	void OnGamePause(); // ���� ���� 
};
