// Fill out your copyright notice in the Description page of Project Settings.

#include "ABCharacter.h"
#include "ABAnimInstance.h"
#include "ABWeapon.h"
#include "ABCharacterStatComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/WidgetComponent.h"
#include "ABCharacterWidget.h"
#include "ABAIController.h"
#include "ABCharacterSetting.h" // 스탯 데이터
#include "ABGameInstance.h" // 스탯 명령어
#include "ABPlayerController.h" 
#include "ABPlayerState.h"
#include "ABHUDWidget.h"
#include "ABGameMode.h"
#include "VisualLogger/VisualLogger.h"

// 초기화 및 프레임별 설정
AABCharacter::AABCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 컴포넌트 동적할당(new)
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	CharacterStat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("CHARACTERSTAT")); // 캐릭터 스탯 컴포먼트는 액터 컴포넌트로 구성되있다.
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET")); // UI 컴포넌트 부착

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);
	HPBarWidget->SetupAttachment(GetMesh());

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f)); // 캐릭터 위치 설정
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));
	if (SK_CARDBOARD.Succeeded()) // 캐릭터 스킨
	{
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);
	}
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/Animations/WarriorAnimBlueprint.WarriorAnimBlueprint_C"));
	if (WARRIOR_ANIM.Succeeded()) // 캐릭터 애니메이션
	{
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}
	SetControlMode(EControlMode::DIABLO); // 카메라(조작)
	ArmLengthSpeed = 3.0f;
	ArmRotationSpeed = 10.0f;
	GetCharacterMovement()->JumpZVelocity = 800.0f;
	IsAttacking = false;
	MaxCombo = 4;
	AttackEndComboState();
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCharacter"));
	AttackRange = 80.0f; // 맨손 사거리
	AttackRadius = 50.0f;

	HPBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(TEXT("WidgetBlueprint'/Game/Book/UI/UI_HPBar.UI_HPBar_C'"));

	if (UI_HUD.Succeeded()) // HUD 설정, ABCharacter에서는 HP바의 기능을 구현하기보다, 출력할 위치 설정과 드로우를 담당한다.
	{
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		HPBarWidget->SetDrawSize(FVector2D(150.0f, 50.0f));
	}

	DeadTimer = 1.0f;
	AIControllerClass = AABAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	// PREINIT 스테이트
	// 캐릭터는 최초로 PREINIT(생성자)에서 시작한다. 그러다가 게임이 시작돼 BeginPlay()가 호출되면, LOADING 스테이트로 넘어간다.
	AssetIndex = 4;
	SetActorHiddenInGame(true); // 액터
	HPBarWidget->SetHiddenInGame(true); // UI
	SetCanBeDamaged(false); // 데미지 판정
	// 캐릭터 생성 전의 스테이트 : 캐릭터와 UI를 숨겨두고, 데미지를 입지 않게 한다.

}

void AABCharacter::PostInitializeComponents() // 델리게이트 초기화 설정
{
	Super::PostInitializeComponents();
	ABAnim = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
	ABCHECK(nullptr != ABAnim);
	ABAnim->OnMontageEnded.AddDynamic(this, &AABCharacter::OnAttackMontageEnded); // OnMontageEnded 델리게이트는 애니메이션 몽타주에서 생성한 델리게이트

	ABAnim->OnNextAttackCheck.AddLambda([this]() -> void { // OnNextAttck 델리게이트에 등록한 함수, 연속 공격시 델리게이트가 호출함

		CanNextCombo = false;

		if (IsComboInputOn)
		{
			AttackStartComboState();
			ABAnim->JumpToAttackMontageSection(CurrentCombo);
		}
	});
	ABAnim->OnAttackHitCheck.AddUObject(this, &AABCharacter::AttackCheck); // 히트 판정 시 델리게이트가 호출할 함수

	CharacterStat->OnHPIsZero.AddLambda([this]()-> void { // hp가 0이 될 시 
		ABAnim->SetDeadAnim(); // isDead = true 죽는 모션
		SetActorEnableCollision(false); // 콜리전(물리) 비활성화
	}); // 델리게이트가 호출할 람다식

//	HPBarWidget->InitWidget(); ※ 4.25 버전 추가 사항 : 사용자 위젯이 초기화되었는 지 확인 필요!! UI 초기화는 BeginPlay()에서 호출되므로 미리 확인 필요, But HPBar 클래스의 멤버 함수에 등록된 브로드캐스트가 두 번 호출된다!
}

