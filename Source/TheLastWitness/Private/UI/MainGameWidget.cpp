// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/MainGameWidget.h"
#include "Core/WitnessGameMode.h"
#include "Core/CaseState.h"
#include "Dialogue/DialogueManager.h"
#include "AI/ABELSystem.h"
#include "UI/EvidenceCardWidget.h"
#include "UI/CharacterCardWidget.h"
#include "UI/DialogueChoiceWidget.h"
#include "UI/ABELSuggestionWidget.h"
#include "UI/LocationCardWidget.h"
#include "UI/SuspectCardWidget.h"
#include "UI/DeductionSlotWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "Components/ScrollBox.h"
#include "TheLastWitness.h"

void UMainGameWidget::SetupWidget(AWitnessGameMode* InGameMode)
{
	GameMode = InGameMode;

	if (GameMode)
	{
		CaseState = GameMode->GetCaseState();
		DialogueManager = GameMode->GetDialogueManager();
		ABELSystem = GameMode->GetABELSystem();

		// イベントをバインド
		GameMode->OnPhaseChanged.AddDynamic(this, &UMainGameWidget::OnPhaseChanged);
		GameMode->OnGameEnded.AddDynamic(this, &UMainGameWidget::OnGameEnded);

		if (DialogueManager)
		{
			DialogueManager->OnDialogueNodeChanged.AddDynamic(this, &UMainGameWidget::OnDialogueNodeChanged);
			DialogueManager->OnChoicesAvailable.AddDynamic(this, &UMainGameWidget::OnChoicesAvailable);
		}

		if (ABELSystem)
		{
			ABELSystem->OnABELSpeaks.AddDynamic(this, &UMainGameWidget::OnABELSpeaks);
		}

		if (CaseState)
		{
			CaseState->OnEvidenceCollected.AddDynamic(this, &UMainGameWidget::OnEvidenceCollected);
		}
	}

	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] 初期化完了"));
}

void UMainGameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// ボタンイベントをバインド
	if (StartGameButton)
	{
		StartGameButton->OnClicked.AddDynamic(this, &UMainGameWidget::OnStartGameClicked);
	}

	if (BeginInvestigationButton)
	{
		BeginInvestigationButton->OnClicked.AddDynamic(this, &UMainGameWidget::OnContinueClicked);
	}

	if (ExamineButton)
	{
		ExamineButton->OnClicked.AddDynamic(this, &UMainGameWidget::OnExamineClicked);
	}

	if (TravelButton)
	{
		TravelButton->OnClicked.AddDynamic(this, &UMainGameWidget::OnTravelClicked);
	}

	if (ConsultABELButton)
	{
		ConsultABELButton->OnClicked.AddDynamic(this, &UMainGameWidget::OnConsultABELClicked);
	}

	if (JournalButton)
	{
		JournalButton->OnClicked.AddDynamic(this, &UMainGameWidget::OnJournalClicked);
	}

	if (DeductionBoardButton)
	{
		DeductionBoardButton->OnClicked.AddDynamic(this, &UMainGameWidget::OnDeductionBoardClicked);
	}

	if (AccuseButton)
	{
		AccuseButton->OnClicked.AddDynamic(this, &UMainGameWidget::OnAccuseClicked);
	}

	if (ContinueDialogueButton)
	{
		ContinueDialogueButton->OnClicked.AddDynamic(this, &UMainGameWidget::OnContinueClicked);
	}

	if (CloseABELButton)
	{
		CloseABELButton->OnClicked.AddDynamic(this, &UMainGameWidget::OnContinueClicked);
	}

	if (ReturnToMenuButton)
	{
		ReturnToMenuButton->OnClicked.AddDynamic(this, &UMainGameWidget::OnStartGameClicked);
	}

	// 移動パネルボタン
	if (CloseTravelButton)
	{
		CloseTravelButton->OnClicked.AddDynamic(this, &UMainGameWidget::HideTravelPanel);
	}

	// 推理パネルボタン
	if (MakeDeductionButton)
	{
		MakeDeductionButton->OnClicked.AddDynamic(this, &UMainGameWidget::TryMakeDeduction);
		UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] MakeDeductionButton をバインドしました"));
	}
	else
	{
		UE_LOG(LogLastWitness, Warning, TEXT("[MainGameWidget] MakeDeductionButton が見つかりません"));
	}

	if (CloseDeductionButton)
	{
		CloseDeductionButton->OnClicked.AddDynamic(this, &UMainGameWidget::OnContinueClicked);
		UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] CloseDeductionButton をバインドしました"));
	}
	else
	{
		UE_LOG(LogLastWitness, Warning, TEXT("[MainGameWidget] CloseDeductionButton が見つかりません"));
	}

	// 告発パネルボタン
	if (CancelAccusationButton)
	{
		CancelAccusationButton->OnClicked.AddDynamic(this, &UMainGameWidget::OnContinueClicked);
	}

	// 証拠帳パネルボタン
	if (CloseJournalButton)
	{
		CloseJournalButton->OnClicked.AddDynamic(this, &UMainGameWidget::HideJournalPanel);
	}

	// 証拠詳細パネルボタン
	if (CloseDetailButton)
	{
		CloseDetailButton->OnClicked.AddDynamic(this, &UMainGameWidget::HideEvidenceDetail);
	}

	// ポップアップパネルを初期非表示
	if (TravelPopupPanel)
	{
		TravelPopupPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (JournalPopupPanel)
	{
		JournalPopupPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (EvidenceDetailPanel)
	{
		EvidenceDetailPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	// 初期状態はメインメニュー
	SwitchToPanel(EPanelIndex::MainMenu);
}

void UMainGameWidget::NativeDestruct()
{
	// イベントのアンバインド
	if (GameMode)
	{
		GameMode->OnPhaseChanged.RemoveDynamic(this, &UMainGameWidget::OnPhaseChanged);
		GameMode->OnGameEnded.RemoveDynamic(this, &UMainGameWidget::OnGameEnded);
	}

	if (DialogueManager)
	{
		DialogueManager->OnDialogueNodeChanged.RemoveDynamic(this, &UMainGameWidget::OnDialogueNodeChanged);
		DialogueManager->OnChoicesAvailable.RemoveDynamic(this, &UMainGameWidget::OnChoicesAvailable);
	}

	if (ABELSystem)
	{
		ABELSystem->OnABELSpeaks.RemoveDynamic(this, &UMainGameWidget::OnABELSpeaks);
	}

	if (CaseState)
	{
		CaseState->OnEvidenceCollected.RemoveDynamic(this, &UMainGameWidget::OnEvidenceCollected);
	}

	Super::NativeDestruct();
}

// ============================================================================
// イベントハンドラ
// ============================================================================

void UMainGameWidget::OnPhaseChanged(EGamePhase NewPhase)
{
	switch (NewPhase)
	{
	case EGamePhase::MainMenu:
		SwitchToPanel(EPanelIndex::MainMenu);
		break;

	case EGamePhase::CaseIntro:
		SwitchToPanel(EPanelIndex::CaseIntro);
		// 事件情報を表示
		if (CaseState && CaseTitleText && CaseSynopsisText)
		{
			const FCaseData& CaseData = CaseState->GetCaseData();
			CaseTitleText->SetText(CaseData.Title);
			CaseSynopsisText->SetText(CaseData.Synopsis);
		}
		break;

	case EGamePhase::Investigation:
		SwitchToPanel(EPanelIndex::Investigation);
		UpdateLocationPanel();
		break;

	case EGamePhase::Dialogue:
		SwitchToPanel(EPanelIndex::Dialogue);
		UpdateDialoguePanel();
		break;

	case EGamePhase::ABELAnalysis:
		SwitchToPanel(EPanelIndex::ABEL);
		UpdateABELPanel();
		break;

	case EGamePhase::Deduction:
		SwitchToPanel(EPanelIndex::Deduction);
		UpdateDeductionPanel();
		break;

	case EGamePhase::Accusation:
		SwitchToPanel(EPanelIndex::Accusation);
		UpdateAccusationPanel();
		break;

	case EGamePhase::Resolution:
		SwitchToPanel(EPanelIndex::Result);
		break;
	}

	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] フェーズ変更: %d"), static_cast<int32>(NewPhase));
}

void UMainGameWidget::OnDialogueNodeChanged(const FDialogueNode& Node)
{
	UpdateDialoguePanel();
}

void UMainGameWidget::OnChoicesAvailable(const TArray<FDialogueChoice>& Choices)
{
	// 選択肢を動的に生成
	if (ChoicesBox)
	{
		ChoicesBox->ClearChildren();

		for (const FDialogueChoice& Choice : Choices)
		{
			if (DialogueChoiceClass)
			{
				UDialogueChoiceWidget* ChoiceWidget = CreateWidget<UDialogueChoiceWidget>(GetOwningPlayer(), DialogueChoiceClass);
				if (ChoiceWidget)
				{
					ChoiceWidget->SetChoiceData(Choice);
					ChoiceWidget->OnChoiceSelected.AddDynamic(this, &UMainGameWidget::OnDialogueChoiceSelected);
					ChoicesBox->AddChild(ChoiceWidget);
				}
			}
			else
			{
				UE_LOG(LogLastWitness, Warning, TEXT("[MainGameWidget] DialogueChoiceClassが設定されていません"));
			}
		}
	}

	// 選択肢がある場合は続行ボタンを非表示
	if (ContinueDialogueButton)
	{
		ContinueDialogueButton->SetVisibility(
			Choices.Num() > 0 ? ESlateVisibility::Collapsed : ESlateVisibility::Visible
		);
	}
}

void UMainGameWidget::OnABELSpeaks(const FText& Message)
{
	if (ABELMessageText)
	{
		ABELMessageText->SetText(Message);
	}
}

void UMainGameWidget::OnEvidenceCollected(const FEvidence& Evidence)
{
	// 証拠収集通知を表示
	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] 証拠を収集: %s"), *Evidence.DisplayName.ToString());

	// 証拠リストを更新
	UpdateEvidenceList();

	// 証拠数を更新
	if (EvidenceCountText && CaseState)
	{
		const int32 Count = CaseState->GetCollectedEvidence().Num();
		EvidenceCountText->SetText(FText::FromString(FString::Printf(TEXT("収集済み証拠: %d 件"), Count)));
	}
}

