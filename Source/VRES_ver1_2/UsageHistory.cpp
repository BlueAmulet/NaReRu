// Fill out your copyright notice in the Description page of Project Settings.


#include "UsageHistory.h"

// Sets default values
AUsageHistory::AUsageHistory()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Http = &FHttpModule::Get();
}

// Called when the game starts or when spawned
void AUsageHistory::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUsageHistory::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AUsageHistory::Test_History(FString companyid, FString patientid) {
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("CompanyID", companyid);
	JsonObject->SetStringField("PatientID", patientid);

	// OutputString
	FString OutputString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

	//Http
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AUsageHistory::OnResponseReceived);
	Request->SetURL("https://virtual-reality-exposure-system.com/test/test5.php");//Œã‚Å•ÏX
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetContentAsString(OutputString);
	Request->ProcessRequest();
}

void AUsageHistory::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

}