void AABCharacter::BeginPlay()
{
	Super::BeginPlay();
	// PREINIT 스테이트
	auto CharacterWidget = Cast<UABCharacterWidget>(HPBarWidget->GetUserWidgetObject());
	if (nullptr != CharacterWidget)
	{
	//	CharacterWidget->BindCharacterStat(CharacterStat); // ABCharacterWidget.cpp의 ABLOG_Long(Warning, TEXT("HPRatio : %f"), CurrentCharacterStat->GetHPRatio()); 로그를 쓰고싶을 때 주석하기!
	}
	else
	{
		ABLOG_Short(Error);
	}

	bIsPlayer = IsPlayerControlled(); // 캐릭터를 플레이어가 컨트롤 할 경우
	if (bIsPlayer)
	{
		ABPlayerController = Cast<AABPlayerController>(GetController()); // 플레이어 컨트롤러는 해당 캐릭터를 제어함
		ABCHECK(nullptr != ABPlayerController);
	}
	else
	{ // 아닐 경우
		ABAIController = Cast<AABAIController>(GetController()); // AI컨트롤러가 해당 캐릭터를 제어함
		ABCHECK(nullptr != ABAIController);
	}
	
	auto DefaultSetting = GetDefault<UABCharacterSetting>(); // 캐릭터 세팅(로드 부분)  ini파일을 불러옴
	if (bIsPlayer)
	{
		auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState()); // 이제 auto 변수는 ABPlayerState 객체가 된다.
		ABCHECK(nullptr != ABPlayerState);
		AssetIndex = ABPlayerState->GetCharacterIndex(); // 설정한 인덱스의 캐릭터 에셋(플레이어용 박스 워리어)
	}
	else
	{
		AssetIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1); // 랜덤으로 하나의 에셋을 골라 적용
	} // 에셋 로딩
	CharacterAssetToLoad = DefaultSetting->CharacterAssets[AssetIndex]; // 캐릭터 에셋 로드(랜덤 또는 4번 에셋)
	auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance()); // StreamableManager를 사용하기 위해 GameInstance 클래스 파일을 객체화(캐스팅)
	ABCHECK(nullptr != ABGameInstance);  // 비동기 방식으로 애셋을 로딩할 때 델리게이트(OnAssetLoadCompleted)를 호출하도록 등록, 캐스팅된 클래스에서 멤버함수를 호출하여 비동기 방식으로(ReqAsycLoad) 애셋을 로딩하도록 한다.
	AssetStreamingHandle = ABGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad, FStreamableDelegate::CreateUObject(this, &AABCharacter::OnAssetLoadCompleted));  // 데이터(에셋 경로)는 ABCharacterSetting에서, 명령어(비동기 애셋 로딩 로직)은 GameInstance에서 사용한다! 데이터와 명령어 구분함
	SetCharacterState(ECharacterState::LOADING); // LOADING 스테이트로 전이
}

void AABCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo, DeltaTime, ArmLengthSpeed);

	switch (CurrentControlMode) // 카메라가 벽에 부딪힐 시 시야처리
	{
	case AABCharacter::EControlMode::GTA:
		break;
	case AABCharacter::EControlMode::DIABLO:
		SpringArm->SetRelativeRotation(FMath::RInterpTo(SpringArm->GetRelativeRotation(), ArmRotationTo, DeltaTime, ArmRotationSpeed));
		break;
	default:
		break;
	}

	switch (CurrentControlMode) // 게임모드별 카메라 시야처리
	{
	case AABCharacter::EControlMode::GTA:
		break;
	case AABCharacter::EControlMode::DIABLO:
		if (DirectionToMove.SizeSquared() > 0.0f)
		{
			GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
			AddMovementInput(DirectionToMove);
		}
		break;
	default:
		break;
	}


}


// 이동 및 카메라 시스템
void AABCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) // 입력장치 설정
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("ViewChange"), EInputEvent::IE_Pressed, this, &AABCharacter::ViewChange);
	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AABCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AABCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AABCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AABCharacter::Turn);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AABCharacter::Attack);
}

