// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Json/Public/Json.h"
#include "Engine.h"
#include "UsageHistory.generated.h"

UCLASS()
class VRES_VER1_2_API AUsageHistory : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditInstanceOnly, Category = Visible)
		FString prop_content;
	FString prop_time;
	FString prop_suds;
	FString prop_date;
	TArray<FString> array_str;
	
public:	
	// Sets default values for this actor's properties
	AUsageHistory();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FHttpModule* Http;
	//HttpMethod
	UFUNCTION(BlueprintCallable, Category = "UsageHistory")
		void Test_History(FString patientid, FString username);
		void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable = "UsageHistory")
		TArray<FString> ArrayResponseLog();

};
