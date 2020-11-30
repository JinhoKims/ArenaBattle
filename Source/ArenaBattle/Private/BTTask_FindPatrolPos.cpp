// Fill out your copyright notice in the Description page of Project Settings.
#include "BTTask_FindPatrolPos.h"
#include "ABAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
	NodeName = TEXT("FindPatrolPos");
	
}


EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn(); // AI 컨트롤러가 빙의 중인 폰정보를 가져옴

	if (nullptr == ControllingPawn)
		return EBTNodeResult::Failed;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld()); // 현재 폰의 네비게이션 정보(위치)를 가져옴
	if (nullptr == NavSystem)
		return EBTNodeResult::Failed;

	FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(AABAIController::HomePosKey); // 오리진(출발점)은 HomePosKey 좌표
	FNavLocation NextPatrol; // 목적지 좌표값을 담고 있는 지역 변수

	if (NavSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, 500.0f, NextPatrol)) // Nexpatrol 변수에 랜덤 포인트 좌표 생성
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(AABAIController::PatrolPosKey, NextPatrol.Location); // PatrolPosKey<Key>에 NextPatrol<Value>값 기록
		return EBTNodeResult::Succeeded;
	}


	return EBTNodeResult::Failed;
}