void AABCharacter::SetControlMode(EControlMode NewControlMode) // 모드별 설정
{
	CurrentControlMode = NewControlMode;
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		ArmLengthTo = 450.0f;
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;
		SpringArm->bDoCollisionTest = true;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
		break;
	case EControlMode::DIABLO:
		ArmLengthTo = 800.0f;
		ArmRotationTo = FRotator(-45.0f, 0.0f, 0.0f);
		SpringArm->bUsePawnControlRotation = false;
		SpringArm->bInheritPitch = false;
		SpringArm->bInheritRoll = false;
		SpringArm->bInheritYaw = false;
		SpringArm->bDoCollisionTest = false;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
		break;
	case EControlMode::NPC:
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 480.0f, 0.0f);
		break;
	}
}

void AABCharacter::UpDown(float NewAxisValue) // 캐릭터 상하
{
	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::X), NewAxisValue);
		break;
	case AABCharacter::EControlMode::DIABLO:
		DirectionToMove.X = NewAxisValue;
		break;
	default:
		break;
	}
}

void AABCharacter::LeftRight(float NewAxisValue) // 캐릭터 좌우
{
	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::Y), NewAxisValue);
		break;
	case AABCharacter::EControlMode::DIABLO:
		DirectionToMove.Y = NewAxisValue;
		break;
	default:
		break;
	}
}

void AABCharacter::LookUp(float NewAxisValue) // 카메라 상하 회전
{
	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		AddControllerPitchInput(NewAxisValue);
		break;
	case AABCharacter::EControlMode::DIABLO: // 쿼터뷰는 위 아래 회전이 필요없다.
		break;
	default:
		break;
	}
}

void AABCharacter::Turn(float NewAxisValue) // 카메라 좌우 회전
{
	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		AddControllerYawInput(NewAxisValue);
		break;
	case AABCharacter::EControlMode::DIABLO:
		break;
	default:
		break;
	}
}

void AABCharacter::ViewChange() // 카메라 모드 토글
{
		switch (CurrentControlMode)
		{
		case AABCharacter::EControlMode::GTA:
			GetController()->SetControlRotation(GetActorRotation());
			SetControlMode(EControlMode::DIABLO);
			break;
		case AABCharacter::EControlMode::DIABLO:
			GetController()->SetControlRotation(SpringArm->GetRelativeRotation());
			SetControlMode(EControlMode::GTA);
			break;
		default:
			break;
		}
}


// 전투 시스템 
bool AABCharacter::CanSetWeapon()
{
	return true;
}

void AABCharacter::PossessedBy(AController* NewController) // 캐릭터의 빙의자가 누구인가(플레이어,npc), 기본 카메라 타입과 이동속도를 설정 
{
	Super::PossessedBy(NewController);
	if (IsPlayerControlled()) // 플레이어가 컨트롤할 경우
	{
		SetControlMode(EControlMode::DIABLO);
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}
	else // AI가 컨트롤 할 경우
	{
		SetControlMode(EControlMode::NPC);
		GetCharacterMovement()->MaxWalkSpeed = 300.0f; // 플레이어보다 좀 느리게
	}
}

void AABCharacter::SetWeapon(AABWeapon* NewWeapon) // 무기 장착 시
{
	ABCHECK(nullptr != NewWeapon);
	if (nullptr != CurrentWeapon) // 이미 무기를 장착하고 있을경우
	{
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform); // 무기 분리
		CurrentWeapon->Destroy(); // 무기 파괴
		CurrentWeapon = nullptr; // null 할당
	}

	FName WeaponSocket(TEXT("hand_rSocket")); // 새로 무기 장착
	if (nullptr != NewWeapon)
	{
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		NewWeapon->SetOwner(this);
		CurrentWeapon = NewWeapon;
	}
}

void AABCharacter::Attack() // 공격
{
	if (IsAttacking) // 이미 공격 중일시 콤보 지속
	{
		ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));
		if (CanNextCombo)
		{
			IsComboInputOn = true;
		}
	}
	else // 첫 공격 시 콤보 시작
	{
			ABCHECK(CurrentCombo == 0);
			AttackStartComboState();
			ABAnim->PlayAttackMontage();
			ABAnim->JumpToAttackMontageSection(CurrentCombo);
			IsAttacking = true;
	}
}

