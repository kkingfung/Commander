// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/CharacterCardWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/ProgressBar.h"
#include "TheLastWitness.h"

void UCharacterCardWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCharacterCardWidget::SetCharacterData(const FCharacterData& InCharacter)
{
	CharacterData = InCharacter;

	// 名前を設定
	if (CharacterNameText)
	{
		CharacterNameText->SetText(CharacterData.DisplayName);
	}

	// 役職を設定
	if (CharacterRoleText)
	{
		CharacterRoleText->SetText(CharacterData.Role);
	}

	// 信頼度バーを設定
	if (TrustBar)
	{
		TrustBar->SetPercent(CharacterData.TrustLevel / 100.0f);
		TrustBar->SetFillColorAndOpacity(GetTrustColor());
	}

	// 感情状態を設定
	if (EmotionText)
	{
		EmotionText->SetText(GetEmotionDisplayText());
	}

	// インタビュー済みインジケーター
	if (InterviewedIndicator)
	{
		InterviewedIndicator->SetVisibility(
			CharacterData.bHasBeenInterviewed ? ESlateVisibility::Visible : ESlateVisibility::Hidden
		);
	}

	UE_LOG(LogLastWitness, Log, TEXT("[CharacterCard] キャラクターデータを設定: %s"), *CharacterData.CharacterId.ToString());
}

void UCharacterCardWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	OnHoverStateChanged(true);
}

void UCharacterCardWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	OnHoverStateChanged(false);
}

FReply UCharacterCardWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnCardClicked.Broadcast(CharacterData.CharacterId);
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FText UCharacterCardWidget::GetEmotionDisplayText() const
{
	switch (CharacterData.EmotionalState)
	{
	case EEmotionalState::Neutral:
		return FText::FromString(TEXT("平静"));
	case EEmotionalState::Nervous:
		return FText::FromString(TEXT("緊張"));
	case EEmotionalState::Angry:
		return FText::FromString(TEXT("怒り"));
	case EEmotionalState::Sad:
		return FText::FromString(TEXT("悲しみ"));
	case EEmotionalState::Fearful:
		return FText::FromString(TEXT("恐怖"));
	case EEmotionalState::Defensive:
		return FText::FromString(TEXT("警戒"));
	case EEmotionalState::Cooperative:
		return FText::FromString(TEXT("協力的"));
	default:
		return FText::FromString(TEXT("不明"));
	}
}

FLinearColor UCharacterCardWidget::GetTrustColor() const
{
	if (CharacterData.TrustLevel >= 70)
	{
		return FLinearColor(0.2f, 0.7f, 0.3f, 1.0f);  // 緑
	}
	else if (CharacterData.TrustLevel >= 40)
	{
		return FLinearColor(0.9f, 0.7f, 0.2f, 1.0f);  // 黄色
	}
	else
	{
		return FLinearColor(0.8f, 0.3f, 0.2f, 1.0f);  // 赤
	}
}
