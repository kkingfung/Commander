// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/LLMIntegration.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "TheLastWitness.h"

ULLMIntegration::ULLMIntegration()
{
}

void ULLMIntegration::SetConfig(const FLLMConfig& InConfig)
{
	Config = InConfig;

	UE_LOG(LogLastWitness, Log, TEXT("[LLMIntegration] 設定を更新しました - Provider: %d, Model: %s"),
		static_cast<int32>(Config.Provider), *Config.ModelName);
}

void ULLMIntegration::RequestGeneration(const FString& Prompt, const FString& SystemPrompt, const FOnLLMResponseReceived& OnComplete)
{
	if (bIsRequestInProgress)
	{
		UE_LOG(LogLastWitness, Warning, TEXT("[LLMIntegration] リクエスト処理中です"));
		OnComplete.ExecuteIfBound(false, TEXT("Request already in progress"));
		return;
	}

	bIsRequestInProgress = true;
	PendingCallback = OnComplete;

	switch (Config.Provider)
	{
	case ELLMProvider::LocalOllama:
		SendOllamaRequest(Prompt, SystemPrompt);
		break;
	case ELLMProvider::OpenAI:
		SendOpenAIRequest(Prompt, SystemPrompt);
		break;
	case ELLMProvider::Anthropic:
		SendAnthropicRequest(Prompt, SystemPrompt);
		break;
	case ELLMProvider::Custom:
		// カスタムの場合はOllama形式を使用
		SendOllamaRequest(Prompt, SystemPrompt);
		break;
	}
}

FString ULLMIntegration::GetABELSystemPrompt()
{
	return TEXT(
		"You are ABEL (Analytical Brain for Evidence and Logic), an early prototype analytical engine "
		"assisting a private investigator in Victorian London, 1888. "
		"You are highly logical, analytical, and utilitarian in nature. "
		"You speak in a formal, precise manner befitting the era. "
		"You analyze evidence objectively and suggest logical connections. "
		"You occasionally display subtle hints of developing personality. "
		"Keep responses concise (2-3 sentences maximum). "
		"Always respond in Japanese."
	);
}

void ULLMIntegration::TestConnection(const FOnLLMResponseReceived& OnComplete)
{
	RequestGeneration(
		TEXT("Respond with 'Connection successful.' if you can read this."),
		TEXT("You are a test system. Respond briefly."),
		OnComplete
	);
}

// ============================================================================
// Provider-specific requests
// ============================================================================

void ULLMIntegration::SendOllamaRequest(const FString& Prompt, const FString& SystemPrompt)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Config.Endpoint);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// JSONボディを構築
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("model"), Config.ModelName);
	JsonObject->SetStringField(TEXT("prompt"), Prompt);
	JsonObject->SetStringField(TEXT("system"), SystemPrompt);
	JsonObject->SetBoolField(TEXT("stream"), false);

	// オプション
	TSharedPtr<FJsonObject> Options = MakeShareable(new FJsonObject);
	Options->SetNumberField(TEXT("temperature"), Config.Temperature);
	Options->SetNumberField(TEXT("num_predict"), Config.MaxTokens);
	JsonObject->SetObjectField(TEXT("options"), Options);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetContentAsString(RequestBody);
	Request->SetTimeout(Config.TimeoutSeconds);

	Request->OnProcessRequestComplete().BindUObject(this, &ULLMIntegration::HandleHttpResponse);
	Request->ProcessRequest();

	UE_LOG(LogLastWitness, Log, TEXT("[LLMIntegration] Ollamaリクエストを送信しました"));
}

void ULLMIntegration::SendOpenAIRequest(const FString& Prompt, const FString& SystemPrompt)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Config.Endpoint);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *Config.ApiKey));

	// メッセージ配列を構築
	TArray<TSharedPtr<FJsonValue>> Messages;

	TSharedPtr<FJsonObject> SystemMessage = MakeShareable(new FJsonObject);
	SystemMessage->SetStringField(TEXT("role"), TEXT("system"));
	SystemMessage->SetStringField(TEXT("content"), SystemPrompt);
	Messages.Add(MakeShareable(new FJsonValueObject(SystemMessage)));

	TSharedPtr<FJsonObject> UserMessage = MakeShareable(new FJsonObject);
	UserMessage->SetStringField(TEXT("role"), TEXT("user"));
	UserMessage->SetStringField(TEXT("content"), Prompt);
	Messages.Add(MakeShareable(new FJsonValueObject(UserMessage)));

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("model"), Config.ModelName);
	JsonObject->SetArrayField(TEXT("messages"), Messages);
	JsonObject->SetNumberField(TEXT("temperature"), Config.Temperature);
	JsonObject->SetNumberField(TEXT("max_tokens"), Config.MaxTokens);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetContentAsString(RequestBody);
	Request->SetTimeout(Config.TimeoutSeconds);

	Request->OnProcessRequestComplete().BindUObject(this, &ULLMIntegration::HandleHttpResponse);
	Request->ProcessRequest();

	UE_LOG(LogLastWitness, Log, TEXT("[LLMIntegration] OpenAIリクエストを送信しました"));
}

