// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsInAttackRange.h"
#include "ABAIController.h"
#include "ABCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_IsInAttackRange::UBTDecorator_IsInAttackRange()
{
	NodeName = TEXT("CanAttack");
}

bool UBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& Ownercomp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(Ownercomp, NodeMemory);

	auto ControllingPawn = Cast<AABCharacter>(Ownercomp.GetAIOwner()->GetPawn()); // 폰을 제어하는 AI 확인
	if (nullptr == ControllingPawn)
		return false;

	auto Target = Cast<AABCharacter>(Ownercomp.GetBlackboardComponent()->GetValueAsObject(AABAIController::TargetKey)); // 타깃 (플레이어) 확인
	if (nullptr == Target)
		return false;

	bResult = (Target->GetDistanceTo(ControllingPawn) <= ControllingPawn->GetFinalAttackRange()); // 플레이어와의 거리가 맨손 사정거리 이하일 때 공격 모션 실행
	return bResult;
}
