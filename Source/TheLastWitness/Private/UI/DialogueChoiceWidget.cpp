// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/DialogueChoiceWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "TheLastWitness.h"

void UDialogueChoiceWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDialogueChoiceWidget::SetChoiceData(const FDialogueChoice& InChoice)
{
	ChoiceData = InChoice;

	// 選択肢テキストを設定
	if (ChoiceText)
	{
		ChoiceText->SetText(ChoiceData.DisplayText);
	}

	// トーンインジケーターの色を設定
	if (ToneIndicator)
	{
		ToneIndicator->SetColorAndOpacity(GetToneColor());
	}

	// トーンラベルを設定
	if (ToneLabel)
	{
		ToneLabel->SetText(GetToneLabel());
	}

	// ボーダーの色をトーンに合わせる
	if (ChoiceBorder)
	{
		FLinearColor BorderColor = GetToneColor();
		BorderColor.A = 0.3f;  // 半透明
		ChoiceBorder->SetBrushColor(BorderColor);
	}

	UE_LOG(LogLastWitness, Log, TEXT("[DialogueChoice] 選択肢データを設定: %s"), *ChoiceData.ChoiceId.ToString());
}

void UDialogueChoiceWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	OnHoverStateChanged(true);
}

void UDialogueChoiceWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	OnHoverStateChanged(false);
}

FReply UDialogueChoiceWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		PlayClickFeedback();
		OnChoiceSelected.Broadcast(ChoiceData.ChoiceId);
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FLinearColor UDialogueChoiceWidget::GetToneColor() const
{
	switch (ChoiceData.Tone)
	{
	case EDialogueTone::Polite:
		return FLinearColor(0.4f, 0.6f, 0.8f, 1.0f);  // 青 - 礼儀正しい
	case EDialogueTone::Direct:
		return FLinearColor(0.7f, 0.7f, 0.7f, 1.0f);  // グレー - 直接的
	case EDialogueTone::Intimidating:
		return FLinearColor(0.8f, 0.3f, 0.3f, 1.0f);  // 赤 - 威圧的
	case EDialogueTone::Empathetic:
		return FLinearColor(0.4f, 0.7f, 0.5f, 1.0f);  // 緑 - 共感的
	case EDialogueTone::Cunning:
		return FLinearColor(0.7f, 0.5f, 0.8f, 1.0f);  // 紫 - 狡猾
	default:
		return FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);
	}
}

FText UDialogueChoiceWidget::GetToneLabel() const
{
	switch (ChoiceData.Tone)
	{
	case EDialogueTone::Polite:
		return FText::FromString(TEXT("[丁寧]"));
	case EDialogueTone::Direct:
		return FText::FromString(TEXT("[直接]"));
	case EDialogueTone::Intimidating:
		return FText::FromString(TEXT("[威圧]"));
	case EDialogueTone::Empathetic:
		return FText::FromString(TEXT("[共感]"));
	case EDialogueTone::Cunning:
		return FText::FromString(TEXT("[誘導]"));
	default:
		return FText::GetEmpty();
	}
}
