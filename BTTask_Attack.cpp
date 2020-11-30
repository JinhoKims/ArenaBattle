// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "ABAIController.h"
#include "ABCharacter.h"

UBTTask_Attack::UBTTask_Attack()
{
    bNotifyTick = true; // ����� Tick Ȱ��ȭ
    IsAttacking = false;
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) // �½�ũ�� ���� ����Ǵ� 1ȸ�� ���ؼ� ȣ�� ��
{ 
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    auto ABCharacter = Cast<AABCharacter>(OwnerComp.GetAIOwner()->GetPawn()); // AI�� ���� ���� ��(ĳ����) ���� Ȯ�� 
    if (nullptr == ABCharacter)
        return EBTNodeResult::Failed;

    ABCharacter->Attack(); // �� �½�ũ�� ����� ��(ExecuteTask) ABCharacter(AI)�� Attack ���� ������ �����Ų��.
    IsAttacking = true; // �� �� true�� �ٲ��� �ִϸ��̼��� ����� ������ ��ٸ��� ������ �Ѵ�.
    ABCharacter->OnAttackEnd.AddLambda([this]()->void { // ABCharacter�� ��������Ʈ�� �ش� ���ٽ��� ����س��´�.
        IsAttacking = false; // �ش� ���ٽ��� ����Ǵ� ������ OnAttackMontageEnded(), �� ���� �ִϸ��̼��� ����Ǵ� �����̴�.
    });

    return EBTNodeResult::InProgress;  // �� �½�ũ�� �ִϸ��̼��� ���� ������ ����ϴ� ���� �½�ũ�̹Ƿ�, ó�� �½�ũ�� ����� ���� ExcuteTask()���� �ϴ� InProgress�� ��ȯ�ϰ� ����Ѵ�.
                                       // �� �� ExceuteTask�� ó�� �� ���� ����ǰ� ��� �� ƽ���� TickTask()�� ����ȴ�.
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) // ù �½�ũ(ExecuteTask)�� ���� �� �� ƽ���� ȣ��Ǹ�, �½�Ʈ�� ����� ��Ȳ�� �˻��Ѵ�.
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    if (!IsAttacking) // ABCharacter�� ��������Ʈ�� ȣ�⿡ ���� IsAttacking �� true ���� false�� �� ���
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded); // FinishLatentTask(Succeeded)�� ȣ���Ͽ� �½�ũ�� ���������� ����Ǿ��ٴ� �� �˷��ش�.
    }
}
