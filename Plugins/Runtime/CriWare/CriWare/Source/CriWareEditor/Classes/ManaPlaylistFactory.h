/****************************************************************************
*
* CRI Middleware SDK
*
* Copyright (c) 2017 CRI Middleware Co., Ltd.
*
* Library  : CRIWARE plugin for Unreal Engine 4
* Module   : Asset Factory of ManaPlaylist
* File     : ManaPlaylistFactory.h
*
****************************************************************************/

#pragma once

#include "Factories/Factory.h"
#include "ManaPlaylistFactory.generated.h"

class UManaMovie;

UCLASS(HideCategories = Object)
class UManaPlaylistFactory : public UFactory
{
	GENERATED_BODY()

public:
	UManaPlaylistFactory(const FObjectInitializer& ObjectInitializer);

	/** Some Mana Movie to set in play list. */
	UPROPERTY()
	TArray<TWeakObjectPtr<UManaMovie>> InitialMovies;

	//~ Begin UFactory Interface
	virtual UObject* FactoryCreateNew(
		UClass* Class,
		UObject* InParent,
		FName Name,
		EObjectFlags Flags,
		UObject* Context,
		FFeedbackContext* Warn) override;
	//~ End UFactory Interface
};
