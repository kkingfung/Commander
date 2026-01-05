// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/WitnessTypes.h"
#include "ABELSuggestionWidget.generated.h"

class UTextBlock;
class UButton;
class UBorder;
class UImage;
class UProgressBar;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSuggestionAccepted, FName, SuggestionId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSuggestionIgnored, FName, SuggestionId);

/// <summary>
/// ABEL提案ウィジェット
/// </summary>
/// <remarks>
/// ABELのAI提案を表示するカード。
/// 確信度バーと承認/無視ボタンを持つ。
/// </remarks>
UCLASS()
class THELASTWITNESS_API UABELSuggestionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/// <summary>
	/// 提案データを設定します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "ABEL")
	void SetSuggestionData(const FABELSuggestion& InSuggestion);

	/// <summary>
	/// 提案IDを取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "ABEL")
	FName GetSuggestionId() const { return SuggestionData.SuggestionId; }

	/// <summary>提案承認時のイベント</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSuggestionAccepted OnAccepted;

	/// <summary>提案無視時のイベント</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSuggestionIgnored OnIgnored;

protected:
	virtual void NativeConstruct() override;

	/// <summary>
	/// 承認ボタンクリック時
	/// </summary>
	UFUNCTION()
	void OnAcceptClicked();

	/// <summary>
	/// 無視ボタンクリック時
	/// </summary>
	UFUNCTION()
	void OnIgnoreClicked();

	/// <summary>
	/// タイプアイコンを更新します（BP実装可能）
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateTypeIcon(EABELSuggestionType Type);

	// ========================================================================
	// バインドウィジェット
	// ========================================================================

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> SuggestionBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SuggestionText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SuggestionTypeText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> ConfidenceBar;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ConfidenceText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> AcceptButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> IgnoreButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> TypeIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> EthicalWarningIcon;

private:
	/// <summary>保持している提案データ</summary>
	FABELSuggestion SuggestionData;

	/// <summary>
	/// 提案タイプの表示名を取得します
	/// </summary>
	FText GetSuggestionTypeDisplayName() const;

	/// <summary>
	/// 確信度に応じた色を取得します
	/// </summary>
	FLinearColor GetConfidenceColor() const;
};
