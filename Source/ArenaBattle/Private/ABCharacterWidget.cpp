// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterWidget.h"
#include "ABCharacterStatComponent.h"
#include "Components/ProgressBar.h"

void UABCharacterWidget::BindCharacterStat(UABCharacterStatComponent* NewCharacterStat)
{
	ABCHECK(nullptr != NewCharacterStat);

	CurrentCharacterStat = NewCharacterStat;

	NewCharacterStat->OnHPChanged.AddLambda([this]()->void { // hp가 변경될 때마다 람다식 호출
		if (CurrentCharacterStat.IsValid()) {
		//	ABLOG_Long(Warning, TEXT("HPRatio : %f"), CurrentCharacterStat->GetHPRatio());
		}
	});

	NewCharacterStat->OnHPChanged.AddUObject(this, &UABCharacterWidget::UpdateHPWidget); // hp가 변경될 때마다 Update 함수 호출
}

void UABCharacterWidget::NativeConstruct()
{
	Super::NativeConstruct();
	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));
	ABCHECK(nullptr != HPProgressBar);
	UpdateHPWidget(); // 최초로 UI 초기화 후 위젯 업데이트(초기값 상태의 체력바)
}

void UABCharacterWidget::UpdateHPWidget()
{
	if (CurrentCharacterStat.IsValid())
	{
		if (nullptr != HPProgressBar)
		{
			HPProgressBar->SetPercent(CurrentCharacterStat->GetHPRatio()); // 체력바의 상태 업데이트
		}
	}
}