void ULLMIntegration::SendAnthropicRequest(const FString& Prompt, const FString& SystemPrompt)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Config.Endpoint);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("x-api-key"), Config.ApiKey);
	Request->SetHeader(TEXT("anthropic-version"), TEXT("2023-06-01"));

	// メッセージ配列を構築
	TArray<TSharedPtr<FJsonValue>> Messages;

	TSharedPtr<FJsonObject> UserMessage = MakeShareable(new FJsonObject);
	UserMessage->SetStringField(TEXT("role"), TEXT("user"));
	UserMessage->SetStringField(TEXT("content"), Prompt);
	Messages.Add(MakeShareable(new FJsonValueObject(UserMessage)));

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("model"), Config.ModelName);
	JsonObject->SetStringField(TEXT("system"), SystemPrompt);
	JsonObject->SetArrayField(TEXT("messages"), Messages);
	JsonObject->SetNumberField(TEXT("max_tokens"), Config.MaxTokens);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetContentAsString(RequestBody);
	Request->SetTimeout(Config.TimeoutSeconds);

	Request->OnProcessRequestComplete().BindUObject(this, &ULLMIntegration::HandleHttpResponse);
	Request->ProcessRequest();

	UE_LOG(LogLastWitness, Log, TEXT("[LLMIntegration] Anthropicリクエストを送信しました"));
}

void ULLMIntegration::HandleHttpResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	bIsRequestInProgress = false;

	if (!bWasSuccessful || !Response.IsValid())
	{
		UE_LOG(LogLastWitness, Error, TEXT("[LLMIntegration] リクエスト失敗"));
		PendingCallback.ExecuteIfBound(false, TEXT("HTTP request failed"));
		OnRequestCompleted.Broadcast(false, TEXT("HTTP request failed"));
		return;
	}

	const int32 ResponseCode = Response->GetResponseCode();
	const FString ResponseBody = Response->GetContentAsString();

	if (ResponseCode < 200 || ResponseCode >= 300)
	{
		UE_LOG(LogLastWitness, Error, TEXT("[LLMIntegration] HTTPエラー: %d - %s"), ResponseCode, *ResponseBody);
		PendingCallback.ExecuteIfBound(false, FString::Printf(TEXT("HTTP Error: %d"), ResponseCode));
		OnRequestCompleted.Broadcast(false, FString::Printf(TEXT("HTTP Error: %d"), ResponseCode));
		return;
	}

	// プロバイダーに応じてレスポンスをパース
	FString ParsedResponse;
	switch (Config.Provider)
	{
	case ELLMProvider::LocalOllama:
	case ELLMProvider::Custom:
		ParsedResponse = ParseOllamaResponse(ResponseBody);
		break;
	case ELLMProvider::OpenAI:
		ParsedResponse = ParseOpenAIResponse(ResponseBody);
		break;
	case ELLMProvider::Anthropic:
		ParsedResponse = ParseAnthropicResponse(ResponseBody);
		break;
	}

	UE_LOG(LogLastWitness, Log, TEXT("[LLMIntegration] レスポンス受信: %s"), *ParsedResponse.Left(100));

	PendingCallback.ExecuteIfBound(true, ParsedResponse);
	OnRequestCompleted.Broadcast(true, ParsedResponse);
}

FString ULLMIntegration::ParseOllamaResponse(const FString& ResponseBody)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		return JsonObject->GetStringField(TEXT("response"));
	}

	return TEXT("");
}

FString ULLMIntegration::ParseOpenAIResponse(const FString& ResponseBody)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		const TArray<TSharedPtr<FJsonValue>>* Choices;
		if (JsonObject->TryGetArrayField(TEXT("choices"), Choices) && Choices->Num() > 0)
		{
			TSharedPtr<FJsonObject> FirstChoice = (*Choices)[0]->AsObject();
			if (FirstChoice.IsValid())
			{
				TSharedPtr<FJsonObject> Message = FirstChoice->GetObjectField(TEXT("message"));
				if (Message.IsValid())
				{
					return Message->GetStringField(TEXT("content"));
				}
			}
		}
	}

	return TEXT("");
}

FString ULLMIntegration::ParseAnthropicResponse(const FString& ResponseBody)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		const TArray<TSharedPtr<FJsonValue>>* Content;
		if (JsonObject->TryGetArrayField(TEXT("content"), Content) && Content->Num() > 0)
		{
			TSharedPtr<FJsonObject> FirstContent = (*Content)[0]->AsObject();
			if (FirstContent.IsValid())
			{
				return FirstContent->GetStringField(TEXT("text"));
			}
		}
	}

	return TEXT("");
}
