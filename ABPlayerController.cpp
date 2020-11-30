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
	static ConstructorHelpers::FClassFinder<UABHUDWidget> UI_HUD_C(TEXT("WidgetBlueprint'/Game/Book/UI/UI_HUD.UI_HUD_C'")); // UI�ּ��� ���۷����� ������
	if (UI_HUD_C.Succeeded())
	{
		HUDWidgetClass = UI_HUD_C.Class; // ���ҽ�(�ּ�)�� Ŭ������ ���ε� (�ν��Ͻ�ȭ �غ�)
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

void AABPlayerController::PostInitializeComponents() // ��Ʈ�ѷ� �ʱ�ȭ
{
	Super::PostInitializeComponents();
	ABLOG_Short(Warning);
}

void AABPlayerController::OnPossess(APawn * aPawn)
{
	ABLOG_Short(Warning); // ��Ʈ�ѷ� �� ����
	Super::OnPossess(aPawn);
}

UABHUDWidget* AABPlayerController::GetHUDWidget() const
{
	return HUDWidget;
}

void AABPlayerController::NPCKill(AABCharacter* KilledNPC) const
{
	ABPlayerState->AddExp(KilledNPC->GetExp()); // ����ġ ȹ��
}

void AABPlayerController::AddGameScore() const
{
	ABPlayerState->AddGameScore(); // ���� ���� ȹ��
}

void AABPlayerController::ChangeInputMode(bool bGameMode)
{
	if (bGameMode) // ĳ���� ���� ȭ��
	{
		SetInputMode(GameInputMode);
		bShowMouseCursor = false;
	}
	else // UI ���� ȭ��
	{
		SetInputMode(UIInputMode);
		bShowMouseCursor = true;
	}
}

void AABPlayerController::ShowResultUI()
{
	auto ABGameState = Cast<AABGameState>(UGameplayStatics::GetGameState(this)); // ���� ������Ʈ ��������
	ABCHECK(nullptr != ABGameState);
	ResultWidget->BindGameState(ABGameState); // ���� ���� �ǳ��ֱ�
	// �� UI�� NativeConstruct �Լ��� AddToViewport()�� ȣ��� �� UI ������ �ʱ�ȭ�Ǵ� Ư¡�� ������ �Լ��� ȣ���ϱ� ���� �̸� ���ӽ�����Ʈ�� ������ �о���̵��� ���ε� ���ش�.
	ResultWidget->AddToViewport(); // ����Ʈ�� ���� Ŭ������ ���
	ChangeInputMode(false);
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ChangeInputMode(true);
	
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	
	HUDWidget = CreateWidget<UABHUDWidget>(this, HUDWidgetClass); // CreateWidget() �Լ��� ������ �ν��Ͻ�ȭ�Ѵ�.
	ABCHECK(nullptr != HUDWidget);
	HUDWidget->AddToViewport(1); // (zorder)�� ��ġ�� �켱������

	ResultWidget = CreateWidget<UABGameplayResultWidget>(this, ResultWidgetClass);
	ABCHECK(nullptr != ResultWidget);

	ABPlayerState = Cast<AABPlayerState>(PlayerState); // HUD ������ �÷��̾� ������Ʈ(�̸�) ����
	ABCHECK(nullptr != ABPlayerState);
	HUDWidget->BindPlayerState(ABPlayerState); // �÷��̾� ���� ���ε�
	ABPlayerState->OnPlayerStateChanged.Broadcast(); // ��������Ʈ ��ε�ĳ����(ȣ��)

}

void AABPlayerController::OnGamePause()
{
	MenuWidget = CreateWidget<UABGamplayWidget>(this, MenuWidgetClass); // ���� UI ����
	ABCHECK(nullptr != MenuWidget);
	MenuWidget->AddToViewport(3);

	SetPause(true); // �Ͻ� ����
	ChangeInputMode(false); // false : �Է��� UI�� �ް��ϰ� ���콺 Ŀ�� Ȱ��ȭ 
}

void AABPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction(TEXT("GamePause"), EInputEvent::IE_Pressed, this, &AABPlayerController::OnGamePause); // Ű ���ε� ����, �����Ϳ��� MŰ�� ������ GamPause�� ������ OnGamePause()�� ȣ���ϵ��� ��������Ʈ�� ����Ѵ�.
	
}


