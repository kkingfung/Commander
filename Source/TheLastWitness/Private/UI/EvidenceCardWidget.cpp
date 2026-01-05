// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/EvidenceCardWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "TheLastWitness.h"

void UEvidenceCardWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UEvidenceCardWidget::SetEvidenceData(const FEvidence& InEvidence)
{
	EvidenceData = InEvidence;

	// 名前を設定
	if (EvidenceNameText)
	{
		EvidenceNameText->SetText(EvidenceData.DisplayName);
	}

	// タイプを設定
	if (EvidenceTypeText)
	{
		EvidenceTypeText->SetText(GetEvidenceTypeDisplayName());
	}

	// 重要度インジケーターの色を設定
	if (ImportanceIndicator)
	{
		ImportanceIndicator->SetColorAndOpacity(GetImportanceColor());
	}

	UE_LOG(LogLastWitness, Log, TEXT("[EvidenceCard] 証拠データを設定: %s"), *EvidenceData.EvidenceId.ToString());
}

void UEvidenceCardWidget::SetSelected(bool bInSelected)
{
	if (bCurrentlySelected != bInSelected)
	{
		bCurrentlySelected = bInSelected;
		OnSelectionStateChanged(bCurrentlySelected);
	}
}

void UEvidenceCardWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	bCurrentlyHovered = true;
	OnHoverStateChanged(true);
}

void UEvidenceCardWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	bCurrentlyHovered = false;
	OnHoverStateChanged(false);
}

FReply UEvidenceCardWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnCardClicked.Broadcast(EvidenceData.EvidenceId);
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FLinearColor UEvidenceCardWidget::GetImportanceColor() const
{
	switch (EvidenceData.Importance)
	{
	case EEvidenceImportance::Critical:
		return FLinearColor(0.8f, 0.2f, 0.2f, 1.0f);  // 赤
	case EEvidenceImportance::Major:
		return FLinearColor(0.9f, 0.6f, 0.1f, 1.0f);  // オレンジ
	case EEvidenceImportance::Normal:
		return FLinearColor(0.7f, 0.7f, 0.5f, 1.0f);  // 薄い金色
	case EEvidenceImportance::Minor:
	default:
		return FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);  // グレー
	}
}

FText UEvidenceCardWidget::GetEvidenceTypeDisplayName() const
{
	switch (EvidenceData.Type)
	{
	case EEvidenceType::Physical:
		return FText::FromString(TEXT("物的証拠"));
	case EEvidenceType::Document:
		return FText::FromString(TEXT("文書"));
	case EEvidenceType::Testimony:
		return FText::FromString(TEXT("証言"));
	case EEvidenceType::Observation:
		return FText::FromString(TEXT("観察"));
	default:
		return FText::FromString(TEXT("不明"));
	}
}
