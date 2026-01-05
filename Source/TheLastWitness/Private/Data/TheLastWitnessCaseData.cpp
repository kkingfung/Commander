// Copyright Epic Games, Inc. All Rights Reserved.

#include "Data/TheLastWitnessCaseData.h"

FCaseData UTheLastWitnessCaseData::CreateCaseData()
{
	FCaseData CaseData;

	CaseData.CaseId = FName("TheLastWitness");
	CaseData.Title = FText::FromString(TEXT("最後の目撃者"));
	CaseData.Synopsis = FText::FromString(TEXT(
		"1888年11月、ロンドン。\n\n"
		"産業界の大物ヘンリー・ブラックウッド卿が、自邸の書斎で死体となって発見された。\n"
		"密室状態の部屋、手に握られた拳銃、そして遺書らしき手紙...\n"
		"スコットランド・ヤードは自殺と断定した。\n\n"
		"しかし、彼の娘エレノアは父の死に疑問を抱き、私立探偵であるあなたに真相の究明を依頼してきた。\n"
		"「父は自ら命を絶つような人間ではありません」\n\n"
		"分析エンジン「ABEL」と共に、霧のロンドンに潜む真実を暴け。"
	));

	CaseData.VictimId = FName("HenryBlackwood");
	CaseData.TrueCulpritId = FName("EdwardBlackwood");

	CaseData.AllCharacters = CreateCharacters();
	CaseData.AllEvidence = CreateEvidence();
	CaseData.AllLocations = CreateLocations();
	CaseData.AllDialogues = CreateDialogues();
	CaseData.AllDeductions = CreateDeductions();

	// 告発に必要な証拠
	CaseData.RequiredEvidenceForAccusation.Add(FName("Evidence_TornLetter"));
	CaseData.RequiredEvidenceForAccusation.Add(FName("Evidence_FinancialRecords"));
	CaseData.RequiredEvidenceForAccusation.Add(FName("Evidence_SecondGunpowder"));

	return CaseData;
}

