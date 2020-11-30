// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TurnToTarget.h"
#include "ABAIController.h"
#include "ABCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
    NodeName = TEXT("Turn");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)  // 블랙보드의 Target(플레이어)에게 공격할 때 Target을 향해 회전하는 태스크
{                                                                                                               
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    auto ABCharacter = Cast<AABCharacter>(OwnerComp.GetAIOwner()->GetPawn()); // AI의 폰을 얻어옴 
    if (nullptr == ABCharacter)
        return EBTNodeResult::Failed;

    auto Target = Cast<AABCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AABAIController::TargetKey)); // 타겟(플레이어)를 얻어옴
    if (nullptr == ABCharacter)
        return EBTNodeResult::Failed;

    FVector LookVector = Target->GetActorLocation() - ABCharacter->GetActorLocation();  // 타겟과의 상대좌표
    LookVector.Z = 0.0f;  // 고도 요소는 제외
    FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator(); // 타겟을 바라보는 방향
    ABCharacter->SetActorRotation(FMath::RInterpTo(ABCharacter->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 2.0f)); // AI캐릭터 회전 보간 설정

    return EBTNodeResult::Succeeded;
}
