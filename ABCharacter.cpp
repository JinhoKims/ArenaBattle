// Fill out your copyright notice in the Description page of Project Settings.

#include "ABCharacter.h"
#include "ABAnimInstance.h"
#include "ABWeapon.h"
#include "ABCharacterStatComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/WidgetComponent.h"
#include "ABCharacterWidget.h"
#include "ABAIController.h"
#include "ABCharacterSetting.h" // ���� ������
#include "ABGameInstance.h" // ���� ��ɾ�
#include "ABPlayerController.h" 
#include "ABPlayerState.h"
#include "ABHUDWidget.h"
#include "ABGameMode.h"
#include "VisualLogger/VisualLogger.h"

// �ʱ�ȭ �� �����Ӻ� ����
AABCharacter::AABCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// ������Ʈ �����Ҵ�(new)
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	CharacterStat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("CHARACTERSTAT")); // ĳ���� ���� ������Ʈ�� ���� ������Ʈ�� �������ִ�.
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET")); // UI ������Ʈ ����

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);
	HPBarWidget->SetupAttachment(GetMesh());

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f)); // ĳ���� ��ġ ����
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));
	if (SK_CARDBOARD.Succeeded()) // ĳ���� ��Ų
	{
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);
	}
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/Animations/WarriorAnimBlueprint.WarriorAnimBlueprint_C"));
	if (WARRIOR_ANIM.Succeeded()) // ĳ���� �ִϸ��̼�
	{
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}
	SetControlMode(EControlMode::DIABLO); // ī�޶�(����)
	ArmLengthSpeed = 3.0f;
	ArmRotationSpeed = 10.0f;
	GetCharacterMovement()->JumpZVelocity = 800.0f;
	IsAttacking = false;
	MaxCombo = 4;
	AttackEndComboState();
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCharacter"));
	AttackRange = 80.0f; // �Ǽ� ��Ÿ�
	AttackRadius = 50.0f;

	HPBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(TEXT("WidgetBlueprint'/Game/Book/UI/UI_HPBar.UI_HPBar_C'"));

	if (UI_HUD.Succeeded()) // HUD ����, ABCharacter������ HP���� ����� �����ϱ⺸��, ����� ��ġ ������ ��ο츦 ����Ѵ�.
	{
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		HPBarWidget->SetDrawSize(FVector2D(150.0f, 50.0f));
	}

	DeadTimer = 1.0f;
	AIControllerClass = AABAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	// PREINIT ������Ʈ
	// ĳ���ʹ� ���ʷ� PREINIT(������)���� �����Ѵ�. �׷��ٰ� ������ ���۵� BeginPlay()�� ȣ��Ǹ�, LOADING ������Ʈ�� �Ѿ��.
	AssetIndex = 4;
	SetActorHiddenInGame(true); // ����
	HPBarWidget->SetHiddenInGame(true); // UI
	SetCanBeDamaged(false); // ������ ����
	// ĳ���� ���� ���� ������Ʈ : ĳ���Ϳ� UI�� ���ܵΰ�, �������� ���� �ʰ� �Ѵ�.

}

void AABCharacter::PostInitializeComponents() // ��������Ʈ �ʱ�ȭ ����
{
	Super::PostInitializeComponents();
	ABAnim = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
	ABCHECK(nullptr != ABAnim);
	ABAnim->OnMontageEnded.AddDynamic(this, &AABCharacter::OnAttackMontageEnded); // OnMontageEnded ��������Ʈ�� �ִϸ��̼� ��Ÿ�ֿ��� ������ ��������Ʈ

	ABAnim->OnNextAttackCheck.AddLambda([this]() -> void { // OnNextAttck ��������Ʈ�� ����� �Լ�, ���� ���ݽ� ��������Ʈ�� ȣ����

		CanNextCombo = false;

		if (IsComboInputOn)
		{
			AttackStartComboState();
			ABAnim->JumpToAttackMontageSection(CurrentCombo);
		}
	});
	ABAnim->OnAttackHitCheck.AddUObject(this, &AABCharacter::AttackCheck); // ��Ʈ ���� �� ��������Ʈ�� ȣ���� �Լ�

	CharacterStat->OnHPIsZero.AddLambda([this]()-> void { // hp�� 0�� �� �� 
		ABAnim->SetDeadAnim(); // isDead = true �״� ���
		SetActorEnableCollision(false); // �ݸ���(����) ��Ȱ��ȭ
	}); // ��������Ʈ�� ȣ���� ���ٽ�

//	HPBarWidget->InitWidget(); �� 4.25 ���� �߰� ���� : ����� ������ �ʱ�ȭ�Ǿ��� �� Ȯ�� �ʿ�!! UI �ʱ�ȭ�� BeginPlay()���� ȣ��ǹǷ� �̸� Ȯ�� �ʿ�, But HPBar Ŭ������ ��� �Լ��� ��ϵ� ��ε�ĳ��Ʈ�� �� �� ȣ��ȴ�!
}

