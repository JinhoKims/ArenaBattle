// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "ABAIController.h"
#include "ABCharacter.h"

UBTTask_Attack::UBTTask_Attack()
{
    bNotifyTick = true; // 노드의 Tick 활성화
    IsAttacking = false;
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) // 태스크가 최초 실행되는 1회에 한해서 호출 됨
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    auto ABCharacter = Cast<AABCharacter>(OwnerComp.GetAIOwner()->GetPawn());
    if (nullptr == ABCharacter)
        return EBTNodeResult::Failed; // 플레이어 확보에 실패하면 Failed를 반환하고 종료
    
    ABCharacter->Attack(); // 태스크가 실행되면 ABCharacter의 attack 공격 로직을 실행시킨다.
    IsAttacking = true; // 그 후 true로 바껴서 애니메이션이 종료될 때까지 기다리는 역할을 한다.
    ABCharacter->OnAttackEnd.AddLambda([this]()->void { // ABCharacter의 델리게이트에 등록
        IsAttacking = false; // 델리게이트가 브로드캐스트되는 시점은 OnAttackMontageEnded() 즉 공격 애니메이션이 종료되는 시점이다.
    });

    return EBTNodeResult::InProgress;  // 공격테스크는 애니메이션이 끝날 때까지 대기해야하는 지연 태스크이므로 일단 InPogress을 반환하면서 대기한다.
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) // 태스크를 실행 후 매 틱마다 호출 되며, 태스트가 종료될 시점을 검사한다.
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    if (!IsAttacking) // ABCharacter의 델리게이트에 호출에 의해 IsAttacking 이 true 에서 false가 될 경우
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded); // FinishLatentTask()를 호출하여 태스크가 성공적으로 종료되었다는 걸 알려준다.
    }
}