void UMainGameWidget::OnGameEnded(const FGameResult& Result)
{
	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] ゲーム終了 - 正解: %s"), Result.bCorrectCulprit ? TEXT("はい") : TEXT("いいえ"));

	// 結果パネルを更新
	UpdateResultPanel(Result);
}

// ============================================================================
// UI更新
// ============================================================================

void UMainGameWidget::UpdateLocationPanel()
{
	if (!CaseState)
	{
		return;
	}

	FLocationData LocData;
	if (CaseState->GetLocationData(CaseState->GetCurrentLocation(), LocData))
	{
		if (LocationNameText)
		{
			LocationNameText->SetText(LocData.DisplayName);
		}

		if (LocationDescriptionText)
		{
			LocationDescriptionText->SetText(LocData.Description);
		}
	}

	UpdateEvidenceList();
	UpdateCharacterList();

	// 告発ボタンの状態を更新
	if (AccuseButton)
	{
		AccuseButton->SetIsEnabled(CaseState->CanMakeAccusation());
	}
}

void UMainGameWidget::UpdateEvidenceList()
{
	if (!EvidenceListBox || !GameMode)
	{
		return;
	}

	EvidenceListBox->ClearChildren();

	const TArray<FEvidence> Evidence = GameMode->GetExaminableEvidenceAtLocation();
	for (const FEvidence& E : Evidence)
	{
		if (EvidenceCardClass)
		{
			UEvidenceCardWidget* Card = CreateWidget<UEvidenceCardWidget>(GetOwningPlayer(), EvidenceCardClass);
			if (Card)
			{
				Card->SetEvidenceData(E);
				Card->OnCardClicked.AddDynamic(this, &UMainGameWidget::OnEvidenceCardClicked);
				EvidenceListBox->AddChild(Card);
			}
		}
		else
		{
			UE_LOG(LogLastWitness, Warning, TEXT("[MainGameWidget] EvidenceCardClassが設定されていません"));
		}
	}
}

void UMainGameWidget::UpdateCharacterList()
{
	if (!CharacterListBox || !GameMode)
	{
		return;
	}

	CharacterListBox->ClearChildren();

	const TArray<FCharacterData> Characters = GameMode->GetCharactersAtLocation();
	for (const FCharacterData& C : Characters)
	{
		if (CharacterCardClass)
		{
			UCharacterCardWidget* Card = CreateWidget<UCharacterCardWidget>(GetOwningPlayer(), CharacterCardClass);
			if (Card)
			{
				Card->SetCharacterData(C);
				Card->OnCardClicked.AddDynamic(this, &UMainGameWidget::OnCharacterCardClicked);
				CharacterListBox->AddChild(Card);
			}
		}
		else
		{
			UE_LOG(LogLastWitness, Warning, TEXT("[MainGameWidget] CharacterCardClassが設定されていません"));
		}
	}
}