void AABCharacter::BeginPlay()
{
	Super::BeginPlay();
	// PREINIT ������Ʈ
	auto CharacterWidget = Cast<UABCharacterWidget>(HPBarWidget->GetUserWidgetObject());
	if (nullptr != CharacterWidget)
	{
	//	CharacterWidget->BindCharacterStat(CharacterStat); // ABCharacterWidget.cpp�� ABLOG_Long(Warning, TEXT("HPRatio : %f"), CurrentCharacterStat->GetHPRatio()); �α׸� ������� �� �ּ��ϱ�!
	}
	else
	{
		ABLOG_Short(Error);
	}

	bIsPlayer = IsPlayerControlled(); // ĳ���͸� �÷��̾ ��Ʈ�� �� ���
	if (bIsPlayer)
	{
		ABPlayerController = Cast<AABPlayerController>(GetController()); // �÷��̾� ��Ʈ�ѷ��� �ش� ĳ���͸� ������
		ABCHECK(nullptr != ABPlayerController);
	}
	else
	{ // �ƴ� ���
		ABAIController = Cast<AABAIController>(GetController()); // AI��Ʈ�ѷ��� �ش� ĳ���͸� ������
		ABCHECK(nullptr != ABAIController);
	}
	
	auto DefaultSetting = GetDefault<UABCharacterSetting>(); // ĳ���� ����(�ε� �κ�)  ini������ �ҷ���
	if (bIsPlayer)
	{
		auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState()); // ���� auto ������ ABPlayerState ��ü�� �ȴ�.
		ABCHECK(nullptr != ABPlayerState);
		AssetIndex = ABPlayerState->GetCharacterIndex(); // ������ �ε����� ĳ���� ����(�÷��̾�� �ڽ� ������)
	}
	else
	{
		AssetIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1); // �������� �ϳ��� ������ ��� ����
	} // ���� �ε�
	CharacterAssetToLoad = DefaultSetting->CharacterAssets[AssetIndex]; // ĳ���� ���� �ε�(���� �Ǵ� 4�� ����)
	auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance()); // StreamableManager�� ����ϱ� ���� GameInstance Ŭ���� ������ ��üȭ(ĳ����)
	ABCHECK(nullptr != ABGameInstance);  // �񵿱� ������� �ּ��� �ε��� �� ��������Ʈ(OnAssetLoadCompleted)�� ȣ���ϵ��� ���, ĳ���õ� Ŭ�������� ����Լ��� ȣ���Ͽ� �񵿱� �������(ReqAsycLoad) �ּ��� �ε��ϵ��� �Ѵ�.
	AssetStreamingHandle = ABGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad, FStreamableDelegate::CreateUObject(this, &AABCharacter::OnAssetLoadCompleted));  // ������(���� ���)�� ABCharacterSetting����, ��ɾ�(�񵿱� �ּ� �ε� ����)�� GameInstance���� ����Ѵ�! �����Ϳ� ��ɾ� ������
	SetCharacterState(ECharacterState::LOADING); // LOADING ������Ʈ�� ����
}

void AABCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo, DeltaTime, ArmLengthSpeed);

	switch (CurrentControlMode) // ī�޶� ���� �ε��� �� �þ�ó��
	{
	case AABCharacter::EControlMode::GTA:
		break;
	case AABCharacter::EControlMode::DIABLO:
		SpringArm->SetRelativeRotation(FMath::RInterpTo(SpringArm->GetRelativeRotation(), ArmRotationTo, DeltaTime, ArmRotationSpeed));
		break;
	default:
		break;
	}

	switch (CurrentControlMode) // ���Ӹ�庰 ī�޶� �þ�ó��
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


// �̵� �� ī�޶� �ý���
void AABCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) // �Է���ġ ����
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

void AABCharacter::SetControlMode(EControlMode NewControlMode) // ��庰 ����
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

void AABCharacter::UpDown(float NewAxisValue) // ĳ���� ����
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

void AABCharacter::LeftRight(float NewAxisValue) // ĳ���� �¿�
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

void AABCharacter::LookUp(float NewAxisValue) // ī�޶� ���� ȸ��
{
	switch (CurrentControlMode)
	{
	case AABCharacter::EControlMode::GTA:
		AddControllerPitchInput(NewAxisValue);
		break;
	case AABCharacter::EControlMode::DIABLO: // ���ͺ�� �� �Ʒ� ȸ���� �ʿ����.
		break;
	default:
		break;
	}
}

void AABCharacter::Turn(float NewAxisValue) // ī�޶� �¿� ȸ��
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

