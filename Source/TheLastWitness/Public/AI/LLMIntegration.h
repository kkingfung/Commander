// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/IHttpRequest.h"
#include "LLMIntegration.generated.h"

/// <summary>
/// LLMプロバイダーの種類
/// </summary>
UENUM(BlueprintType)
enum class ELLMProvider : uint8
{
	/// <summary>ローカルOllama</summary>
	LocalOllama,
	/// <summary>OpenAI API</summary>
	OpenAI,
	/// <summary>Anthropic Claude API</summary>
	Anthropic,
	/// <summary>カスタムエンドポイント</summary>
	Custom
};

/// <summary>
/// LLM設定
/// </summary>
USTRUCT(BlueprintType)
struct FLLMConfig
{
	GENERATED_BODY()

	/// <summary>使用するプロバイダー</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELLMProvider Provider = ELLMProvider::LocalOllama;

	/// <summary>APIエンドポイント</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Endpoint = TEXT("http://localhost:11434/api/generate");

	/// <summary>APIキー（必要な場合）</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ApiKey;

	/// <summary>モデル名</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ModelName = TEXT("llama3");

	/// <summary>温度パラメータ</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Temperature = 0.7f;

	/// <summary>最大トークン数</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxTokens = 256;

	/// <summary>タイムアウト（秒）</summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeoutSeconds = 30.0f;
};

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnLLMResponseReceived, bool, bSuccess, const FString&, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLLMRequestCompleted, bool, bSuccess, const FString&, Response);

/// <summary>
/// LLM統合クラス
/// </summary>
/// <remarks>
/// Ollama、OpenAI、Anthropic等のLLMに接続してテキスト生成を行います。
/// ABELSystemのフレーバーテキスト生成に使用されます。
/// </remarks>
UCLASS(BlueprintType)
class THELASTWITNESS_API ULLMIntegration : public UObject
{
	GENERATED_BODY()

public:
	ULLMIntegration();

	/// <summary>
	/// 設定を適用します
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "LLM")
	void SetConfig(const FLLMConfig& InConfig);

	/// <summary>
	/// 現在の設定を取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "LLM")
	FLLMConfig GetConfig() const { return Config; }

	/// <summary>
	/// テキスト生成をリクエストします
	/// </summary>
	/// <param name="Prompt">プロンプト</param>
	/// <param name="SystemPrompt">システムプロンプト（キャラクター設定等）</param>
	/// <param name="OnComplete">完了時のコールバック</param>
	UFUNCTION(BlueprintCallable, Category = "LLM")
	void RequestGeneration(const FString& Prompt, const FString& SystemPrompt, const FOnLLMResponseReceived& OnComplete);

	/// <summary>
	/// ABEL用のシステムプロンプトを取得します
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "LLM")
	static FString GetABELSystemPrompt();

	/// <summary>
	/// 接続をテストします
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "LLM")
	void TestConnection(const FOnLLMResponseReceived& OnComplete);

	/// <summary>
	/// リクエスト中かどうか
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "LLM")
	bool IsRequestInProgress() const { return bIsRequestInProgress; }

	/// <summary>リクエスト完了時に発火</summary>
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLLMRequestCompleted OnRequestCompleted;

protected:
	/// <summary>
	/// Ollama用のリクエストを構築・送信します
	/// </summary>
	void SendOllamaRequest(const FString& Prompt, const FString& SystemPrompt);

	/// <summary>
	/// OpenAI用のリクエストを構築・送信します
	/// </summary>
	void SendOpenAIRequest(const FString& Prompt, const FString& SystemPrompt);

	/// <summary>
	/// Anthropic用のリクエストを構築・送信します
	/// </summary>
	void SendAnthropicRequest(const FString& Prompt, const FString& SystemPrompt);

	/// <summary>
	/// HTTPレスポンスを処理します
	/// </summary>
	void HandleHttpResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	/// <summary>
	/// Ollamaレスポンスをパースします
	/// </summary>
	FString ParseOllamaResponse(const FString& ResponseBody);

	/// <summary>
	/// OpenAIレスポンスをパースします
	/// </summary>
	FString ParseOpenAIResponse(const FString& ResponseBody);

	/// <summary>
	/// Anthropicレスポンスをパースします
	/// </summary>
	FString ParseAnthropicResponse(const FString& ResponseBody);

	/// <summary>現在の設定</summary>
	UPROPERTY()
	FLLMConfig Config;

	/// <summary>リクエスト中フラグ</summary>
	bool bIsRequestInProgress = false;

	/// <summary>保留中のコールバック</summary>
	FOnLLMResponseReceived PendingCallback;
};
