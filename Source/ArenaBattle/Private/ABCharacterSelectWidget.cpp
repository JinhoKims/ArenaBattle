// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterSelectWidget.h"
#include "ABCharacterSetting.h"
#include "ABGameInstance.h"
#include "EngineUtils.h"
#include "Animation/SkeletalMeshActor.h"
#include "Components/Button.h" // 버튼 리소스
#include "Components/EditableTextBox.h"
#include "ABSaveGame.h"
#include "ABPlayerState.h"

void UABCharacterSelectWidget::NextCharacter(bool bForward)
{
	bForward ? CurrentIndex++ : CurrentIndex--; // 인자 bForward가 참이면 전진, 거짓이면 후진

	if (CurrentIndex == -1) CurrentIndex = MaxIndex - 1;
	if (CurrentIndex == MaxIndex) CurrentIndex = 0;

	auto CharacterSetting = GetDefault<UABCharacterSetting>();
	auto AssetRef = CharacterSetting->CharacterAssets[CurrentIndex]; // 인덱스에 맞는 에셋의 레퍼런스(경로)를 CharacterSetting에서 꺼내옴

	auto ABGameInstance = GetWorld()->GetGameInstance<UABGameInstance>();
	ABCHECK(nullptr != ABGameInstance);
	ABCHECK(TargetComponent.IsValid());

	USkeletalMesh* Asset = ABGameInstance->StreamableManager.LoadSynchronous<USkeletalMesh>(AssetRef); // 비동기 방식으로 애셋(스켈레탈 메시)를 로딩함
	if (nullptr != Asset)
	{
		TargetComponent->SetSkeletalMesh(Asset); // 스켈레탈 메시 적용
	}
}



void UABCharacterSelectWidget::NativeConstruct() // 최초 생성자
{
	Super::NativeConstruct();

	CurrentIndex = 0;
	auto CharacterSetting = GetDefault<UABCharacterSetting>();
	MaxIndex = CharacterSetting->CharacterAssets.Num(); // 애셋 갯수 반환

	for (TActorIterator<ASkeletalMeshActor> It(GetWorld()); It; ++It) // 현재 월드에 있는 특정 타입을 상속받은 액터의 목록은 TActorIterator 구문을 사용해 가져올 수 있다.
	{
		TargetComponent = It->GetSkeletalMeshComponent(); // 타겟 컴포넌트에 스켈레탈 메시 저장
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

	PrevButton->OnClicked.AddDynamic(this, &UABCharacterSelectWidget::OnPrevClicked); // 델리게이트로 버튼 클릭시 관련 함수가 호출되게 한다.
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
	if (CharacterName.Len() <= 0 || CharacterName.Len() > 10) return; // 이름이 길면 리턴

	UABSaveGame* NewPlayerData = NewObject<UABSaveGame>(); // 새로운 세이브 파일 생성 
	NewPlayerData->PlayerName = CharacterName;
	NewPlayerData->Level = 1; // 최초 캐릭터 세팅
	NewPlayerData->Exp = 0;
	NewPlayerData->HighScore = 0;
	NewPlayerData->CharacterIndex = CurrentIndex;

	auto ABPlayerState = GetDefault<AABPlayerState>(); // 세이브 슬롯 활성화
	if (UGameplayStatics::SaveGameToSlot(NewPlayerData, ABPlayerState->SaveSlotName, 0)) // 인덱스 0번의 세이브 슬롯에 세이브 파일 저장
	{
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("GamePlay")); // OpenLevel : 다른 레벨로 이동, 해당 인수로 GamePlay 레벨로 이동한다.
	}
	else
	{
		ABLOG_Long(Error, TEXT("SaveGame Error!"));
	}
}

