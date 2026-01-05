// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WitnessTypes.generated.h"

// ============================================================================
// 列挙型 (Enumerations)
// ============================================================================

/// <summary>
/// ゲームの進行フェーズ
/// </summary>
UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	/// <summary>メインメニュー</summary>
	MainMenu,
	/// <summary>事件概要の説明</summary>
	CaseIntro,
	/// <summary>現場調査中</summary>
	Investigation,
	/// <summary>対話中</summary>
	Dialogue,
	/// <summary>ABELとの分析中</summary>
	ABELAnalysis,
	/// <summary>推理ボード使用中</summary>
	Deduction,
	/// <summary>犯人告発</summary>
	Accusation,
	/// <summary>事件解決・エンディング</summary>
	Resolution
};

/// <summary>
/// 証拠の種類
/// </summary>
UENUM(BlueprintType)
enum class EEvidenceType : uint8
{
	/// <summary>物的証拠（物体、痕跡）</summary>
	Physical,
	/// <summary>文書（手紙、書類）</summary>
	Document,
	/// <summary>証言（聞き取り内容）</summary>
	Testimony,
	/// <summary>観察（状況、様子）</summary>
	Observation
};

/// <summary>
/// 証拠の重要度
/// </summary>
UENUM(BlueprintType)
enum class EEvidenceImportance : uint8
{
	/// <summary>補足的な情報</summary>
	Minor,
	/// <summary>通常の手がかり</summary>
	Normal,
	/// <summary>重要な手がかり</summary>
	Major,
	/// <summary>決定的証拠</summary>
	Critical
};

/// <summary>
/// キャラクターの感情状態
/// </summary>
UENUM(BlueprintType)
enum class EEmotionalState : uint8
{
	Neutral,
	Nervous,
	Angry,
	Sad,
	Fearful,
	Defensive,
	Cooperative
};

/// <summary>
/// 対話選択肢のトーン
/// </summary>
UENUM(BlueprintType)
enum class EDialogueTone : uint8
{
	/// <summary>丁寧・礼儀正しい</summary>
	Polite,
	/// <summary>直接的・ストレート</summary>
	Direct,
	/// <summary>威圧的・圧力をかける</summary>
	Intimidating,
	/// <summary>共感的・理解を示す</summary>
	Empathetic,
	/// <summary>狡猾・誘導的</summary>
	Cunning
};

/// <summary>
/// ABELの提案タイプ
/// </summary>
UENUM(BlueprintType)
enum class EABELSuggestionType : uint8
{
	/// <summary>証拠間の関連性</summary>
	EvidenceConnection,
	/// <summary>容疑者への質問提案</summary>
	InterrogationTip,
	/// <summary>理論・仮説</summary>
	Theory,
	/// <summary>次の行動提案</summary>
	NextAction,
	/// <summary>警告・注意</summary>
	Warning
};

/// <summary>
/// ABELの性格傾向（プレイヤーとの関係に影響）
/// </summary>
UENUM(BlueprintType)
enum class EABELDisposition : uint8
{
	/// <summary>純粋に論理的</summary>
	Analytical,
	/// <summary>効率重視・功利主義的</summary>
	Utilitarian,
	/// <summary>プレイヤーへの信頼が低下</summary>
	Skeptical,
	/// <summary>プレイヤーと協調的</summary>
	Cooperative
};

/// <summary>
/// ロケーション（調査場所）
/// </summary>
UENUM(BlueprintType)
enum class ELocation : uint8
{
	/// <summary>被害者の書斎</summary>
	Study,
	/// <summary>応接室</summary>
	DrawingRoom,
	/// <summary>使用人の部屋</summary>
	ServantsQuarters,
	/// <summary>庭園</summary>
	Garden,
	/// <summary>工場</summary>
	Factory,
	/// <summary>パブ（情報収集）</summary>
	Pub,
	/// <summary>自分のオフィス</summary>
	Office
};

