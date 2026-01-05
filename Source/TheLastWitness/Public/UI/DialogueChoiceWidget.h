// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/WitnessTypes.h"
#include "DialogueChoiceWidget.generated.h"

class UTextBlock;
class UButton;
class UBorder;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueChoiceSelected, FName, ChoiceId);

/// <summary>
/// 対話選択肢ウィジェット
/// </summary>
/// <remarks>
/// 対話中に表示される選択肢ボタン。
/// トーンに応じた色分けとホバー効果を持つ。
/// </remarks>
UCLASS()
class THELASTWITNESS_API UDialogueChoiceWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/// <summary>
	/// 選択肢データを設定します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SetChoiceData(const FDialogueChoice& InChoice);

	/// <summary>
	/// 選択肢IDを取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Dialogue")
	FName GetChoiceId() const { return ChoiceData.ChoiceId; }

	/// <summary>選択時のイベント</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDialogueChoiceSelected OnChoiceSelected;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/// <summary>
	/// ホバー状態変更時
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnHoverStateChanged(bool bIsHovered);

	/// <summary>
	/// クリック時のフィードバック
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void PlayClickFeedback();

	// ========================================================================
	// バインドウィジェット
	// ========================================================================

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> ChoiceBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ChoiceText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> ToneIndicator;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ToneLabel;

private:
	/// <summary>保持している選択肢データ</summary>
	FDialogueChoice ChoiceData;

	/// <summary>
	/// トーンに応じた色を取得します
	/// </summary>
	FLinearColor GetToneColor() const;

	/// <summary>
	/// トーンの表示ラベルを取得します
	/// </summary>
	FText GetToneLabel() const;
};
