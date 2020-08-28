// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerController.h"
#include "ABHUDWidget.h"
#include "ABPlayerState.h"
#include "ABCharacter.h"

AABPlayerController::AABPlayerController()
{
	static ConstructorHelpers::FClassFinder<UABHUDWidget> UI_HUD_C(TEXT("WidgetBlueprint'/Game/Book/UI/UI_HUD.UI_HUD_C'")); // UI애셋의 레퍼런스를 복사해
	if (UI_HUD_C.Succeeded())
	{
		HUDWidgetClass = UI_HUD_C.Class; // 위젯 인스턴스를 생선한 후
	}
}

void AABPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABLOG_Short(Warning);
}

void AABPlayerController::OnPossess(APawn * aPawn)
{
	ABLOG_Short(Warning); // 컨트롤러 폰 빙의
	Super::OnPossess(aPawn);
}

UABHUDWidget* AABPlayerController::GetHUDWidget() const
{
	return HUDWidget;
}

void AABPlayerController::NPCKill(AABCharacter* KilledNPC) const
{
	ABPlayerState->AddExp(KilledNPC->GetExp());
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	
	HUDWidget = CreateWidget<UABHUDWidget>(this, HUDWidgetClass); // CreateWidget() 함수로 이를 플레이어의 화면에 씌워준다.
	HUDWidget->AddToViewport();

	ABPlayerState = Cast<AABPlayerState>(PlayerState); // HUD 위젯과 플레이어 스테이트(이름) 연결
	ABCHECK(nullptr != ABPlayerState);
	HUDWidget->BindPlayerState(ABPlayerState);
	ABPlayerState->OnPlayerStateChanged.Broadcast(); // 델리게이트 브로드캐스팅(호출)
	
}