TArray<FCharacterData> UTheLastWitnessCaseData::CreateCharacters()
{
	TArray<FCharacterData> Characters;

	// 被害者: ヘンリー・ブラックウッド
	{
		FCharacterData C;
		C.CharacterId = FName("HenryBlackwood");
		C.DisplayName = FText::FromString(TEXT("ヘンリー・ブラックウッド卿"));
		C.Role = FText::FromString(TEXT("被害者 / 実業家"));
		C.Description = FText::FromString(TEXT(
			"ブラックウッド製鉄所のオーナー。55歳。\n"
			"厳格だが公正な人物として知られていた。\n"
			"最近、工場の労働環境改善を進めていたという。"
		));
		C.RelationToVictim = FText::FromString(TEXT("本人"));
		C.bIsSuspect = false;
		C.CurrentLocation = ELocation::Study;
		Characters.Add(C);
	}

	// 容疑者1: エレノア・ブラックウッド（依頼人）
	{
		FCharacterData C;
		C.CharacterId = FName("EleanorBlackwood");
		C.DisplayName = FText::FromString(TEXT("エレノア・ブラックウッド"));
		C.Role = FText::FromString(TEXT("依頼人 / 被害者の娘"));
		C.Description = FText::FromString(TEXT(
			"ブラックウッド卿の一人娘。24歳。\n"
			"父の死を自殺と認めず、真相究明を依頼してきた。\n"
			"聡明で意志が強いが、何かを隠しているようにも見える。"
		));
		C.RelationToVictim = FText::FromString(TEXT("娘"));
		C.Motive = FText::FromString(TEXT("遺産相続の可能性があるが、父との関係は良好だった。"));
		C.bIsSuspect = true;
		C.CurrentLocation = ELocation::DrawingRoom;
		C.TrustLevel = 70;
		Characters.Add(C);
	}

	// 容疑者2: エドワード・ブラックウッド（弟）
	{
		FCharacterData C;
		C.CharacterId = FName("EdwardBlackwood");
		C.DisplayName = FText::FromString(TEXT("エドワード・ブラックウッド"));
		C.Role = FText::FromString(TEXT("被害者の弟"));
		C.Description = FText::FromString(TEXT(
			"ブラックウッド卿の弟。48歳。\n"
			"工場の経営には関わらず、社交界で浪費生活を送っている。\n"
			"最近、多額の借金を抱えているという噂がある。"
		));
		C.RelationToVictim = FText::FromString(TEXT("弟"));
		C.Motive = FText::FromString(TEXT("兄の死により遺産の一部を相続できる。借金問題。"));
		C.bIsSuspect = true;
		C.CurrentLocation = ELocation::DrawingRoom;
		C.TrustLevel = 40;
		C.EmotionalState = EEmotionalState::Defensive;
		Characters.Add(C);
	}

	// 容疑者3: トーマス・ハート（執事）
	{
		FCharacterData C;
		C.CharacterId = FName("ThomasHart");
		C.DisplayName = FText::FromString(TEXT("トーマス・ハート"));
		C.Role = FText::FromString(TEXT("執事"));
		C.Description = FText::FromString(TEXT(
			"ブラックウッド家に30年仕える執事。62歳。\n"
			"屋敷のことは何でも知っている。\n"
			"主人に忠実だったが、事件の夜は不在だったと主張している。"
		));
		C.RelationToVictim = FText::FromString(TEXT("使用人"));
		C.Motive = FText::FromString(TEXT("表向きの動機はないが、長年の秘密を握っている可能性がある。"));
		C.bIsSuspect = true;
		C.CurrentLocation = ELocation::ServantsQuarters;
		C.TrustLevel = 50;
		Characters.Add(C);
	}

	// 容疑者4: メアリー・コリンズ（メイド）
	{
		FCharacterData C;
		C.CharacterId = FName("MaryCollins");
		C.DisplayName = FText::FromString(TEXT("メアリー・コリンズ"));
		C.Role = FText::FromString(TEXT("メイド"));
		C.Description = FText::FromString(TEXT(
			"ブラックウッド家のメイド。22歳。\n"
			"工場労働者の家庭出身。\n"
			"事件当夜、何かを目撃したような素振りを見せている。"
		));
		C.RelationToVictim = FText::FromString(TEXT("使用人"));
		C.Motive = FText::FromString(TEXT("労働者階級として、雇用主への恨みがある可能性。"));
		C.bIsSuspect = true;
		C.CurrentLocation = ELocation::ServantsQuarters;
		C.TrustLevel = 45;
		C.EmotionalState = EEmotionalState::Nervous;
		Characters.Add(C);
	}

	// 容疑者5: ジェームズ・モーガン（工場監督）
	{
		FCharacterData C;
		C.CharacterId = FName("JamesMorgan");
		C.DisplayName = FText::FromString(TEXT("ジェームズ・モーガン"));
		C.Role = FText::FromString(TEXT("工場監督"));
		C.Description = FText::FromString(TEXT(
			"ブラックウッド製鉄所の監督。45歳。\n"
			"労働者には厳しいが、効率的な経営で知られる。\n"
			"最近の労働環境改善計画に反対していたという。"
		));
		C.RelationToVictim = FText::FromString(TEXT("従業員"));
		C.Motive = FText::FromString(TEXT("労働改革が実施されれば、自分の権限が弱まる。"));
		C.bIsSuspect = true;
		C.CurrentLocation = ELocation::Factory;
		C.TrustLevel = 35;
		C.EmotionalState = EEmotionalState::Defensive;
		Characters.Add(C);
	}

	return Characters;
}

