// Fill out your copyright notice in the Description page of Project Settings.

#include "Login.h"

// Sets default values
ALogin::ALogin()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Http = &FHttpModule::Get();
}

// Called when the game starts or when spawned
void ALogin::BeginPlay() { Super::BeginPlay(); }

// Called every frame
void ALogin::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void ALogin::SendMethod(FString userid, FString password)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("UserId", userid);
	JsonObject->SetStringField("Password", password);

	// OutputString
	FString OutputString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

	//Http
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ALogin::OnResponseReceived);
	Request->SetURL("https://virtual-reality-exposure-system.com/Release/login/login.php");//Œã‚Å•ÏX
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetContentAsString(OutputString);
	Request->ProcessRequest();
}

void ALogin::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
		FString return_message_true = JsonObject->GetStringField("True");
		FString return_message_false = JsonObject->GetStringField("False");
		FString return_message_empty = JsonObject->GetStringField("Empty");
		FString return_message_error = JsonObject->GetStringField("Error");

		int8 int_true = FCString::Atoi(*return_message_true);
		int8 int_false = FCString::Atoi(*return_message_false);
		int8 int_empty = FCString::Atoi(*return_message_empty);
		int8 int_error = FCString::Atoi(*return_message_error);

		if (int_true) {
			OutputResponse = "true";
		}
		if (int_false) {
			OutputResponse = "false";
		}
		if (int_empty) {
			OutputResponse = "empty";
		}
		if (int_error) {
			OutputResponse = "error";
		}
	}
}

FString ALogin::OutputMethod() {
	return OutputResponse;
}