void AABCharacter::ViewChange() // ī�޶� ��� ���
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


// ���� �ý��� 
bool AABCharacter::CanSetWeapon()
{
	return true;
}

void AABCharacter::PossessedBy(AController* NewController) // ĳ������ �����ڰ� �����ΰ�(�÷��̾�,npc), �⺻ ī�޶� Ÿ�԰� �̵��ӵ��� ���� 
{
	Super::PossessedBy(NewController);
	if (IsPlayerControlled()) // �÷��̾ ��Ʈ���� ���
	{
		SetControlMode(EControlMode::DIABLO);
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}
	else // AI�� ��Ʈ�� �� ���
	{
		SetControlMode(EControlMode::NPC);
		GetCharacterMovement()->MaxWalkSpeed = 300.0f; // �÷��̾�� �� ������
	}
}

void AABCharacter::SetWeapon(AABWeapon* NewWeapon) // ���� ���� ��
{
	ABCHECK(nullptr != NewWeapon);
	if (nullptr != CurrentWeapon) // �̹� ���⸦ �����ϰ� �������
	{
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform); // ���� �и�
		CurrentWeapon->Destroy(); // ���� �ı�
		CurrentWeapon = nullptr; // null �Ҵ�
	}

	FName WeaponSocket(TEXT("hand_rSocket")); // ���� ���� ����
	if (nullptr != NewWeapon)
	{
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		NewWeapon->SetOwner(this);
		CurrentWeapon = NewWeapon;
	}
}

void AABCharacter::Attack() // ����
{
	if (IsAttacking) // �̹� ���� ���Ͻ� �޺� ����
	{
		ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));
		if (CanNextCombo)
		{
			IsComboInputOn = true;
		}
	}
	else // ù ���� �� �޺� ����
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
	case ECharacterState::LOADING: // LOADING ������Ʈ ���� 
	{
		if (bIsPlayer)
		{
			DisableInput(ABPlayerController); // �ε� �� �Է� ��Ȱ��ȭ
			ABPlayerController->GetHUDWidget()->BindCharacterStat(CharacterStat); // HUD ������ ĳ���� ����������
			auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState()); // �÷��̾� ���� ĳ����
			ABCHECK(nullptr != ABPlayerState);
			CharacterStat->SetNewLevel(ABPlayerState->GetCharacterLevel()); // �÷��̾� ���� ���� (5����)
		}
		else
		{
			auto ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode()); // ���� ���� �߿� ���� ����� �����͸� ������ ���� GetAuthGameMode() �Լ��� ����Ѵ�. (�߿��� �����͸� �����ϴ� ������ ������)
			ABCHECK(nullptr != ABGameMode);
			int32 TargetLevel = FMath::CeilToInt(((float)ABGameMode->GetScore() * 0.8f)); // �ҷ��� ���� ����
			int32 FinalLevel = FMath::Clamp<int32>(TargetLevel, 1, 20); // �ִ� ���� ���Ѽ���
			ABLOG_Long(Warning, TEXT("Now New NPC Level Available is %d"), FinalLevel);
			CharacterStat->SetNewLevel(FinalLevel); // NPC ���� ����
		}

		SetActorHiddenInGame(true);
		HPBarWidget->SetHiddenInGame(true);
		SetCanBeDamaged(false);
	}
	break;
	case ECharacterState::READY: // READY ������Ʈ = ����ǥ��, UI, ������ó�� ǥ��
	{
		SetActorHiddenInGame(false);
		HPBarWidget->SetHiddenInGame(false);
		SetCanBeDamaged(true);
		CharacterStat->OnHPIsZero.AddLambda([this]()->void {
			SetCharacterState(ECharacterState::DEAD); // HP�� Zero�� DEAD ������Ʈ�� ���� ���̵ǰ� ��������Ʈ�� ���
		});
		HPBarWidget->InitWidget();
		auto CharacterWidget = Cast<UABCharacterWidget>(HPBarWidget->GetUserWidgetObject());
		ABCHECK(nullptr != CharacterWidget);
		CharacterWidget->BindCharacterStat(CharacterStat);

		if (bIsPlayer) // �÷��̾��� ��� ī�޶�, �̵��ӵ� �Ҵ�
		{
			SetControlMode(EControlMode::DIABLO);
			GetCharacterMovement()->MaxWalkSpeed = 600.0f;
			EnableInput(ABPlayerController);
		}
		else
		{
			SetControlMode(EControlMode::NPC);
			GetCharacterMovement()->MaxWalkSpeed = 400.0f;
			ABAIController->RunAI(); // NPC�� ��� AI Ʈ���� �����Ų��.
		}
	}
	break;
	case ECharacterState::DEAD:
	{
		SetActorEnableCollision(false); // ���� ���� ����
		GetMesh()->SetHiddenInGame(false);
		HPBarWidget->SetHiddenInGame(true);
		ABAnim->SetDeadAnim(); // ���� ǥ��
		SetCanBeDamaged(false);

		if (bIsPlayer) // ���� ĳ���Ͱ� �÷��̾��� ���
		{
			DisableInput(ABPlayerController); // �۵� ���� ����
		}
		else
		{
			ABAIController->StopAI(); // NPC�� ��� �ص� Ʈ���� �ߴܽ�Ų��.
		} 
		GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]()->void { // �׸��� �����ð� ��-
			if (bIsPlayer) // ĳ���Ͱ� ���� ��� ���� Ÿ�̸�(1.0f) �Ŀ� ���ٷ� ��ϵ� ��������Ʈ ȣ�� SetTimer()
			{
				ABPlayerController->ShowResultUI(); // ��� ȭ�� ���
			}
			else
			{
				Destroy(); // NPC�� ����
			}
		}), DeadTimer, false); // DeadTimer�� �����ð��̸�, 5.0f�� �����Ǿ���.
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
	return (nullptr != CurrentWeapon) ? CurrentWeapon->GetAttackRange() : AttackRange; // ���⸦ ��� ������ ���� ��Ÿ��� ��ȯ
}

