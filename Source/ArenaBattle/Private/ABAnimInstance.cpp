// Fill out your copyright notice in the Description page of Project Settings.


#include "ABAnimInstance.h"

UABAnimInstance::UABAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsInAir = false;
	IsDead = false;
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("AnimMontage'/Game/Book/Animations/SK_Mannequin_Skeleton_Montage.SK_Mannequin_Skeleton_Montage'")); // 스켈레톤 지정
	if (ATTACK_MONTAGE.Succeeded())
	{
		AttackMontage = ATTACK_MONTAGE.Object;
	}
}

void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	auto Pawn = TryGetPawnOwner(); // 애니메이션 시스템이 앞서 폰(로직)이 유효한지 폰 객체를 점검하는 명령어 (UE4의 순서 : 입력->로직->애니메이션)
	if (!::IsValid(Pawn)) return;
	if (!IsDead) // 죽지 않은상태면 애니메이션 반복
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
		auto Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			IsInAir = Character->GetMovementComponent()->IsFalling();
		}
	}

}

void UABAnimInstance::PlayAttackMontage() // 공격 애니메이션
{
	ABCHECK(!IsDead);
	Montage_Play(AttackMontage, 1.0f); // 애니메이션 실행
}

void UABAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	ABCHECK(!IsDead);
	ABCHECK(Montage_IsPlaying(AttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage); // 점프 공격 설정
}

void UABAnimInstance::AnimNotify_AttackHitCheck() // 에니메이션에서 (AttackHitCheck)노티파이가 발생하면 실행되는 함수
{ // 애니메이션 중 노티파이가 발생할 경우 현 함수가 호출되며, 함수 안에서 델리게이트를 호출한다.
	ABLOG_Long(Error, TEXT("Start Deleagate"));
	OnAttackHitCheck.Broadcast(); // 델리게이트에 등록한 함수 실행 : ABCharacter.cpp의 ABAnim->OnAttackHitCheck.AddUObject(this, &AABCharacter::AttackCheck);
}

void UABAnimInstance::AnimNotify_NextAttackCheck() // 에니메이션에서 (NextAttackCheck)노티파이가 발생하면 실행되는 함수
{
	OnNextAttackCheck.Broadcast(); // 델리게이트에 등록한 함수 실행 : ABAnim->OnNextAttackCheck.AddLambda([this]() -> ...
}

FName UABAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	ABCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"),Section));
}
