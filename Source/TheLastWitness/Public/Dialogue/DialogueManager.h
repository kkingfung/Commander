// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Core/WitnessTypes.h"
#include "DialogueManager.generated.h"

class UCaseState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueStarted, FName, CharacterId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueNodeChanged, const FDialogueNode&, Node);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChoicesAvailable, const TArray<FDialogueChoice>&, Choices);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEvidenceGainedFromDialogue, const TArray<FName>&, EvidenceIds);

/// <summary>
/// 対話システムを管理するクラス
/// </summary>
/// <remarks>
/// 対話ツリーの進行、選択肢の提示、証拠の取得などを管理します。
/// </remarks>
UCLASS(BlueprintType)
class THELASTWITNESS_API UDialogueManager : public UObject
{
	GENERATED_BODY()

public:
	UDialogueManager();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="InCaseState">事件状態への参照</param>
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void Initialize(UCaseState* InCaseState);

	// ========================================================================
	// 対話制御
	// ========================================================================

	/// <summary>
	/// キャラクターとの対話を開始します
	/// </summary>
	/// <param name="CharacterId">キャラクターID</param>
	/// <returns>対話が開始できたかどうか</returns>
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool StartDialogue(FName CharacterId);

	/// <summary>
	/// 選択肢を選びます
	/// </summary>
	/// <param name="ChoiceId">選択肢ID</param>
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SelectChoice(FName ChoiceId);

	/// <summary>
	/// 次のノードに進みます（選択肢がない場合）
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void AdvanceDialogue();

	/// <summary>
	/// 対話を終了します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void EndDialogue();

	// ========================================================================
	// 状態取得
	// ========================================================================

	/// <summary>
	/// 対話中かどうかを取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Dialogue")
	bool IsInDialogue() const { return bIsInDialogue; }

	/// <summary>
	/// 現在の対話相手を取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Dialogue")
	FName GetCurrentCharacterId() const { return CurrentCharacterId; }

	/// <summary>
	/// 現在のノードを取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Dialogue")
	bool GetCurrentNode(FDialogueNode& OutNode) const;

	/// <summary>
	/// 現在利用可能な選択肢を取得します（条件を満たすもののみ）
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Dialogue")
	TArray<FDialogueChoice> GetAvailableChoices() const;

	/// <summary>
	/// 選択肢があるかどうかを取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Dialogue")
	bool HasChoices() const;

	/// <summary>
	/// 現在のノードが終了ノードかどうかを取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Dialogue")
	bool IsAtEndNode() const;

	// ========================================================================
	// 対話ツリー管理
	// ========================================================================

	/// <summary>
	/// 対話ツリーを登録します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void RegisterDialogueTree(const FDialogueTree& Tree);

	/// <summary>
	/// キャラクターの対話ツリーを取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Dialogue")
	bool GetDialogueTreeForCharacter(FName CharacterId, FDialogueTree& OutTree) const;

	// ========================================================================
	// イベント
	// ========================================================================

	/// <summary>対話開始時に発火</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDialogueStarted OnDialogueStarted;

	/// <summary>対話終了時に発火</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDialogueEnded OnDialogueEnded;

	/// <summary>ノード変更時に発火</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDialogueNodeChanged OnDialogueNodeChanged;

	/// <summary>選択肢が利用可能になった時に発火</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnChoicesAvailable OnChoicesAvailable;

	/// <summary>対話から証拠を取得した時に発火</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEvidenceGainedFromDialogue OnEvidenceGainedFromDialogue;

protected:
	/// <summary>
	/// ノードに移動します
	/// </summary>
	void GoToNode(FName NodeId);

	/// <summary>
	/// 選択肢が表示条件を満たすか確認します
	/// </summary>
	bool CanShowChoice(const FDialogueChoice& Choice) const;

	/// <summary>
	/// ノードから得られる証拠を処理します
	/// </summary>
	void ProcessNodeEvidence(const FDialogueNode& Node);

	/// <summary>CaseStateへの参照</summary>
	UPROPERTY()
	TObjectPtr<UCaseState> CaseState;

	/// <summary>登録された対話ツリー</summary>
	UPROPERTY()
	TMap<FName, FDialogueTree> DialogueTrees;

	/// <summary>対話中かどうか</summary>
	UPROPERTY()
	bool bIsInDialogue = false;

	/// <summary>現在の対話相手</summary>
	UPROPERTY()
	FName CurrentCharacterId;

	/// <summary>現在の対話ツリー</summary>
	UPROPERTY()
	FDialogueTree CurrentTree;

	/// <summary>現在のノードID</summary>
	UPROPERTY()
	FName CurrentNodeId;

private:
	/// <summary>
	/// ノードをIDで検索します
	/// </summary>
	const FDialogueNode* FindNodeById(FName NodeId) const;

	/// <summary>
	/// 選択肢をIDで検索します
	/// </summary>
	const FDialogueChoice* FindChoiceById(FName ChoiceId) const;
};
