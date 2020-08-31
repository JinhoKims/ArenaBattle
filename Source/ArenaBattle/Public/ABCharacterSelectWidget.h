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
	UFUNCTION(BlueprintCallable) // Callable : ȣ�� ����
	void NextCharacter(bool bForward = true); // �������Ʈ���� ����� �� �ֵ��� ��ũ�θ� ������

	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	int32 CurrentIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	int32 MaxIndex;
	
	TWeakObjectPtr<USkeletalMeshComponent> TargetComponent;

	UPROPERTY()
	class UButton * PrevButton; // ��ư ���ε�

	UPROPERTY()
	class UButton * NextButton;

	UPROPERTY()
	class UEditableTextBox * TextBox; // �ؽ�Ʈ�ڽ�

	UPROPERTY()
	class UButton * ConfirmButton; // Ȯ�� ��ư

private:
	UFUNCTION()
	void OnPrevClicked(); // ��ư Ŭ�� ��

	UFUNCTION()
	void OnNextClicked();

	UFUNCTION()
	void OnConfirmClicked();
};