TArray<FEvidence> UTheLastWitnessCaseData::CreateEvidence()
{
	TArray<FEvidence> Evidence;

	// 書斎の証拠
	{
		FEvidence E;
		E.EvidenceId = FName("Evidence_Pistol");
		E.DisplayName = FText::FromString(TEXT("拳銃"));
		E.Description = FText::FromString(TEXT(
			"被害者の手に握られていた拳銃。\n"
			"弾倉には一発だけ使用された形跡がある。\n"
			"ブラックウッド卿の私物であることが確認されている。"
		));
		E.Type = EEvidenceType::Physical;
		E.Importance = EEvidenceImportance::Critical;
		E.FoundAt = ELocation::Study;
		E.ABELComment = FText::FromString(TEXT(
			"火薬残渣の分布パターンを分析中... "
			"通常の自殺における発砲とは異なる痕跡を検出しました。"
		));
		E.RelatedEvidence.Add(FName("Evidence_SecondGunpowder"));
		Evidence.Add(E);
	}

	{
		FEvidence E;
		E.EvidenceId = FName("Evidence_SuicideNote");
		E.DisplayName = FText::FromString(TEXT("遺書"));
		E.Description = FText::FromString(TEXT(
			"被害者のデスクで発見された手紙。\n"
			"「罪の重さに耐えられない」と記されている。\n"
			"筆跡はブラックウッド卿のものに酷似している。"
		));
		E.Type = EEvidenceType::Document;
		E.Importance = EEvidenceImportance::Major;
		E.FoundAt = ELocation::Study;
		E.ABELComment = FText::FromString(TEXT(
			"筆跡分析を実行中... 基本的なパターンは一致しますが、"
			"筆圧と傾斜角に微細な不一致を検出しました。"
		));
		E.RelatedEvidence.Add(FName("Evidence_TornLetter"));
		Evidence.Add(E);
	}

	{
		FEvidence E;
		E.EvidenceId = FName("Evidence_LockedDoor");
		E.DisplayName = FText::FromString(TEXT("施錠された扉"));
		E.Description = FText::FromString(TEXT(
			"書斎の扉は内側から鍵がかかっていた。\n"
			"発見者は扉を壊して入室した。\n"
			"窓も内側から施錠されていた。"
		));
		E.Type = EEvidenceType::Observation;
		E.Importance = EEvidenceImportance::Major;
		E.FoundAt = ELocation::Study;
		E.ABELComment = FText::FromString(TEXT(
			"密室状態を分析中... 既知の手法で密室を作り出す可能性を検討します。"
		));
		Evidence.Add(E);
	}

	{
		FEvidence E;
		E.EvidenceId = FName("Evidence_SecondGunpowder");
		E.DisplayName = FText::FromString(TEXT("第二の火薬痕"));
		E.Description = FText::FromString(TEXT(
			"被害者の服に、拳銃の発砲位置とは異なる角度からの火薬痕を発見。\n"
			"これは自殺では説明できない。\n"
			"誰かが近距離から発砲した可能性を示唆している。"
		));
		E.Type = EEvidenceType::Physical;
		E.Importance = EEvidenceImportance::Critical;
		E.FoundAt = ELocation::Study;
		E.ABELComment = FText::FromString(TEXT(
			"決定的な証拠です。この痕跡は被害者が自ら発砲したものではありません。"
			"第三者の関与を強く示唆しています。"
		));
		E.RelatedEvidence.Add(FName("Evidence_Pistol"));
		Evidence.Add(E);
	}

	{
		FEvidence E;
		E.EvidenceId = FName("Evidence_TornLetter");
		E.DisplayName = FText::FromString(TEXT("破られた手紙の断片"));
		E.Description = FText::FromString(TEXT(
			"ゴミ箱から発見された手紙の断片。\n"
			"「...お前の裏切りは許さん...」「...全てを告発する...」という文言が読める。\n"
			"筆跡はブラックウッド卿のもの。"
		));
		E.Type = EEvidenceType::Document;
		E.Importance = EEvidenceImportance::Critical;
		E.FoundAt = ELocation::Study;
		E.ABELComment = FText::FromString(TEXT(
			"この手紙は事件の動機を示唆しています。"
			"ブラックウッド卿は誰かの裏切りを知り、告発しようとしていたようです。"
		));
		E.RelatedCharacters.Add(FName("EdwardBlackwood"));
		Evidence.Add(E);
	}

	// 応接室の証拠
	{
		FEvidence E;
		E.EvidenceId = FName("Evidence_WillDocument");
		E.DisplayName = FText::FromString(TEXT("遺言書"));
		E.Description = FText::FromString(TEXT(
			"ブラックウッド卿の遺言書の写し。\n"
			"遺産の大部分はエレノアに、一部がエドワードに相続される。\n"
			"ただし、特定の条件下ではエドワードへの相続は取り消される。"
		));
		E.Type = EEvidenceType::Document;
		E.Importance = EEvidenceImportance::Major;
		E.FoundAt = ELocation::DrawingRoom;
		E.ABELComment = FText::FromString(TEXT(
			"遺言の条項を分析中... "
			"エドワード氏への相続取り消し条件は「不正行為の発覚」です。"
		));
		E.RelatedCharacters.Add(FName("EdwardBlackwood"));
		E.RelatedCharacters.Add(FName("EleanorBlackwood"));
		Evidence.Add(E);
	}

	// 使用人部屋の証拠
	{
		FEvidence E;
		E.EvidenceId = FName("Evidence_MaryTestimony");
		E.DisplayName = FText::FromString(TEXT("メアリーの証言"));
		E.Description = FText::FromString(TEXT(
			"メアリーは事件当夜、書斎の前を通った際に\n"
			"中から二人の男性の声を聞いたと証言した。\n"
			"一人はブラックウッド卿、もう一人は聞き覚えのある声だった。"
		));
		E.Type = EEvidenceType::Testimony;
		E.Importance = EEvidenceImportance::Critical;
		E.FoundAt = ELocation::ServantsQuarters;
		E.ABELComment = FText::FromString(TEXT(
			"重要な証言です。密室で「自殺」した被害者に、"
			"死亡時刻前後に訪問者がいた可能性を示しています。"
		));
		E.RelatedCharacters.Add(FName("MaryCollins"));
		Evidence.Add(E);
	}

	// 工場の証拠
	{
		FEvidence E;
		E.EvidenceId = FName("Evidence_FinancialRecords");
		E.DisplayName = FText::FromString(TEXT("財務記録"));
		E.Description = FText::FromString(TEXT(
			"工場の財務記録に不審な出金が見つかった。\n"
			"過去3年間で大量の資金が「設備費」として計上されているが、\n"
			"実際にはその設備は存在しない。エドワードの署名がある。"
		));
		E.Type = EEvidenceType::Document;
		E.Importance = EEvidenceImportance::Critical;
		E.FoundAt = ELocation::Factory;
		E.ABELComment = FText::FromString(TEXT(
			"横領の証拠です。エドワード・ブラックウッド氏が\n"
			"工場資金を私的に流用していたことを示しています。"
		));
		E.RelatedCharacters.Add(FName("EdwardBlackwood"));
		E.RelatedEvidence.Add(FName("Evidence_TornLetter"));
		Evidence.Add(E);
	}

	// 庭園の証拠
	{
		FEvidence E;
		E.EvidenceId = FName("Evidence_Footprints");
		E.DisplayName = FText::FromString(TEXT("足跡"));
		E.Description = FText::FromString(TEXT(
			"庭園で発見された足跡。\n"
			"書斎の窓の下から裏門へと続いている。\n"
			"サイズは男性用で、高級な革靴の痕跡。"
		));
		E.Type = EEvidenceType::Physical;
		E.Importance = EEvidenceImportance::Major;
		E.FoundAt = ELocation::Garden;
		E.ABELComment = FText::FromString(TEXT(
			"足跡の深さから推定される体重は約75kg。\n"
			"エドワード・ブラックウッド氏の体格と一致します。"
		));
		E.RelatedCharacters.Add(FName("EdwardBlackwood"));
		Evidence.Add(E);
	}

	{
		FEvidence E;
		E.EvidenceId = FName("Evidence_BrokenLatch");
		E.DisplayName = FText::FromString(TEXT("壊れた掛け金"));
		E.Description = FText::FromString(TEXT(
			"書斎の窓の掛け金が内側から無理やり閉められた形跡がある。\n"
			"これにより、外から窓を閉めて密室を作ることが可能だった。"
		));
		E.Type = EEvidenceType::Physical;
		E.Importance = EEvidenceImportance::Major;
		E.FoundAt = ELocation::Garden;
		E.ABELComment = FText::FromString(TEXT(
			"密室のトリックを解明しました。犯人は窓から脱出した後、\n"
			"細い道具を使って内側から掛け金を閉めたと推定されます。"
		));
		E.RelatedEvidence.Add(FName("Evidence_LockedDoor"));
		Evidence.Add(E);
	}

	return Evidence;
}

