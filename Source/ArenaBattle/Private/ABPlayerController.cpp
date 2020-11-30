// Fill out your copyright notice in the Description page of Project Settings.

#include "ABPlayerController.h"
#include "ABHUDWidget.h"
#include "ABPlayerState.h"
#include "ABCharacter.h"
#include "ABGamplayWidget.h"
#include "ABGameplayResultWidget.h"
#include "ABGameState.h"

AABPlayerController::AABPlayerController()
{
	static ConstructorHelpers::FClassFinder<UABHUDWidget> UI_HUD_C(TEXT("WidgetBlueprint'/Game/Book/UI/UI_HUD.UI_HUD_C'")); // UI애셋의 레퍼런스를 복사해
	if (UI_HUD_C.Succeeded())
	{
		HUDWidgetClass = UI_HUD_C.Class; // 리소스(애셋)과 클래스를 바인딩 (인스턴스화 준비)
	}

	static ConstructorHelpers::FClassFinder<UABGamplayWidget> UI_MENU_C(TEXT("WidgetBlueprint'/Game/Book/UI/UI_Menu.UI_Menu_C'"));
	if (UI_MENU_C.Succeeded())
	{
		MenuWidgetClass = UI_MENU_C.Class; 
	}

	static ConstructorHelpers::FClassFinder<UABGameplayResultWidget> UI_RESULT_C(TEXT("WidgetBlueprint'/Game/Book/UI/UI_Result.UI_Result_C'"));
	if (UI_RESULT_C.Succeeded())
	{
		ResultWidgetClass = UI_RESULT_C.Class; 
	}
}

void AABPlayerController::PostInitializeComponents() // 컨트롤러 초기화
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
	ABPlayerState->AddExp(KilledNPC->GetExp()); // 경험치 획득
}

void AABPlayerController::AddGameScore() const
{
	ABPlayerState->AddGameScore(); // 개인 점수 획득
}

void AABPlayerController::ChangeInputMode(bool bGameMode)
{
	if (bGameMode) // 캐릭터 조작 화면
	{
		SetInputMode(GameInputMode);
		bShowMouseCursor = false;
	}
	else // UI 조작 화면
	{
		SetInputMode(UIInputMode);
		bShowMouseCursor = true;
	}
}

void AABPlayerController::ShowResultUI()
{
	auto ABGameState = Cast<AABGameState>(UGameplayStatics::GetGameState(this)); // 게임 스테이트 가져오기
	ABCHECK(nullptr != ABGameState);
	ResultWidget->BindGameState(ABGameState); // 게임 정보 건내주기
	// ※ UI의 NativeConstruct 함수는 AddToViewport()가 호출될 때 UI 위젯이 초기화되는 특징이 있으니 함수를 호출하기 전에 미리 게임스테이트의 정보를 읽어들이도록 바인딩 해준다.
	ResultWidget->AddToViewport(); // 뷰포트에 위젯 클래스를 띄움
	ChangeInputMode(false);
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ChangeInputMode(true);
	
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	
	HUDWidget = CreateWidget<UABHUDWidget>(this, HUDWidgetClass); // CreateWidget() 함수로 위젯을 인스턴스화한다.
	ABCHECK(nullptr != HUDWidget);
	HUDWidget->AddToViewport(1); // (zorder)는 겹치기 우선순위다

	ResultWidget = CreateWidget<UABGameplayResultWidget>(this, ResultWidgetClass);
	ABCHECK(nullptr != ResultWidget);

	ABPlayerState = Cast<AABPlayerState>(PlayerState); // HUD 위젯과 플레이어 스테이트(이름) 연결
	ABCHECK(nullptr != ABPlayerState);
	HUDWidget->BindPlayerState(ABPlayerState); // 플레이어 정보 바인딩
	ABPlayerState->OnPlayerStateChanged.Broadcast(); // 델리게이트 브로드캐스팅(호출)

}

void AABPlayerController::OnGamePause()
{
	MenuWidget = CreateWidget<UABGamplayWidget>(this, MenuWidgetClass); // 위젯 UI 생성
	ABCHECK(nullptr != MenuWidget);
	MenuWidget->AddToViewport(3);

	SetPause(true); // 일시 정지
	ChangeInputMode(false); // false : 입력을 UI에 받게하고 마우스 커서 활성화 
}

void AABPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction(TEXT("GamePause"), EInputEvent::IE_Pressed, this, &AABPlayerController::OnGamePause); // 키 바인딩 설정, 에디터에서 M키로 설정된 GamPause를 누르면 OnGamePause()를 호출하도록 델리게이트에 등록한다.
	
}


