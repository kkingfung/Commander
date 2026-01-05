// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CaseState.h"
#include "TheLastWitness.h"

UCaseState::UCaseState()
{
}

void UCaseState::InitializeCase(const FCaseData& InCaseData)
{
	CaseData = InCaseData;
	ResetState();

	UE_LOG(LogLastWitness, Log, TEXT("[CaseState] 事件を初期化しました: %s"), *CaseData.CaseId.ToString());
}

void UCaseState::ResetState()
{
	CurrentLocation = ELocation::Office;
	SetFlags.Empty();
	ABELSuggestionsFollowed = 0;
	ABELSuggestionsIgnored = 0;
	EthicalViolations = 0;

	// 全証拠を未収集状態にリセット
	for (FEvidence& Evidence : CaseData.AllEvidence)
	{
		Evidence.bIsCollected = false;
		Evidence.bIsExamined = false;
	}

	// 全キャラクターをリセット
	for (FCharacterData& Character : CaseData.AllCharacters)
	{
		Character.bHasBeenInterviewed = false;
		Character.TrustLevel = 50;
		Character.EmotionalState = EEmotionalState::Neutral;
	}

	// 全ロケーションをリセット
	for (FLocationData& Location : CaseData.AllLocations)
	{
		Location.bHasVisited = false;
	}

	// 全推理をリセット
	for (FDeduction& Deduction : CaseData.AllDeductions)
	{
		Deduction.bIsUnlocked = false;
	}

	UE_LOG(LogLastWitness, Log, TEXT("[CaseState] 状態をリセットしました"));
}

// ============================================================================
// 証拠関連
// ============================================================================

bool UCaseState::CollectEvidence(FName EvidenceId)
{
	const int32 Index = FindEvidenceIndex(EvidenceId);
	if (Index == INDEX_NONE)
	{
		UE_LOG(LogLastWitness, Warning, TEXT("[CaseState] 証拠が見つかりません: %s"), *EvidenceId.ToString());
		return false;
	}

	if (CaseData.AllEvidence[Index].bIsCollected)
	{
		return false; // 既に収集済み
	}

	CaseData.AllEvidence[Index].bIsCollected = true;

	UE_LOG(LogLastWitness, Log, TEXT("[CaseState] 証拠を収集しました: %s"), *EvidenceId.ToString());

	OnEvidenceCollected.Broadcast(CaseData.AllEvidence[Index]);

	return true;
}

void UCaseState::ExamineEvidence(FName EvidenceId)
{
	const int32 Index = FindEvidenceIndex(EvidenceId);
	if (Index != INDEX_NONE)
	{
		CaseData.AllEvidence[Index].bIsExamined = true;
	}
}

bool UCaseState::HasEvidence(FName EvidenceId) const
{
	const int32 Index = FindEvidenceIndex(EvidenceId);
	if (Index == INDEX_NONE)
	{
		return false;
	}
	return CaseData.AllEvidence[Index].bIsCollected;
}

TArray<FEvidence> UCaseState::GetCollectedEvidence() const
{
	TArray<FEvidence> Result;
	for (const FEvidence& Evidence : CaseData.AllEvidence)
	{
		if (Evidence.bIsCollected)
		{
			Result.Add(Evidence);
		}
	}
	return Result;
}

bool UCaseState::GetEvidenceById(FName EvidenceId, FEvidence& OutEvidence) const
{
	const int32 Index = FindEvidenceIndex(EvidenceId);
	if (Index == INDEX_NONE)
	{
		return false;
	}
	OutEvidence = CaseData.AllEvidence[Index];
	return true;
}

// ============================================================================
// 推理関連
// ============================================================================

