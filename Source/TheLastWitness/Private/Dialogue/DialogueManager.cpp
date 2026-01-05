// Copyright Epic Games, Inc. All Rights Reserved.

#include "Dialogue/DialogueManager.h"
#include "Core/CaseState.h"
#include "TheLastWitness.h"

UDialogueManager::UDialogueManager()
{
}

void UDialogueManager::Initialize(UCaseState* InCaseState)
{
	CaseState = InCaseState;

	// CaseStateから対話ツリーを読み込み
	if (CaseState)
	{
		const FCaseData& CaseData = CaseState->GetCaseData();
		for (const FDialogueTree& Tree : CaseData.AllDialogues)
		{
			RegisterDialogueTree(Tree);
		}
	}

	UE_LOG(LogLastWitness, Log, TEXT("[DialogueManager] 初期化完了 - %d 個の対話ツリーを登録"),
		DialogueTrees.Num());
}

// ============================================================================
// 対話制御
// ============================================================================

bool UDialogueManager::StartDialogue(FName CharacterId)
{
	if (bIsInDialogue)
	{
		UE_LOG(LogLastWitness, Warning, TEXT("[DialogueManager] 既に対話中です"));
		return false;
	}

	// 対話ツリーを検索
	FDialogueTree Tree;
	if (!GetDialogueTreeForCharacter(CharacterId, Tree))
	{
		UE_LOG(LogLastWitness, Warning, TEXT("[DialogueManager] 対話ツリーが見つかりません: %s"),
			*CharacterId.ToString());
		return false;
	}

	CurrentCharacterId = CharacterId;
	CurrentTree = Tree;
	bIsInDialogue = true;

	// キャラクターをインタビュー済みにマーク
	if (CaseState)
	{
		CaseState->MarkCharacterInterviewed(CharacterId);
	}

	UE_LOG(LogLastWitness, Log, TEXT("[DialogueManager] 対話開始: %s"), *CharacterId.ToString());

	OnDialogueStarted.Broadcast(CharacterId);

	// 開始ノードに移動
	GoToNode(CurrentTree.StartNodeId);

	return true;
}

void UDialogueManager::SelectChoice(FName ChoiceId)
{
	UE_LOG(LogLastWitness, Log, TEXT("[DialogueManager] SelectChoice called - ChoiceId: %s, bIsInDialogue: %s, CurrentNodeId: %s"),
		*ChoiceId.ToString(),
		bIsInDialogue ? TEXT("true") : TEXT("false"),
		*CurrentNodeId.ToString());

	if (!bIsInDialogue)
	{
		UE_LOG(LogLastWitness, Warning, TEXT("[DialogueManager] 対話中ではありません"));
		return;
	}

	// 現在のノードをデバッグ出力
	const FDialogueNode* CurrentNode = FindNodeById(CurrentNodeId);
	if (CurrentNode)
	{
		UE_LOG(LogLastWitness, Log, TEXT("[DialogueManager] CurrentNode found - Choices count: %d"), CurrentNode->Choices.Num());
		for (const FDialogueChoice& C : CurrentNode->Choices)
		{
			UE_LOG(LogLastWitness, Log, TEXT("[DialogueManager]   Choice: %s"), *C.ChoiceId.ToString());
		}
	}
	else
	{
		UE_LOG(LogLastWitness, Warning, TEXT("[DialogueManager] CurrentNode not found! CurrentTree.Nodes count: %d"), CurrentTree.Nodes.Num());
	}

	const FDialogueChoice* Choice = FindChoiceById(ChoiceId);
	if (!Choice)
	{
		UE_LOG(LogLastWitness, Warning, TEXT("[DialogueManager] 選択肢が見つかりません: %s"),
			*ChoiceId.ToString());
		return;
	}

	// 信頼度変更を適用
	if (CaseState && Choice->TrustDelta != 0)
	{
		CaseState->ModifyCharacterTrust(CurrentCharacterId, Choice->TrustDelta);
	}

	// フラグを設定
	if (CaseState)
	{
		for (const FName& FlagName : Choice->SetsFlags)
		{
			CaseState->SetFlag(FlagName);
		}
	}

	UE_LOG(LogLastWitness, Log, TEXT("[DialogueManager] 選択肢を選択: %s -> 次のノード: %s"),
		*ChoiceId.ToString(), *Choice->NextNodeId.ToString());

	// 次のノードに移動
	GoToNode(Choice->NextNodeId);
}

void UDialogueManager::AdvanceDialogue()
{
	if (!bIsInDialogue)
	{
		return;
	}

	const FDialogueNode* CurrentNode = FindNodeById(CurrentNodeId);
	if (!CurrentNode)
	{
		return;
	}

	// 終了ノードなら対話を終了
	if (CurrentNode->bIsEndNode)
	{
		EndDialogue();
		return;
	}

	// 選択肢がある場合は進めない
	if (CurrentNode->Choices.Num() > 0)
	{
		return;
	}

	// 次のノードに移動
	if (!CurrentNode->NextNodeId.IsNone())
	{
		GoToNode(CurrentNode->NextNodeId);
	}
	else
	{
		EndDialogue();
	}
}

