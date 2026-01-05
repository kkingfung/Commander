// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WitnessTypes.h"
#include "WitnessGameMode.generated.h"

class UCaseState;
class UDialogueManager;
class UABELSystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged, EGamePhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCaseStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameEnded, const FGameResult&, Result);

/// <summary>
/// ゲームの進行を管理するGameMode
/// </summary>
/// <remarks>
/// フェーズ遷移、事件データの読み込み、各サブシステムの連携を担当します。
/// </remarks>
UCLASS()
class THELASTWITNESS_API AWitnessGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AWitnessGameMode();

	// ========================================================================
	// ゲームサイクル
	// ========================================================================

	virtual void BeginPlay() override;

	/// <summary>
	/// 事件を開始します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Game")
	void StartCase();

	/// <summary>
	/// 現在のフェーズを取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Game")
	EGamePhase GetCurrentPhase() const { return CurrentPhase; }

	/// <summary>
	/// フェーズを変更します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Game")
	void SetPhase(EGamePhase NewPhase);

	// ========================================================================
	// ロケーション
	// ========================================================================

	/// <summary>
	/// ロケーションに移動します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Location")
	void TravelToLocation(ELocation Location);

	/// <summary>
	/// 現在のロケーションで利用可能なアクションを取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Location")
	TArray<FName> GetAvailableActionsAtLocation() const;

	// ========================================================================
	// 証拠調査
	// ========================================================================

	/// <summary>
	/// 現在のロケーションで調査可能な証拠を取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Investigation")
	TArray<FEvidence> GetExaminableEvidenceAtLocation() const;

	/// <summary>
	/// 証拠を調査します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Investigation")
	void ExamineEvidence(FName EvidenceId);

	/// <summary>
	/// 証拠を収集します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Investigation")
	void CollectEvidence(FName EvidenceId);

	// ========================================================================
	// 対話
	// ========================================================================

	/// <summary>
	/// 現在のロケーションで話しかけられるキャラクターを取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Dialogue")
	TArray<FCharacterData> GetCharactersAtLocation() const;

	/// <summary>
	/// キャラクターとの対話を開始します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartDialogueWith(FName CharacterId);

	/// <summary>
	/// 対話選択肢を選びます
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SelectDialogueChoice(FName ChoiceId);

	/// <summary>
	/// 対話を終了します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void EndDialogue();

	// ========================================================================
	// ABEL操作
	// ========================================================================

	/// <summary>
	/// ABELに分析を依頼します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "ABEL")
	void RequestABELAnalysis();

	/// <summary>
	/// ABELの提案に従います
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "ABEL")
	void FollowABELSuggestion(FName SuggestionId);

	/// <summary>
	/// ABELの提案を無視します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "ABEL")
	void IgnoreABELSuggestion(FName SuggestionId);

	// ========================================================================
	// 推理ボード
	// ========================================================================

	/// <summary>
	/// 推理ボードを開きます
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Deduction")
	void OpenDeductionBoard();

	/// <summary>
	/// 2つの証拠を結びつけて推理を試みます
	/// </summary>
	/// <param name="EvidenceA">証拠A</param>
	/// <param name="EvidenceB">証拠B</param>
	/// <param name="OutDeduction">成功時の推理データ</param>
	/// <returns>推理が成功したかどうか</returns>
	UFUNCTION(BlueprintCallable, Category = "Deduction")
	bool TryMakeDeduction(FName EvidenceA, FName EvidenceB, FDeduction& OutDeduction);

	// ========================================================================
	// 告発
	// ========================================================================

	/// <summary>
	/// 告発モードを開始します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Accusation")
	void BeginAccusation();

	/// <summary>
	/// 犯人を告発します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Accusation")
	void AccuseCharacter(FName CharacterId);

	// ========================================================================
	// サブシステムアクセス
	// ========================================================================

	/// <summary>
	/// CaseStateを取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Systems")
	UCaseState* GetCaseState() const { return CaseState; }

	/// <summary>
	/// DialogueManagerを取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Systems")
	UDialogueManager* GetDialogueManager() const { return DialogueManager; }

	/// <summary>
	/// ABELSystemを取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Systems")
	UABELSystem* GetABELSystem() const { return ABELSystem; }

	// ========================================================================
	// イベント
	// ========================================================================

	/// <summary>フェーズ変更時に発火</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPhaseChanged OnPhaseChanged;

	/// <summary>事件開始時に発火</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCaseStarted OnCaseStarted;

	/// <summary>ゲーム終了時に発火</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGameEnded OnGameEnded;

protected:
	/// <summary>
	/// 事件データを初期化します（子クラスでオーバーライド可能）
	/// </summary>
	UFUNCTION(BlueprintNativeEvent, Category = "Setup")
	FCaseData CreateCaseData();

	/// <summary>
	/// サブシステムを初期化します
	/// </summary>
	virtual void InitializeSubsystems();

	/// <summary>現在のフェーズ</summary>
	UPROPERTY(BlueprintReadOnly, Category = "State")
	EGamePhase CurrentPhase = EGamePhase::MainMenu;

	/// <summary>事件状態管理</summary>
	UPROPERTY()
	TObjectPtr<UCaseState> CaseState;

	/// <summary>対話管理</summary>
	UPROPERTY()
	TObjectPtr<UDialogueManager> DialogueManager;

	/// <summary>ABELシステム</summary>
	UPROPERTY()
	TObjectPtr<UABELSystem> ABELSystem;
};
