// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/ABELSystem.h"
#include "Core/CaseState.h"
#include "TheLastWitness.h"

UABELSystem::UABELSystem()
{
}

void UABELSystem::Initialize(UCaseState* InCaseState)
{
	CaseState = InCaseState;

	UE_LOG(LogLastWitness, Log, TEXT("[ABELSystem] 初期化完了"));
}

void UABELSystem::OnCaseStarted()
{
	CurrentSuggestions.Empty();
	RelationshipValue = 0;
	CurrentDisposition = EABELDisposition::Analytical;

	// 開始時の挨拶
	QueueComment(FText::FromString(TEXT(
		"分析エンジン「ABEL」、起動完了。"
		"ヘンリー・ブラックウッド氏の死について、データを処理する準備が整いました。"
		"私の分析が必要な際は、いつでもお申し付けください。"
	)));

	UE_LOG(LogLastWitness, Log, TEXT("[ABELSystem] 事件開始を処理しました"));
}

// ============================================================================
// 提案生成
// ============================================================================

void UABELSystem::GenerateSuggestions()
{
	CurrentSuggestions.Empty();

	GenerateScriptedSuggestions();

	UE_LOG(LogLastWitness, Log, TEXT("[ABELSystem] %d 個の提案を生成しました"), CurrentSuggestions.Num());

	// 各提案についてイベント発火
	for (const FABELSuggestion& Suggestion : CurrentSuggestions)
	{
		OnSuggestionReady.Broadcast(Suggestion);
	}
}

void UABELSystem::GenerateScriptedSuggestions()
{
	if (!CaseState)
	{
		return;
	}

	GenerateEvidenceConnectionSuggestions();
	GenerateInterrogationSuggestions();
	GenerateNextActionSuggestions();
}

void UABELSystem::GenerateEvidenceConnectionSuggestions()
{
	const TArray<FEvidence> CollectedEvidence = CaseState->GetCollectedEvidence();

	if (CollectedEvidence.Num() < 2)
	{
		return;
	}

	// 関連する証拠の組み合わせを探す
	for (int32 i = 0; i < CollectedEvidence.Num(); i++)
	{
		for (int32 j = i + 1; j < CollectedEvidence.Num(); j++)
		{
			const FEvidence& A = CollectedEvidence[i];
			const FEvidence& B = CollectedEvidence[j];

			// 関連があるかチェック
			if (A.RelatedEvidence.Contains(B.EvidenceId) || B.RelatedEvidence.Contains(A.EvidenceId))
			{
				// まだ推理で結びつけていないかチェック
				FDeduction TempDeduction;
				// 既に解放済みの推理を確認
				bool bAlreadyDeduced = false;
				for (const FDeduction& D : CaseState->GetUnlockedDeductions())
				{
					if ((D.EvidenceA == A.EvidenceId && D.EvidenceB == B.EvidenceId) ||
					    (D.EvidenceA == B.EvidenceId && D.EvidenceB == A.EvidenceId))
					{
						bAlreadyDeduced = true;
						break;
					}
				}

				if (!bAlreadyDeduced)
				{
					FABELSuggestion Suggestion;
					Suggestion.SuggestionId = GenerateSuggestionId();
					Suggestion.Type = EABELSuggestionType::EvidenceConnection;
					Suggestion.Confidence = 0.75f;
					Suggestion.bIsCorrect = true;
					Suggestion.bIsEthicallyQuestionable = false;
					Suggestion.RelatedEvidence.Add(A.EvidenceId);
					Suggestion.RelatedEvidence.Add(B.EvidenceId);

					Suggestion.Content = FText::Format(
						NSLOCTEXT("ABEL", "ConnectionSuggestion",
							"「{0}」と「{1}」の間に関連性を検出しました。推理ボードで結びつけることを推奨します。"),
						A.DisplayName,
						B.DisplayName
					);

					CurrentSuggestions.Add(Suggestion);
				}
			}
		}
	}
}

