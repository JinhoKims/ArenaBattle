// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterWidget.h"
#include "ABCharacterStatComponent.h"
#include "Components/ProgressBar.h"

void UABCharacterWidget::BindCharacterStat(UABCharacterStatComponent* NewCharacterStat)
{
	ABCHECK(nullptr != NewCharacterStat);

	CurrentCharacterStat = NewCharacterStat;

	NewCharacterStat->OnHPChanged.AddLambda([this]()->void { // hp�� ����� ������ ���ٽ� ȣ��
		if (CurrentCharacterStat.IsValid()) {
		//	ABLOG_Long(Warning, TEXT("HPRatio : %f"), CurrentCharacterStat->GetHPRatio());
		}
	});

	NewCharacterStat->OnHPChanged.AddUObject(this, &UABCharacterWidget::UpdateHPWidget); // hp�� ����� ������ Update �Լ� ȣ��
}

void UABCharacterWidget::NativeConstruct()
{
	Super::NativeConstruct();
	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));
	ABCHECK(nullptr != HPProgressBar);
	UpdateHPWidget(); // ���ʷ� UI �ʱ�ȭ �� ���� ������Ʈ(�ʱⰪ ������ ü�¹�)
}

void UABCharacterWidget::UpdateHPWidget()
{
	if (CurrentCharacterStat.IsValid())
	{
		if (nullptr != HPProgressBar)
		{
			HPProgressBar->SetPercent(CurrentCharacterStat->GetHPRatio()); // ü�¹��� ���� ������Ʈ
		}
	}
}
