// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/SuspectCardWidget.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "TheLastWitness.h"

void USuspectCardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AccuseButton)
	{
		AccuseButton->OnClicked.AddDynamic(this, &USuspectCardWidget::OnAccuseButtonClicked);
	}
}

void USuspectCardWidget::SetSuspectData(const FCharacterData& InCharacter)
{
	CharacterData = InCharacter;

	if (SuspectNameText)
	{
		SuspectNameText->SetText(CharacterData.DisplayName);
	}

	if (SuspectRoleText)
	{
		SuspectRoleText->SetText(CharacterData.Role);
	}

	if (MotiveText)
	{
		MotiveText->SetText(CharacterData.Motive);
	}

	if (RelationText)
	{
		RelationText->SetText(CharacterData.RelationToVictim);
	}

	UE_LOG(LogLastWitness, Log, TEXT("[SuspectCard] 容疑者データを設定: %s"), *CharacterData.CharacterId.ToString());
}

void USuspectCardWidget::OnAccuseButtonClicked()
{
	OnAccused.Broadcast(CharacterData.CharacterId);
}

void USuspectCardWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	OnHoverStateChanged(true);
}

void USuspectCardWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	OnHoverStateChanged(false);
}
