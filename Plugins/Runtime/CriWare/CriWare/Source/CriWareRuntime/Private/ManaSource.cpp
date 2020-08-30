/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Mana source asset
 * File     : ManaSource.cpp
 *
 ****************************************************************************/
#include "ManaSource.h"

#include "CriWareRuntimePrivatePCH.h"

#define LOCTEXT_NAMESPACE "ManaSource"


//----------------------------------------
// UManaSource class
//----------------------------------------
UManaSource::UManaSource(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	// Localization of unreal properties metadata with LOCTEXT markups and reflection
	CRI_LOCCLASS(GetClass());
#endif
}

#undef LOCTEXT_NAMESPACE
