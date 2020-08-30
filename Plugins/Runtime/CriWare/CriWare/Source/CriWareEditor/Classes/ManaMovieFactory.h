/****************************************************************************
*
* CRI Middleware SDK
*
* Copyright (c) 2017 CRI Middleware Co., Ltd.
*
* Library  : CRIWARE plugin for Unreal Engine 4
* Module   : Asset Factory of ManaMovie
* File     : ManaMovieFactory.h
*
****************************************************************************/

#pragma once

#include "Factories/Factory.h"
#include "ManaMovieFactory.generated.h"


UCLASS(HideCategories = Object)
class UManaMovieFactory : public UFactory
{
	GENERATED_BODY()

public:
	UManaMovieFactory(const FObjectInitializer& ObjectInitializer);

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
