// Fill out your copyright notice in the Description page of Project Settings.


#include "Result_Log.h"
#include "Engine.h"

// Sets default values
AResult_Log::AResult_Log()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Http = &FHttpModule::Get();

}

// Called when the game starts or when spawned
void AResult_Log::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AResult_Log::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AResult_Log::SendResult_Log(FString username,FString patient_id, FString contentsname, FString sudslog, FString time)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("UserName", username);
	JsonObject->SetStringField("PatientID", patient_id);
	JsonObject->SetStringField("PlayContent", contentsname);
	JsonObject->SetStringField("PlaySUDs", sudslog);
	JsonObject->SetStringField("PlayTimes", time);

	// OutputString
	FString OutputString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

	//Http
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AResult_Log::OnResponseReceived);
	Request->SetURL("https://virtual-reality-exposure-system.com/Release/Result/Result.php");//Œã‚Å•ÏX
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetContentAsString(OutputString);
	Request->ProcessRequest();
}

void AResult_Log::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
		FString return_error = JsonObject->GetStringField("CompanyID");
		ReturnError = return_error;
		GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, return_error);
		

	}
}

FString AResult_Log::ResultResponse() {
	return ReturnError;
}