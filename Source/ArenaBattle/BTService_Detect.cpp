// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Detect.h"
#include "ABAIController.h"
#include "ABCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect"); // BTNode의 속성
	Interval = 1.0f; // 호출 주기
}


void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn(); // 제어할 폰 받아오기
	if (nullptr == ControllingPawn) return;

	UWorld* World = ControllingPawn->GetWorld();
	FVector Center = ControllingPawn->GetActorLocation();
	float DetectRadius = 600.0f;

	if (nullptr == World) return;
	TArray<FOverlapResult> OverlapResults; // 캐릭터 스캔 그물(영역)
	FCollisionQueryParams CollisionQueryParm(NAME_None, false, ControllingPawn);
	bool bResult = World->OverlapMultiByChannel( 
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParm
	);

	if (bResult)
	{
		for (auto OverlapResult : OverlapResults) // 영역에 들어온 캐릭터를 스캔 
		{
			AABCharacter* ABCharacter = Cast<AABCharacter>(OverlapResult.GetActor());  // character로 캐스팅
			if (ABCharacter && ABCharacter->GetController()->IsPlayerController()) // 그 중에 플레이어 캐릭터를 발견하면~
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, ABCharacter); // 타겟키를 플레이어로 지정
				DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f); // 확인용 초록색 원 생성
				return; // 반복문 종료
			}
			else // 플레이어 캐릭터를 못 발견하면 
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, nullptr); // 타겟키를 nullptr로 설정
			}
		}


	}


	DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f); // Sphere(구) 그리기
}
