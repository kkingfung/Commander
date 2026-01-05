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
class UEvidenceCardWidget;
class UCharacterCardWidget;
class UDialogueChoiceWidget;
class UABELSuggestionWidget;
class ULocationCardWidget;
class USuspectCardWidget;
class UDeductionSlotWidget;
class UEvidenceDetailWidget;
class UBorder;
class UImage;
class UOverlay;

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

	/// <summary>
	/// ゲーム終了時
	/// </summary>
	UFUNCTION()
	void OnGameEnded(const FGameResult& Result);

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
	// 動的ウィジェットクラス
	// ========================================================================

	/// <summary>証拠カードウィジェットクラス</summary>
	UPROPERTY(EditDefaultsOnly, Category = "Widget Classes")
	TSubclassOf<UEvidenceCardWidget> EvidenceCardClass;

	/// <summary>キャラクターカードウィジェットクラス</summary>
	UPROPERTY(EditDefaultsOnly, Category = "Widget Classes")
	TSubclassOf<UCharacterCardWidget> CharacterCardClass;

	/// <summary>対話選択肢ウィジェットクラス</summary>
	UPROPERTY(EditDefaultsOnly, Category = "Widget Classes")
	TSubclassOf<UDialogueChoiceWidget> DialogueChoiceClass;

	/// <summary>ABEL提案ウィジェットクラス</summary>
	UPROPERTY(EditDefaultsOnly, Category = "Widget Classes")
	TSubclassOf<UABELSuggestionWidget> ABELSuggestionClass;

	/// <summary>ロケーションカードウィジェットクラス</summary>
	UPROPERTY(EditDefaultsOnly, Category = "Widget Classes")
	TSubclassOf<ULocationCardWidget> LocationCardClass;

	/// <summary>容疑者カードウィジェットクラス</summary>
	UPROPERTY(EditDefaultsOnly, Category = "Widget Classes")
	TSubclassOf<USuspectCardWidget> SuspectCardClass;

	/// <summary>推理スロットウィジェットクラス</summary>
	UPROPERTY(EditDefaultsOnly, Category = "Widget Classes")
	TSubclassOf<UDeductionSlotWidget> DeductionSlotClass;

	// ========================================================================
	// 動的ウィジェットイベントハンドラ
	// ========================================================================

	/// <summary>証拠カードクリック時</summary>
	UFUNCTION()
	void OnEvidenceCardClicked(FName EvidenceId);

	/// <summary>キャラクターカードクリック時</summary>
	UFUNCTION()
	void OnCharacterCardClicked(FName CharacterId);

	/// <summary>対話選択肢選択時</summary>
	UFUNCTION()
	void OnDialogueChoiceSelected(FName ChoiceId);

	/// <summary>ABEL提案承認時</summary>
	UFUNCTION()
	void OnABELSuggestionAccepted(FName SuggestionId);

	/// <summary>ABEL提案無視時</summary>
	UFUNCTION()
	void OnABELSuggestionIgnored(FName SuggestionId);

	/// <summary>ロケーションカードクリック時</summary>
	UFUNCTION()
	void OnLocationCardClicked(ELocation Location);

	/// <summary>容疑者告発時</summary>
	UFUNCTION()
	void OnSuspectAccused(FName CharacterId);

	/// <summary>推理スロットクリック時</summary>
	UFUNCTION()
	void OnDeductionSlotClicked(int32 SlotIndex);

	/// <summary>推理スロットクリア時</summary>
	UFUNCTION()
	void OnDeductionSlotCleared(int32 SlotIndex);

	// ========================================================================
	// 推理・告発・移動パネル
	// ========================================================================

	/// <summary>移動パネルを表示します</summary>
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowTravelPanel();

	/// <summary>移動パネルを非表示にします</summary>
	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideTravelPanel();

	/// <summary>推理パネルを更新します</summary>
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateDeductionPanel();

	/// <summary>告発パネルを更新します</summary>
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateAccusationPanel();

	/// <summary>結果パネルを更新します</summary>
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateResultPanel(const FGameResult& Result);

	/// <summary>推理を試みます</summary>
	UFUNCTION(BlueprintCallable, Category = "UI")
	void TryMakeDeduction();

	/// <summary>証拠帳パネルを表示します</summary>
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowJournalPanel();

	/// <summary>証拠帳パネルを非表示にします</summary>
	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideJournalPanel();

	/// <summary>証拠詳細パネルを表示します</summary>
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowEvidenceDetail(const FEvidence& Evidence);

	/// <summary>証拠詳細パネルを非表示にします</summary>
	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideEvidenceDetail();

	/// <summary>証拠帳からの証拠カードクリック時</summary>
	UFUNCTION()
	void OnJournalEvidenceCardClicked(FName EvidenceId);

	/// <summary>推理用の証拠カードクリック時</summary>
	UFUNCTION()
	void OnDeductionEvidenceCardClicked(FName EvidenceId);

	// ----- 移動パネル (ポップアップ) -----
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> TravelPopupPanel;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UVerticalBox> LocationListBox;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> CloseTravelButton;

	// ----- 推理パネル -----
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UVerticalBox> CollectedEvidenceBox;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UVerticalBox> DeductionSlotsBox;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> MakeDeductionButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DeductionResultText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> CloseDeductionButton;

	// ----- 告発パネル -----
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UVerticalBox> SuspectListBox;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> CancelAccusationButton;

	// ----- 証拠帳パネル (ポップアップ) -----
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> JournalPopupPanel;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UVerticalBox> JournalEvidenceListBox;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EvidenceCountText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> CloseJournalButton;

	// ----- 証拠詳細パネル (ポップアップ) -----
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> EvidenceDetailPanel;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DetailEvidenceNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DetailEvidenceTypeText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DetailEvidenceDescText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DetailABELCommentText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> CloseDetailButton;

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

	/// <summary>推理スロットに設定された証拠ID（2つ）</summary>
	TArray<FName> DeductionSlotEvidence;

	/// <summary>現在選択中の推理スロットインデックス</summary>
	int32 CurrentDeductionSlot = -1;

	/// <summary>推理スロットウィジェットの参照</summary>
	UPROPERTY()
	TArray<TObjectPtr<UDeductionSlotWidget>> DeductionSlots;
};
