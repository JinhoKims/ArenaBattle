// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TurnToTarget.h"
#include "ABAIController.h"
#include "ABCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
    NodeName = TEXT("Turn");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)  // 타겟(플레이어)에게 공격할 때 플레이어를 향해 회전하는 기능
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    auto ABCharacter = Cast<AABCharacter>(OwnerComp.GetAIOwner()->GetPawn()); // 캐릭터가 ai인지
    if (nullptr == ABCharacter)
        return EBTNodeResult::Failed;

    auto Target = Cast<AABCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AABAIController::TargetKey)); // 타겟이 일치(플레이어)하는지
    if (nullptr == ABCharacter)
        return EBTNodeResult::Failed;

    FVector LookVector = Target->GetActorLocation() - ABCharacter->GetActorLocation();  // 타겟의 좌표
    LookVector.Z = 0.0f;  // 고도 요소는 제외
    FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator(); // 타겟을 바라보는 방향
    ABCharacter->SetActorRotation(FMath::RInterpTo(ABCharacter->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 2.0f)); // 캐릭터 회전 보간 설정

    return EBTNodeResult::Succeeded;
}