float AABCharacter::GetFinalAttackDamage() const
{
	float AttackDamage = (nullptr != CurrentWeapon) ? (CharacterStat->GetAttack() + CurrentWeapon->GetAttackDamage()) : CharacterStat->GetAttack();
	float AttackModifier = (nullptr != CurrentWeapon) ? CurrentWeapon->GetAttackModifier() : 1.0f; // �Ǽ��̸� ����ġ 1.0 ����

	return AttackDamage * AttackModifier;
}

void AABCharacter::AttackCheck() // ������ üũ
{
	float FinalAttackRange = GetFinalAttackRange(); // ���� ��Ÿ�
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this); // Parm ���� 
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * FinalAttackRange, // ���� ��Ÿ� ����
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(50.0f),
		Params);

#if ENABLE_DRAW_DEBUG // �α� ���
	FVector TraceVec = GetActorForwardVector() * FinalAttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = FinalAttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 5.0f;

	DrawDebugCapsule(GetWorld(), // ����Ʈ�� �α� ���
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime);

	UE_VLOG_LOCATION(this, ArenaBattle, Verbose, GetActorLocation(), 50.0f, FColor::Blue, TEXT("Attack Position")); // ������ ĳ������ ��ġ�� ���־� �ΰŷ� ���
	UE_VLOG_CAPSULE(this, ArenaBattle, Verbose, GetActorLocation() - GetActorForwardVector() * AttackRadius, HalfHeight, AttackRadius, CapsuleRot, DrawColor, TEXT("Attack Area")); // ���� ���� ��ġ�� ���־� �ΰŷ� ���
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
	TSoftObjectPtr<USkeletalMesh> LoadAssetPath(CharacterAssetToLoad); // �ּ��� ��� ������ �ش�Ǵ� ����ƽ �޽� ������
	ABCHECK(LoadAssetPath.IsValid());
	GetMesh()->SetSkeletalMesh(LoadAssetPath.Get());
	SetCharacterState(ECharacterState::READY); // �ε尡 ������ READY�� ����
	
}

void AABCharacter::AttackStartComboState() // �޺�
{
	CanNextCombo = true;
	IsComboInputOn = false;
	ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void AABCharacter::AttackEndComboState() // �޺� ������
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

void AABCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted) // ���� �ִϸ��̼��� �������� IsAttacking ������ false�� ����
{
	ABCHECK(IsAttacking);
	ABCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();
	OnAttackEnd.Broadcast();
}

float AABCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) // ĳ���Ͱ� ���� ���� ���
{																						   // �� Instigator = ������ (���� ĳ����)
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (EventInstigator->IsPlayerController())
		ABLOG_Long(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);

	CharacterStat->SetDamage(FinalDamage);
	if (CurrentState == ECharacterState::DEAD)
	{
		if (EventInstigator->IsPlayerController()) // ������ (������ ��)�� �÷��̾��� ���, �´� �ڰ� AI�� ���
		{ 
			auto ABPlayerControllerIsPlayer = Cast<AABPlayerController>(EventInstigator);
			ABCHECK(nullptr != ABPlayerControllerIsPlayer, 0.0f);
			ABPlayerControllerIsPlayer->NPCKill(this); // ABPlayerControllerIsPlayer(�÷��̾�)�� NPCKill�� ȣ���Ͽ� ���� ĳ������ DropExp(��� ����ġ)�� �÷��̾�� ����
		}
	}
	return FinalDamage;
}