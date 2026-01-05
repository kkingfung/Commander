// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WitnessTypes.h"
#include "CaseState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEvidenceCollected, const FEvidence&, Evidence);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeductionUnlocked, const FDeduction&, Deduction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFlagSet, FName, FlagName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLocationVisited, ELocation, Location);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterTrustChanged, FName, CharacterId, int32, NewTrust);

/// <summary>
/// 現在の事件の進行状態を管理するクラス
/// </summary>
/// <remarks>
/// 収集した証拠、解放した推理、設定されたフラグなどを追跡します。
/// GameModeによって所有され、UIやシステムから参照されます。
/// </remarks>
UCLASS(BlueprintType)
class THELASTWITNESS_API UCaseState : public UObject
{
	GENERATED_BODY()

public:
	UCaseState();

	// ========================================================================
	// 初期化
	// ========================================================================

	/// <summary>
	/// 事件データで状態を初期化します
	/// </summary>
	/// <param name="CaseData">事件データ</param>
	UFUNCTION(BlueprintCallable, Category = "Case")
	void InitializeCase(const FCaseData& CaseData);

	/// <summary>
	/// 状態をリセットします
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Case")
	void ResetState();

	// ========================================================================
	// 証拠関連
	// ========================================================================

	/// <summary>
	/// 証拠を収集します
	/// </summary>
	/// <param name="EvidenceId">証拠ID</param>
	/// <returns>収集に成功したかどうか</returns>
	UFUNCTION(BlueprintCallable, Category = "Evidence")
	bool CollectEvidence(FName EvidenceId);

	/// <summary>
	/// 証拠を調査済みにします
	/// </summary>
	/// <param name="EvidenceId">証拠ID</param>
	UFUNCTION(BlueprintCallable, Category = "Evidence")
	void ExamineEvidence(FName EvidenceId);

	/// <summary>
	/// 証拠が収集済みか確認します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Evidence")
	bool HasEvidence(FName EvidenceId) const;

	/// <summary>
	/// 収集済みの全証拠を取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Evidence")
	TArray<FEvidence> GetCollectedEvidence() const;

	/// <summary>
	/// 証拠データを取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Evidence")
	bool GetEvidenceById(FName EvidenceId, FEvidence& OutEvidence) const;

	// ========================================================================
	// 推理関連
	// ========================================================================

	/// <summary>
	/// 2つの証拠を結びつけて推理を試みます
	/// </summary>
	/// <param name="EvidenceA">証拠A</param>
	/// <param name="EvidenceB">証拠B</param>
	/// <param name="OutDeduction">成功時の推理データ</param>
	/// <returns>有効な推理が成立したかどうか</returns>
	UFUNCTION(BlueprintCallable, Category = "Deduction")
	bool TryDeduction(FName EvidenceA, FName EvidenceB, FDeduction& OutDeduction);

	/// <summary>
	/// 解放済みの全推理を取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Deduction")
	TArray<FDeduction> GetUnlockedDeductions() const;

	/// <summary>
	/// 推理が解放済みか確認します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Deduction")
	bool IsDeductionUnlocked(FName DeductionId) const;

	// ========================================================================
	// フラグ関連
	// ========================================================================

	/// <summary>
	/// フラグを設定します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Flags")
	void SetFlag(FName FlagName);

	/// <summary>
	/// フラグが設定されているか確認します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Flags")
	bool HasFlag(FName FlagName) const;

	/// <summary>
	/// 複数のフラグが全て設定されているか確認します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Flags")
	bool HasAllFlags(const TArray<FName>& FlagNames) const;

	// ========================================================================
	// キャラクター関連
	// ========================================================================

	/// <summary>
	/// キャラクターデータを取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Characters")
	bool GetCharacterById(FName CharacterId, FCharacterData& OutCharacter) const;

	/// <summary>
	/// キャラクターの信頼度を変更します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Characters")
	void ModifyCharacterTrust(FName CharacterId, int32 Delta);