void UABELSystem::GenerateInterrogationSuggestions()
{
	// 現在のロケーションにいるキャラクターへの質問を提案
	const ELocation CurrentLoc = CaseState->GetCurrentLocation();
	FLocationData LocData;
	if (!CaseState->GetLocationData(CurrentLoc, LocData))
	{
		return;
	}

	for (const FName& CharId : LocData.CharactersPresent)
	{
		FCharacterData Character;
		if (!CaseState->GetCharacterById(CharId, Character))
		{
			continue;
		}

		if (!Character.bHasBeenInterviewed)
		{
			FABELSuggestion Suggestion;
			Suggestion.SuggestionId = GenerateSuggestionId();
			Suggestion.Type = EABELSuggestionType::InterrogationTip;
			Suggestion.Confidence = 0.6f;
			Suggestion.bIsCorrect = true;

			Suggestion.Content = FText::Format(
				NSLOCTEXT("ABEL", "InterrogationSuggestion",
					"{0}にはまだ話を聞いていません。{1}として、有用な情報を持っている可能性があります。"),
				Character.DisplayName,
				Character.Role
			);

			CurrentSuggestions.Add(Suggestion);
		}
	}
}

void UABELSystem::GenerateNextActionSuggestions()
{
	const TArray<FEvidence> CollectedEvidence = CaseState->GetCollectedEvidence();

	// 告発可能なら提案
	if (CaseState->CanMakeAccusation())
	{
		FABELSuggestion Suggestion;
		Suggestion.SuggestionId = GenerateSuggestionId();
		Suggestion.Type = EABELSuggestionType::NextAction;
		Suggestion.Confidence = 0.9f;
		Suggestion.bIsCorrect = true;

		Suggestion.Content = FText::FromString(TEXT(
			"必要な証拠が揃いました。犯人を告発する準備が整っています。"
			"ただし、より多くの証拠を集めることで、確実性を高めることも可能です。"
		));

		CurrentSuggestions.Add(Suggestion);
	}
	// 証拠が少ない場合は調査を促す
	else if (CollectedEvidence.Num() < 3)
	{
		FABELSuggestion Suggestion;
		Suggestion.SuggestionId = GenerateSuggestionId();
		Suggestion.Type = EABELSuggestionType::NextAction;
		Suggestion.Confidence = 0.8f;
		Suggestion.bIsCorrect = true;

		Suggestion.Content = FText::FromString(TEXT(
			"現時点での証拠は限定的です。"
			"現場をより詳しく調査することを推奨します。"
		));

		CurrentSuggestions.Add(Suggestion);
	}
}

void UABELSystem::OnSuggestionFollowed(FName SuggestionId)
{
	UpdateDisposition(5);

	// 提案をマーク
	for (FABELSuggestion& Suggestion : CurrentSuggestions)
	{
		if (Suggestion.SuggestionId == SuggestionId)
		{
			Suggestion.bHasBeenShown = true;

			// フィードバック
			if (Suggestion.bIsCorrect)
			{
				QueueComment(FText::FromString(TEXT("論理的な選択です。")));
			}
			break;
		}
	}

	UE_LOG(LogLastWitness, Log, TEXT("[ABELSystem] 提案に従いました: %s"), *SuggestionId.ToString());
}

void UABELSystem::OnSuggestionIgnored(FName SuggestionId)
{
	UpdateDisposition(-3);

	// 提案を検索
	for (FABELSuggestion& Suggestion : CurrentSuggestions)
	{
		if (Suggestion.SuggestionId == SuggestionId)
		{
			Suggestion.bHasBeenShown = true;

			// 無視された時の反応
			if (CurrentDisposition == EABELDisposition::Skeptical)
			{
				QueueComment(FText::FromString(TEXT(
					"...了解しました。独自の判断を尊重します。"
				)));
			}
			break;
		}
	}

	UE_LOG(LogLastWitness, Log, TEXT("[ABELSystem] 提案を無視しました: %s"), *SuggestionId.ToString());
}

// ============================================================================
// 分析機能
// ============================================================================