TArray<FLocationData> UTheLastWitnessCaseData::CreateLocations()
{
	TArray<FLocationData> Locations;

	{
		FLocationData L;
		L.Location = ELocation::Study;
		L.DisplayName = FText::FromString(TEXT("書斎"));
		L.Description = FText::FromString(TEXT(
			"ブラックウッド卿が発見された部屋。\n"
			"重厚なオーク材のデスク、本棚、そして暖炉がある。\n"
			"窓からは庭園が見える。"
		));
		L.AvailableEvidence.Add(FName("Evidence_Pistol"));
		L.AvailableEvidence.Add(FName("Evidence_SuicideNote"));
		L.AvailableEvidence.Add(FName("Evidence_LockedDoor"));
		L.AvailableEvidence.Add(FName("Evidence_SecondGunpowder"));
		L.AvailableEvidence.Add(FName("Evidence_TornLetter"));
		Locations.Add(L);
	}

	{
		FLocationData L;
		L.Location = ELocation::DrawingRoom;
		L.DisplayName = FText::FromString(TEXT("応接室"));
		L.Description = FText::FromString(TEXT(
			"屋敷の応接室。\n"
			"エレノアとエドワードが待機している。\n"
			"壁には家族の肖像画が飾られている。"
		));
		L.AvailableEvidence.Add(FName("Evidence_WillDocument"));
		L.CharactersPresent.Add(FName("EleanorBlackwood"));
		L.CharactersPresent.Add(FName("EdwardBlackwood"));
		Locations.Add(L);
	}

	{
		FLocationData L;
		L.Location = ELocation::ServantsQuarters;
		L.DisplayName = FText::FromString(TEXT("使用人の部屋"));
		L.Description = FText::FromString(TEXT(
			"屋敷の使用人たちが生活する区画。\n"
			"執事のハートとメイドのメアリーがいる。\n"
			"質素だが清潔に保たれている。"
		));
		L.AvailableEvidence.Add(FName("Evidence_MaryTestimony"));
		L.CharactersPresent.Add(FName("ThomasHart"));
		L.CharactersPresent.Add(FName("MaryCollins"));
		Locations.Add(L);
	}

	{
		FLocationData L;
		L.Location = ELocation::Garden;
		L.DisplayName = FText::FromString(TEXT("庭園"));
		L.Description = FText::FromString(TEXT(
			"屋敷を囲む広大な庭園。\n"
			"書斎の窓の下には花壇がある。\n"
			"裏門は通りに面している。"
		));
		L.AvailableEvidence.Add(FName("Evidence_Footprints"));
		L.AvailableEvidence.Add(FName("Evidence_BrokenLatch"));
		Locations.Add(L);
	}

	{
		FLocationData L;
		L.Location = ELocation::Factory;
		L.DisplayName = FText::FromString(TEXT("ブラックウッド製鉄所"));
		L.Description = FText::FromString(TEXT(
			"ブラックウッド卿が経営する製鉄所。\n"
			"煙突からは煙が立ち上っている。\n"
			"監督のモーガンがいる。"
		));
		L.AvailableEvidence.Add(FName("Evidence_FinancialRecords"));
		L.CharactersPresent.Add(FName("JamesMorgan"));
		Locations.Add(L);
	}

	{
		FLocationData L;
		L.Location = ELocation::Pub;
		L.DisplayName = FText::FromString(TEXT("黒馬亭"));
		L.Description = FText::FromString(TEXT(
			"工場労働者たちが集まるパブ。\n"
			"噂話や情報を集めるのに適している。"
		));
		Locations.Add(L);
	}

	{
		FLocationData L;
		L.Location = ELocation::Office;
		L.DisplayName = FText::FromString(TEXT("探偵事務所"));
		L.Description = FText::FromString(TEXT(
			"あなたのオフィス。\n"
			"ABELの端末が設置されている。\n"
			"収集した情報を整理できる。"
		));
		Locations.Add(L);
	}

	return Locations;
}

