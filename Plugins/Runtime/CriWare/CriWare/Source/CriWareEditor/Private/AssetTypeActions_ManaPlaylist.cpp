/****************************************************************************
*
* CRI Middleware SDK
*
* Copyright (c) 2017 CRI Middleware Co., Ltd.
*
* Library  : CRIWARE plugin for Unreal Engine 4
* Module   : Asset Type Action of Mana Playlist
* File     : AssetTypeActions_ManaPlaylist.cpp
*
****************************************************************************/

/* モジュールヘッダ */
#include "AssetTypeActions_ManaPlaylist.h"

/* ProjectPluginとして利用する場合MonolithicHeaderBoilerplate.hをインクルード */
#if !defined(UE_IS_ENGINE_MODULE)
#include "Misc/MonolithicHeaderBoilerplate.h"
MONOLITHIC_HEADER_BOILERPLATE()
#endif

/* CRIWAREプラグインヘッダ */
#include "CriWareEditorPrivatePCH.h"
#include "CriWareInitializer.h"

#include "ManaTexture.h"
#include "ManaTextureFactory.h"

/* Unreal Engine 4関連ヘッダ */
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "EditorStyleSet.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "EditorFramework/AssetImportData.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

#include "CriWareEditorStyle.h"

FAssetTypeActions_ManaPlaylist::FAssetTypeActions_ManaPlaylist(EAssetTypeCategories::Type InAssetCategory)
	: MyAssetCategory(InAssetCategory)
{
}

void FAssetTypeActions_ManaPlaylist::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
	/* ManaTextureアセットの右クリックメニューに「マテリアルを作成」項目を追加 */
	auto Playlists = GetTypedWeakObjectPtrs<UManaPlaylist>(InObjects);
	MenuBuilder.AddMenuEntry(
		LOCTEXT("ManaPlaylist_CreateManaTexture", "Create Mana Texture"),
		LOCTEXT("Texture_CreateManaTextureTooltip", "Creates a new texture using this Sofdec2 Movie Playlist as source."),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "ClassIcon.ManaTexture"),
		FUIAction(
			FExecuteAction::CreateSP(this, &FAssetTypeActions_ManaPlaylist::ExecuteCreateTexture, Playlists),
			FCanExecuteAction()
		)
	);
}

void FAssetTypeActions_ManaPlaylist::ExecuteCreateTexture(TArray<TWeakObjectPtr<UManaPlaylist>> Playlists)
{
	const FString Suffix = TEXT("_Texture");

	IContentBrowserSingleton& ContentBrowserSingleton = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser").Get();
	TArray<UObject*> ObjectsToSync;

	for (auto PlaylistIt = Playlists.CreateConstIterator(); PlaylistIt; ++PlaylistIt) {
		auto Playlist = (*PlaylistIt).Get();
		if (Playlist) {
			FString Name;
			FString PackagePath;
			CreateUniqueAssetName(Playlist->GetOutermost()->GetName(), Suffix, PackagePath, Name);

			UManaTextureFactory* Factory = NewObject<UManaTextureFactory>();
			Factory->InitialMovieSource = Playlist;

			FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
			UObject* NewAsset = AssetToolsModule.Get().CreateAsset(Name, FPackageName::GetLongPackagePath(PackagePath), UManaTexture::StaticClass(), Factory);

			if (NewAsset) {
				ObjectsToSync.Add(NewAsset);
			}
		}
	}

	if (ObjectsToSync.Num() > 0) {
		ContentBrowserSingleton.SyncBrowserToAssets(ObjectsToSync);
	}
}

#undef LOCTEXT_NAMESPACE