FText UABELSystem::AnalyzeEvidence(const FEvidence& Evidence)
{
	// ABELコメントが設定されていればそれを返す
	if (!Evidence.ABELComment.IsEmpty())
	{
		return Evidence.ABELComment;
	}

	// デフォルトの分析
	FString Analysis;

	switch (Evidence.Type)
	{
	case EEvidenceType::Physical:
		Analysis = FString::Printf(TEXT("物的証拠「%s」を分析中... "),
			*Evidence.DisplayName.ToString());
		break;
	case EEvidenceType::Document:
		Analysis = FString::Printf(TEXT("文書「%s」の内容を解析中... "),
			*Evidence.DisplayName.ToString());
		break;
	case EEvidenceType::Testimony:
		Analysis = FString::Printf(TEXT("証言「%s」の信憑性を評価中... "),
			*Evidence.DisplayName.ToString());
		break;
	case EEvidenceType::Observation:
		Analysis = FString::Printf(TEXT("観察事項「%s」を記録中... "),
			*Evidence.DisplayName.ToString());
		break;
	}

	switch (Evidence.Importance)
	{
	case EEvidenceImportance::Critical:
		Analysis += TEXT("重要度: 極めて高い。この証拠は事件の核心に関わると推定されます。");
		break;
	case EEvidenceImportance::Major:
		Analysis += TEXT("重要度: 高い。詳細な分析を推奨します。");
		break;
	case EEvidenceImportance::Normal:
		Analysis += TEXT("重要度: 通常。他の証拠との関連性を検討してください。");
		break;
	case EEvidenceImportance::Minor:
		Analysis += TEXT("重要度: 補足的。直接的な関連性は低い可能性があります。");
		break;
	}

	return FText::FromString(Analysis);
}

FText UABELSystem::AnalyzeConnection(const FEvidence& EvidenceA, const FEvidence& EvidenceB)
{
	return FText::Format(
		NSLOCTEXT("ABEL", "ConnectionAnalysis",
			"「{0}」と「{1}」の関連性を分析中... "
			"両者の接点を検出しました。推理ボードで結びつけることで、新たな洞察が得られる可能性があります。"),
		EvidenceA.DisplayName,
		EvidenceB.DisplayName
	);
}

FText UABELSystem::AnalyzeCharacter(const FCharacterData& Character)
{
	FString Analysis = FString::Printf(
		TEXT("%s（%s）について分析中...\n"),
		*Character.DisplayName.ToString(),
		*Character.Role.ToString()
	);

	// 感情状態の分析
	switch (Character.EmotionalState)
	{
	case EEmotionalState::Nervous:
		Analysis += TEXT("観察: 対象は神経質な様子を見せています。何かを隠している可能性があります。");
		break;
	case EEmotionalState::Defensive:
		Analysis += TEXT("観察: 対象は防御的な態度を取っています。慎重にアプローチすることを推奨します。");
		break;
	case EEmotionalState::Cooperative:
		Analysis += TEXT("観察: 対象は協力的です。追加情報を得られる可能性が高いです。");
		break;
	case EEmotionalState::Angry:
		Analysis += TEXT("観察: 対象は怒りを示しています。刺激を避けることを推奨します。");
		break;
	case EEmotionalState::Sad:
		Analysis += TEXT("観察: 対象は悲しみを表しています。共感的なアプローチが有効かもしれません。");
		break;
	case EEmotionalState::Fearful:
		Analysis += TEXT("観察: 対象は恐怖を感じているようです。何かを恐れている可能性があります。");
		break;
	default:
		Analysis += TEXT("観察: 感情状態は安定しています。");
		break;
	}

	// 信頼度の分析
	Analysis += FString::Printf(TEXT("\n信頼度: %d%%"), Character.TrustLevel);

	if (Character.TrustLevel < 30)
	{
		Analysis += TEXT(" - 対象はあなたを警戒しています。");
	}
	else if (Character.TrustLevel > 70)
	{
		Analysis += TEXT(" - 対象はあなたを信頼しています。");
	}

	return FText::FromString(Analysis);
}

// ============================================================================
// イベントハンドラ
// ============================================================================

void UABELSystem::OnEvidenceExamined(const FEvidence& Evidence)
{
	// 重要な証拠の場合はコメント
	if (Evidence.Importance >= EEvidenceImportance::Major)
	{
		QueueComment(AnalyzeEvidence(Evidence));
	}
}