void AABCharacter::SetCharacterState(ECharacterState NewState)
{
	ABCHECK(CurrentState != NewState);
	CurrentState = NewState;

	switch (CurrentState)
	{
	case ECharacterState::LOADING: // LOADING 스테이트 설정 
	{
		if (bIsPlayer)
		{
			DisableInput(ABPlayerController); // 로딩 중 입력 비활성화
			ABPlayerController->GetHUDWidget()->BindCharacterStat(CharacterStat); // HUD 위젯과 캐릭터 스탯을연결
			auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState()); // 플레이어 스탯 캐스팅
			ABCHECK(nullptr != ABPlayerState);
			CharacterStat->SetNewLevel(ABPlayerState->GetCharacterLevel()); // 플레이어 레벨 세팅 (5레벨)
		}
		else
		{
			auto ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode()); // 게임 실행 중에 게임 모드의 포인터를 가져올 때는 GetAuthGameMode() 함수를 사용한다. (중요한 데이터를 인증하는 권한을 가진다)
			ABCHECK(nullptr != ABGameMode);
			int32 TargetLevel = FMath::CeilToInt(((float)ABGameMode->GetScore() * 0.8f)); // 불러올 레벨 설정
			int32 FinalLevel = FMath::Clamp<int32>(TargetLevel, 1, 20); // 최대 레벨 제한설정
			ABLOG_Long(Warning, TEXT("Now New NPC Level Available is %d"), FinalLevel);
			CharacterStat->SetNewLevel(FinalLevel); // NPC 레벨 세팅
		}

		SetActorHiddenInGame(true);
		HPBarWidget->SetHiddenInGame(true);
		SetCanBeDamaged(false);
	}
	break;
	case ECharacterState::READY: // READY 스테이트 = 액터표시, UI, 데미지처리 표시
	{
		SetActorHiddenInGame(false);
		HPBarWidget->SetHiddenInGame(false);
		SetCanBeDamaged(true);
		CharacterStat->OnHPIsZero.AddLambda([this]()->void {
			SetCharacterState(ECharacterState::DEAD); // HP가 Zero면 DEAD 스테이트로 상태 전이되게 델리게이트에 등록
		});
		HPBarWidget->InitWidget();
		auto CharacterWidget = Cast<UABCharacterWidget>(HPBarWidget->GetUserWidgetObject());
		ABCHECK(nullptr != CharacterWidget);
		CharacterWidget->BindCharacterStat(CharacterStat);

		if (bIsPlayer) // 플레이어일 경우 카메라, 이동속도 할당
		{
			SetControlMode(EControlMode::DIABLO);
			GetCharacterMovement()->MaxWalkSpeed = 600.0f;
			EnableInput(ABPlayerController);
		}
		else
		{
			SetControlMode(EControlMode::NPC);
			GetCharacterMovement()->MaxWalkSpeed = 400.0f;
			ABAIController->RunAI(); // NPC일 경우 AI 트리를 실행시킨다.
		}
	}
	break;
	case ECharacterState::DEAD:
	{
		SetActorEnableCollision(false); // 물리 비중 제거
		GetMesh()->SetHiddenInGame(false);
		HPBarWidget->SetHiddenInGame(true);
		ABAnim->SetDeadAnim(); // 죽음 표시
		SetCanBeDamaged(false);

		if (bIsPlayer) // 죽은 캐릭터가 플레이어일 경우
		{
			DisableInput(ABPlayerController); // 작동 권한 해제
		}
		else
		{
			ABAIController->StopAI(); // NPC일 경우 해동 트리를 중단시킨다.
		} 
		GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]()->void { // 그리고 일정시간 후-
			if (bIsPlayer) // 캐릭터가 죽을 경우 일정 타이머(1.0f) 후에 람다로 등록된 델리게이트 호출 SetTimer()
			{
				ABPlayerController->ShowResultUI(); // 결과 화면 출력
			}
			else
			{
				Destroy(); // NPC는 삭제
			}
		}), DeadTimer, false); // DeadTimer가 일정시간이며, 5.0f로 설정되었다.
	}
		break;
	default:
		break;
	}
}

