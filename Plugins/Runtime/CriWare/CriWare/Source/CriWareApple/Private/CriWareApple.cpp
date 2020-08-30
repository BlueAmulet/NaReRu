/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE Apple plugin for Unreal Engine 4
 * Module   : IModuleInterface Class for CriWareApple module
 * File     : CriWareApple.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* CRIWAREプラグインヘッダ */
#include "CriWareManaApple.h"

/* Unreal Engine 4関連ヘッダ */
#include "Engine/Engine.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/

DECLARE_LOG_CATEGORY_EXTERN(LogCriWareApple, Log, All);
DEFINE_LOG_CATEGORY(LogCriWareApple);

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
class FCriWareAppleModule : public IModuleInterface
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override
	{
		// Check if CriWareRuntime is enabled
		bool IsCriWareRuntimeEnabled = false;
		auto AllPlugins = IPluginManager::Get().GetEnabledPlugins();
		for (auto& Plugin : AllPlugins) {
			if (Plugin->GetName() == "CriWare") {
				IsCriWareRuntimeEnabled = true;
			}
		}

		if (IsCriWareRuntimeEnabled) {
			ManaApple = MakeShareable(new FCriWareManaApple());
		} else {
			UE_LOG(LogCriWareApple, Error, TEXT("CriWare Plugin is not Enabled."));
		}
	}

	virtual void ShutdownModule() override
	{
		ManaApple.Reset();
	}

private:
	TSharedPtr<FCriWareManaApple> ManaApple;
};

IMPLEMENT_MODULE(FCriWareAppleModule, CriWareApple)

/* --- end of file --- */