void UMainGameWidget::UpdateDialoguePanel()
{
	if (!DialogueManager)
	{
		return;
	}

	FDialogueNode CurrentNode;
	if (DialogueManager->GetCurrentNode(CurrentNode))
	{
		// 話者名を設定
		if (SpeakerNameText)
		{
			if (CurrentNode.SpeakerId.IsNone())
			{
				SpeakerNameText->SetText(FText::FromString(TEXT("ナレーション")));
			}
			else if (CaseState)
			{
				FCharacterData Character;
				if (CaseState->GetCharacterById(CurrentNode.SpeakerId, Character))
				{
					SpeakerNameText->SetText(Character.DisplayName);
				}
			}
		}

		// 対話テキストを設定
		if (DialogueText)
		{
			DialogueText->SetText(CurrentNode.Text);
		}

		// 終了ノードなら続行ボタンのテキストを変更
		if (ContinueDialogueButton)
		{
			// ボタン内のTextBlockを探して更新
			UTextBlock* ButtonText = Cast<UTextBlock>(ContinueDialogueButton->GetChildAt(0));
			if (ButtonText)
			{
				if (DialogueManager->IsAtEndNode())
				{
					ButtonText->SetText(FText::FromString(TEXT("終了")));
				}
				else
				{
					ButtonText->SetText(FText::FromString(TEXT("続ける")));
				}
			}
		}
	}
}

void UMainGameWidget::UpdateABELPanel()
{
	if (!ABELSystem || !ABELSuggestionsBox)
	{
		return;
	}

	ABELSuggestionsBox->ClearChildren();

	const TArray<FABELSuggestion> Suggestions = ABELSystem->GetCurrentSuggestions();
	int32 ActiveSuggestionCount = 0;

	for (const FABELSuggestion& S : Suggestions)
	{
		// 既に処理済みの提案はスキップ
		if (S.bHasBeenShown)
		{
			continue;
		}

		if (ABELSuggestionClass)
		{
			UABELSuggestionWidget* SuggestionWidget = CreateWidget<UABELSuggestionWidget>(GetOwningPlayer(), ABELSuggestionClass);
			if (SuggestionWidget)
			{
				SuggestionWidget->SetSuggestionData(S);
				SuggestionWidget->OnAccepted.AddDynamic(this, &UMainGameWidget::OnABELSuggestionAccepted);
				SuggestionWidget->OnIgnored.AddDynamic(this, &UMainGameWidget::OnABELSuggestionIgnored);
				ABELSuggestionsBox->AddChild(SuggestionWidget);
				ActiveSuggestionCount++;
			}
		}
		else
		{
			UE_LOG(LogLastWitness, Warning, TEXT("[MainGameWidget] ABELSuggestionClassが設定されていません"));
		}
	}

	// 保留中のコメントがあれば表示
	if (ABELSystem->HasPendingComment() && ABELMessageText)
	{
		ABELMessageText->SetText(ABELSystem->GetAndClearPendingComment());
	}

	// 提案がない場合のメッセージ
	if (ActiveSuggestionCount == 0 && ABELMessageText)
	{
		if (ABELMessageText->GetText().IsEmpty())
		{
			ABELMessageText->SetText(FText::FromString(TEXT("現在、新たな提案はありません。調査を続けてください。")));
		}
	}
}

// ============================================================================
// ボタンハンドラ
// ============================================================================

void UMainGameWidget::OnStartGameClicked()
{
	if (GameMode)
	{
		GameMode->StartCase();
	}
}

void UMainGameWidget::OnContinueClicked()
{
	if (!GameMode)
	{
		return;
	}

	const EGamePhase CurrentPhase = GameMode->GetCurrentPhase();

	switch (CurrentPhase)
	{
	case EGamePhase::CaseIntro:
		// 調査フェーズに移行
		GameMode->TravelToLocation(ELocation::Study);
		break;

	case EGamePhase::Dialogue:
		if (DialogueManager)
		{
			if (DialogueManager->IsAtEndNode())
			{
				GameMode->EndDialogue();
			}
			else
			{
				DialogueManager->AdvanceDialogue();
			}
		}
		break;

	case EGamePhase::ABELAnalysis:
		GameMode->SetPhase(EGamePhase::Investigation);
		break;

	case EGamePhase::Deduction:
		GameMode->SetPhase(EGamePhase::Investigation);
		break;

	case EGamePhase::Accusation:
		GameMode->SetPhase(EGamePhase::Investigation);
		break;

	default:
		break;
	}
}

void UMainGameWidget::OnExamineClicked()
{
	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] 調査ボタンがクリックされました"));

	// 現在のロケーションで調査可能な証拠がある場合、自動的に最初の証拠を調査
	if (GameMode)
	{
		const TArray<FEvidence> Evidence = GameMode->GetExaminableEvidenceAtLocation();
		if (Evidence.Num() > 0)
		{
			// 詳細パネルを表示
			ShowEvidenceDetail(Evidence[0]);
		}
	}
}

void UMainGameWidget::OnTravelClicked()
{
	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] 移動ボタンがクリックされました"));
	ShowTravelPanel();
}

