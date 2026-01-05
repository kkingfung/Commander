// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/EvidenceDetailWidget.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "TheLastWitness.h"

void UEvidenceDetailWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &UEvidenceDetailWidget::OnCloseButtonClicked);
	}
}

void UEvidenceDetailWidget::ShowEvidence(const FEvidence& InEvidence)
{
	CurrentEvidence = InEvidence;

	if (EvidenceNameText)
	{
		EvidenceNameText->SetText(CurrentEvidence.DisplayName);
	}

	if (EvidenceTypeText)
	{
		EvidenceTypeText->SetText(GetEvidenceTypeText());
	}

	if (DescriptionText)
	{
		DescriptionText->SetText(CurrentEvidence.Description);
	}

	if (ABELCommentText)
	{
		if (CurrentEvidence.ABELComment.IsEmpty())
		{
			ABELCommentText->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			ABELCommentText->SetText(CurrentEvidence.ABELComment);
			ABELCommentText->SetVisibility(ESlateVisibility::Visible);
		}
	}

	if (ImportanceText)
	{
		ImportanceText->SetText(GetImportanceText());
	}

	if (FoundAtText)
	{
		FoundAtText->SetText(GetLocationText());
	}

	SetVisibility(ESlateVisibility::Visible);

	UE_LOG(LogLastWitness, Log, TEXT("[EvidenceDetail] 証拠を表示: %s"), *CurrentEvidence.EvidenceId.ToString());
}

void UEvidenceDetailWidget::Close()
{
	SetVisibility(ESlateVisibility::Collapsed);
	OnClosed.Broadcast();
}

void UEvidenceDetailWidget::OnCloseButtonClicked()
{
	Close();
}

FText UEvidenceDetailWidget::GetEvidenceTypeText() const
{
	switch (CurrentEvidence.Type)
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

FText UEvidenceDetailWidget::GetImportanceText() const
{
	switch (CurrentEvidence.Importance)
	{
	case EEvidenceImportance::Critical:
		return FText::FromString(TEXT("決定的証拠"));
	case EEvidenceImportance::Major:
		return FText::FromString(TEXT("重要"));
	case EEvidenceImportance::Normal:
		return FText::FromString(TEXT("通常"));
	case EEvidenceImportance::Minor:
		return FText::FromString(TEXT("補足"));
	default:
		return FText::FromString(TEXT("不明"));
	}
}

FText UEvidenceDetailWidget::GetLocationText() const
{
	switch (CurrentEvidence.FoundAt)
	{
	case ELocation::Study:
		return FText::FromString(TEXT("書斎"));
	case ELocation::DrawingRoom:
		return FText::FromString(TEXT("応接室"));
	case ELocation::ServantsQuarters:
		return FText::FromString(TEXT("使用人の部屋"));
	case ELocation::Garden:
		return FText::FromString(TEXT("庭園"));
	case ELocation::Factory:
		return FText::FromString(TEXT("工場"));
	case ELocation::Pub:
		return FText::FromString(TEXT("黒馬亭"));
	case ELocation::Office:
		return FText::FromString(TEXT("探偵事務所"));
	default:
		return FText::FromString(TEXT("不明"));
	}
}
