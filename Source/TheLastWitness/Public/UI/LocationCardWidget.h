// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/WitnessTypes.h"
#include "LocationCardWidget.generated.h"

class UTextBlock;
class UBorder;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLocationCardClicked, ELocation, Location);

/// <summary>
/// ロケーションカードウィジェット
/// </summary>
/// <remarks>
/// 移動先選択時に表示されるロケーションカード。
/// 訪問済みかどうかを視覚的に表示する。
/// </remarks>
UCLASS()
class THELASTWITNESS_API ULocationCardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/// <summary>
	/// ロケーションデータを設定します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Location")
	void SetLocationData(const FLocationData& InLocation);

	/// <summary>
	/// ロケーションを取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Location")
	ELocation GetLocation() const { return LocationData.Location; }

	/// <summary>
	/// 現在地として設定します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Location")
	void SetAsCurrent(bool bIsCurrent);

	/// <summary>カードクリック時のイベント</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLocationCardClicked OnCardClicked;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnHoverStateChanged(bool bIsHovered);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnCurrentStateChanged(bool bIsCurrent);

	// バインドウィジェット
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> CardBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LocationNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> LocationDescText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> VisitedIndicator;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> CurrentIndicator;

private:
	FLocationData LocationData;
	bool bIsCurrentLocation = false;
};
