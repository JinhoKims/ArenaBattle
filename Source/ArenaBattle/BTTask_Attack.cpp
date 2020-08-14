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

    auto ABCharacter = Cast<AABCharacter>(OwnerComp.GetAIOwner()->GetPawn());
    if (nullptr == ABCharacter)
        return EBTNodeResult::Failed; // �÷��̾� Ȯ���� �����ϸ� Failed�� ��ȯ�ϰ� ����
    
    ABCharacter->Attack(); // �½�ũ�� ����Ǹ� ABCharacter�� attack ���� ������ �����Ų��.
    IsAttacking = true; // �� �� true�� �ٲ��� �ִϸ��̼��� ����� ������ ��ٸ��� ������ �Ѵ�.
    ABCharacter->OnAttackEnd.AddLambda([this]()->void { // ABCharacter�� ��������Ʈ�� ���
        IsAttacking = false; // ��������Ʈ�� ��ε�ĳ��Ʈ�Ǵ� ������ OnAttackMontageEnded() �� ���� �ִϸ��̼��� ����Ǵ� �����̴�.
    });

    return EBTNodeResult::InProgress;  // �����׽�ũ�� �ִϸ��̼��� ���� ������ ����ؾ��ϴ� ���� �½�ũ�̹Ƿ� �ϴ� InPogress�� ��ȯ�ϸ鼭 ����Ѵ�.
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) // �½�ũ�� ���� �� �� ƽ���� ȣ�� �Ǹ�, �½�Ʈ�� ����� ������ �˻��Ѵ�.
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    if (!IsAttacking) // ABCharacter�� ��������Ʈ�� ȣ�⿡ ���� IsAttacking �� true ���� false�� �� ���
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded); // FinishLatentTask()�� ȣ���Ͽ� �½�ũ�� ���������� ����Ǿ��ٴ� �� �˷��ش�.
    }
}
