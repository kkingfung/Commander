// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "WitnessHUD.generated.h"

class UMainGameWidget;
class AWitnessGameMode;

/// <summary>
/// ゲームのHUD管理クラス
/// </summary>
UCLASS()
class THELASTWITNESS_API AWitnessHUD : public AHUD
{
	GENERATED_BODY()

public:
	AWitnessHUD();

	virtual void BeginPlay() override;

	/// <summary>
	/// メインウィジェットを取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "UI")
	UMainGameWidget* GetMainWidget() const { return MainWidget; }

	/// <summary>
	/// UIを表示/非表示にします
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetUIVisible(bool bVisible);

protected:
	/// <summary>
	/// メインウィジェットのクラス
	/// </summary>
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UMainGameWidget> MainWidgetClass;

	/// <summary>
	/// メインウィジェットのインスタンス
	/// </summary>
	UPROPERTY()
	TObjectPtr<UMainGameWidget> MainWidget;

	/// <summary>
	/// GameModeへの参照
	/// </summary>
	UPROPERTY()
	TObjectPtr<AWitnessGameMode> GameMode;
};
