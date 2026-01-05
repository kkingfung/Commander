// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/LocationCardWidget.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "TheLastWitness.h"

void ULocationCardWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void ULocationCardWidget::SetLocationData(const FLocationData& InLocation)
{
	LocationData = InLocation;

	if (LocationNameText)
	{
		LocationNameText->SetText(LocationData.DisplayName);
	}

	if (LocationDescText)
	{
		LocationDescText->SetText(LocationData.Description);
	}

	if (VisitedIndicator)
	{
		VisitedIndicator->SetVisibility(
			LocationData.bHasVisited ? ESlateVisibility::Visible : ESlateVisibility::Hidden
		);
	}

	UE_LOG(LogLastWitness, Log, TEXT("[LocationCard] ロケーションデータを設定: %d"), static_cast<int32>(LocationData.Location));
}

void ULocationCardWidget::SetAsCurrent(bool bIsCurrent)
{
	bIsCurrentLocation = bIsCurrent;

	if (CurrentIndicator)
	{
		CurrentIndicator->SetVisibility(
			bIsCurrent ? ESlateVisibility::Visible : ESlateVisibility::Hidden
		);
	}

	OnCurrentStateChanged(bIsCurrent);
}

void ULocationCardWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	OnHoverStateChanged(true);
}

void ULocationCardWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	OnHoverStateChanged(false);
}

FReply ULocationCardWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (!bIsCurrentLocation && LocationData.bIsAccessible)
		{
			OnCardClicked.Broadcast(LocationData.Location);
		}
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
