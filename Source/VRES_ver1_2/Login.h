// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Json/Public/Json.h"
#include "Login.generated.h"

UCLASS()
class VRES_VER1_2_API ALogin : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditInstanceOnly, Category = "Visible")
		FString OutputResponse;
	
public:	
	// Sets default values for this actor's properties
	ALogin();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	FHttpModule* Http;

	//HttpMethod
	UFUNCTION(BlueprintCallable, Category = "Http")
		void SendMethod(FString userid, FString password);
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable, Category = "Output")
		FString OutputMethod();
};
