// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/WitnessTypes.h"
#include "CharacterCardWidget.generated.h"

class UTextBlock;
class UImage;
class UButton;
class UBorder;
class UProgressBar;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterCardClicked, FName, CharacterId);

/// <summary>
/// キャラクターカードウィジェット
/// </summary>
/// <remarks>
/// 容疑者や証人を表示するカード型ウィジェット。
/// 信頼度と感情状態を視覚的に表示する。
/// </remarks>
UCLASS()
class THELASTWITNESS_API UCharacterCardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/// <summary>
	/// キャラクターデータを設定します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Character")
	void SetCharacterData(const FCharacterData& InCharacter);

	/// <summary>
	/// キャラクターIDを取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "Character")
	FName GetCharacterId() const { return CharacterData.CharacterId; }

	/// <summary>カードクリック時のイベント</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCharacterCardClicked OnCardClicked;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/// <summary>
	/// ホバー状態変更時
	/// </summary>
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnHoverStateChanged(bool bIsHovered);

	// ========================================================================
	// バインドウィジェット
	// ========================================================================

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> CardBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CharacterNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CharacterRoleText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> CharacterPortrait;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> TrustBar;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TrustLevelText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EmotionText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> InterviewedIndicator;

private:
	/// <summary>保持しているキャラクターデータ</summary>
	FCharacterData CharacterData;

	/// <summary>
	/// 感情状態の表示テキストを取得します
	/// </summary>
	FText GetEmotionDisplayText() const;

	/// <summary>
	/// 信頼度に応じた色を取得します
	/// </summary>
	FLinearColor GetTrustColor() const;
};