TArray<FDialogueTree> UTheLastWitnessCaseData::CreateDialogues()
{
	TArray<FDialogueTree> Dialogues;

	// エレノアとの対話
	{
		FDialogueTree Tree;
		Tree.TreeId = FName("Dialogue_Eleanor");
		Tree.CharacterId = FName("EleanorBlackwood");
		Tree.StartNodeId = FName("Eleanor_Start");

		// 開始ノード
		{
			FDialogueNode Node;
			Node.NodeId = FName("Eleanor_Start");
			Node.SpeakerId = FName("EleanorBlackwood");
			Node.Text = FText::FromString(TEXT(
				"探偵さん、来てくださってありがとうございます。\n"
				"父は...父は自殺なんかする人ではありません。\n"
				"どうか、真実を明らかにしてください。"
			));
			Node.Emotion = EEmotionalState::Sad;

			FDialogueChoice Choice1;
			Choice1.ChoiceId = FName("Eleanor_Choice_Comfort");
			Choice1.DisplayText = FText::FromString(TEXT("お悔やみ申し上げます。できる限りのことをします。"));
			Choice1.Tone = EDialogueTone::Empathetic;
			Choice1.NextNodeId = FName("Eleanor_Grateful");
			Choice1.TrustDelta = 10;
			Node.Choices.Add(Choice1);

			FDialogueChoice Choice2;
			Choice2.ChoiceId = FName("Eleanor_Choice_Direct");
			Choice2.DisplayText = FText::FromString(TEXT("なぜ自殺ではないと思うのですか？"));
			Choice2.Tone = EDialogueTone::Direct;
			Choice2.NextNodeId = FName("Eleanor_Explain");
			Node.Choices.Add(Choice2);

			FDialogueChoice Choice3;
			Choice3.ChoiceId = FName("Eleanor_Choice_Suspicious");
			Choice3.DisplayText = FText::FromString(TEXT("あなた自身には何かやましいことは？"));
			Choice3.Tone = EDialogueTone::Intimidating;
			Choice3.NextNodeId = FName("Eleanor_Offended");
			Choice3.TrustDelta = -15;
			Node.Choices.Add(Choice3);

			Tree.Nodes.Add(Node);
		}

		// 感謝ノード
		{
			FDialogueNode Node;
			Node.NodeId = FName("Eleanor_Grateful");
			Node.SpeakerId = FName("EleanorBlackwood");
			Node.Text = FText::FromString(TEXT(
				"ありがとうございます...\n"
				"父は最近、何かに悩んでいるようでした。\n"
				"叔父のエドワードと言い争っているのを何度か見ました。"
			));
			Node.Emotion = EEmotionalState::Sad;
			Node.GainsEvidence.Add(FName("Evidence_WillDocument"));
			Node.NextNodeId = FName("Eleanor_End");
			Tree.Nodes.Add(Node);
		}

		// 説明ノード
		{
			FDialogueNode Node;
			Node.NodeId = FName("Eleanor_Explain");
			Node.SpeakerId = FName("EleanorBlackwood");
			Node.Text = FText::FromString(TEXT(
				"父は強い人でした。どんな困難にも立ち向かう人でした。\n"
				"それに、来月には工場の労働環境改善計画を発表する予定だったんです。\n"
				"希望を持っていた人が自ら命を絶つなんて..."
			));
			Node.Emotion = EEmotionalState::Sad;
			Node.NextNodeId = FName("Eleanor_End");
			Tree.Nodes.Add(Node);
		}

		// 立腹ノード
		{
			FDialogueNode Node;
			Node.NodeId = FName("Eleanor_Offended");
			Node.SpeakerId = FName("EleanorBlackwood");
			Node.Text = FText::FromString(TEXT(
				"...私を疑っているのですか？\n"
				"父は私の全てでした。そんな質問をされるとは思いませんでした。"
			));
			Node.Emotion = EEmotionalState::Angry;
			Node.NextNodeId = FName("Eleanor_End");
			Tree.Nodes.Add(Node);
		}

		// 終了ノード
		{
			FDialogueNode Node;
			Node.NodeId = FName("Eleanor_End");
			Node.SpeakerId = FName("EleanorBlackwood");
			Node.Text = FText::FromString(TEXT(
				"他に聞きたいことがあれば、いつでもどうぞ。\n"
				"真実のためなら、何でもお話しします。"
			));
			Node.bIsEndNode = true;
			Tree.Nodes.Add(Node);
		}

		Dialogues.Add(Tree);
	}

	// エドワードとの対話
	{
		FDialogueTree Tree;
		Tree.TreeId = FName("Dialogue_Edward");
		Tree.CharacterId = FName("EdwardBlackwood");
		Tree.StartNodeId = FName("Edward_Start");

		{
			FDialogueNode Node;
			Node.NodeId = FName("Edward_Start");
			Node.SpeakerId = FName("EdwardBlackwood");
			Node.Text = FText::FromString(TEXT(
				"ああ、探偵さんですか。\n"
				"姪が雇ったそうですね。無駄な出費だと思いますがね。\n"
				"兄は自殺したんです。警察もそう言っている。"
			));
			Node.Emotion = EEmotionalState::Defensive;

			FDialogueChoice Choice1;
			Choice1.ChoiceId = FName("Edward_Choice_Polite");
			Choice1.DisplayText = FText::FromString(TEXT("念のための調査です。ご協力いただければ。"));
			Choice1.Tone = EDialogueTone::Polite;
			Choice1.NextNodeId = FName("Edward_Reluctant");
			Node.Choices.Add(Choice1);

			FDialogueChoice Choice2;
			Choice2.ChoiceId = FName("Edward_Choice_Money");
			Choice2.DisplayText = FText::FromString(TEXT("お兄様の死で、遺産を相続されるそうですね。"));
			Choice2.Tone = EDialogueTone::Cunning;
			Choice2.NextNodeId = FName("Edward_Defensive");
			Choice2.TrustDelta = -10;
			Node.Choices.Add(Choice2);

			FDialogueChoice Choice3;
			Choice3.ChoiceId = FName("Edward_Choice_Accuse");
			Choice3.DisplayText = FText::FromString(TEXT("事件当夜、どこにいましたか？"));
			Choice3.Tone = EDialogueTone::Direct;
			Choice3.NextNodeId = FName("Edward_Alibi");
			Node.Choices.Add(Choice3);

			Tree.Nodes.Add(Node);
		}

		{
			FDialogueNode Node;
			Node.NodeId = FName("Edward_Reluctant");
			Node.SpeakerId = FName("EdwardBlackwood");
			Node.Text = FText::FromString(TEXT(
				"まあ、好きにすればいい。\n"
				"どうせ何も見つからないでしょうがね。"
			));
			Node.Emotion = EEmotionalState::Neutral;
			Node.NextNodeId = FName("Edward_End");
			Tree.Nodes.Add(Node);
		}

		{
			FDialogueNode Node;
			Node.NodeId = FName("Edward_Defensive");
			Node.SpeakerId = FName("EdwardBlackwood");
			Node.Text = FText::FromString(TEXT(
				"何が言いたいのです！？\n"
				"私が兄を殺したとでも？ 馬鹿な！\n"
				"私はその夜、クラブにいました。証人もいます！"
			));
			Node.Emotion = EEmotionalState::Angry;
			Node.NextNodeId = FName("Edward_End");
			Tree.Nodes.Add(Node);
		}

		{
			FDialogueNode Node;
			Node.NodeId = FName("Edward_Alibi");
			Node.SpeakerId = FName("EdwardBlackwood");
			Node.Text = FText::FromString(TEXT(
				"その夜は...ホワイトチャペル・クラブにいました。\n"
				"夜11時まで。バーテンダーに確認できます。"
			));
			Node.Emotion = EEmotionalState::Nervous;
			Node.SetsFlags.Add(FName("Flag_EdwardAlibiClaimed"));
			Node.NextNodeId = FName("Edward_End");
			Tree.Nodes.Add(Node);
		}

		{
			FDialogueNode Node;
			Node.NodeId = FName("Edward_End");
			Node.SpeakerId = FName("EdwardBlackwood");
			Node.Text = FText::FromString(TEXT(
				"これ以上の質問は弁護士を通してください。"
			));
			Node.bIsEndNode = true;
			Tree.Nodes.Add(Node);
		}

		Dialogues.Add(Tree);
	}

	// メアリーとの対話
	{
		FDialogueTree Tree;
		Tree.TreeId = FName("Dialogue_Mary");
		Tree.CharacterId = FName("MaryCollins");
		Tree.StartNodeId = FName("Mary_Start");

		{
			FDialogueNode Node;
			Node.NodeId = FName("Mary_Start");
			Node.SpeakerId = FName("MaryCollins");
			Node.Text = FText::FromString(TEXT(
				"あ、あの...探偵さんですか？\n"
				"私、何も知りませんから..."
			));
			Node.Emotion = EEmotionalState::Nervous;

			FDialogueChoice Choice1;
			Choice1.ChoiceId = FName("Mary_Choice_Kind");
			Choice1.DisplayText = FText::FromString(TEXT("怖がらなくていい。話を聞かせてくれるだけでいいんだ。"));
			Choice1.Tone = EDialogueTone::Empathetic;
			Choice1.NextNodeId = FName("Mary_Opens");
			Choice1.TrustDelta = 15;
			Node.Choices.Add(Choice1);

			FDialogueChoice Choice2;
			Choice2.ChoiceId = FName("Mary_Choice_Pressure");
			Choice2.DisplayText = FText::FromString(TEXT("何か隠しているなら、今話した方がいい。"));
			Choice2.Tone = EDialogueTone::Intimidating;
			Choice2.NextNodeId = FName("Mary_Scared");
			Choice2.TrustDelta = -20;
			Node.Choices.Add(Choice2);

			Tree.Nodes.Add(Node);
		}

		{
			FDialogueNode Node;
			Node.NodeId = FName("Mary_Opens");
			Node.SpeakerId = FName("MaryCollins");
			Node.Text = FText::FromString(TEXT(
				"実は...あの夜、書斎の前を通った時...\n"
				"中から声が聞こえたんです。二人の男の人の声。\n"
				"一人は旦那様で、もう一人は..."
			));
			Node.Emotion = EEmotionalState::Nervous;
			Node.NextNodeId = FName("Mary_Reveal");
			Tree.Nodes.Add(Node);
		}

		{
			FDialogueNode Node;
			Node.NodeId = FName("Mary_Reveal");
			Node.SpeakerId = FName("MaryCollins");
			Node.Text = FText::FromString(TEXT(
				"もう一人の声...エドワード様に似ていました。\n"
				"「金のことは黙っていろ」って怒鳴っているのが聞こえて...\n"
				"私、怖くなって逃げたんです..."
			));
			Node.Emotion = EEmotionalState::Fearful;
			Node.GainsEvidence.Add(FName("Evidence_MaryTestimony"));
			Node.NextNodeId = FName("Mary_End");
			Tree.Nodes.Add(Node);
		}

		{
			FDialogueNode Node;
			Node.NodeId = FName("Mary_Scared");
			Node.SpeakerId = FName("MaryCollins");
			Node.Text = FText::FromString(TEXT(
				"ひっ...! お、脅さないでください...\n"
				"本当に何も知らないんです...!"
			));
			Node.Emotion = EEmotionalState::Fearful;
			Node.NextNodeId = FName("Mary_End");
			Tree.Nodes.Add(Node);
		}

		{
			FDialogueNode Node;
			Node.NodeId = FName("Mary_End");
			Node.SpeakerId = FName("MaryCollins");
			Node.Text = FText::FromString(TEXT(
				"もう...行ってもいいですか...?"
			));
			Node.bIsEndNode = true;
			Tree.Nodes.Add(Node);
		}

		Dialogues.Add(Tree);
	}

	return Dialogues;
}

