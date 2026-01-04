// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/WitnessTypes.h"
#include "TheLastWitnessCaseData.generated.h"

/// <summary>
/// 「最後の目撃者」事件のデータを提供するクラス
/// </summary>
/// <remarks>
/// ロンドン1888年を舞台にした殺人事件のストーリーデータを生成します。
/// </remarks>
UCLASS(BlueprintType)
class THELASTWITNESS_API UTheLastWitnessCaseData : public UObject
{
	GENERATED_BODY()

public:
	/// <summary>
	/// 完全な事件データを生成します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Case Data")
	static FCaseData CreateCaseData();

private:
	/// <summary>
	/// キャラクターデータを生成します
	/// </summary>
	static TArray<FCharacterData> CreateCharacters();

	/// <summary>
	/// 証拠データを生成します
	/// </summary>
	static TArray<FEvidence> CreateEvidence();

	/// <summary>
	/// ロケーションデータを生成します
	/// </summary>
	static TArray<FLocationData> CreateLocations();

	/// <summary>
	/// 対話データを生成します
	/// </summary>
	static TArray<FDialogueTree> CreateDialogues();

	/// <summary>
	/// 推理データを生成します
	/// </summary>
	static TArray<FDeduction> CreateDeductions();
};
