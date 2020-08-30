/****************************************************************************
*
* CRI Middleware SDK
*
* Copyright (c) 2017 CRI Middleware Co., Ltd.
*
* Library  : CRIWARE plugin for Unreal Engine 4
* Module   : Asset Type Action of Mana Movie Source
* File     : AssetTypeActions_ManaMovie.h
*
****************************************************************************/

#pragma once

#include "AssetTypeActions_Base.h"
#include "ManaMovie.h"

class FAssetTypeActions_ManaMovie : public FAssetTypeActions_Base
{
public:
	FAssetTypeActions_ManaMovie(EAssetTypeCategories::Type InAssetCategory);
	//~ Begin IAssetTypeActions interface
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_ManaMovie", "Sofdec2 Movie Source"); }
	virtual UClass* GetSupportedClass() const override { return UManaMovie::StaticClass(); }
	virtual FColor GetTypeColor() const override { return FColor::White; }
	virtual bool HasActions(const TArray<UObject*>& InObjects) const override { return true; }
	virtual void GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) override;
	//virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor) override;
	virtual bool CanFilter() override { return true; }
	virtual uint32 GetCategories() override { return MyAssetCategory; }
	virtual bool IsImportedAsset() const override { return true; }
	virtual void GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets, TArray<FString>& OutSourceFilePaths) const override;
	//~ End IAssetTypeActions interface

private:
	EAssetTypeCategories::Type MyAssetCategory;

private:
	void ExecuteCreateTexture(TArray<TWeakObjectPtr<UManaMovie>> ManaMovies);
	void ExecuteCreatePlaylist(TArray<TWeakObjectPtr<UManaMovie>> ManaMovies);
private:

};
