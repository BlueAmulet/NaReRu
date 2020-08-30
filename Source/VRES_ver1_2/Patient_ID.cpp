// Fill out your copyright notice in the Description page of Project Settings.


#include "Patient_ID.h"

// Sets default values
APatient_ID::APatient_ID()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Http = &FHttpModule::Get();
}

// Called when the game starts or when spawned
void APatient_ID::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APatient_ID::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APatient_ID::GenerateID(FString username)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("UserName", username);
	// OutputString
	FString OutputString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

	//Http
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &APatient_ID::OnResponseReceived);
	Request->SetURL("https://virtual-reality-exposure-system.com/test/test_patient2.php");//後で変更
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetContentAsString(OutputString);
	Request->ProcessRequest();
}

void APatient_ID::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	// Jsonオブジェクトをデシリアライズ
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		// Jsonから変数名myTextの値を取得
		FString str = JsonObject->GetStringField("PatientID");
		// UE4クライアントにデバッグメッセージを表示
		GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, str);
		Response_PatientID = str;
	}
}

FString APatient_ID::ResponsePatientID() {
	return Response_PatientID;
}