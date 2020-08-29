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

	auto ControllingPawn = Cast<AABCharacter>(Ownercomp.GetAIOwner()->GetPawn()); // ���� �����ϴ� AI Ȯ��
	if (nullptr == ControllingPawn)
		return false;

	auto Target = Cast<AABCharacter>(Ownercomp.GetBlackboardComponent()->GetValueAsObject(AABAIController::TargetKey)); // Ÿ�� (�÷��̾�) Ȯ��
	if (nullptr == Target)
		return false;

	bResult = (Target->GetDistanceTo(ControllingPawn) <= ControllingPawn->GetFinalAttackRange()); // �÷��̾���� �Ÿ��� �Ǽ� �����Ÿ� ������ �� ���� ��� ����
	return bResult;
}