void UABELSystem::OnEvidenceCollected(const FEvidence& Evidence)
{
	FText Comment = FText::Format(
		NSLOCTEXT("ABEL", "EvidenceCollected", "証拠「{0}」をデータベースに登録しました。"),
		Evidence.DisplayName
	);
	QueueComment(Comment);
}

void UABELSystem::OnLocationChanged(ELocation NewLocation)
{
	// ロケーションに応じたコメント
	FString Comment;

	switch (NewLocation)
	{
	case ELocation::Study:
		Comment = TEXT("書斎に到着しました。ここが事件現場です。詳細な調査を推奨します。");
		break;
	case ELocation::DrawingRoom:
		Comment = TEXT("応接室です。屋敷の住人と面会できます。");
		break;
	case ELocation::ServantsQuarters:
		Comment = TEXT("使用人の居住区です。彼らは屋敷の内情に詳しいはずです。");
		break;
	case ELocation::Garden:
		Comment = TEXT("庭園です。外部からのアクセスポイントとして注目に値します。");
		break;
	case ELocation::Factory:
		Comment = TEXT("ブラックウッド氏の工場です。彼のビジネスについて調べられるかもしれません。");
		break;
	case ELocation::Pub:
		Comment = TEXT("地元のパブです。噂話が聞けるかもしれません。");
		break;
	case ELocation::Office:
		Comment = TEXT("オフィスに戻りました。収集した情報を整理しましょう。");
		break;
	}

	if (!Comment.IsEmpty())
	{
		QueueComment(FText::FromString(Comment));
	}
}

void UABELSystem::OnDialogueStarted(FName CharacterId)
{
	FCharacterData Character;
	if (CaseState && CaseState->GetCharacterById(CharacterId, Character))
	{
		// 対話開始時の観察
		if (Character.EmotionalState != EEmotionalState::Neutral)
		{
			QueueComment(AnalyzeCharacter(Character));
		}
	}
}

void UABELSystem::OnDeductionMade(const FDeduction& Deduction)
{
	QueueComment(FText::Format(
		NSLOCTEXT("ABEL", "DeductionMade",
			"推理「{0}」が成立しました。新たな洞察が得られました。"),
		Deduction.Title
	));

	// 関係値を少し上げる
	UpdateDisposition(2);
}

// ============================================================================
// 性格・状態
// ============================================================================

FText UABELSystem::GetAndClearPendingComment()
{
	FText Comment = PendingComment;
	PendingComment = FText::GetEmpty();
	bHasPendingComment = false;
	return Comment;
}

void UABELSystem::UpdateDisposition(int32 RelationshipDelta)
{
	RelationshipValue = FMath::Clamp(RelationshipValue + RelationshipDelta, -100, 100);

	EABELDisposition OldDisposition = CurrentDisposition;

	// 関係値に基づいて性格を更新
	if (RelationshipValue <= -50)
	{
		CurrentDisposition = EABELDisposition::Skeptical;
	}
	else if (RelationshipValue >= 50)
	{
		CurrentDisposition = EABELDisposition::Cooperative;
	}
	else if (RelationshipValue <= -20)
	{
		CurrentDisposition = EABELDisposition::Utilitarian;
	}
	else
	{
		CurrentDisposition = EABELDisposition::Analytical;
	}

	if (OldDisposition != CurrentDisposition)
	{
		OnDispositionChanged.Broadcast(CurrentDisposition);

		UE_LOG(LogLastWitness, Log, TEXT("[ABELSystem] 性格傾向が変化しました: %d -> %d"),
			static_cast<int32>(OldDisposition), static_cast<int32>(CurrentDisposition));
	}
}

void UABELSystem::QueueComment(const FText& Comment)
{
	PendingComment = Comment;
	bHasPendingComment = true;

	OnABELSpeaks.Broadcast(Comment);
}

FName UABELSystem::GenerateSuggestionId()
{
	SuggestionCounter++;
	return FName(*FString::Printf(TEXT("Suggestion_%d"), SuggestionCounter));
}