// ============================================================================
// 構造体 (Structures)
// ============================================================================

/// <summary>
/// 証拠データ
/// </summary>
USTRUCT(BlueprintType)
struct FEvidence
{
	GENERATED_BODY()

	/// <summary>証拠の一意ID</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName EvidenceId;

	/// <summary>表示名</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;

	/// <summary>詳細説明</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	/// <summary>証拠の種類</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEvidenceType Type = EEvidenceType::Physical;

	/// <summary>重要度</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEvidenceImportance Importance = EEvidenceImportance::Normal;

	/// <summary>発見場所</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELocation FoundAt = ELocation::Study;

	/// <summary>関連するキャラクターID</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> RelatedCharacters;

	/// <summary>関連する他の証拠ID</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> RelatedEvidence;

	/// <summary>ABELの分析コメント</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ABELComment;

	/// <summary>収集済みかどうか</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsCollected = false;

	/// <summary>調査済みかどうか</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsExamined = false;
};

/// <summary>
/// キャラクター（容疑者・証人）データ
/// </summary>
USTRUCT(BlueprintType)
struct FCharacterData
{
	GENERATED_BODY()

	/// <summary>キャラクターの一意ID</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CharacterId;

	/// <summary>表示名</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;

	/// <summary>役職・立場</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Role;

	/// <summary>キャラクター説明</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	/// <summary>被害者との関係</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText RelationToVictim;

	/// <summary>動機（プレイヤーが発見後に表示）</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Motive;

	/// <summary>現在の感情状態</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEmotionalState EmotionalState = EEmotionalState::Neutral;

	/// <summary>プレイヤーへの信頼度（0-100）</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TrustLevel = 50;

	/// <summary>既に話したかどうか</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasBeenInterviewed = false;

	/// <summary>容疑者かどうか</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsSuspect = true;

	/// <summary>現在いる場所</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELocation CurrentLocation = ELocation::DrawingRoom;
};

/// <summary>
/// 対話選択肢
/// </summary>
USTRUCT(BlueprintType)
struct FDialogueChoice
{
	GENERATED_BODY()

	/// <summary>選択肢ID</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ChoiceId;

	/// <summary>表示テキスト</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayText;

	/// <summary>選択時のトーン</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDialogueTone Tone = EDialogueTone::Polite;

	/// <summary>この選択肢を表示するために必要な証拠ID</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> RequiredEvidence;

	/// <summary>この選択肢を表示するために必要なフラグ</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> RequiredFlags;

	/// <summary>選択後に遷移する対話ノードID</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName NextNodeId;

	/// <summary>選択時に設定されるフラグ</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> SetsFlags;

	/// <summary>信頼度変化</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TrustDelta = 0;
};

/// <summary>
/// 対話ノード（1つの発言単位）
/// </summary>
USTRUCT(BlueprintType)
struct FDialogueNode
{
	GENERATED_BODY()

	/// <summary>ノードID</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName NodeId;

	/// <summary>発言者のキャラクターID（空ならナレーション）</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SpeakerId;

	/// <summary>発言内容</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Text;

	/// <summary>発言時の感情</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEmotionalState Emotion = EEmotionalState::Neutral;

	/// <summary>選択肢（空なら自動で次へ）</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDialogueChoice> Choices;

	/// <summary>選択肢がない場合の次のノードID</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName NextNodeId;

	/// <summary>この対話で得られる証拠ID</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> GainsEvidence;

	/// <summary>このノードに到達時に設定されるフラグ</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> SetsFlags;

	/// <summary>対話終了ノードかどうか</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsEndNode = false;
};

/// <summary>
/// 対話ツリー全体
/// </summary>
USTRUCT(BlueprintType)
struct FDialogueTree
{
	GENERATED_BODY()

	/// <summary>対話ツリーID</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TreeId;

	/// <summary>対話相手のキャラクターID</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CharacterId;

