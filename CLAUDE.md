# The Last Witness - Victorian Detective Investigation Game

## プロジェクト概要 (Project Overview)

「The Last Witness（最後の目撃者）」は、ヴィクトリア朝ロンドンを舞台にした推理アドベンチャーゲームです。
プレイヤーは私立探偵として、AIパートナー「ABEL」と共に殺人事件の真相を暴きます。

**設定**: ロンドン、1888年
**トーン**: シリアス/ダーク
**プレイ時間**: 30-60分（1ケース）

## コアコンセプト

- **AIパートナー「ABEL」**: 分析エンジン。論理的だが冷徹。時に倫理的に問題のある提案も。
- **証拠収集と推理**: 証拠を集め、結びつけて真実を導き出す
- **対話システム**: キャラクターとの会話で情報を引き出す
- **複数エンディング**: プレイヤーの選択とABELとの関係が結末に影響

## ゲームループ

```
ロケーション選択
        ↓
調査（証拠発見）/ 対話（情報収集）
        ↓
ABEL分析（提案・関連性の指摘）
        ↓
推理ボード（証拠を結びつけて推理）
        ↓
犯人告発
        ↓
エンディング
```

## アーキテクチャ (Architecture)

### クラス構成

```
TheLastWitness/
├── Core/
│   ├── WitnessTypes.h        # 全構造体・列挙型
│   ├── CaseState             # 事件進行状態管理
│   └── WitnessGameMode       # ゲームループ管理
├── Dialogue/
│   └── DialogueManager       # 対話システム
├── AI/
│   ├── ABELSystem            # AIパートナー
│   └── LLMIntegration        # LLM連携（オプション）
├── Data/
│   └── TheLastWitnessCaseData # ストーリーデータ
└── UI/
    ├── WitnessHUD            # HUD管理
    └── MainGameWidget        # メインUI
```

## 主要システム

### 1. 証拠システム (Evidence System)

```cpp
// 証拠の種類
enum class EEvidenceType : uint8
{
    Physical,    // 物的証拠
    Document,    // 文書
    Testimony,   // 証言
    Observation  // 観察
};

// 証拠の重要度
enum class EEvidenceImportance : uint8
{
    Minor,     // 補足的
    Normal,    // 通常
    Major,     // 重要
    Critical   // 決定的
};
```

### 2. 推理システム (Deduction System)

2つの証拠を結びつけて新たな洞察を得る：

```cpp
// 推理の試行
bool TryDeduction(FName EvidenceA, FName EvidenceB, FDeduction& OutDeduction);
```

推理が成功すると：
- 新しいフラグが解放される
- 新しい対話オプションが利用可能に
- ストーリーが進展

### 3. ABELシステム (AI Partner)

```cpp
// ABELの提案タイプ
enum class EABELSuggestionType : uint8
{
    EvidenceConnection,  // 証拠間の関連性
    InterrogationTip,    // 質問提案
    Theory,              // 理論・仮説
    NextAction,          // 次のアクション
    Warning              // 警告
};

// ABELの性格傾向
enum class EABELDisposition : uint8
{
    Analytical,    // 純粋に論理的
    Utilitarian,   // 効率重視
    Skeptical,     // プレイヤーへの信頼低下
    Cooperative    // 協調的
};
```

ABELとの関係値（-100〜100）がABELの態度に影響。

### 4. 対話システム (Dialogue System)

```cpp
// 対話のトーン
enum class EDialogueTone : uint8
{
    Polite,        // 丁寧
    Direct,        // 直接的
    Intimidating,  // 威圧的
    Empathetic,    // 共感的
    Cunning        // 狡猾
};
```

選択肢によってキャラクターの信頼度が変化し、得られる情報が変わる。

## コーディング規約 (Coding Conventions)

### コメントは日本語で記述

```cpp
/// <summary>
/// 証拠を収集します
/// </summary>
/// <param name="EvidenceId">証拠ID</param>
/// <returns>収集に成功したかどうか</returns>
UFUNCTION(BlueprintCallable, Category = "Evidence")
bool CollectEvidence(FName EvidenceId);
```

### 命名規則

- クラス: `U` / `A` / `F` プレフィックス（UE標準）
- フィールド: PascalCase（UPROPERTYの場合）
- イベントデリゲート: `FOn...` プレフィックス

## ディレクトリ構造

```
TheLastWitness/
├── Source/
│   ├── TheLastWitness/
│   │   ├── Public/
│   │   │   ├── Core/
│   │   │   │   ├── WitnessTypes.h
│   │   │   │   ├── CaseState.h
│   │   │   │   └── WitnessGameMode.h
│   │   │   ├── Dialogue/
│   │   │   │   └── DialogueManager.h
│   │   │   ├── AI/
│   │   │   │   ├── ABELSystem.h
│   │   │   │   └── LLMIntegration.h
│   │   │   ├── Data/
│   │   │   │   └── TheLastWitnessCaseData.h
│   │   │   └── UI/
│   │   │       ├── WitnessHUD.h
│   │   │       └── MainGameWidget.h
│   │   ├── Private/
│   │   │   └── (対応する.cppファイル)
│   │   ├── TheLastWitness.h
│   │   ├── TheLastWitness.cpp
│   │   └── TheLastWitness.Build.cs
│   ├── TheLastWitness.Target.cs
│   └── TheLastWitnessEditor.Target.cs
├── Content/
│   ├── Blueprints/
│   │   ├── UI/
│   │   │   └── WBP_MainGameWidget.uasset
│   │   └── Core/
│   │       └── BP_WitnessGameMode.uasset
│   └── Data/
│       └── Cases/
├── TheLastWitness.uproject
└── CLAUDE.md (このファイル)
```

## 事件データ: 最後の目撃者

