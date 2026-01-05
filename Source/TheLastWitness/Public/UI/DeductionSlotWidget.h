// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/WitnessTypes.h"
#include "DeductionSlotWidget.generated.h"

class UTextBlock;
class UBorder;
class UImage;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeductionSlotClicked, int32, SlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeductionSlotCleared, int32, SlotIndex);

/// <summary>
/// 推理スロットウィジェット
/// </summary>
/// <remarks>
/// 推理ボードで証拠を配置するスロット。
/// 2つのスロットを結びつけて推理を行う。
/// </remarks>
UCLASS()
class THELASTWITNESS_API UDeductionSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/// <summary>
	/// スロットインデックスを設定します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Deduction")
	void SetSlotIndex(int32 InIndex);

	/// <summary>
	/// 証拠を設定します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Deduction")
	void SetEvidence(const FEvidence& InEvidence);

	/// <summary>
	/// 証拠をクリアします
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Deduction")
	void ClearEvidence();

	/// <summary>
	/// 証拠が設定されているか
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Deduction")
	bool HasEvidence() const { return bHasEvidence; }

	/// <summary>
	/// 証拠IDを取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Deduction")
	FName GetEvidenceId() const { return CurrentEvidence.EvidenceId; }

	/// <summary>
	/// 選択状態を設定します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Deduction")
	void SetSelected(bool bInSelected);

	/// <summary>スロットクリック時のイベント</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeductionSlotClicked OnSlotClicked;

	/// <summary>スロットクリア時のイベント</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeductionSlotCleared OnSlotCleared;

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UFUNCTION()
	void OnClearButtonClicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnEvidenceChanged(bool bHasNewEvidence);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnSelectionChanged(bool bIsSelected);

	// バインドウィジェット
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> SlotBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SlotLabelText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EvidenceNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> ClearButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> EvidenceIcon;

private:
	int32 SlotIndex = 0;
	FEvidence CurrentEvidence;
	bool bHasEvidence = false;
	bool bCurrentlySelected = false;
};
