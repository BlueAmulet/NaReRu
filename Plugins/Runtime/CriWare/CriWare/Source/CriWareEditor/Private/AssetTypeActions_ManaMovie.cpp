/****************************************************************************
*
* CRI Middleware SDK
*
* Copyright (c) 2017 CRI Middleware Co., Ltd.
*
* Library  : CRIWARE plugin for Unreal Engine 4
* Module   : Asset Type Action of Mana Movie Source
* File     : AssetTypeActions_ManaMovie.cpp
*
****************************************************************************/

/* モジュールヘッダ */
#include "AssetTypeActions_ManaMovie.h"

/* ProjectPluginとして利用する場合MonolithicHeaderBoilerplate.hをインクルード */
#if !defined(UE_IS_ENGINE_MODULE)
#include "Misc/MonolithicHeaderBoilerplate.h"
MONOLITHIC_HEADER_BOILERPLATE()
#endif

/* CRIWAREプラグインヘッダ */
#include "CriWareEditorPrivatePCH.h"
#include "CriWareInitializer.h"

#include "ManaPlaylist.h"
#include "ManaPlaylistFactory.h"
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

FAssetTypeActions_ManaMovie::FAssetTypeActions_ManaMovie(EAssetTypeCategories::Type InAssetCategory)
	: MyAssetCategory(InAssetCategory)
{
}

void FAssetTypeActions_ManaMovie::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
	/* ManaTextureアセットの右クリックメニューに「マテリアルを作成」項目を追加 */
	auto ManaMovie = GetTypedWeakObjectPtrs<UManaMovie>(InObjects);
	MenuBuilder.AddMenuEntry(
		LOCTEXT("ManaMovie_CreateManaTexture", "Create Mana Texture"),
		LOCTEXT("Texture_CreateManaTextureTooltip", "Creates a new texture using this Sofdec2 Movie as source."),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "ClassIcon.ManaTexture"),
		FUIAction(
			FExecuteAction::CreateSP(this, &FAssetTypeActions_ManaMovie::ExecuteCreateTexture, ManaMovie),
			FCanExecuteAction()
		)
	);
	MenuBuilder.AddMenuEntry(
		LOCTEXT("ManaMovie_CreateManaMoviePlaylist", "Create Mana Movie Playlist"),
		LOCTEXT("Texture_CreateManaMoviePlaylistTooltip", "Creates a new playlist using this Sofdec2 Movie."),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "ClassIcon.ManaTexture"),
		FUIAction(
			FExecuteAction::CreateSP(this, &FAssetTypeActions_ManaMovie::ExecuteCreatePlaylist, ManaMovie),
			FCanExecuteAction()
		)
	);
}

void FAssetTypeActions_ManaMovie::GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets, TArray<FString>& OutSourceFilePaths) const
{
	for (auto& Asset : TypeAssets)
	{
		if (Asset->GetClass() == UFileManaMovie::StaticClass()) {
			auto* ManaMovie = CastChecked<UFileManaMovie>(Asset);
			auto ResolvedSourceFilePath = ManaMovie->GetUrl().RightChop(7);
			OutSourceFilePaths.Add(UAssetImportData::ResolveImportFilename(ResolvedSourceFilePath, ManaMovie->GetOutermost()));
		}
	}
}

void FAssetTypeActions_ManaMovie::ExecuteCreateTexture(TArray<TWeakObjectPtr<UManaMovie>> ManaMovies)
{
	const FString Suffix = TEXT("_Texture");

	IContentBrowserSingleton& ContentBrowserSingleton = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser").Get();
	TArray<UObject*> ObjectsToSync;

	for (auto MovieIt = ManaMovies.CreateConstIterator(); MovieIt; ++MovieIt) {
		auto ManaMovie = (*MovieIt).Get();
		if (ManaMovie) {
			FString Name;
			FString PackagePath;
			CreateUniqueAssetName(ManaMovie->GetOutermost()->GetName(), Suffix, PackagePath, Name);

			UManaTextureFactory* Factory = NewObject<UManaTextureFactory>();
			Factory->InitialMovieSource = ManaMovie;

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

void FAssetTypeActions_ManaMovie::ExecuteCreatePlaylist(TArray<TWeakObjectPtr<UManaMovie>> ManaMovies)
{
	const FString Suffix = TEXT("_Playlist");

	if (ManaMovies.Num() > 0) {

		FString Name;
		FString PackagePath;
		CreateUniqueAssetName(ManaMovies[0]->GetOutermost()->GetName(), Suffix, PackagePath, Name);

		UManaPlaylistFactory* Factory = NewObject<UManaPlaylistFactory>();
		
		// initialize play list with selected movies 
		Factory->InitialMovies = ManaMovies;

		FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
		ContentBrowserModule.Get().CreateNewAsset(Name, FPackageName::GetLongPackagePath(PackagePath), UManaPlaylist::StaticClass(), Factory);
	}
}

#undef LOCTEXT_NAMESPACE
