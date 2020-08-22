// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Animation/AnimInstance.h"
#include "ABAnimInstance.generated.h"

// 델리게이트 선언, 시그니처 지정
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
	virtual void NativeUpdateAnimation(float DeltaSeconds) override; // 매 틱마다 호출되는 애니메이션 함수
	void PlayAttackMontage();
	void JumpToAttackMontageSection(int32 NewSection);
	void SetDeadAnim() { IsDead = true; }

private:
	UFUNCTION()
	void AnimNotify_AttackHitCheck(); // ☆ 에니메이션 몽타주 BP에서 노티파이가 실행되면 호출되는 함수!!! ★

	UFUNCTION()
	void AnimNotify_NextAttackCheck();  // 함수 이름은 노티파이명과 동일해야 한다!

	FName GetAttackMontageSectionName(int32 Section);
};