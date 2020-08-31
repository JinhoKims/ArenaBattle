// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterSelectWidget.h"
#include "ABCharacterSetting.h"
#include "ABGameInstance.h"
#include "EngineUtils.h"
#include "Animation/SkeletalMeshActor.h"
#include "Components/Button.h" // ��ư ���ҽ�
#include "Components/EditableTextBox.h"
#include "ABSaveGame.h"
#include "ABPlayerState.h"

void UABCharacterSelectWidget::NextCharacter(bool bForward)
{
	bForward ? CurrentIndex++ : CurrentIndex--; // ���� bForward�� ���̸� ����, �����̸� ����

	if (CurrentIndex == -1) CurrentIndex = MaxIndex - 1;
	if (CurrentIndex == MaxIndex) CurrentIndex = 0;

	auto CharacterSetting = GetDefault<UABCharacterSetting>();
	auto AssetRef = CharacterSetting->CharacterAssets[CurrentIndex]; // �ε����� �´� ������ ���۷���(���)�� CharacterSetting���� ������

	auto ABGameInstance = GetWorld()->GetGameInstance<UABGameInstance>();
	ABCHECK(nullptr != ABGameInstance);
	ABCHECK(TargetComponent.IsValid());

	USkeletalMesh* Asset = ABGameInstance->StreamableManager.LoadSynchronous<USkeletalMesh>(AssetRef); // �񵿱� ������� �ּ�(���̷�Ż �޽�)�� �ε���
	if (nullptr != Asset)
	{
		TargetComponent->SetSkeletalMesh(Asset); // ���̷�Ż �޽� ����
	}
}



void UABCharacterSelectWidget::NativeConstruct() // ���� ������
{
	Super::NativeConstruct();

	CurrentIndex = 0;
	auto CharacterSetting = GetDefault<UABCharacterSetting>();
	MaxIndex = CharacterSetting->CharacterAssets.Num(); // �ּ� ���� ��ȯ

	for (TActorIterator<ASkeletalMeshActor> It(GetWorld()); It; ++It) // ���� ���忡 �ִ� Ư�� Ÿ���� ��ӹ��� ������ ����� TActorIterator ������ ����� ������ �� �ִ�.
	{
		TargetComponent = It->GetSkeletalMeshComponent(); // Ÿ�� ������Ʈ�� ���̷�Ż �޽� ����
		break;
	}

	PrevButton = Cast<UButton>(GetWidgetFromName(TEXT("btnPrev")));
	ABCHECK(nullptr != PrevButton);

	NextButton = Cast<UButton>(GetWidgetFromName(TEXT("btnNext")));
	ABCHECK(nullptr != NextButton);

	TextBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("edtPlayerName")));
	ABCHECK(nullptr != TextBox);

	ConfirmButton = Cast<UButton>(GetWidgetFromName(TEXT("btnConfirm")));
	ABCHECK(nullptr != ConfirmButton);

	PrevButton->OnClicked.AddDynamic(this, &UABCharacterSelectWidget::OnPrevClicked); // ��������Ʈ�� ��ư Ŭ���� ���� �Լ��� ȣ��ǰ� �Ѵ�.
	NextButton->OnClicked.AddDynamic(this, &UABCharacterSelectWidget::OnNextClicked);
	ConfirmButton->OnClicked.AddDynamic(this, &UABCharacterSelectWidget::OnConfirmClicked);
}

void UABCharacterSelectWidget::OnPrevClicked()
{
	NextCharacter(false);
}

void UABCharacterSelectWidget::OnNextClicked()
{
	NextCharacter(true);
}

void UABCharacterSelectWidget::OnConfirmClicked()

{
	FString CharacterName = TextBox->GetText().ToString();
	if (CharacterName.Len() <= 0 || CharacterName.Len() > 10) return; // �̸��� ��� ����

	UABSaveGame* NewPlayerData = NewObject<UABSaveGame>(); // ���ο� ���̺� ���� ���� 
	NewPlayerData->PlayerName = CharacterName;
	NewPlayerData->Level = 1; // ���� ĳ���� ����
	NewPlayerData->Exp = 0;
	NewPlayerData->HighScore = 0;
	NewPlayerData->CharacterIndex = CurrentIndex;

	auto ABPlayerState = GetDefault<AABPlayerState>(); // ���̺� ���� Ȱ��ȭ
	if (UGameplayStatics::SaveGameToSlot(NewPlayerData, ABPlayerState->SaveSlotName, 0)) // �ε��� 0���� ���̺� ���Կ� ���̺� ���� ����
	{
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("GamePlay")); // OpenLevel : �ٸ� ������ �̵�, �ش� �μ��� GamePlay ������ �̵��Ѵ�.
	}
	else
	{
		ABLOG_Long(Error, TEXT("SaveGame Error!"));
	}
}