TArray<FDeduction> UTheLastWitnessCaseData::CreateDeductions()
{
	TArray<FDeduction> Deductions;

	// 推理1: 密室トリック
	{
		FDeduction D;
		D.DeductionId = FName("Deduction_LockedRoom");
		D.Title = FText::FromString(TEXT("密室のトリック"));
		D.Description = FText::FromString(TEXT(
			"壊れた掛け金と施錠された扉を結びつけると...\n"
			"犯人は窓から脱出した後、細い道具を使って内側から掛け金を閉めた。\n"
			"これで密室を作り出したのだ。"
		));
		D.EvidenceA = FName("Evidence_LockedDoor");
		D.EvidenceB = FName("Evidence_BrokenLatch");
		D.UnlocksFlags.Add(FName("Flag_LockedRoomSolved"));
		Deductions.Add(D);
	}

	// 推理2: 偽装された自殺
	{
		FDeduction D;
		D.DeductionId = FName("Deduction_FakedSuicide");
		D.Title = FText::FromString(TEXT("偽装された自殺"));
		D.Description = FText::FromString(TEXT(
			"拳銃と第二の火薬痕を結びつけると...\n"
			"被害者は自ら発砲したのではない。\n"
			"誰かが近距離から発砲し、その後拳銃を被害者の手に握らせた。"
		));
		D.EvidenceA = FName("Evidence_Pistol");
		D.EvidenceB = FName("Evidence_SecondGunpowder");
		D.UnlocksFlags.Add(FName("Flag_SuicideFaked"));
		Deductions.Add(D);
	}

	// 推理3: 動機の解明
	{
		FDeduction D;
		D.DeductionId = FName("Deduction_Motive");
		D.Title = FText::FromString(TEXT("横領という動機"));
		D.Description = FText::FromString(TEXT(
			"破られた手紙と財務記録を結びつけると...\n"
			"ブラックウッド卿はエドワードの横領を発見し、告発しようとしていた。\n"
			"エドワードには卿を殺す強い動機があった。"
		));
		D.EvidenceA = FName("Evidence_TornLetter");
		D.EvidenceB = FName("Evidence_FinancialRecords");
		D.UnlocksFlags.Add(FName("Flag_MotiveFound"));
		Deductions.Add(D);
	}

	// 推理4: 目撃者の証言
	{
		FDeduction D;
		D.DeductionId = FName("Deduction_Witness");
		D.Title = FText::FromString(TEXT("目撃証言との一致"));
		D.Description = FText::FromString(TEXT(
			"メアリーの証言と足跡を結びつけると...\n"
			"メアリーが聞いたのはエドワードの声だった。\n"
			"足跡も彼の体格と一致する。エドワードは事件当夜、現場にいた。"
		));
		D.EvidenceA = FName("Evidence_MaryTestimony");
		D.EvidenceB = FName("Evidence_Footprints");
		D.UnlocksFlags.Add(FName("Flag_EdwardAtScene"));
		Deductions.Add(D);
	}

	return Deductions;
}
