// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/WitnessTypes.h"
#include "MainGameWidget.generated.h"

class AWitnessGameMode;
class UCaseState;
class UDialogueManager;
class UABELSystem;
class UTextBlock;
class UButton;
class UVerticalBox;
class UWidgetSwitcher;
class UScrollBox;

/// <summary>
/// メインゲームUIウィジェット
/// </summary>
/// <remarks>
/// ゲームの全UIパネルを管理し、フェーズに応じて表示を切り替えます。
/// </remarks>
UCLASS()
class THELASTWITNESS_API UMainGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/// <summary>
	/// GameModeを設定して初期化します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetupWidget(AWitnessGameMode* InGameMode);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ========================================================================
	// イベントハンドラ
	// ========================================================================

	/// <summary>
	/// フェーズ変更時
	/// </summary>
	UFUNCTION()
	void OnPhaseChanged(EGamePhase NewPhase);

	/// <summary>
	/// 対話ノード変更時
	/// </summary>
	UFUNCTION()
	void OnDialogueNodeChanged(const FDialogueNode& Node);

	/// <summary>
	/// 選択肢が利用可能になった時
	/// </summary>
	UFUNCTION()
	void OnChoicesAvailable(const TArray<FDialogueChoice>& Choices);

	/// <summary>
	/// ABELが発言した時
	/// </summary>
	UFUNCTION()
	void OnABELSpeaks(const FText& Message);

	/// <summary>
	/// 証拠収集時
	/// </summary>
	UFUNCTION()
	void OnEvidenceCollected(const FEvidence& Evidence);

	// ========================================================================
	// UI更新
	// ========================================================================

	/// <summary>
	/// ロケーションパネルを更新します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateLocationPanel();

	/// <summary>
	/// 証拠一覧を更新します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateEvidenceList();

	/// <summary>
	/// キャラクター一覧を更新します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateCharacterList();

	/// <summary>
	/// 対話パネルを更新します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateDialoguePanel();

	/// <summary>
	/// ABELパネルを更新します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateABELPanel();

	// ========================================================================
	// ボタンハンドラ
	// ========================================================================

	/// <summary>
	/// ゲーム開始ボタン
	/// </summary>
	UFUNCTION()
	void OnStartGameClicked();

	/// <summary>
	/// 続行ボタン
	/// </summary>
	UFUNCTION()
	void OnContinueClicked();

	/// <summary>
	/// 調査ボタン
	/// </summary>
	UFUNCTION()
	void OnExamineClicked();

	/// <summary>
	/// 移動ボタン
	/// </summary>
	UFUNCTION()
	void OnTravelClicked();

	/// <summary>
	/// ABEL相談ボタン
	/// </summary>
	UFUNCTION()
	void OnConsultABELClicked();

	/// <summary>
	/// 証拠帳ボタン
	/// </summary>
	UFUNCTION()
	void OnJournalClicked();

	/// <summary>
	/// 推理ボードボタン
	/// </summary>
	UFUNCTION()
	void OnDeductionBoardClicked();

	/// <summary>
	/// 告発ボタン
	/// </summary>
	UFUNCTION()
	void OnAccuseClicked();

	// ========================================================================
	// バインドウィジェット
	// ========================================================================

	/// <summary>パネル切り替え用</summary>
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> PanelSwitcher;

	// ----- メインメニューパネル -----
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartGameButton;

	// ----- 事件紹介パネル -----
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CaseTitleText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CaseSynopsisText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BeginInvestigationButton;

	// ----- 調査パネル -----
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LocationNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LocationDescriptionText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> EvidenceListBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> CharacterListBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ExamineButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> TravelButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ConsultABELButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> JournalButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> DeductionBoardButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AccuseButton;

	// ----- 対話パネル -----
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SpeakerNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DialogueText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> ChoicesBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ContinueDialogueButton;

	// ----- ABELパネル -----
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ABELMessageText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ABELSuggestionsBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseABELButton;

	// ----- 結果パネル -----
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ResultTitleText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ResultNarrativeText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ReturnToMenuButton;

	// ========================================================================
	// 参照
	// ========================================================================

	UPROPERTY()
	TObjectPtr<AWitnessGameMode> GameMode;

	UPROPERTY()
	TObjectPtr<UCaseState> CaseState;

	UPROPERTY()
	TObjectPtr<UDialogueManager> DialogueManager;

	UPROPERTY()
	TObjectPtr<UABELSystem> ABELSystem;

private:
	/// <summary>
	/// パネルインデックス定義
	/// </summary>
	enum class EPanelIndex : int32
	{
		MainMenu = 0,
		CaseIntro = 1,
		Investigation = 2,
		Dialogue = 3,
		ABEL = 4,
		Deduction = 5,
		Accusation = 6,
		Result = 7
	};

	/// <summary>
	/// 指定したパネルに切り替えます
	/// </summary>
	void SwitchToPanel(EPanelIndex PanelIndex);
};
