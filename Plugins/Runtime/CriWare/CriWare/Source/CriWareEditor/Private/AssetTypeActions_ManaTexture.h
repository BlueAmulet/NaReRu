/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2015 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Asset Type Action of Mana YUV Texture
 * File     : AssetTypeActions_ManaTexture.h
 *
 ****************************************************************************/

/* 多重定義防止 */
#pragma once

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* Unreal Engine 4関連ヘッダ */
#include "AssetTypeActions_Base.h"
#include "ManaTexture.h"
#include "ManaMaterialFactory.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/

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

class FAssetTypeActions_ManaTexture : public FAssetTypeActions_Base
{
public:
	FAssetTypeActions_ManaTexture(EAssetTypeCategories::Type InAssetCategory);
	// IAssetTypeActions Implementation
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_ManaTexture", "Sofdec2 Movie Texture"); }
	virtual UClass* GetSupportedClass() const override { return UManaTexture::StaticClass(); }
	virtual FColor GetTypeColor() const override { return FColor(192, 64, 64); }
	virtual bool HasActions(const TArray<UObject*>& InObjects ) const override { return true; }
	virtual void GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) override;
	virtual void OpenAssetEditor( const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor ) override;
	virtual bool CanFilter() override { return true; }
	virtual uint32 GetCategories() override { return EAssetTypeCategories::MaterialsAndTextures | MyAssetCategory; }
	virtual bool IsImportedAsset() const override { return false; }
	// End of IAssetTypeActions interface

private:
	EAssetTypeCategories::Type MyAssetCategory;

private:
	void ExecuteCreateMaterial(TArray<TWeakObjectPtr<UManaTexture>> Textures);
	bool ConfigureProperties();

private:
	FManaMaterialFactoryOptions Options;
};

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/

/* --- end of file --- */