void UMainGameWidget::OnConsultABELClicked()
{
	if (GameMode)
	{
		GameMode->RequestABELAnalysis();
	}
}

void UMainGameWidget::OnJournalClicked()
{
	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] 証拠帳ボタンがクリックされました"));
	ShowJournalPanel();
}

void UMainGameWidget::OnDeductionBoardClicked()
{
	if (GameMode)
	{
		GameMode->OpenDeductionBoard();
	}
}

void UMainGameWidget::OnAccuseClicked()
{
	if (GameMode)
	{
		GameMode->BeginAccusation();
	}
}

void UMainGameWidget::SwitchToPanel(EPanelIndex PanelIndex)
{
	if (PanelSwitcher)
	{
		PanelSwitcher->SetActiveWidgetIndex(static_cast<int32>(PanelIndex));
	}
}

// ============================================================================
// 動的ウィジェットイベントハンドラ
// ============================================================================

void UMainGameWidget::OnEvidenceCardClicked(FName EvidenceId)
{
	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] 証拠カードがクリックされました: %s"), *EvidenceId.ToString());

	if (GameMode)
	{
		// 証拠を調査して収集
		GameMode->ExamineEvidence(EvidenceId);
		GameMode->CollectEvidence(EvidenceId);

		// リストを更新
		UpdateEvidenceList();
	}
}

void UMainGameWidget::OnCharacterCardClicked(FName CharacterId)
{
	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] キャラクターカードがクリックされました: %s"), *CharacterId.ToString());

	if (GameMode)
	{
		// 対話を開始
		GameMode->StartDialogueWith(CharacterId);
	}
}

void UMainGameWidget::OnDialogueChoiceSelected(FName ChoiceId)
{
	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] 対話選択肢が選ばれました: %s"), *ChoiceId.ToString());

	if (GameMode)
	{
		GameMode->SelectDialogueChoice(ChoiceId);
	}
}

void UMainGameWidget::OnABELSuggestionAccepted(FName SuggestionId)
{
	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] ABEL提案を承認しました: %s"), *SuggestionId.ToString());

	if (GameMode)
	{
		GameMode->FollowABELSuggestion(SuggestionId);
	}

	// ABELパネルを更新して提案リストをリフレッシュ
	UpdateABELPanel();
}

void UMainGameWidget::OnABELSuggestionIgnored(FName SuggestionId)
{
	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] ABEL提案を無視しました: %s"), *SuggestionId.ToString());

	if (GameMode)
	{
		GameMode->IgnoreABELSuggestion(SuggestionId);
	}

	// ABELパネルを更新して提案リストをリフレッシュ
	UpdateABELPanel();
}

void UMainGameWidget::OnLocationCardClicked(ELocation Location)
{
	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] ロケーションが選択されました: %d"), static_cast<int32>(Location));

	HideTravelPanel();

	if (GameMode)
	{
		GameMode->TravelToLocation(Location);

		// 場所変更後にUIを更新
		UpdateLocationPanel();
	}
}

void UMainGameWidget::OnSuspectAccused(FName CharacterId)
{
	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] 容疑者を告発しました: %s"), *CharacterId.ToString());

	if (GameMode)
	{
		GameMode->AccuseCharacter(CharacterId);
	}
}

void UMainGameWidget::OnDeductionSlotClicked(int32 SlotIndex)
{
	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] 推理スロット%dがクリックされました (スロット数: %d)"), SlotIndex, DeductionSlots.Num());
	CurrentDeductionSlot = SlotIndex;

	// 選択状態を表示
	if (DeductionResultText)
	{
		DeductionResultText->SetText(FText::FromString(FString::Printf(TEXT("スロット%dに配置する証拠を選択してください"), SlotIndex + 1)));
		UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] 結果テキストを更新しました"));
	}
	else
	{
		UE_LOG(LogLastWitness, Warning, TEXT("[MainGameWidget] DeductionResultTextがnullです"));
	}

	// スロットの選択状態を視覚的に更新
	for (int32 i = 0; i < DeductionSlots.Num(); i++)
	{
		if (DeductionSlots[i])
		{
			DeductionSlots[i]->SetSelected(i == SlotIndex);
		}
	}
}

void UMainGameWidget::OnDeductionSlotCleared(int32 SlotIndex)
{
	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] 推理スロット%dがクリアされました"), SlotIndex);

	if (SlotIndex >= 0 && SlotIndex < DeductionSlotEvidence.Num())
	{
		DeductionSlotEvidence[SlotIndex] = NAME_None;
	}
}

// ============================================================================
// 移動・推理・告発パネル
// ============================================================================