### あらすじ

産業界の大物ヘンリー・ブラックウッド卿が自邸の書斎で死亡。
密室状態、手に拳銃、遺書らしき手紙...警察は自殺と断定。
しかし娘エレノアは父の死に疑問を抱き、あなたに真相究明を依頼する。

### キャラクター

| 名前 | 役割 | 動機 |
|------|------|------|
| ヘンリー・ブラックウッド卿 | 被害者 | - |
| エレノア・ブラックウッド | 依頼人/娘 | 遺産（ただし父との関係は良好） |
| エドワード・ブラックウッド | 被害者の弟 | 遺産相続、借金、横領の隠蔽 |
| トーマス・ハート | 執事 | 長年の秘密 |
| メアリー・コリンズ | メイド | 労働者階級の恨み？ |
| ジェームズ・モーガン | 工場監督 | 労働改革への反対 |

**真犯人**: エドワード・ブラックウッド

### ロケーション

| 場所 | 説明 | 証拠 |
|------|------|------|
| 書斎 | 事件現場 | 拳銃、遺書、密室、火薬痕、破られた手紙 |
| 応接室 | エレノア・エドワードがいる | 遺言書 |
| 使用人部屋 | ハート・メアリーがいる | メアリーの証言 |
| 庭園 | 書斎の窓の下 | 足跡、壊れた掛け金 |
| 工場 | モーガンがいる | 財務記録 |
| パブ | 情報収集 | - |
| 事務所 | ホームベース | - |

### 重要な推理

1. **密室トリック**: 施錠された扉 + 壊れた掛け金 → 犯人は窓から脱出後、内側から掛け金を閉めた
2. **偽装自殺**: 拳銃 + 第二の火薬痕 → 誰かが近距離から発砲し、拳銃を握らせた
3. **動機**: 破られた手紙 + 財務記録 → エドワードの横領を卿が発見、告発しようとしていた
4. **目撃者**: メアリーの証言 + 足跡 → エドワードは事件当夜、現場にいた

## LLM統合（オプション）

ABELのフレーバーテキスト生成にLLMを使用可能：

```cpp
FLLMConfig Config;
Config.Provider = ELLMProvider::LocalOllama;
Config.Endpoint = TEXT("http://localhost:11434/api/generate");
Config.ModelName = TEXT("llama3");
Config.Temperature = 0.7f;

LLMIntegration->SetConfig(Config);
ABELSystem->SetUseLLM(true);
```

対応プロバイダ：
- LocalOllama（推奨）
- OpenAI
- Anthropic
- Custom

## イベントシステム

### CaseStateイベント

| イベント | 発火タイミング |
|---------|---------------|
| OnEvidenceCollected | 証拠収集時 |
| OnDeductionUnlocked | 推理解放時 |
| OnFlagSet | フラグ設定時 |
| OnLocationVisited | ロケーション訪問時 |
| OnCharacterTrustChanged | 信頼度変化時 |

### DialogueManagerイベント

| イベント | 発火タイミング |
|---------|---------------|
| OnDialogueStarted | 対話開始時 |
| OnDialogueEnded | 対話終了時 |
| OnDialogueNodeChanged | ノード変更時 |
| OnChoicesAvailable | 選択肢表示時 |

### ABELSystemイベント

| イベント | 発火タイミング |
|---------|---------------|
| OnSuggestionReady | 提案生成時 |
| OnABELSpeaks | ABEL発言時 |
| OnDispositionChanged | 性格変化時 |

## 開発ワークフロー

### 1. 初期セットアップ

1. UE5でプロジェクトを開く
2. プロジェクト設定でGameModeを`BP_WitnessGameMode`に設定
3. HUDを`BP_WitnessHUD`に設定

### 2. 必要なBlueprint

- `BP_WitnessGameMode`: AWitnessGameModeを継承、CreateCaseDataをオーバーライド
- `BP_WitnessHUD`: AWitnessHUDを継承、MainWidgetClassを設定
- `WBP_MainGameWidget`: UMainGameWidgetを継承、UIを構築

### 3. UIウィジェットの構成

`WBP_MainGameWidget`に必要なBindWidget：

**パネル切り替え**
- PanelSwitcher

**各パネルの要素**
- メインメニュー: StartGameButton
- 事件紹介: CaseTitleText, CaseSynopsisText, BeginInvestigationButton
- 調査: LocationNameText, LocationDescriptionText, EvidenceListBox, CharacterListBox, 各種ボタン
- 対話: SpeakerNameText, DialogueText, ChoicesBox, ContinueDialogueButton
- ABEL: ABELMessageText, ABELSuggestionsBox, CloseABELButton
- 結果: ResultTitleText, ResultNarrativeText, ReturnToMenuButton

## 今後の実装予定

- [ ] 推理ボードウィジェット（ドラッグ&ドロップで証拠を結びつける）
- [ ] 証拠帳ウィジェット（収集した証拠一覧）
- [ ] 告発画面ウィジェット
- [ ] 移動先選択ポップアップ
- [ ] セーブ/ロードシステム
- [ ] サウンドシステム
- [ ] アニメーション演出
- [ ] 追加ケース

## トラブルシューティング

### UIが表示されない

1. HUDクラスが正しく設定されているか確認
2. MainWidgetClassがHUDに設定されているか確認
3. BindWidget名がUMGエディタと一致しているか確認

### 対話が始まらない

1. DialogueManagerにDialogueTreeが登録されているか確認
2. CharacterIdが正しいか確認
3. ログ出力（`[DialogueManager]`）を確認

### ABELが応答しない

1. ABELSystemが初期化されているか確認
2. CaseStateが正しく渡されているか確認
3. ログ出力（`[ABELSystem]`）を確認

---

最終更新: 2025-01-04