bool UCaseState::TryDeduction(FName EvidenceA, FName EvidenceB, FDeduction& OutDeduction)
{
	UE_LOG(LogLastWitness, Log, TEXT("[CaseState] TryDeduction: %s + %s"), *EvidenceA.ToString(), *EvidenceB.ToString());

	// 両方の証拠を持っているか確認
	const bool bHasA = HasEvidence(EvidenceA);
	const bool bHasB = HasEvidence(EvidenceB);

	UE_LOG(LogLastWitness, Log, TEXT("[CaseState] 証拠所持確認 - %s: %s, %s: %s"),
		*EvidenceA.ToString(), bHasA ? TEXT("あり") : TEXT("なし"),
		*EvidenceB.ToString(), bHasB ? TEXT("あり") : TEXT("なし"));

	if (!bHasA || !bHasB)
	{
		UE_LOG(LogLastWitness, Warning, TEXT("[CaseState] 証拠が不足しています"));
		return false;
	}

	UE_LOG(LogLastWitness, Log, TEXT("[CaseState] 推理データ数: %d"), CaseData.AllDeductions.Num());

	// 一致する推理を探す
	for (FDeduction& Deduction : CaseData.AllDeductions)
	{
		UE_LOG(LogLastWitness, Log, TEXT("[CaseState] 推理チェック: %s (%s + %s) - 解放済み: %s"),
			*Deduction.DeductionId.ToString(),
			*Deduction.EvidenceA.ToString(),
			*Deduction.EvidenceB.ToString(),
			Deduction.bIsUnlocked ? TEXT("Yes") : TEXT("No"));

		// 順番に関係なく一致するかチェック
		const bool bMatches = (Deduction.EvidenceA == EvidenceA && Deduction.EvidenceB == EvidenceB) ||
		                      (Deduction.EvidenceA == EvidenceB && Deduction.EvidenceB == EvidenceA);

		if (bMatches)
		{
			OutDeduction = Deduction;

			// 既に解放済みの場合は結果のみ返す（フラグは再設定しない）
			if (Deduction.bIsUnlocked)
			{
				UE_LOG(LogLastWitness, Log, TEXT("[CaseState] 既に解放済みの推理を再表示: %s"), *Deduction.DeductionId.ToString());
				return true;
			}

			// 新規解放
			Deduction.bIsUnlocked = true;

			// フラグを設定
			for (const FName& FlagName : Deduction.UnlocksFlags)
			{
				SetFlag(FlagName);
			}

			UE_LOG(LogLastWitness, Log, TEXT("[CaseState] 推理を解放しました: %s"), *Deduction.DeductionId.ToString());

			OnDeductionUnlocked.Broadcast(Deduction);

			return true;
		}
	}

	UE_LOG(LogLastWitness, Log, TEXT("[CaseState] 一致する推理が見つかりませんでした"));
	return false;
}

TArray<FDeduction> UCaseState::GetUnlockedDeductions() const
{
	TArray<FDeduction> Result;
	for (const FDeduction& Deduction : CaseData.AllDeductions)
	{
		if (Deduction.bIsUnlocked)
		{
			Result.Add(Deduction);
		}
	}
	return Result;
}

bool UCaseState::IsDeductionUnlocked(FName DeductionId) const
{
	const int32 Index = FindDeductionIndex(DeductionId);
	if (Index == INDEX_NONE)
	{
		return false;
	}
	return CaseData.AllDeductions[Index].bIsUnlocked;
}

// ============================================================================
// フラグ関連
// ============================================================================

void UCaseState::SetFlag(FName FlagName)
{
	if (!SetFlags.Contains(FlagName))
	{
		SetFlags.Add(FlagName);
		UE_LOG(LogLastWitness, Log, TEXT("[CaseState] フラグを設定しました: %s"), *FlagName.ToString());
		OnFlagSet.Broadcast(FlagName);
	}
}

bool UCaseState::HasFlag(FName FlagName) const
{
	return SetFlags.Contains(FlagName);
}

bool UCaseState::HasAllFlags(const TArray<FName>& FlagNames) const
{
	for (const FName& FlagName : FlagNames)
	{
		if (!SetFlags.Contains(FlagName))
		{
			return false;
		}
	}
	return true;
}

// ============================================================================
// キャラクター関連
// ============================================================================

bool UCaseState::GetCharacterById(FName CharacterId, FCharacterData& OutCharacter) const
{
	const int32 Index = FindCharacterIndex(CharacterId);
	if (Index == INDEX_NONE)
	{
		return false;
	}
	OutCharacter = CaseData.AllCharacters[Index];
	return true;
}

void UCaseState::ModifyCharacterTrust(FName CharacterId, int32 Delta)
{
	const int32 Index = FindCharacterIndex(CharacterId);
	if (Index == INDEX_NONE)
	{
		return;
	}

	FCharacterData& Character = CaseData.AllCharacters[Index];
	Character.TrustLevel = FMath::Clamp(Character.TrustLevel + Delta, 0, 100);

	UE_LOG(LogLastWitness, Log, TEXT("[CaseState] %s の信頼度が変化しました: %d"),
		*CharacterId.ToString(), Character.TrustLevel);

	OnCharacterTrustChanged.Broadcast(CharacterId, Character.TrustLevel);
}

void UCaseState::MarkCharacterInterviewed(FName CharacterId)
{
	const int32 Index = FindCharacterIndex(CharacterId);
	if (Index != INDEX_NONE)
	{
		CaseData.AllCharacters[Index].bHasBeenInterviewed = true;
	}
}

TArray<FCharacterData> UCaseState::GetAllSuspects() const
{
	TArray<FCharacterData> Result;
	for (const FCharacterData& Character : CaseData.AllCharacters)
	{
		if (Character.bIsSuspect)
		{
			Result.Add(Character);
		}
	}
	return Result;
}

// ============================================================================
// ロケーション関連
// ============================================================================

