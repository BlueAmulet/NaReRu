// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Json/Public/Json.h"
#include "Result_Log.generated.h"

UCLASS()
class VRES_VER1_2_API AResult_Log : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditInstanceOnly, Category = "Visible")
		FString ReturnError;
	
public:	
	// Sets default values for this actor's properties
	AResult_Log();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	FHttpModule* Http;
	//HttpMethod
	UFUNCTION(BlueprintCallable, Category = "Result")
	void SendResult_Log(FString username,FString patient_id, FString contentsname,  FString sudslog, FString time);
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable, Category = "Result")
		FString ResultResponse();

};
