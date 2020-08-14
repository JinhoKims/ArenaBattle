// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TurnToTarget.h"
#include "ABAIController.h"
#include "ABCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
    NodeName = TEXT("Turn");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)  // Ÿ��(�÷��̾�)���� ������ �� �÷��̾ ���� ȸ���ϴ� ���
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    auto ABCharacter = Cast<AABCharacter>(OwnerComp.GetAIOwner()->GetPawn()); // ĳ���Ͱ� ai����
    if (nullptr == ABCharacter)
        return EBTNodeResult::Failed;

    auto Target = Cast<AABCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AABAIController::TargetKey)); // Ÿ���� ��ġ(�÷��̾�)�ϴ���
    if (nullptr == ABCharacter)
        return EBTNodeResult::Failed;

    FVector LookVector = Target->GetActorLocation() - ABCharacter->GetActorLocation();  // Ÿ���� ��ǥ
    LookVector.Z = 0.0f;  // �� ��Ҵ� ����
    FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator(); // Ÿ���� �ٶ󺸴� ����
    ABCharacter->SetActorRotation(FMath::RInterpTo(ABCharacter->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 2.0f)); // ĳ���� ȸ�� ���� ����

    return EBTNodeResult::Succeeded;
}
