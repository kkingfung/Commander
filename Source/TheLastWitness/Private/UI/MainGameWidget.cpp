// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/MainGameWidget.h"
#include "Core/WitnessGameMode.h"
#include "Core/CaseState.h"
#include "Dialogue/DialogueManager.h"
#include "AI/ABELSystem.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
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

	// 初期状態はメインメニュー
	SwitchToPanel(EPanelIndex::MainMenu);
}

void UMainGameWidget::NativeDestruct()
{
	// イベントのアンバインド
	if (GameMode)
	{
		GameMode->OnPhaseChanged.RemoveDynamic(this, &UMainGameWidget::OnPhaseChanged);
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
		break;

	case EGamePhase::Accusation:
		SwitchToPanel(EPanelIndex::Accusation);
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
			// TODO: 選択肢ボタンウィジェットを動的に生成
			UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] 選択肢: %s"), *Choice.DisplayText.ToString());
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
		// TODO: 証拠アイテムウィジェットを動的に生成
		UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] 調査可能な証拠: %s"), *E.DisplayName.ToString());
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
		// TODO: キャラクターアイテムウィジェットを動的に生成
		UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] この場所のキャラクター: %s"), *C.DisplayName.ToString());
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
		if (ContinueDialogueButton && DialogueManager->IsAtEndNode())
		{
			// TODO: ボタンテキストを「終了」に変更
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
	for (const FABELSuggestion& S : Suggestions)
	{
		// TODO: 提案ウィジェットを動的に生成
		UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] ABEL提案: %s"), *S.Content.ToString());
	}

	// 保留中のコメントがあれば表示
	if (ABELSystem->HasPendingComment() && ABELMessageText)
	{
		ABELMessageText->SetText(ABELSystem->GetAndClearPendingComment());
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

	default:
		break;
	}
}

void UMainGameWidget::OnExamineClicked()
{
	// TODO: 調査可能な証拠を選択するポップアップを表示
	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] 調査ボタンがクリックされました"));
}

void UMainGameWidget::OnTravelClicked()
{
	// TODO: 移動先選択ポップアップを表示
	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] 移動ボタンがクリックされました"));
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
	// TODO: 証拠帳ウィジェットを表示
	UE_LOG(LogLastWitness, Log, TEXT("[MainGameWidget] 証拠帳ボタンがクリックされました"));
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
