// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Animation/AnimInstance.h"
#include "ABAnimInstance.generated.h"

// ��������Ʈ ����, �ñ״�ó ����
DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);

UCLASS()
class ARENABATTLE_API UABAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
								
public:
	FOnNextAttackCheckDelegate OnNextAttackCheck; 
	FOnAttackHitCheckDelegate OnAttackHitCheck;

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true));
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsDead;


public:
	UABAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override; // �� ƽ���� ȣ��Ǵ� �ִϸ��̼� �Լ�
	void PlayAttackMontage();
	void JumpToAttackMontageSection(int32 NewSection);
	void SetDeadAnim() { IsDead = true; }

private:
	UFUNCTION()
	void AnimNotify_AttackHitCheck(); // �� ���ϸ��̼� ��Ÿ�� BP���� ��Ƽ���̰� ����Ǹ� ȣ��Ǵ� �Լ�!!! ��

	UFUNCTION()
	void AnimNotify_NextAttackCheck();  // �Լ� �̸��� ��Ƽ���̸�� �����ؾ� �Ѵ�!

	FName GetAttackMontageSectionName(int32 Section);
};