void UMainGameWidget::ShowTravelPanel()
{
	if (!TravelPopupPanel || !LocationListBox || !CaseState)
	{
		return;
	}

	LocationListBox->ClearChildren();

	// 全てのアクセス可能なロケーションを表示
	const TArray<FLocationData> Locations = CaseState->GetAccessibleLocations();
	const ELocation CurrentLoc = CaseState->GetCurrentLocation();

	for (const FLocationData& Loc : Locations)
	{
		if (LocationCardClass)
		{
			ULocationCardWidget* Card = CreateWidget<ULocationCardWidget>(GetOwningPlayer(), LocationCardClass);
			if (Card)
			{
				Card->SetLocationData(Loc);
				Card->SetAsCurrent(Loc.Location == CurrentLoc);
				Card->OnCardClicked.AddDynamic(this, &UMainGameWidget::OnLocationCardClicked);
				LocationListBox->AddChild(Card);
			}
		}
	}

	TravelPopupPanel->SetVisibility(ESlateVisibility::Visible);
}

void UMainGameWidget::HideTravelPanel()
{
	if (TravelPopupPanel)
	{
		TravelPopupPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMainGameWidget::UpdateDeductionPanel()
{
	if (!CaseState)
	{
		return;
	}

	// 推理スロットを初期化
	DeductionSlotEvidence.SetNum(2);
	DeductionSlotEvidence[0] = NAME_None;
	DeductionSlotEvidence[1] = NAME_None;
	DeductionSlots.Empty();

	// 推理スロットを作成
	if (DeductionSlotsBox && DeductionSlotClass)
	{
		DeductionSlotsBox->ClearChildren();

		for (int32 i = 0; i < 2; i++)
		{
			UDeductionSlotWidget* SlotWidget = CreateWidget<UDeductionSlotWidget>(GetOwningPlayer(), DeductionSlotClass);
			if (SlotWidget)
			{
				SlotWidget->SetSlotIndex(i);
				SlotWidget->OnSlotClicked.AddDynamic(this, &UMainGameWidget::OnDeductionSlotClicked);
				SlotWidget->OnSlotCleared.AddDynamic(this, &UMainGameWidget::OnDeductionSlotCleared);
				DeductionSlotsBox->AddChild(SlotWidget);
				DeductionSlots.Add(SlotWidget);
			}
		}
	}

	// 収集済み証拠を表示（推理用）
	if (CollectedEvidenceBox && EvidenceCardClass)
	{
		CollectedEvidenceBox->ClearChildren();

		const TArray<FEvidence> CollectedEvidence = CaseState->GetCollectedEvidence();
		for (const FEvidence& E : CollectedEvidence)
		{
			UEvidenceCardWidget* Card = CreateWidget<UEvidenceCardWidget>(GetOwningPlayer(), EvidenceCardClass);
			if (Card)
			{
				Card->SetEvidenceData(E);
				// 推理用のクリックハンドラを使用
				Card->OnCardClicked.AddDynamic(this, &UMainGameWidget::OnDeductionEvidenceCardClicked);
				CollectedEvidenceBox->AddChild(Card);
			}
		}
	}

	// 推理結果テキストをクリア
	if (DeductionResultText)
	{
		DeductionResultText->SetText(FText::GetEmpty());
	}
}

void UMainGameWidget::UpdateAccusationPanel()
{
	if (!SuspectListBox || !CaseState)
	{
		return;
	}

	SuspectListBox->ClearChildren();

	// 全ての容疑者を表示
	const TArray<FCharacterData> Suspects = CaseState->GetAllSuspects();
	for (const FCharacterData& S : Suspects)
	{
		if (SuspectCardClass)
		{
			USuspectCardWidget* Card = CreateWidget<USuspectCardWidget>(GetOwningPlayer(), SuspectCardClass);
			if (Card)
			{
				Card->SetSuspectData(S);
				Card->OnAccused.AddDynamic(this, &UMainGameWidget::OnSuspectAccused);
				SuspectListBox->AddChild(Card);
			}
		}
	}
}

void UMainGameWidget::UpdateResultPanel(const FGameResult& Result)
{
	if (ResultTitleText)
	{
		if (Result.bCorrectCulprit)
		{
			ResultTitleText->SetText(FText::FromString(TEXT("事件解決")));
		}
		else
		{
			ResultTitleText->SetText(FText::FromString(TEXT("冤罪...")));
		}
	}

	if (ResultNarrativeText)
	{
		ResultNarrativeText->SetText(Result.EndingNarrative);
	}
}

void UMainGameWidget::TryMakeDeduction()
{
	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] 推理ボタンがクリックされました"));

	if (!GameMode || DeductionSlotEvidence.Num() < 2)
	{
		UE_LOG(LogLastWitness, Warning, TEXT("[MainGameWidget] GameModeがないか、スロットが不足"));
		return;
	}

	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] スロット0: %s, スロット1: %s"),
		*DeductionSlotEvidence[0].ToString(),
		*DeductionSlotEvidence[1].ToString());

	if (DeductionSlotEvidence[0].IsNone() || DeductionSlotEvidence[1].IsNone())
	{
		UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] 証拠が足りません"));
		if (DeductionResultText)
		{
			DeductionResultText->SetText(FText::FromString(TEXT("2つの証拠を選択してください")));
		}
		return;
	}

	FDeduction OutDeduction;
	const bool bSuccess = GameMode->TryMakeDeduction(DeductionSlotEvidence[0], DeductionSlotEvidence[1], OutDeduction);

	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] 推理結果: %s"), bSuccess ? TEXT("成功") : TEXT("失敗"));

	if (DeductionResultText)
	{
		if (bSuccess)
		{
			// 推理のタイトルと説明を表示
			const FString ResultMessage = FString::Printf(
				TEXT("【%s】\n%s"),
				*OutDeduction.Title.ToString(),
				*OutDeduction.Description.ToString()
			);
			DeductionResultText->SetText(FText::FromString(ResultMessage));
			UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] 推理成功: %s"), *OutDeduction.Title.ToString());
		}
		else
		{
			DeductionResultText->SetText(FText::FromString(TEXT("この組み合わせからは何も導けません...")));
		}
	}
	else
	{
		UE_LOG(LogLastWitness, Warning, TEXT("[MainGameWidget] DeductionResultTextがバインドされていません"));
	}

	// スロットをクリア
	DeductionSlotEvidence[0] = NAME_None;
	DeductionSlotEvidence[1] = NAME_None;
	for (UDeductionSlotWidget* SlotWidget : DeductionSlots)
	{
		if (SlotWidget)
		{
			SlotWidget->ClearEvidence();
		}
	}
}

