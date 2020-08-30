// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Json/Public/Json.h"
#include "Patient_ID.generated.h"

UCLASS()
class VRES_VER1_2_API APatient_ID : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditInstanceOnly, Category = "Visible")
		FString Response_PatientID;
	
public:	
	// Sets default values for this actor's properties
	APatient_ID();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FHttpModule* Http;

	UFUNCTION(BlueprintCallable, Category = "Patient_ID")
		void GenerateID(FString username);
		void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

		UFUNCTION(BlueprintCallable, Category = "Patient_ID")
			FString ResponsePatientID();
};
