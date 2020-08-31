// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGamplayWidget.h"
#include "Components/Button.h"
#include "ABPlayerController.h"

void UABGamplayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ResumeButton = Cast<UButton>(GetWidgetFromName(TEXT("btnResume")));
	if (nullptr != ResumeButton)
	{
		ResumeButton->OnClicked.AddDynamic(this, &UABGamplayWidget::OnResumeClicked); // 버튼 클릭시 해당 함수 실행
	}

	ReturnToTitleButton = Cast<UButton>(GetWidgetFromName(TEXT("btnReturnToTitle")));
	if (nullptr != ReturnToTitleButton)
	{
		ReturnToTitleButton->OnClicked.AddDynamic(this, &UABGamplayWidget::OnReturnToTitleClicked);
	}

	RetryGameButton = Cast<UButton>(GetWidgetFromName(TEXT("btnRetryGame")));
	if (nullptr != RetryGameButton)
	{
		RetryGameButton->OnClicked.AddDynamic(this, &UABGamplayWidget::OnRetryGameClicked);
	}
}

void UABGamplayWidget::OnResumeClicked() // Resume 버튼을 누를 시
{
	auto ABPlayerController = Cast<AABPlayerController>(GetOwningPlayer()); // OwningPlayer() 함수를 사용해 플레이어 컨트롤러의 [정보]를 가져온다.
	ABCHECK(nullptr != ABPlayerController);

	RemoveFromParent(); // 현재 뷰포트에 띄워진 자신(UI)을 제거한다.
	ABPlayerController->ChangeInputMode(true); // 입력모드 정상화(캐릭터)
	ABPlayerController->SetPause(false); // 일시정지 해제
}

void UABGamplayWidget::OnReturnToTitleClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Title")); // 레벨 변경 -> 타이틀 화면
}

void UABGamplayWidget::OnRetryGameClicked()
{
	auto ABPlayerController = Cast<AABPlayerController>(GetOwningPlayer()); // 현재 플레이어 컨트롤러 정보
	ABCHECK(nullptr != ABPlayerController);
	ABPlayerController->RestartLevel(); // 레벨 재시작
}