ECharacterState AABCharacter::GetCharacterState() const
{
	return ECharacterState();
}

int32 AABCharacter::GetExp() const
{
	return CharacterStat->GetDropExp();
}

float AABCharacter::GetFinalAttackRange() const
{
	return (nullptr != CurrentWeapon) ? CurrentWeapon->GetAttackRange() : AttackRange; // 무기를 들고 있으면 무기 사거리로 반환
}

float AABCharacter::GetFinalAttackDamage() const
{
	float AttackDamage = (nullptr != CurrentWeapon) ? (CharacterStat->GetAttack() + CurrentWeapon->GetAttackDamage()) : CharacterStat->GetAttack();
	float AttackModifier = (nullptr != CurrentWeapon) ? CurrentWeapon->GetAttackModifier() : 1.0f; // 맨손이면 가중치 1.0 고정

	return AttackDamage * AttackModifier;
}

void AABCharacter::AttackCheck() // 데미지 체크
{
	float FinalAttackRange = GetFinalAttackRange(); // 무기 사거리
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this); // Parm 설정 
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * FinalAttackRange, // 공격 사거리 설정
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(50.0f),
		Params);

#if ENABLE_DRAW_DEBUG // 로그 기능
	FVector TraceVec = GetActorForwardVector() * FinalAttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = FinalAttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 5.0f;

	DrawDebugCapsule(GetWorld(), // 뷰포트에 로그 출력
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime);

	UE_VLOG_LOCATION(this, ArenaBattle, Verbose, GetActorLocation(), 50.0f, FColor::Blue, TEXT("Attack Position")); // 공격한 캐릭터의 위치를 비주얼 로거로 출력
	UE_VLOG_CAPSULE(this, ArenaBattle, Verbose, GetActorLocation() - GetActorForwardVector() * AttackRadius, HalfHeight, AttackRadius, CapsuleRot, DrawColor, TEXT("Attack Area")); // 공격 판정 위치를 비주얼 로거로 출력
#endif

	if (bResult)
	{
		if (HitResult.Actor.IsValid())
		{
			ABLOG_Long(Warning, TEXT("Hit Actor Name %s"), *HitResult.Actor->GetName());
			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(GetFinalAttackDamage(), DamageEvent, GetController(), this);
		}
	}
}

void AABCharacter::OnAssetLoadCompleted()
{
	AssetStreamingHandle->ReleaseHandle();
	TSoftObjectPtr<USkeletalMesh> LoadAssetPath(CharacterAssetToLoad); // 애셋의 경로 정보에 해당되는 스태틱 메시 입히기
	ABCHECK(LoadAssetPath.IsValid());
	GetMesh()->SetSkeletalMesh(LoadAssetPath.Get());
	SetCharacterState(ECharacterState::READY); // 로드가 끝나면 READY로 전이
	
}

void AABCharacter::AttackStartComboState() // 콤보
{
	CanNextCombo = true;
	IsComboInputOn = false;
	ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void AABCharacter::AttackEndComboState() // 콤보 마지막
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

void AABCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted) // 공격 애니메이션이 끝났으니 IsAttacking 변수를 false로 변경
{
	ABCHECK(IsAttacking);
	ABCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();
	OnAttackEnd.Broadcast();
}

float AABCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) // 캐릭터가 받은 피해 계산
{																						   // ※ Instigator = 가해자 (때린 캐릭터)
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (EventInstigator->IsPlayerController())
		ABLOG_Long(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);

	CharacterStat->SetDamage(FinalDamage);
	if (CurrentState == ECharacterState::DEAD)
	{
		if (EventInstigator->IsPlayerController()) // 가해자 (때리는 자)가 플레이어일 경우, 맞는 자가 AI일 경우
		{ 
			auto ABPlayerControllerIsPlayer = Cast<AABPlayerController>(EventInstigator);
			ABCHECK(nullptr != ABPlayerControllerIsPlayer, 0.0f);
			ABPlayerControllerIsPlayer->NPCKill(this); // ABPlayerControllerIsPlayer(플레이어)는 NPCKill을 호출하여 현재 캐릭터의 DropExp(사냥 경험치)를 플레이어에게 전송
		}
	}
	return FinalDamage;
}