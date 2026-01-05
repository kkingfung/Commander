// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/WitnessTypes.h"
#include "EvidenceDetailWidget.generated.h"

class UTextBlock;
class UBorder;
class UImage;
class UButton;
class UScrollBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEvidenceDetailClosed);

/// <summary>
/// 証拠詳細ウィジェット
/// </summary>
/// <remarks>
/// 収集した証拠の詳細情報を表示するポップアップ。
/// ABELのコメントや関連情報を表示する。
/// </remarks>
UCLASS()
class THELASTWITNESS_API UEvidenceDetailWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/// <summary>
	/// 証拠データを設定して表示します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Evidence")
	void ShowEvidence(const FEvidence& InEvidence);

	/// <summary>
	/// ウィジェットを閉じます
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Evidence")
	void Close();

	/// <summary>閉じた時のイベント</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEvidenceDetailClosed OnClosed;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnCloseButtonClicked();

	// バインドウィジェット
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> DetailBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EvidenceNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EvidenceTypeText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DescriptionText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ABELCommentText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ImportanceText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> FoundAtText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> EvidenceImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;

private:
	FEvidence CurrentEvidence;

	FText GetEvidenceTypeText() const;
	FText GetImportanceText() const;
	FText GetLocationText() const;
};