void UDialogueManager::EndDialogue()
{
	if (!bIsInDialogue)
	{
		return;
	}

	UE_LOG(LogLastWitness, Log, TEXT("[DialogueManager] 対話終了: %s"), *CurrentCharacterId.ToString());

	bIsInDialogue = false;
	CurrentCharacterId = NAME_None;
	CurrentNodeId = NAME_None;

	OnDialogueEnded.Broadcast();
}

// ============================================================================
// 状態取得
// ============================================================================

bool UDialogueManager::GetCurrentNode(FDialogueNode& OutNode) const
{
	if (!bIsInDialogue)
	{
		return false;
	}

	const FDialogueNode* Node = FindNodeById(CurrentNodeId);
	if (!Node)
	{
		return false;
	}

	OutNode = *Node;
	return true;
}

TArray<FDialogueChoice> UDialogueManager::GetAvailableChoices() const
{
	TArray<FDialogueChoice> Result;

	if (!bIsInDialogue)
	{
		return Result;
	}

	const FDialogueNode* CurrentNode = FindNodeById(CurrentNodeId);
	if (!CurrentNode)
	{
		return Result;
	}

	for (const FDialogueChoice& Choice : CurrentNode->Choices)
	{
		if (CanShowChoice(Choice))
		{
			Result.Add(Choice);
		}
	}

	return Result;
}

bool UDialogueManager::HasChoices() const
{
	return GetAvailableChoices().Num() > 0;
}

bool UDialogueManager::IsAtEndNode() const
{
	if (!bIsInDialogue)
	{
		return false;
	}

	const FDialogueNode* CurrentNode = FindNodeById(CurrentNodeId);
	return CurrentNode && CurrentNode->bIsEndNode;
}

// ============================================================================
// 対話ツリー管理
// ============================================================================

void UDialogueManager::RegisterDialogueTree(const FDialogueTree& Tree)
{
	DialogueTrees.Add(Tree.CharacterId, Tree);

	UE_LOG(LogLastWitness, Log, TEXT("[DialogueManager] 対話ツリーを登録: %s (%d ノード)"),
		*Tree.CharacterId.ToString(), Tree.Nodes.Num());
}

bool UDialogueManager::GetDialogueTreeForCharacter(FName CharacterId, FDialogueTree& OutTree) const
{
	const FDialogueTree* Found = DialogueTrees.Find(CharacterId);
	if (Found)
	{
		OutTree = *Found;
		return true;
	}
	return false;
}

// ============================================================================
// Protected
// ============================================================================

void UDialogueManager::GoToNode(FName NodeId)
{
	const FDialogueNode* Node = FindNodeById(NodeId);
	if (!Node)
	{
		UE_LOG(LogLastWitness, Warning, TEXT("[DialogueManager] ノードが見つかりません: %s"),
			*NodeId.ToString());
		EndDialogue();
		return;
	}

	CurrentNodeId = NodeId;

	UE_LOG(LogLastWitness, Log, TEXT("[DialogueManager] ノード移動: %s"), *NodeId.ToString());

	// 証拠取得を処理
	ProcessNodeEvidence(*Node);

	// フラグ設定を処理
	if (CaseState)
	{
		for (const FName& FlagName : Node->SetsFlags)
		{
			CaseState->SetFlag(FlagName);
		}
	}

	// イベント発火
	OnDialogueNodeChanged.Broadcast(*Node);

	// 選択肢を通知（空の場合もUIが古い選択肢をクリアできるように）
	const TArray<FDialogueChoice> Choices = GetAvailableChoices();
	OnChoicesAvailable.Broadcast(Choices);
}

bool UDialogueManager::CanShowChoice(const FDialogueChoice& Choice) const
{
	if (!CaseState)
	{
		return true;
	}

	// 必要な証拠をチェック
	for (const FName& EvidenceId : Choice.RequiredEvidence)
	{
		if (!CaseState->HasEvidence(EvidenceId))
		{
			return false;
		}
	}

	// 必要なフラグをチェック
	for (const FName& FlagName : Choice.RequiredFlags)
	{
		if (!CaseState->HasFlag(FlagName))
		{
			return false;
		}
	}

	return true;
}

void UDialogueManager::ProcessNodeEvidence(const FDialogueNode& Node)
{
	if (Node.GainsEvidence.Num() == 0 || !CaseState)
	{
		return;
	}

	TArray<FName> GainedIds;

	for (const FName& EvidenceId : Node.GainsEvidence)
	{
		if (CaseState->CollectEvidence(EvidenceId))
		{
			GainedIds.Add(EvidenceId);
		}
	}

	if (GainedIds.Num() > 0)
	{
		OnEvidenceGainedFromDialogue.Broadcast(GainedIds);
	}
}

// ============================================================================
// Private
// ============================================================================

const FDialogueNode* UDialogueManager::FindNodeById(FName NodeId) const
{
	for (const FDialogueNode& Node : CurrentTree.Nodes)
	{
		if (Node.NodeId == NodeId)
		{
			return &Node;
		}
	}
	return nullptr;
}

const FDialogueChoice* UDialogueManager::FindChoiceById(FName ChoiceId) const
{
	const FDialogueNode* CurrentNode = FindNodeById(CurrentNodeId);
	if (!CurrentNode)
	{
		return nullptr;
	}

	for (const FDialogueChoice& Choice : CurrentNode->Choices)
	{
		if (Choice.ChoiceId == ChoiceId)
		{
			return &Choice;
		}
	}
	return nullptr;
}
