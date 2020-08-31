// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Blueprint/UserWidget.h"
#include "ABCharacterSelectWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABCharacterSelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintCallable) // Callable : 호출 가능
	void NextCharacter(bool bForward = true); // 블루프린트에서 사용할 수 있도록 매크로를 설정함

	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	int32 CurrentIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	int32 MaxIndex;
	
	TWeakObjectPtr<USkeletalMeshComponent> TargetComponent;

	UPROPERTY()
	class UButton * PrevButton; // 버튼 바인딩

	UPROPERTY()
	class UButton * NextButton;

	UPROPERTY()
	class UEditableTextBox * TextBox; // 텍스트박스

	UPROPERTY()
	class UButton * ConfirmButton; // 확인 버튼

private:
	UFUNCTION()
	void OnPrevClicked(); // 버튼 클릭 시

	UFUNCTION()
	void OnNextClicked();

	UFUNCTION()
	void OnConfirmClicked();
};
