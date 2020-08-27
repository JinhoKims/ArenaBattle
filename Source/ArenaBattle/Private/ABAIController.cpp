// Fill out your copyright notice in the Description page of Project Settings.


#include "ABAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName AABAIController::HomePosKey(TEXT("HomePos"));
const FName AABAIController::PatrolPosKey(TEXT("PatrolPos"));
const FName AABAIController::TargetKey(TEXT("Target"));

AABAIController::AABAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("BlackboardData'/Game/Book/AI/BB_ABCharacter.BB_ABCharacter'"));
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("BehaviorTree'/Game/Book/AI/BT_ABCharacter.BT_ABCharacter'"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}
}

void AABAIController::OnPossess(APawn* InPawn) // 폰이 빙의하는 시점
{
	Super::OnPossess(InPawn);
	/*	if (UseBlackboard(BBAsset, Blackboard))
		{
			Blackboard->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());
			if (!RunBehaviorTree(BTAsset))
			{
				ABLOG_Long(Error, TEXT("AIController couldn't run behavior tree!"));
			}
		}
	*/
}

void AABAIController::RunAI() // AI 실행시
{
	if (UseBlackboard(BBAsset, Blackboard)) // 블랙보드를 쓰고있다면
	{
		Blackboard->SetValueAsVector(HomePosKey, GetPawn()->GetActorLocation()); // NPC가 생성된 현재 위치를 HomePosKey로 지정
		if (!RunBehaviorTree(BTAsset))
		{
			ABLOG_Long(Error, TEXT("AIController couldn't run behavior tree!"));
		}
	}
}

void AABAIController::StopAI() // AI 중단시
{
	auto BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent); // 비헤비어 트리 캐스팅
	if (nullptr != BehaviorTreeComponent)
	{
		BehaviorTreeComponent->StopTree(EBTStopMode::Safe); // 해동 트리 순회를 중지한다.
	}
}
