/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Mana Source asset
 * File     : ManaSource.h
 *
 ****************************************************************************/
#pragma once

#include "ManaSource.generated.h"

/** ManaSource abstract class represents a Sofdec2 Movie Source */
UCLASS(Abstract, BlueprintType)
class CRIWARERUNTIME_API UManaSource : public UObject
{
	GENERATED_UCLASS_BODY()

protected:
	virtual bool CanBeInCluster() const override { return false; }
};