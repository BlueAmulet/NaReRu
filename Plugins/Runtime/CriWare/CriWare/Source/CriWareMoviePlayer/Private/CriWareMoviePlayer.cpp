/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : IModuleInterface Class for CriWareMoviePlayer module
 * File     : CriMovePlayer.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* CRIWAREプラグインヘッダ */
#include "CriWareMovieStreamer.h"
#include "CriWarePluginSettings.h"

/* Unreal Engine 4関連ヘッダ */
#include "Engine/Engine.h"
#include "MoviePlayer.h"
#include "MoviePlayerSettings.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

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

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/

/***************************************************************************
 *      変数定義
 *      Variable Definition
 ***************************************************************************/
TSharedPtr<FManaPlayerMovieStreamer> CriWareMovieStreamer;

/***************************************************************************
 *      クラス定義
 *      Class Definition
 ***************************************************************************/
class FCriWareMoviePlayerModule : public IModuleInterface
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override
	{
		GetMutableDefault<UCriWarePluginSettings>()->LoadConfig();
		if (GetDefault<UCriWarePluginSettings>()->bUseManaStartupMovies)
		{
			// override unreal curent platform specific MovieStreamer with Sofdec MovieStreamer
			FManaPlayerMovieStreamer *Streamer = new FManaPlayerMovieStreamer;
			CriWareMovieStreamer = MakeShareable(Streamer);
			GetMoviePlayer()->RegisterMovieStreamer(CriWareMovieStreamer);
			SetupLoadingScreenFromIni();

			// Disable other MoviePlayer modules to avoid them to override us withntheri MovieStreamer
			// Actually only one MoviePlayer can be used: the last to load.
			auto AllPlugins = IPluginManager::Get().GetEnabledPlugins();
			for (auto& Plugin : AllPlugins) {
				auto& Descriptor = Plugin->GetDescriptor();
				for (auto& Module : Descriptor.Modules) {
					if (Module.LoadingPhase == ELoadingPhase::PreLoadingScreen
						&& Module.Type == EHostType::RuntimeNoCommandlet
						&& Module.Name.ToString().EndsWith("MoviePlayer", ESearchCase::CaseSensitive) // MoviePlayer only
						&& !Module.Name.ToString().StartsWith("CriWare", ESearchCase::CaseSensitive)) // not us
					{
						// override loading phase to None.
						const_cast<FModuleDescriptor&>(Module).LoadingPhase = ELoadingPhase::None;
					}
				}
			}
		}
	}

	virtual void ShutdownModule() override
	{
		CriWareMovieStreamer.Reset();
	}

	void SetupLoadingScreenFromIni()
	{
		// fill out the attributes
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = !GetDefault<UCriWarePluginSettings>()->bWaitForMoviesToComplete;
		LoadingScreen.bMoviesAreSkippable = GetDefault<UCriWarePluginSettings>()->bMoviesAreSkippable;

		// look in the settings for a list of loading movies
		const TArray<FString>& StartupMovies = GetDefault<UCriWarePluginSettings>()->StartupMovies;

		if (StartupMovies.Num() == 0) {
			LoadingScreen.MoviePaths.Add(TEXT("Default_Startup"));
		} else {
			for (const FString& Movie : StartupMovies) {
				if (!Movie.IsEmpty()) {
					LoadingScreen.MoviePaths.Add(Movie);
				}
			}
		}

		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	}
};

IMPLEMENT_MODULE(FCriWareMoviePlayerModule, CriWareMoviePlayer)

/***************************************************************************
 *      関数定義
 *      Function Definition
 ***************************************************************************/

/* --- end of file --- */
