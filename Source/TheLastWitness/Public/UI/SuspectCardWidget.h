// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/WitnessTypes.h"
#include "SuspectCardWidget.generated.h"

class UTextBlock;
class UBorder;
class UImage;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSuspectAccused, FName, CharacterId);

/// <summary>
/// 容疑者カードウィジェット（告発用）
/// </summary>
/// <remarks>
/// 告発フェーズで表示される容疑者選択カード。
/// 動機や関連証拠を表示する。
/// </remarks>
UCLASS()
class THELASTWITNESS_API USuspectCardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/// <summary>
	/// 容疑者データを設定します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Suspect")
	void SetSuspectData(const FCharacterData& InCharacter);

	/// <summary>
	/// キャラクターIDを取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Suspect")
	FName GetCharacterId() const { return CharacterData.CharacterId; }

	/// <summary>告発時のイベント</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSuspectAccused OnAccused;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnAccuseButtonClicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnHoverStateChanged(bool bIsHovered);

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	// バインドウィジェット
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> CardBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SuspectNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SuspectRoleText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MotiveText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> RelationText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> SuspectPortrait;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AccuseButton;

private:
	FCharacterData CharacterData;
};
