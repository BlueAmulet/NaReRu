/****************************************************************************
*
* CRI Middleware SDK
*
* Copyright (c) 2017 CRI Middleware Co., Ltd.
*
* Library  : CRIWARE plugin for Unreal Engine 4
* Module   : Asset Type Action of Mana Playlist
* File     : AssetTypeActions_ManaPlaylist.h
*
****************************************************************************/

#pragma once

#include "AssetTypeActions_Base.h"
#include "ManaPlaylist.h"

class FAssetTypeActions_ManaPlaylist : public FAssetTypeActions_Base
{
public:
	FAssetTypeActions_ManaPlaylist(EAssetTypeCategories::Type InAssetCategory);
	//~ Begin IAssetTypeActions interface
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_ManaPlaylist", "Sofdec2 Movie Playlist"); }
	virtual UClass* GetSupportedClass() const override { return UManaPlaylist::StaticClass(); }
	virtual FColor GetTypeColor() const override { return FColor::Yellow; }
	virtual bool HasActions(const TArray<UObject*>& InObjects) const override { return true; }
	virtual void GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) override;
	//virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor) override;
	virtual bool CanFilter() override { return false; }
	virtual uint32 GetCategories() override { return MyAssetCategory; }
	virtual bool IsImportedAsset() const override { return false; }
	//virtual void GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets, TArray<FString>& OutSourceFilePaths) const override;
	//~ End IAssetTypeActions interface

private:
	EAssetTypeCategories::Type MyAssetCategory;

private:
	void ExecuteCreateTexture(TArray<TWeakObjectPtr<UManaPlaylist>> Playlists);
private:

};
