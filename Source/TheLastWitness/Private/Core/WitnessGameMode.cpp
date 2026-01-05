// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/WitnessGameMode.h"
#include "Core/CaseState.h"
#include "Dialogue/DialogueManager.h"
#include "AI/ABELSystem.h"
#include "Data/TheLastWitnessCaseData.h"
#include "TheLastWitness.h"

AWitnessGameMode::AWitnessGameMode()
{
}

void AWitnessGameMode::BeginPlay()
{
	Super::BeginPlay();

	InitializeSubsystems();

	UE_LOG(LogLastWitness, Log, TEXT("[GameMode] ゲームモード初期化完了"));
}

void AWitnessGameMode::InitializeSubsystems()
{
	// CaseStateを作成
	CaseState = NewObject<UCaseState>(this, UCaseState::StaticClass());

	// DialogueManagerを作成
	DialogueManager = NewObject<UDialogueManager>(this, UDialogueManager::StaticClass());
	if (DialogueManager)
	{
		DialogueManager->Initialize(CaseState);
	}

	// ABELSystemを作成
	ABELSystem = NewObject<UABELSystem>(this, UABELSystem::StaticClass());
	if (ABELSystem)
	{
		ABELSystem->Initialize(CaseState);
	}

	UE_LOG(LogLastWitness, Log, TEXT("[GameMode] サブシステム初期化完了"));
}

// ============================================================================
// ゲームサイクル
// ============================================================================

void AWitnessGameMode::StartCase()
{
	// 事件データを作成
	const FCaseData CaseData = CreateCaseData();

	// CaseStateを初期化
	if (CaseState)
	{
		CaseState->InitializeCase(CaseData);
	}

	// DialogueManagerに対話ツリーを登録（CaseState初期化後）
	if (DialogueManager && CaseState)
	{
		DialogueManager->Initialize(CaseState);
	}

	// ABELを初期化
	if (ABELSystem)
	{
		ABELSystem->OnCaseStarted();
	}

	// フェーズを事件紹介に変更
	SetPhase(EGamePhase::CaseIntro);

	OnCaseStarted.Broadcast();

	UE_LOG(LogLastWitness, Log, TEXT("[GameMode] 事件を開始しました: %s"), *CaseData.CaseId.ToString());
}

void AWitnessGameMode::SetPhase(EGamePhase NewPhase)
{
	if (CurrentPhase != NewPhase)
	{
		const EGamePhase OldPhase = CurrentPhase;
		CurrentPhase = NewPhase;

		UE_LOG(LogLastWitness, Log, TEXT("[GameMode] フェーズ変更: %d -> %d"),
			static_cast<int32>(OldPhase), static_cast<int32>(NewPhase));

		OnPhaseChanged.Broadcast(NewPhase);
	}
}

// ============================================================================
// ロケーション
// ============================================================================

void AWitnessGameMode::TravelToLocation(ELocation Location)
{
	if (!CaseState)
	{
		return;
	}

	CaseState->TravelToLocation(Location);
	SetPhase(EGamePhase::Investigation);

	// ABELに場所移動を通知
	if (ABELSystem)
	{
		ABELSystem->OnLocationChanged(Location);
	}
}

TArray<FName> AWitnessGameMode::GetAvailableActionsAtLocation() const
{
	TArray<FName> Actions;

	if (!CaseState)
	{
		return Actions;
	}

	// 常に利用可能なアクション
	Actions.Add(FName("Examine"));      // 調査
	Actions.Add(FName("TalkTo"));       // 話しかける
	Actions.Add(FName("ConsultABEL")); // ABELに相談
	Actions.Add(FName("OpenJournal")); // 証拠帳を開く
	Actions.Add(FName("Travel"));       // 移動

	// 告発可能な場合
	if (CaseState->CanMakeAccusation())
	{
		Actions.Add(FName("Accuse"));
	}

	return Actions;
}

// ============================================================================
// 証拠調査
// ============================================================================

TArray<FEvidence> AWitnessGameMode::GetExaminableEvidenceAtLocation() const
{
	TArray<FEvidence> Result;

	if (!CaseState)
	{
		return Result;
	}

	const ELocation CurrentLoc = CaseState->GetCurrentLocation();
	FLocationData LocData;
	if (!CaseState->GetLocationData(CurrentLoc, LocData))
	{
		return Result;
	}

	// この場所にある証拠を取得
	for (const FName& EvidenceId : LocData.AvailableEvidence)
	{
		FEvidence Evidence;
		if (CaseState->GetEvidenceById(EvidenceId, Evidence))
		{
			// まだ収集していない証拠のみ
			if (!Evidence.bIsCollected)
			{
				Result.Add(Evidence);
			}
		}
	}

	return Result;
}

