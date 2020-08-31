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
		ResumeButton->OnClicked.AddDynamic(this, &UABGamplayWidget::OnResumeClicked); // ��ư Ŭ���� �ش� �Լ� ����
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

void UABGamplayWidget::OnResumeClicked() // Resume ��ư�� ���� ��
{
	auto ABPlayerController = Cast<AABPlayerController>(GetOwningPlayer()); // OwningPlayer() �Լ��� ����� �÷��̾� ��Ʈ�ѷ��� [����]�� �����´�.
	ABCHECK(nullptr != ABPlayerController);

	RemoveFromParent(); // ���� ����Ʈ�� ����� �ڽ�(UI)�� �����Ѵ�.
	ABPlayerController->ChangeInputMode(true); // �Է¸�� ����ȭ(ĳ����)
	ABPlayerController->SetPause(false); // �Ͻ����� ����
}

void UABGamplayWidget::OnReturnToTitleClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Title")); // ���� ���� -> Ÿ��Ʋ ȭ��
}

void UABGamplayWidget::OnRetryGameClicked()
{
	auto ABPlayerController = Cast<AABPlayerController>(GetOwningPlayer()); // ���� �÷��̾� ��Ʈ�ѷ� ����
	ABCHECK(nullptr != ABPlayerController);
	ABPlayerController->RestartLevel(); // ���� �����
}
