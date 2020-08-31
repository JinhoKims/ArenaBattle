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
	virtual void PostInitializeComponents() override; // 폰과 플레이어 컨트롤러(액터)가 생성되는 시점
	virtual void OnPossess(APawn * aPawn) override; // 빙의(Possess)를 진행하는 시점
	class UABHUDWidget* GetHUDWidget() const;
	void NPCKill(class AABCharacter* KilledNPC) const;
	void AddGameScore() const;
	void ChangeInputMode(bool bGameMode = true);
	void ShowResultUI(); // 게임 결과 화면

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override; // 단축키 바인딩
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UABHUDWidget> HUDWidgetClass; // 에디터에서 UI에셋(리소스)과 연동할 템플릿 클래스
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UABGamplayWidget> MenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UABGameplayResultWidget> ResultWidgetClass;
	
private:
	UPROPERTY()
	class UABHUDWidget* HUDWidget; // 인스턴스화될 위젯 클래스들

	UPROPERTY()
	class AABPlayerState* ABPlayerState;
	
	UPROPERTY()
	class UABGamplayWidget* MenuWidget;

	UPROPERTY()
	class UABGameplayResultWidget * ResultWidget; // 위젯 클래스 : (AddToViewport())로 화면에 출력할 수 있다.

	FInputModeGameOnly GameInputMode;
	FInputModeUIOnly UIInputMode;
	void OnGamePause(); // 게임 중지 
};