	/// <summary>開始ノードID</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName StartNodeId;

	/// <summary>全ノード</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDialogueNode> Nodes;
};

/// <summary>
/// 推理（2つの証拠を結びつけて結論を導く）
/// </summary>
USTRUCT(BlueprintType)
struct FDeduction
{
	GENERATED_BODY()

	/// <summary>推理ID</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DeductionId;

	/// <summary>推理のタイトル</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Title;

	/// <summary>推理の説明（解放後に表示）</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	/// <summary>必要な証拠ID（2つ）</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName EvidenceA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName EvidenceB;

	/// <summary>この推理で解放されるフラグ</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> UnlocksFlags;

	/// <summary>この推理で解放される新しい対話</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> UnlocksDialogue;

	/// <summary>解放済みかどうか</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsUnlocked = false;
};

/// <summary>
/// ABELの提案
/// </summary>
USTRUCT(BlueprintType)
struct FABELSuggestion
{
	GENERATED_BODY()

	/// <summary>提案ID</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SuggestionId;

	/// <summary>提案タイプ</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EABELSuggestionType Type = EABELSuggestionType::Theory;

	/// <summary>提案内容</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Content;

	/// <summary>ABELの確信度（0.0-1.0）</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Confidence = 0.5f;

	/// <summary>この提案が正しいかどうか（内部判定用）</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsCorrect = true;

	/// <summary>倫理的に問題のある提案か</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsEthicallyQuestionable = false;

	/// <summary>関連する証拠ID</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> RelatedEvidence;

	/// <summary>既に表示したかどうか</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasBeenShown = false;
};

/// <summary>
/// ロケーションデータ
/// </summary>
USTRUCT(BlueprintType)
struct FLocationData
{
	GENERATED_BODY()

	/// <summary>ロケーションタイプ</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELocation Location = ELocation::Study;

	/// <summary>表示名</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;

	/// <summary>説明</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	/// <summary>この場所で見つかる証拠ID</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> AvailableEvidence;

	/// <summary>この場所にいるキャラクターID</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> CharactersPresent;

	/// <summary>訪問済みかどうか</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasVisited = false;

	/// <summary>アクセス可能かどうか</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsAccessible = true;
};

/// <summary>
/// 事件データ全体
/// </summary>
USTRUCT(BlueprintType)
struct FCaseData
{
	GENERATED_BODY()

	/// <summary>事件ID</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CaseId;

	/// <summary>事件タイトル</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Title;

	/// <summary>事件概要</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Synopsis;

	/// <summary>被害者のキャラクターID</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName VictimId;

	/// <summary>真犯人のキャラクターID</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TrueCulpritId;

	/// <summary>全証拠</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEvidence> AllEvidence;

	/// <summary>全キャラクター</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCharacterData> AllCharacters;

	/// <summary>全ロケーション</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLocationData> AllLocations;

	/// <summary>全対話ツリー</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDialogueTree> AllDialogues;

	/// <summary>全推理</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDeduction> AllDeductions;

	/// <summary>犯人告発に必要な証拠ID</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> RequiredEvidenceForAccusation;
};

/// <summary>
/// ゲーム結果・エンディング
/// </summary>
USTRUCT(BlueprintType)
struct FGameResult
{
	GENERATED_BODY()

	/// <summary>正しい犯人を告発したか</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCorrectCulprit = false;

	/// <summary>告発したキャラクターID</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AccusedCharacterId;

	/// <summary>ABELの提案に従った回数</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ABELSuggestionsFollowed = 0;

	/// <summary>ABELの提案を無視した回数</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ABELSuggestionsIgnored = 0;

	/// <summary>倫理的に問題のある行動を取った回数</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 EthicalViolations = 0;

	/// <summary>収集した証拠の割合</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EvidenceCollectionRate = 0.0f;

	/// <summary>エンディングテキスト</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText EndingNarrative;
};
