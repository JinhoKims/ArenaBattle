// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Detect.h"
#include "ABAIController.h"
#include "ABCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect"); // BTNode�� �Ӽ�
	Interval = 1.0f; // ȣ�� �ֱ�
}


void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn(); // ������ �� �޾ƿ���
	if (nullptr == ControllingPawn) return;

	UWorld* World = ControllingPawn->GetWorld();
	FVector Center = ControllingPawn->GetActorLocation();
	float DetectRadius = 600.0f;

	if (nullptr == World) return;
	TArray<FOverlapResult> OverlapResults; // ĳ���� ��ĵ �׹�(����)
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
		for (auto OverlapResult : OverlapResults) // ������ ���� ĳ���͸� ��ĵ 
		{
			AABCharacter* ABCharacter = Cast<AABCharacter>(OverlapResult.GetActor());  // character�� ĳ����
			if (ABCharacter && ABCharacter->GetController()->IsPlayerController()) // �� �߿� �÷��̾� ĳ���͸� �߰��ϸ�~
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, ABCharacter); // Ÿ��Ű�� �÷��̾�� ����
				DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f); // Ȯ�ο� �ʷϻ� �� ����
				return; // �ݺ��� ����
			}
			else // �÷��̾� ĳ���͸� �� �߰��ϸ� 
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, nullptr); // Ÿ��Ű�� nullptr�� ����
			}
		}


	}


	DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f); // Sphere(��) �׸���
}
