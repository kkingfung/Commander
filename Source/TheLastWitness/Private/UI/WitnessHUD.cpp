// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/WitnessHUD.h"
#include "UI/MainGameWidget.h"
#include "Core/WitnessGameMode.h"
#include "Blueprint/UserWidget.h"
#include "TheLastWitness.h"

AWitnessHUD::AWitnessHUD()
{
}

void AWitnessHUD::BeginPlay()
{
	Super::BeginPlay();

	// GameModeを取得
	GameMode = Cast<AWitnessGameMode>(GetWorld()->GetAuthGameMode());

	// メインウィジェットを作成
	if (MainWidgetClass)
	{
		MainWidget = CreateWidget<UMainGameWidget>(GetOwningPlayerController(), MainWidgetClass);
		if (MainWidget)
		{
			MainWidget->AddToViewport();
			MainWidget->SetupWidget(GameMode);

			UE_LOG(LogLastWitness, Log, TEXT("[WitnessHUD] メインウィジェットを作成しました"));
		}
	}
	else
	{
		UE_LOG(LogLastWitness, Warning, TEXT("[WitnessHUD] MainWidgetClassが設定されていません"));
	}
}

void AWitnessHUD::SetUIVisible(bool bVisible)
{
	if (MainWidget)
	{
		MainWidget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}