	/// <summary>
	/// キャラクターをインタビュー済みにします
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Characters")
	void MarkCharacterInterviewed(FName CharacterId);

	/// <summary>
	/// 全容疑者を取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Characters")
	TArray<FCharacterData> GetAllSuspects() const;

	// ========================================================================
	// ロケーション関連
	// ========================================================================

	/// <summary>
	/// 現在のロケーションを取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Location")
	ELocation GetCurrentLocation() const { return CurrentLocation; }

	/// <summary>
	/// ロケーションを移動します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Location")
	void TravelToLocation(ELocation NewLocation);

	/// <summary>
	/// ロケーションデータを取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Location")
	bool GetLocationData(ELocation Location, FLocationData& OutLocation) const;

	/// <summary>
	/// アクセス可能なロケーション一覧を取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Location")
	TArray<FLocationData> GetAccessibleLocations() const;

	// ========================================================================
	// 告発関連
	// ========================================================================

	/// <summary>
	/// 告発に必要な証拠が揃っているか確認します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Accusation")
	bool CanMakeAccusation() const;

	/// <summary>
	/// 犯人を告発します
	/// </summary>
	/// <param name="CharacterId">告発するキャラクターID</param>
	/// <returns>ゲーム結果</returns>
	UFUNCTION(BlueprintCallable, Category = "Accusation")
	FGameResult MakeAccusation(FName CharacterId);

	// ========================================================================
	// 統計関連
	// ========================================================================

	/// <summary>
	/// ABELの提案に従った回数を増やします
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void IncrementABELFollowed() { ABELSuggestionsFollowed++; }

	/// <summary>
	/// ABELの提案を無視した回数を増やします
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void IncrementABELIgnored() { ABELSuggestionsIgnored++; }

	/// <summary>
	/// 倫理的違反回数を増やします
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void IncrementEthicalViolations() { EthicalViolations++; }

	// ========================================================================
	// イベント
	// ========================================================================

	/// <summary>証拠収集時に発火</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEvidenceCollected OnEvidenceCollected;

	/// <summary>推理解放時に発火</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeductionUnlocked OnDeductionUnlocked;

	/// <summary>フラグ設定時に発火</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnFlagSet OnFlagSet;

	/// <summary>ロケーション訪問時に発火</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLocationVisited OnLocationVisited;

	/// <summary>キャラクター信頼度変化時に発火</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCharacterTrustChanged OnCharacterTrustChanged;

	// ========================================================================
	// データアクセス
	// ========================================================================

	/// <summary>
	/// 事件データを取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Case")
	const FCaseData& GetCaseData() const { return CaseData; }

protected:
	/// <summary>事件データ</summary>
	UPROPERTY()
	FCaseData CaseData;

	/// <summary>現在のロケーション</summary>
	UPROPERTY()
	ELocation CurrentLocation = ELocation::Office;

	/// <summary>設定されたフラグ</summary>
	UPROPERTY()
	TSet<FName> SetFlags;

	/// <summary>ABELに従った回数</summary>
	UPROPERTY()
	int32 ABELSuggestionsFollowed = 0;

	/// <summary>ABELを無視した回数</summary>
	UPROPERTY()
	int32 ABELSuggestionsIgnored = 0;

	/// <summary>倫理的違反回数</summary>
	UPROPERTY()
	int32 EthicalViolations = 0;

private:
	/// <summary>
	/// 内部的に証拠配列のインデックスを取得します
	/// </summary>
	int32 FindEvidenceIndex(FName EvidenceId) const;

	/// <summary>
	/// 内部的にキャラクター配列のインデックスを取得します
	/// </summary>
	int32 FindCharacterIndex(FName CharacterId) const;

	/// <summary>
	/// 内部的にロケーション配列のインデックスを取得します
	/// </summary>
	int32 FindLocationIndex(ELocation Location) const;

	/// <summary>
	/// 内部的に推理配列のインデックスを取得します
	/// </summary>
	int32 FindDeductionIndex(FName DeductionId) const;
};
