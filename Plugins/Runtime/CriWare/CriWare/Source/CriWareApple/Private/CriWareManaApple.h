/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE Apple plugin for Unreal Engine 4
 * Module   : Sofdec2 Mana for Apple
 * File     : CriWareManaApple.h
 *
 ****************************************************************************/

#pragma once

#include "ManaComponent.h"
#include "ManaTexture.h"
#include "CriWarePlatform.h"
#include "CriWareManaApple.generated.h"

class UManaComponentApple;

class FCriWareManaApple : ICriWarePlatformMana {

public:
	FCriWareManaApple();
	virtual ~FCriWareManaApple();

	virtual void InitializeManaLibrary();
	virtual void FinalizeManaLibrary();
	virtual void InitializeManaComponent(UManaComponent& ManaComponent);
	virtual void UninitializeManaComponent(UManaComponent& ManaComponent);
	virtual FRHITexture2D* UpdateManaTexture(const UManaTexture& Owner, const CriManaTextureBuffer& ManaTextureBuffer,
		EManaComponentTextureType ComponentType, const FIntPoint& Dimension, FRHITexture2D *Texture);
	
private:
	TMap<UManaComponent*, UManaComponentApple*> ManaComponents;

	FORCEINLINE UManaComponentApple* GetManaComponentApple(const UManaComponent* ManaComponent) const
	{
		auto Result = ManaComponents.Find(ManaComponent);
		return Result ? *Result : nullptr;
	}

	FORCEINLINE UManaComponentApple* GetManaComponentApple(const UManaTexture* ManaTexture) const
	{
		for (auto ManaComponentPair : ManaComponents)
		{
			if (ManaComponentPair.Key->GetTexture() == ManaTexture)
			{
				return ManaComponentPair.Value;
			}
		}

		return nullptr;
	}
};

// Internal listener to listen status changes in ManaComponent and old navitve textures for each component.
UCLASS()
class UManaComponentApple : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void OnManaComponentStatusChanged(EManaComponentStatus Status, UManaComponent* ManaComponent);

public:
    TArray<FTexture2DRHIRef, TFixedAllocator<3>> VideoTextures;
    
	FCriWareManaApple * ManaApple;
	UManaComponent*	ManaComponent;
	EManaMovieType CurrentMovieType;

public:
	void InitializeResources();
};