void AWitnessGameMode::ExamineEvidence(FName EvidenceId)
{
	if (CaseState)
	{
		CaseState->ExamineEvidence(EvidenceId);

		// ABELに証拠調査を通知
		if (ABELSystem)
		{
			FEvidence Evidence;
			if (CaseState->GetEvidenceById(EvidenceId, Evidence))
			{
				ABELSystem->OnEvidenceExamined(Evidence);
			}
		}
	}
}

void AWitnessGameMode::CollectEvidence(FName EvidenceId)
{
	if (CaseState)
	{
		CaseState->CollectEvidence(EvidenceId);

		// ABELに証拠収集を通知
		if (ABELSystem)
		{
			FEvidence Evidence;
			if (CaseState->GetEvidenceById(EvidenceId, Evidence))
			{
				ABELSystem->OnEvidenceCollected(Evidence);
			}
		}
	}
}

// ============================================================================
// 対話
// ============================================================================

TArray<FCharacterData> AWitnessGameMode::GetCharactersAtLocation() const
{
	TArray<FCharacterData> Result;

	if (!CaseState)
	{
		return Result;
	}

	const ELocation CurrentLoc = CaseState->GetCurrentLocation();
	FLocationData LocData;
	if (!CaseState->GetLocationData(CurrentLoc, LocData))
	{
		return Result;
	}

	for (const FName& CharId : LocData.CharactersPresent)
	{
		FCharacterData Character;
		if (CaseState->GetCharacterById(CharId, Character))
		{
			Result.Add(Character);
		}
	}

	return Result;
}

void AWitnessGameMode::StartDialogueWith(FName CharacterId)
{
	if (!DialogueManager)
	{
		return;
	}

	DialogueManager->StartDialogue(CharacterId);
	SetPhase(EGamePhase::Dialogue);

	// ABELに対話開始を通知
	if (ABELSystem)
	{
		ABELSystem->OnDialogueStarted(CharacterId);
	}
}

void AWitnessGameMode::SelectDialogueChoice(FName ChoiceId)
{
	if (DialogueManager)
	{
		DialogueManager->SelectChoice(ChoiceId);
	}
}

void AWitnessGameMode::EndDialogue()
{
	if (DialogueManager)
	{
		DialogueManager->EndDialogue();
	}

	SetPhase(EGamePhase::Investigation);
}

// ============================================================================
// ABEL操作
// ============================================================================

void AWitnessGameMode::RequestABELAnalysis()
{
	if (!ABELSystem)
	{
		return;
	}

	SetPhase(EGamePhase::ABELAnalysis);
	ABELSystem->GenerateSuggestions();
}

void AWitnessGameMode::FollowABELSuggestion(FName SuggestionId)
{
	if (ABELSystem)
	{
		ABELSystem->OnSuggestionFollowed(SuggestionId);
	}

	if (CaseState)
	{
		CaseState->IncrementABELFollowed();
	}
}

void AWitnessGameMode::IgnoreABELSuggestion(FName SuggestionId)
{
	if (ABELSystem)
	{
		ABELSystem->OnSuggestionIgnored(SuggestionId);
	}

	if (CaseState)
	{
		CaseState->IncrementABELIgnored();
	}
}

// ============================================================================
// 推理ボード
// ============================================================================

void AWitnessGameMode::OpenDeductionBoard()
{
	SetPhase(EGamePhase::Deduction);
}

bool AWitnessGameMode::TryMakeDeduction(FName EvidenceA, FName EvidenceB, FDeduction& OutDeduction)
{
	if (!CaseState)
	{
		return false;
	}

	const bool bSuccess = CaseState->TryDeduction(EvidenceA, EvidenceB, OutDeduction);

	if (bSuccess && ABELSystem)
	{
		ABELSystem->OnDeductionMade(OutDeduction);
	}

	return bSuccess;
}

// ============================================================================
// 告発
// ============================================================================

void AWitnessGameMode::BeginAccusation()
{
	SetPhase(EGamePhase::Accusation);
}

void AWitnessGameMode::AccuseCharacter(FName CharacterId)
{
	if (!CaseState)
	{
		return;
	}

	const FGameResult Result = CaseState->MakeAccusation(CharacterId);

	SetPhase(EGamePhase::Resolution);
	OnGameEnded.Broadcast(Result);

	UE_LOG(LogLastWitness, Log, TEXT("[GameMode] ゲーム終了 - 正解: %s"),
		Result.bCorrectCulprit ? TEXT("はい") : TEXT("いいえ"));
}

// ============================================================================
// 事件データ作成（デフォルト実装）
// ============================================================================

FCaseData AWitnessGameMode::CreateCaseData_Implementation()
{
	// TheLastWitnessCaseDataから事件データを取得
	return UTheLastWitnessCaseData::CreateCaseData();
}
