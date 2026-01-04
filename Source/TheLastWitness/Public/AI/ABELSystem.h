// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Core/WitnessTypes.h"
#include "ABELSystem.generated.h"

class UCaseState;
class ULLMIntegration;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnABELSuggestionReady, const FABELSuggestion&, Suggestion);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnABELSpeaks, const FText&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnABELDispositionChanged, EABELDisposition, NewDisposition);

/// <summary>
/// ABELシステム - AI分析パートナー
/// </summary>
/// <remarks>
/// プレイヤーに提案を行い、証拠を分析し、推理を支援します。
/// スクリプト化されたコア機能とLLM統合によるフレーバーテキストを持ちます。
/// </remarks>
UCLASS(BlueprintType)
class THELASTWITNESS_API UABELSystem : public UObject
{
	GENERATED_BODY()

public:
	UABELSystem();

	/// <summary>
	/// 初期化
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "ABEL")
	void Initialize(UCaseState* InCaseState);

	/// <summary>
	/// 事件開始時の処理
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "ABEL")
	void OnCaseStarted();

	// ========================================================================
	// 提案生成
	// ========================================================================

	/// <summary>
	/// 現在の状態に基づいて提案を生成します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "ABEL")
	void GenerateSuggestions();

	/// <summary>
	/// 利用可能な提案を取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "ABEL")
	TArray<FABELSuggestion> GetCurrentSuggestions() const { return CurrentSuggestions; }

	/// <summary>
	/// 提案に従った時の処理
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "ABEL")
	void OnSuggestionFollowed(FName SuggestionId);

	/// <summary>
	/// 提案を無視した時の処理
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "ABEL")
	void OnSuggestionIgnored(FName SuggestionId);

	// ========================================================================
	// 分析機能
	// ========================================================================

	/// <summary>
	/// 証拠を分析してコメントを生成します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "ABEL")
	FText AnalyzeEvidence(const FEvidence& Evidence);

	/// <summary>
	/// 2つの証拠の関連性を分析します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "ABEL")
	FText AnalyzeConnection(const FEvidence& EvidenceA, const FEvidence& EvidenceB);

	/// <summary>
	/// キャラクターについての所見を生成します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "ABEL")
	FText AnalyzeCharacter(const FCharacterData& Character);

	// ========================================================================
	// イベントハンドラ
	// ========================================================================

	/// <summary>
	/// 証拠が調査された時
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "ABEL")
	void OnEvidenceExamined(const FEvidence& Evidence);

	/// <summary>
	/// 証拠が収集された時
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "ABEL")
	void OnEvidenceCollected(const FEvidence& Evidence);

	/// <summary>
	/// ロケーションが変わった時
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "ABEL")
	void OnLocationChanged(ELocation NewLocation);

	/// <summary>
	/// 対話が開始された時
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "ABEL")
	void OnDialogueStarted(FName CharacterId);

	/// <summary>
	/// 推理が成功した時
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "ABEL")
	void OnDeductionMade(const FDeduction& Deduction);

	// ========================================================================
	// 性格・状態
	// ========================================================================

	/// <summary>
	/// 現在の性格傾向を取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "ABEL")
	EABELDisposition GetCurrentDisposition() const { return CurrentDisposition; }

	/// <summary>
	/// ABELとの関係値を取得します（-100 ～ 100）
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "ABEL")
	int32 GetRelationshipValue() const { return RelationshipValue; }

	/// <summary>
	/// ABELが「話したい」状態かどうか
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "ABEL")
	bool HasPendingComment() const { return bHasPendingComment; }

	/// <summary>
	/// 保留中のコメントを取得してクリアします
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "ABEL")
	FText GetAndClearPendingComment();

	// ========================================================================
	// LLM設定
	// ========================================================================

	/// <summary>
	/// LLMを使用するかどうかを設定します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "ABEL|LLM")
	void SetUseLLM(bool bUse) { bUseLLM = bUse; }

	/// <summary>
	/// LLMを使用しているかどうか
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "ABEL|LLM")
	bool IsUsingLLM() const { return bUseLLM; }

	// ========================================================================
	// イベント
	// ========================================================================

	/// <summary>提案が準備できた時に発火</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnABELSuggestionReady OnSuggestionReady;

	/// <summary>ABELが発言する時に発火</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnABELSpeaks OnABELSpeaks;

	/// <summary>性格傾向が変化した時に発火</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnABELDispositionChanged OnDispositionChanged;

protected:
	/// <summary>
	/// スクリプト化された提案を生成します
	/// </summary>
	void GenerateScriptedSuggestions();

	/// <summary>
	/// 関連性の高い証拠の組み合わせを提案します
	/// </summary>
	void GenerateEvidenceConnectionSuggestions();

	/// <summary>
	/// 質問の提案を生成します
	/// </summary>
	void GenerateInterrogationSuggestions();

	/// <summary>
	/// 次のアクションの提案を生成します
	/// </summary>
	void GenerateNextActionSuggestions();

	/// <summary>
	/// 性格傾向を更新します
	/// </summary>
	void UpdateDisposition(int32 RelationshipDelta);

	/// <summary>
	/// ABELの発言を追加します
	/// </summary>
	void QueueComment(const FText& Comment);

	/// <summary>CaseStateへの参照</summary>
	UPROPERTY()
	TObjectPtr<UCaseState> CaseState;

	/// <summary>LLM統合（オプション）</summary>
	UPROPERTY()
	TObjectPtr<ULLMIntegration> LLMIntegration;

	/// <summary>現在の提案リスト</summary>
	UPROPERTY()
	TArray<FABELSuggestion> CurrentSuggestions;

	/// <summary>現在の性格傾向</summary>
	UPROPERTY()
	EABELDisposition CurrentDisposition = EABELDisposition::Analytical;

	/// <summary>関係値（-100 ～ 100）</summary>
	UPROPERTY()
	int32 RelationshipValue = 0;

	/// <summary>保留中のコメントがあるか</summary>
	UPROPERTY()
	bool bHasPendingComment = false;

	/// <summary>保留中のコメント</summary>
	UPROPERTY()
	FText PendingComment;

	/// <summary>LLMを使用するか</summary>
	UPROPERTY()
	bool bUseLLM = false;

	/// <summary>提案ID用のカウンター</summary>
	int32 SuggestionCounter = 0;

private:
	/// <summary>
	/// 一意の提案IDを生成します
	/// </summary>
	FName GenerateSuggestionId();
};