// ============================================================================
// 証拠帳パネル
// ============================================================================

void UMainGameWidget::ShowJournalPanel()
{
	if (!JournalPopupPanel || !JournalEvidenceListBox || !CaseState)
	{
		UE_LOG(LogLastWitness, Warning, TEXT("[MainGameWidget] 証拠帳パネルが設定されていません"));
		return;
	}

	JournalEvidenceListBox->ClearChildren();

	// 収集済み証拠を表示
	const TArray<FEvidence> CollectedEvidence = CaseState->GetCollectedEvidence();
	for (const FEvidence& E : CollectedEvidence)
	{
		if (EvidenceCardClass)
		{
			UEvidenceCardWidget* Card = CreateWidget<UEvidenceCardWidget>(GetOwningPlayer(), EvidenceCardClass);
			if (Card)
			{
				Card->SetEvidenceData(E);
				Card->OnCardClicked.AddDynamic(this, &UMainGameWidget::OnJournalEvidenceCardClicked);
				JournalEvidenceListBox->AddChild(Card);
			}
		}
	}

	// 証拠数を更新
	if (EvidenceCountText)
	{
		EvidenceCountText->SetText(FText::FromString(FString::Printf(TEXT("収集済み証拠: %d 件"), CollectedEvidence.Num())));
		UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] EvidenceCountTextを更新: %d 件"), CollectedEvidence.Num());
	}
	else
	{
		UE_LOG(LogLastWitness, Warning, TEXT("[MainGameWidget] EvidenceCountTextがバインドされていません"));
	}

	JournalPopupPanel->SetVisibility(ESlateVisibility::Visible);
	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] 証拠帳を表示 (証拠数: %d)"), CollectedEvidence.Num());
}