void UCaseState::TravelToLocation(ELocation NewLocation)
{
	const int32 Index = FindLocationIndex(NewLocation);
	if (Index == INDEX_NONE)
	{
		UE_LOG(LogLastWitness, Warning, TEXT("[CaseState] ロケーションが見つかりません"));
		return;
	}

	if (!CaseData.AllLocations[Index].bIsAccessible)
	{
		UE_LOG(LogLastWitness, Warning, TEXT("[CaseState] ロケーションにアクセスできません"));
		return;
	}

	CurrentLocation = NewLocation;

	if (!CaseData.AllLocations[Index].bHasVisited)
	{
		CaseData.AllLocations[Index].bHasVisited = true;
		OnLocationVisited.Broadcast(NewLocation);
	}

	UE_LOG(LogLastWitness, Log, TEXT("[CaseState] ロケーションに移動しました: %d"), static_cast<int32>(NewLocation));
}

bool UCaseState::GetLocationData(ELocation Location, FLocationData& OutLocation) const
{
	const int32 Index = FindLocationIndex(Location);
	if (Index == INDEX_NONE)
	{
		return false;
	}
	OutLocation = CaseData.AllLocations[Index];
	return true;
}

TArray<FLocationData> UCaseState::GetAccessibleLocations() const
{
	TArray<FLocationData> Result;
	for (const FLocationData& Location : CaseData.AllLocations)
	{
		if (Location.bIsAccessible)
		{
			Result.Add(Location);
		}
	}
	return Result;
}

// ============================================================================
// 告発関連
// ============================================================================

bool UCaseState::CanMakeAccusation() const
{
	for (const FName& RequiredId : CaseData.RequiredEvidenceForAccusation)
	{
		if (!HasEvidence(RequiredId))
		{
			return false;
		}
	}
	return true;
}

FGameResult UCaseState::MakeAccusation(FName CharacterId)
{
	FGameResult Result;
	Result.AccusedCharacterId = CharacterId;
	Result.bCorrectCulprit = (CharacterId == CaseData.TrueCulpritId);
	Result.ABELSuggestionsFollowed = ABELSuggestionsFollowed;
	Result.ABELSuggestionsIgnored = ABELSuggestionsIgnored;
	Result.EthicalViolations = EthicalViolations;

	// 証拠収集率を計算
	int32 CollectedCount = 0;
	for (const FEvidence& Evidence : CaseData.AllEvidence)
	{
		if (Evidence.bIsCollected)
		{
			CollectedCount++;
		}
	}
	Result.EvidenceCollectionRate = CaseData.AllEvidence.Num() > 0
		? static_cast<float>(CollectedCount) / static_cast<float>(CaseData.AllEvidence.Num())
		: 0.0f;

	// エンディングテキストを生成
	if (Result.bCorrectCulprit)
	{
		if (Result.EthicalViolations == 0)
		{
			Result.EndingNarrative = FText::FromString(TEXT("真実は暴かれ、正義は果たされた。あなたの手法は非の打ち所がなかった。"));
		}
		else
		{
			Result.EndingNarrative = FText::FromString(TEXT("犯人は捕まった。だが、あなたの手法には疑問が残る..."));
		}
	}
	else
	{
		Result.EndingNarrative = FText::FromString(TEXT("無実の人間が告発された。真犯人は今も自由の身だ..."));
	}

	UE_LOG(LogLastWitness, Log, TEXT("[CaseState] 告発完了: %s (正解: %s)"),
		*CharacterId.ToString(),
		Result.bCorrectCulprit ? TEXT("はい") : TEXT("いいえ"));

	return Result;
}

// ============================================================================
// Private ヘルパー関数
// ============================================================================

int32 UCaseState::FindEvidenceIndex(FName EvidenceId) const
{
	for (int32 i = 0; i < CaseData.AllEvidence.Num(); i++)
	{
		if (CaseData.AllEvidence[i].EvidenceId == EvidenceId)
		{
			return i;
		}
	}
	return INDEX_NONE;
}

int32 UCaseState::FindCharacterIndex(FName CharacterId) const
{
	for (int32 i = 0; i < CaseData.AllCharacters.Num(); i++)
	{
		if (CaseData.AllCharacters[i].CharacterId == CharacterId)
		{
			return i;
		}
	}
	return INDEX_NONE;
}

int32 UCaseState::FindLocationIndex(ELocation Location) const
{
	for (int32 i = 0; i < CaseData.AllLocations.Num(); i++)
	{
		if (CaseData.AllLocations[i].Location == Location)
		{
			return i;
		}
	}
	return INDEX_NONE;
}

int32 UCaseState::FindDeductionIndex(FName DeductionId) const
{
	for (int32 i = 0; i < CaseData.AllDeductions.Num(); i++)
	{
		if (CaseData.AllDeductions[i].DeductionId == DeductionId)
		{
			return i;
		}
	}
	return INDEX_NONE;
}
