// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/DeductionSlotWidget.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "TheLastWitness.h"

void UDeductionSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ClearButton)
	{
		ClearButton->OnClicked.AddDynamic(this, &UDeductionSlotWidget::OnClearButtonClicked);
		ClearButton->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UDeductionSlotWidget::SetSlotIndex(int32 InIndex)
{
	SlotIndex = InIndex;

	if (SlotLabelText)
	{
		SlotLabelText->SetText(FText::FromString(FString::Printf(TEXT("証拠 %d"), SlotIndex + 1)));
	}
}

void UDeductionSlotWidget::SetEvidence(const FEvidence& InEvidence)
{
	CurrentEvidence = InEvidence;
	bHasEvidence = true;

	if (EvidenceNameText)
	{
		EvidenceNameText->SetText(CurrentEvidence.DisplayName);
		EvidenceNameText->SetVisibility(ESlateVisibility::Visible);
	}

	if (SlotLabelText)
	{
		SlotLabelText->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (ClearButton)
	{
		ClearButton->SetVisibility(ESlateVisibility::Visible);
	}

	OnEvidenceChanged(true);

	UE_LOG(LogLastWitness, Log, TEXT("[DeductionSlot] スロット%dに証拠を設定: %s"), SlotIndex, *CurrentEvidence.EvidenceId.ToString());
}

void UDeductionSlotWidget::ClearEvidence()
{
	CurrentEvidence = FEvidence();
	bHasEvidence = false;

	if (EvidenceNameText)
	{
		EvidenceNameText->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (SlotLabelText)
	{
		SlotLabelText->SetVisibility(ESlateVisibility::Visible);
	}

	if (ClearButton)
	{
		ClearButton->SetVisibility(ESlateVisibility::Collapsed);
	}

	OnEvidenceChanged(false);
	OnSlotCleared.Broadcast(SlotIndex);

	UE_LOG(LogLastWitness, Log, TEXT("[DeductionSlot] スロット%dをクリア"), SlotIndex);
}

FReply UDeductionSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (!bHasEvidence)
		{
			OnSlotClicked.Broadcast(SlotIndex);
		}
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UDeductionSlotWidget::OnClearButtonClicked()
{
	ClearEvidence();
}

void UDeductionSlotWidget::SetSelected(bool bInSelected)
{
	if (bCurrentlySelected != bInSelected)
	{
		bCurrentlySelected = bInSelected;

		// 視覚的なフィードバック：ボーダーの色を変更
		if (SlotBorder)
		{
			if (bCurrentlySelected)
			{
				// 選択時は金色のハイライト
				SlotBorder->SetBrushColor(FLinearColor(0.78f, 0.63f, 0.15f, 1.0f)); // #C9A227
			}
			else
			{
				// 非選択時は通常色
				SlotBorder->SetBrushColor(FLinearColor(0.1f, 0.08f, 0.07f, 1.0f)); // #1A1412
			}
		}

		OnSelectionChanged(bCurrentlySelected);
	}
}
