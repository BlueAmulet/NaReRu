/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2016-2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Material Factory for ManaTexture
 * File     : ManaMaterialFactory.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* モジュールヘッダ */
#include "ManaMaterialFactory.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareEditorPrivatePCH.h"
#include "ManaTexture.h"
#include "MaterialExpressionManaColorSpaceConverter.h"

/* Unreal Engine 4関連ヘッダ */
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/

#define LOCTEXT_NAMESPACE "UManaMaterialFactory"

/***************************************************************************
 *      処理マクロ
 *      Macro Functions
 ***************************************************************************/

/***************************************************************************
 *      データ型宣言
 *      Data Type Declarations
 ***************************************************************************/

/***************************************************************************
 *      変数宣言
 *      Prototype Variables
 ***************************************************************************/

/***************************************************************************
 *      クラス宣言
 *      Prototype Classes
 ***************************************************************************/

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/

/***************************************************************************
 *      変数定義
 *      Variable Definition
 ***************************************************************************/

/***************************************************************************
 *      クラス定義
 *      Class Definition
 ***************************************************************************/

/* UManaMaterialFactory constructor
 *****************************************************************************/

UManaMaterialFactory::UManaMaterialFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UMaterial::StaticClass();
	bCreateNew = false;
	bEditAfterNew = true;
	Options.CreateChoice = EManaMaterialFactoryCreateChoices::CreateRenderToTextureMaterial;
	Options.AlphaChannelSupport = EManaMaterialFactoryOptionChoices::OptionAuto;
	Options.HWDecodedMovieSupport = EManaMaterialFactoryOptionChoices::OptionAuto;
}

/* UManaMaterialFactory implementation
*****************************************************************************/

UObject* UManaMaterialFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UMaterial* NewMaterial = NewObject<UMaterial>(InParent, Class, Name, Flags);

	if (InitialTexture != nullptr)
	{
		if (Options.CreateChoice == EManaMaterialFactoryCreateChoices::CreateComponentMaterial) {
			CreateComponentTexturesMaterial(NewMaterial);
		} else {
			CreateRenderTargetTextureMaterial(NewMaterial);
			// Ensure that ManaTexture will generate RenderTargetTexture
			if (InitialTexture->bRenderToTexture != true) {
				InitialTexture->bRenderToTexture = true;
				InitialTexture->PostEditChange();
				InitialTexture->MarkPackageDirty();
			}
		}
	}

	return NewMaterial;
}

void UManaMaterialFactory::CreateRenderTargetTextureMaterial(UMaterial* NewMaterial)
{
	auto TextureSampler = NewObject<UMaterialExpressionTextureSample>(NewMaterial);
	TextureSampler->MaterialExpressionEditorX = -250;
	TextureSampler->MaterialExpressionEditorY = 0;
	TextureSampler->Texture = InitialTexture;
	TextureSampler->SamplerType = SAMPLERTYPE_Color;
	TextureSampler->Rename(TEXT("Render Target Texture Sampler"));

	NewMaterial->Expressions.Add(TextureSampler);

	// Attach RGB output from texture sampler to material.
	TextureSampler->ConnectExpression(&NewMaterial->BaseColor, 0);

	// Attach alpha directly form alpha texture sampler to material opacity.
	if (InitialTexture->IsAlphaMovie() && Options.AlphaChannelSupport == EManaMaterialFactoryOptionChoices::OptionAuto) {
		
		FExpressionOutput& Output = TextureSampler->GetOutputs()[4]; // alpha channel
		FExpressionInput& Input = (FExpressionInput&)NewMaterial->Opacity;

		Input.Expression = TextureSampler;
		Input.Mask = Output.Mask;
		Input.MaskR = Output.MaskR;
		Input.MaskG = Output.MaskG;
		Input.MaskB = Output.MaskB;
		Input.MaskA = Output.MaskA;

		if (Options.AlphaPlusSupport == EManaMaterialFactoryOptionChoices::OptionTrue) {
			// Set premultiplied alpha blending.
			NewMaterial->BlendMode = BLEND_AlphaComposite;
		} else {
			// Set material translucent.
			NewMaterial->BlendMode = BLEND_Translucent;
		}
	} else if (Options.AlphaPlusSupport == EManaMaterialFactoryOptionChoices::OptionTrue) {
		// Set additive blending.
		NewMaterial->BlendMode = BLEND_Additive;
	}

	// Create and attach UVTiling if display size is different.
	auto TextureCoordinate = CreateTextureUVTiling(NewMaterial, -500, 0);
	if (TextureCoordinate != nullptr) {
		TextureCoordinate->ConnectExpression(&TextureSampler->Coordinates, 0);
	}

	NewMaterial->PostEditChange();
}

