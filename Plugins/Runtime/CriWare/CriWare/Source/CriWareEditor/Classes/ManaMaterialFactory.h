/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2016 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Material Factory for ManaTexture
 * File     : ManaMaterialFactory.h
 *
 ****************************************************************************/
#pragma once
#include "Materials/Material.h"
#include "ActorFactoryAtomSound.h"
#include "Factories/Factory.h"

#include "ManaMaterialFactory.generated.h"


 /**
 * Options for UManaMaterialFactory.
 */
enum EManaMaterialFactoryCreateChoices {
	// create a Material with YUV components support based on current Mana Texture settings.
	CreateComponentMaterial,
	// create a Material with RenderToTexture supoort based on current Mana Texture settings.
	CreateRenderToTextureMaterial,
};

enum EManaMaterialFactoryOptionChoices {
	OptionAuto,
	OptionTrue,
	OptionFalse
};

struct FManaMaterialFactoryOptions
{
	EManaMaterialFactoryCreateChoices CreateChoice;
	EManaMaterialFactoryOptionChoices AlphaChannelSupport;
	EManaMaterialFactoryOptionChoices AlphaPlusSupport;
	EManaMaterialFactoryOptionChoices HWDecodedMovieSupport; // Component material only
};


class UManaTexture;
class UMaterialExpression;

UCLASS(hidecategories = Object, collapsecategories, MinimalAPI)
class UManaMaterialFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

	/** An initial texture to place in the newly created material */
	UPROPERTY()
	class UManaTexture* InitialTexture;

	/** Creation Options */
	FManaMaterialFactoryOptions Options;

	//~ Begin UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	//~ Begin UFactory Interface	

private:
	/** Create a material that use single render target texture */
	void CreateRenderTargetTextureMaterial(UMaterial* NewMaterial);
	
	/** Create a material that use multiple component textures */
	void CreateComponentTexturesMaterial(UMaterial* NewMaterial);

	/** Create a TexCoord Expression and set UV tiling based on texture display size */
	UMaterialExpression* CreateTextureUVTiling(UMaterial* NewMaterial, int32 EditorX, int32 EditorY);
};