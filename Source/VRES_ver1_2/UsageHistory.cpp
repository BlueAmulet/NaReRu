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
void AUsageHistory::Test_History(FString patientid, FString username) {
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("PatientID", patientid);
	JsonObject->SetStringField("UserName", username);

	// OutputString
	FString OutputString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

	//Http
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AUsageHistory::OnResponseReceived);
	Request->SetURL("https://virtual-reality-exposure-system.com/test/test5.php");//å„Ç≈ïœçX
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetContentAsString(OutputString);
	Request->ProcessRequest();
}

void AUsageHistory::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
		//ç≈èIìIÇ»ãZèpìIïsà¿ì_
		
		for (int32 i = 0; i < 32;i++) {
			FString str_i = FString::FromInt(i);
			TSharedPtr<FJsonObject> array = JsonObject->GetObjectField(str_i);
			FString res_content = *array->GetStringField(TEXT("PlayContent"));
			FString res_time = *array->GetStringField(TEXT("PlayTime"));
			FString res_suds = *array->GetStringField(TEXT("PlaySUDs"));
			FString res_date = *array->GetStringField(TEXT("Date"));
			//UPROPERTYÇ÷ÉçÉOÇäiî[
			prop_content = res_content;
			prop_time = res_time;
			prop_suds = res_suds;
			prop_date = res_date;

			if (prop_content == "" || prop_time == "" || prop_suds == "" || prop_date == "") {
				break;
			}

			array_str.Add(prop_content);
			array_str.Add(prop_time);
			array_str.Add(prop_suds);
			array_str.Add(prop_date);
			GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, res_content);
		}

		
	}

}

TArray<FString> AUsageHistory::ArrayResponseLog() {
	TArray<FString> array = { "a","b","c","d","e" };
	return array_str;
}