void UManaMaterialFactory::CreateComponentTexturesMaterial(UMaterial* NewMaterial)
{
	// Create YUV to RGB converter material expression.
	auto ManaYUVToRGB = NewObject<UMaterialExpressionManaColorSpaceConverter>(NewMaterial);
	ManaYUVToRGB->MaterialExpressionEditorX = -250;
	NewMaterial->Expressions.Add(ManaYUVToRGB);

	// Loop on component textures.
	UMaterialExpressionTextureSample* TextureSampler[4];
	int32 NumTextures = InitialTexture->GetNumComponentTextures();
	for (int32 i = 0; i != NumTextures; i++) {

		TextureSampler[i] = NewObject<UMaterialExpressionTextureSample>(NewMaterial);

		TextureSampler[i]->MaterialExpressionEditorX = -550;
		TextureSampler[i]->MaterialExpressionEditorY = 220 * i;
		TextureSampler[i]->Texture = InitialTexture->GetComponentTexture(i);
		TextureSampler[i]->SamplerType = SAMPLERTYPE_LinearColor;

		switch (i) {
		case 0:
			TextureSampler[i]->Rename(TEXT("Texture Sampler Y"));
			break;
		case 1:
			TextureSampler[i]->Rename(TEXT("Texture Sampler U"));
			break;
		case 2:
			TextureSampler[i]->Rename(TEXT("Texture Sampler V"));
			break;
		case 3:
			TextureSampler[i]->Rename(TEXT("Texture Sampler A"));
			break;
		default:
			break;
		}

		NewMaterial->Expressions.Add(TextureSampler[i]);

		// Attach YUVA texture samplers to converter inputs.
		if (i < 4) {
			TextureSampler[i]->ConnectExpression(ManaYUVToRGB->GetInput(i), 0); // rgb channels
		}
	}

	EManaMovieType VideoType = EManaMovieType::Unknown;

	if (InitialTexture->GetMovie() != nullptr) {
		VideoType = InitialTexture->GetMovie()->GetVideoTrackType(0);
	}

	// Enable different pixel format by platform for H264 and VP9 movies.
	if (NumTextures > 0 && (VideoType == EManaMovieType::H264 || VideoType == EManaMovieType::VP9 ||
			Options.HWDecodedMovieSupport == EManaMaterialFactoryOptionChoices::OptionTrue)) {
		ManaYUVToRGB->MovieType = VideoType;
	}

	// Attach RGB output from converter to material.
	ManaYUVToRGB->ConnectExpression(&NewMaterial->BaseColor, 0);


	// Attach alpha to material opacity.
	if (NumTextures > 3 && Options.AlphaChannelSupport == EManaMaterialFactoryOptionChoices::OptionAuto) {
		
		FExpressionOutput& Output = ManaYUVToRGB->GetOutputs()[4]; // alpha channel
		FExpressionInput& Input = (FExpressionInput&)NewMaterial->Opacity;

		Input.Expression = ManaYUVToRGB;
		Input.Mask = Output.Mask;
		Input.MaskR = Output.MaskR;
		Input.MaskG = Output.MaskG;
		Input.MaskB = Output.MaskB;
		Input.MaskA = Output.MaskA;

		if (Options.AlphaPlusSupport == EManaMaterialFactoryOptionChoices::OptionTrue) {
			// Set premultiplied alpha blending.
			NewMaterial->BlendMode = BLEND_AlphaComposite;
		} else {
			// Set material translucent.
			NewMaterial->BlendMode = BLEND_Translucent;
		}
	} else if (Options.AlphaPlusSupport == EManaMaterialFactoryOptionChoices::OptionTrue) {
		// Set additive blending.
		NewMaterial->BlendMode = BLEND_Additive;
	}

	// Create and attach UVTiling if display size is different.
	auto TextureCoordinate = CreateTextureUVTiling(NewMaterial, -800, 0);
	if (TextureCoordinate != nullptr) {
		for (int32 i = 0; i != NumTextures; i++) {
			TextureCoordinate->ConnectExpression(&TextureSampler[i]->Coordinates, 0);
		}
	}

	NewMaterial->PostEditChange();
}

UMaterialExpression* UManaMaterialFactory::CreateTextureUVTiling(UMaterial* NewMaterial, int32 EditorX, int32 EditorY)
{
	FIntPoint TextureDimensions, DisplayDimensions;

	if (InitialTexture->GetMovie() != nullptr) {
		TextureDimensions = InitialTexture->GetMovie()->GetVideoTrackTextureDimensions(0);
		DisplayDimensions = InitialTexture->GetMovie()->GetVideoTrackDisplayDimensions(0);
	}

	if (DisplayDimensions != TextureDimensions) {
		
		auto TextureCoordinate = NewObject<UMaterialExpressionTextureCoordinate>(NewMaterial);
		
		TextureCoordinate->MaterialExpressionEditorX = EditorX;
		TextureCoordinate->MaterialExpressionEditorY = EditorY;
		TextureCoordinate->UTiling = (DisplayDimensions.X - 0.75f) / TextureDimensions.X;
		TextureCoordinate->VTiling = (DisplayDimensions.Y - 0.75f) / TextureDimensions.Y;
		
		return TextureCoordinate;
	}

	return nullptr;
}

/***************************************************************************
 *      関数定義
 *      Function Definition
 ***************************************************************************/

#undef LOCTEXT_NAMESPACE
/* --- end of file --- */
