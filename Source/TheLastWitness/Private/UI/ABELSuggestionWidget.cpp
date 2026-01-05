// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/ABELSuggestionWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "TheLastWitness.h"

void UABELSuggestionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// ボタンイベントをバインド
	if (AcceptButton)
	{
		AcceptButton->OnClicked.AddDynamic(this, &UABELSuggestionWidget::OnAcceptClicked);
	}

	if (IgnoreButton)
	{
		IgnoreButton->OnClicked.AddDynamic(this, &UABELSuggestionWidget::OnIgnoreClicked);
	}
}

void UABELSuggestionWidget::SetSuggestionData(const FABELSuggestion& InSuggestion)
{
	SuggestionData = InSuggestion;

	// 提案テキストを設定
	if (SuggestionText)
	{
		SuggestionText->SetText(SuggestionData.Content);
	}

	// タイプテキストを設定
	if (SuggestionTypeText)
	{
		SuggestionTypeText->SetText(GetSuggestionTypeDisplayName());
	}

	// 確信度バーを設定
	if (ConfidenceBar)
	{
		ConfidenceBar->SetPercent(SuggestionData.Confidence);
		ConfidenceBar->SetFillColorAndOpacity(GetConfidenceColor());
	}

	// 確信度テキストを設定
	if (ConfidenceText)
	{
		const int32 ConfidencePercent = FMath::RoundToInt(SuggestionData.Confidence * 100.0f);
		ConfidenceText->SetText(FText::FromString(FString::Printf(TEXT("確信度: %d%%"), ConfidencePercent)));
	}

	// 倫理的警告アイコンを設定
	if (EthicalWarningIcon)
	{
		EthicalWarningIcon->SetVisibility(
			SuggestionData.bIsEthicallyQuestionable ? ESlateVisibility::Visible : ESlateVisibility::Collapsed
		);
	}

	// タイプアイコンを更新（BP実装）
	UpdateTypeIcon(SuggestionData.Type);

	UE_LOG(LogLastWitness, Log, TEXT("[ABELSuggestion] 提案データを設定: %s"), *SuggestionData.SuggestionId.ToString());
}

void UABELSuggestionWidget::OnAcceptClicked()
{
	OnAccepted.Broadcast(SuggestionData.SuggestionId);
}

void UABELSuggestionWidget::OnIgnoreClicked()
{
	OnIgnored.Broadcast(SuggestionData.SuggestionId);
}

FText UABELSuggestionWidget::GetSuggestionTypeDisplayName() const
{
	switch (SuggestionData.Type)
	{
	case EABELSuggestionType::EvidenceConnection:
		return FText::FromString(TEXT("証拠の関連性"));
	case EABELSuggestionType::InterrogationTip:
		return FText::FromString(TEXT("尋問のヒント"));
	case EABELSuggestionType::Theory:
		return FText::FromString(TEXT("仮説"));
	case EABELSuggestionType::NextAction:
		return FText::FromString(TEXT("次の行動"));
	case EABELSuggestionType::Warning:
		return FText::FromString(TEXT("警告"));
	default:
		return FText::FromString(TEXT("提案"));
	}
}

FLinearColor UABELSuggestionWidget::GetConfidenceColor() const
{
	if (SuggestionData.Confidence >= 0.8f)
	{
		return FLinearColor(0.2f, 0.8f, 0.4f, 1.0f);  // 高確信 - 緑
	}
	else if (SuggestionData.Confidence >= 0.5f)
	{
		return FLinearColor(0.9f, 0.7f, 0.2f, 1.0f);  // 中確信 - 黄
	}
	else
	{
		return FLinearColor(0.8f, 0.4f, 0.2f, 1.0f);  // 低確信 - オレンジ
	}
}