void UMainGameWidget::HideJournalPanel()
{
	if (JournalPopupPanel)
	{
		JournalPopupPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMainGameWidget::OnJournalEvidenceCardClicked(FName EvidenceId)
{
	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] 証拠帳の証拠がクリックされました: %s"), *EvidenceId.ToString());

	if (CaseState)
	{
		FEvidence Evidence;
		if (CaseState->GetEvidenceById(EvidenceId, Evidence))
		{
			ShowEvidenceDetail(Evidence);
		}
	}
}

// ============================================================================
// 証拠詳細パネル
// ============================================================================

void UMainGameWidget::ShowEvidenceDetail(const FEvidence& Evidence)
{
	if (!EvidenceDetailPanel)
	{
		UE_LOG(LogLastWitness, Warning, TEXT("[MainGameWidget] 証拠詳細パネルが設定されていません"));
		return;
	}

	// 証拠名
	if (DetailEvidenceNameText)
	{
		DetailEvidenceNameText->SetText(Evidence.DisplayName);
	}

	// 証拠タイプ
	if (DetailEvidenceTypeText)
	{
		FText TypeText;
		switch (Evidence.Type)
		{
		case EEvidenceType::Physical:
			TypeText = FText::FromString(TEXT("物的証拠"));
			break;
		case EEvidenceType::Document:
			TypeText = FText::FromString(TEXT("文書"));
			break;
		case EEvidenceType::Testimony:
			TypeText = FText::FromString(TEXT("証言"));
			break;
		case EEvidenceType::Observation:
			TypeText = FText::FromString(TEXT("観察"));
			break;
		default:
			TypeText = FText::FromString(TEXT("不明"));
			break;
		}
		DetailEvidenceTypeText->SetText(TypeText);
	}

	// 説明
	if (DetailEvidenceDescText)
	{
		DetailEvidenceDescText->SetText(Evidence.Description);
	}

	// ABELコメント
	if (DetailABELCommentText)
	{
		if (Evidence.ABELComment.IsEmpty())
		{
			DetailABELCommentText->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			DetailABELCommentText->SetText(Evidence.ABELComment);
			DetailABELCommentText->SetVisibility(ESlateVisibility::Visible);
		}
	}

	EvidenceDetailPanel->SetVisibility(ESlateVisibility::Visible);
	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] 証拠詳細を表示: %s"), *Evidence.EvidenceId.ToString());
}

void UMainGameWidget::HideEvidenceDetail()
{
	if (EvidenceDetailPanel)
	{
		EvidenceDetailPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

// ============================================================================
// 推理用証拠選択
// ============================================================================

void UMainGameWidget::OnDeductionEvidenceCardClicked(FName EvidenceId)
{
	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] 推理用証拠がクリックされました: %s (現在のスロット: %d, スロット数: %d)"),
		*EvidenceId.ToString(), CurrentDeductionSlot, DeductionSlots.Num());

	// スロットが選択されていない場合は無視
	if (CurrentDeductionSlot < 0 || CurrentDeductionSlot >= DeductionSlots.Num())
	{
		UE_LOG(LogLastWitness, Warning, TEXT("[MainGameWidget] スロットが選択されていません。まずスロットをクリックしてください。"));
		if (DeductionResultText)
		{
			DeductionResultText->SetText(FText::FromString(TEXT("まずスロットをクリックしてください")));
		}
		return;
	}

	// 既に同じ証拠がスロットにある場合は無視
	for (const FName& SlotEvidence : DeductionSlotEvidence)
	{
		if (SlotEvidence == EvidenceId)
		{
			if (DeductionResultText)
			{
				DeductionResultText->SetText(FText::FromString(TEXT("同じ証拠は選択できません")));
			}
			return;
		}
	}

	// 証拠をスロットに設定
	if (CaseState)
	{
		FEvidence Evidence;
		if (CaseState->GetEvidenceById(EvidenceId, Evidence))
		{
			DeductionSlotEvidence[CurrentDeductionSlot] = EvidenceId;
			UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] 証拠をスロット%dに設定: %s"), CurrentDeductionSlot, *EvidenceId.ToString());

			if (DeductionSlots[CurrentDeductionSlot])
			{
				DeductionSlots[CurrentDeductionSlot]->SetEvidence(Evidence);
			}

			// 両方のスロットに証拠があるかチェック
			const bool bBothSlotsFilled = !DeductionSlotEvidence[0].IsNone() && !DeductionSlotEvidence[1].IsNone();
			UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] スロット状態 - 0: %s, 1: %s, 両方埋まった: %s"),
				*DeductionSlotEvidence[0].ToString(),
				*DeductionSlotEvidence[1].ToString(),
				bBothSlotsFilled ? TEXT("Yes") : TEXT("No"));

			// スロット選択を解除
			CurrentDeductionSlot = -1;
			for (UDeductionSlotWidget* SlotWidget : DeductionSlots)
			{
				if (SlotWidget)
				{
					SlotWidget->SetSelected(false);
				}
			}

			// 結果テキストを更新
			if (DeductionResultText)
			{
				if (bBothSlotsFilled)
				{
					DeductionResultText->SetText(FText::FromString(TEXT("「推理する」ボタンで結果を確認")));
				}
				else
				{
					DeductionResultText->SetText(FText::FromString(TEXT("もう1つの証拠を選択してください")));
				}
			}
		}
		else
		{
			UE_LOG(LogLastWitness, Warning, TEXT("[MainGameWidget] 証拠が見つかりません: %s"), *EvidenceId.ToString());
		}
	}
}
