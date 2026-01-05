// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/WitnessTypes.h"
#include "EvidenceCardWidget.generated.h"

class UTextBlock;
class UImage;
class UButton;
class UBorder;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEvidenceCardClicked, FName, EvidenceId);

/// <summary>
/// 証拠カードウィジェット
/// </summary>
/// <remarks>
/// 証拠アイテムを表示するカード型ウィジェット。
/// ホバー効果とクリック処理を持つ。
/// </remarks>
UCLASS()
class THELASTWITNESS_API UEvidenceCardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/// <summary>
	/// 証拠データを設定します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Evidence")
	void SetEvidenceData(const FEvidence& InEvidence);

	/// <summary>
	/// 証拠IDを取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Evidence")
	FName GetEvidenceId() const { return EvidenceData.EvidenceId; }

	/// <summary>
	/// 選択状態を設定します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Evidence")
	void SetSelected(bool bInSelected);

	/// <summary>カードクリック時のイベント</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEvidenceCardClicked OnCardClicked;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/// <summary>
	/// ホバー状態を更新します
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnHoverStateChanged(bool bIsHovered);

	/// <summary>
	/// 選択状態を更新します
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnSelectionStateChanged(bool bIsSelected);

	// ========================================================================
	// バインドウィジェット
	// ========================================================================

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> CardBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EvidenceNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EvidenceTypeText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> EvidenceIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> ImportanceIndicator;

private:
	/// <summary>保持している証拠データ</summary>
	FEvidence EvidenceData;

	/// <summary>選択状態</summary>
	bool bCurrentlySelected = false;

	/// <summary>ホバー状態</summary>
	bool bCurrentlyHovered = false;

	/// <summary>
	/// 重要度に応じた色を取得します
	/// </summary>
	FLinearColor GetImportanceColor() const;

	/// <summary>
	/// 証拠タイプの表示名を取得します
	/// </summary>
	FText GetEvidenceTypeDisplayName() const;
};
