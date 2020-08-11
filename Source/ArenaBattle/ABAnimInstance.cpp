// Fill out your copyright notice in the Description page of Project Settings.


#include "ABAnimInstance.h"

UABAnimInstance::UABAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsInAir = false;
	IsDead = false;
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("AnimMontage'/Game/Book/Animations/SK_Mannequin_Skeleton_Montage.SK_Mannequin_Skeleton_Montage'")); // ���̷��� ����
	if (ATTACK_MONTAGE.Succeeded())
	{
		AttackMontage = ATTACK_MONTAGE.Object;
	}
}

void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	auto Pawn = TryGetPawnOwner(); // �ִϸ��̼� �ý����� �ռ� ��(����)�� ��ȿ���� �� ��ü�� �����ϴ� ��ɾ� (UE4�� ���� : �Է�->����->�ִϸ��̼�)
	if (!::IsValid(Pawn)) return;
	if (!IsDead) // ���� �������¸� �ִϸ��̼� �ݺ�
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
		auto Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			IsInAir = Character->GetMovementComponent()->IsFalling();
		}
	}

}

void UABAnimInstance::PlayAttackMontage() // ���� �ִϸ��̼�
{
	ABCHECK(!IsDead);
	Montage_Play(AttackMontage, 1.0f); // �ִϸ��̼� ����
}

void UABAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	ABCHECK(!IsDead);
	ABCHECK(Montage_IsPlaying(AttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage); // ���� ���� ����
}

void UABAnimInstance::AnimNotify_AttackHitCheck() // ���ϸ��̼ǿ��� (AttackHitCheck)��Ƽ���̰� �߻��ϸ� ����Ǵ� �Լ�
{ // �ִϸ��̼� �� ��Ƽ���̰� �߻��� ��� �� �Լ��� ȣ��Ǹ�, �Լ� �ȿ��� ��������Ʈ�� ȣ���Ѵ�.
	ABLOG_Long(Error, TEXT("Start Deleagate"));
	OnAttackHitCheck.Broadcast(); // ��������Ʈ�� ����� �Լ� ���� : ABCharacter.cpp�� ABAnim->OnAttackHitCheck.AddUObject(this, &AABCharacter::AttackCheck);
}

void UABAnimInstance::AnimNotify_NextAttackCheck() // ���ϸ��̼ǿ��� (NextAttackCheck)��Ƽ���̰� �߻��ϸ� ����Ǵ� �Լ�
{
	OnNextAttackCheck.Broadcast(); // ��������Ʈ�� ����� �Լ� ���� : ABAnim->OnNextAttackCheck.AddLambda([this]() -> ...
}

FName UABAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	ABCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"),Section));
}
