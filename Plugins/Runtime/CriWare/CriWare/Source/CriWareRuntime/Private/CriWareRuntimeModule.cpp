/****************************************************************************
*
* CRI Middleware SDK
*
* Copyright (c) 2013-2016 CRI Middleware Co., Ltd.
*
* Library  : CRIWARE plugin for Unreal Engine 4
* Module   : CriWareRuntime Module Implementation
* File     : CriWareRuntimeModule.cpp
*
****************************************************************************/

/***************************************************************************
*      インクルードファイル
*      Include files
***************************************************************************/
#include "CriWareRuntimePrivatePCH.h"
#include "ICriWareRuntime.h"
#include "HAL/PlatformFilemanager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"

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

/***************************************************************************
*      クラス定義
*      Class Definition
***************************************************************************/
/**
* CriWareRuntime module implementation (private)
*/

#if !defined(CRIWARE_UE4_LE)	/* <cri_delete_if_LE> */

class FCriWareRuntimeModule : public ICriWareRuntime
{
public:
	/** IModuleInterface */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FCriWareRuntimeModule, CriWareRuntime);

void FCriWareRuntimeModule::StartupModule()
{
	// ここで作られる RealShaderSourceDir は CriWare.uplugin と同じディレクトリ
	// Prior to 4.21, plugin shaders was automatically loaded, from 4.21 we need to do this manually.
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 21
	FString RealShaderSourceDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("CriWare"))->GetBaseDir(), TEXT("Shaders"));
	FString VirtualShaderSourceDir = FString(TEXT("/Plugin/CriWare"));
	UE_LOG(LogCriWareRuntime, Log,
		TEXT("Mapping shader source directory %s to virtual directory %s"),
		*RealShaderSourceDir, *VirtualShaderSourceDir);
	AddShaderSourceDirectoryMapping(VirtualShaderSourceDir, RealShaderSourceDir);
#endif
}

void FCriWareRuntimeModule::ShutdownModule()
{
}

#else	/* </cri_delete_if_LE> */

class FCriWareRuntimeModule : public ICriWareRuntime
{
public:
	/** IModuleInterface */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FCriWareRuntimeModule, CriWareRuntime);

void FCriWareRuntimeModule::StartupModule()
{
}

void FCriWareRuntimeModule::ShutdownModule()
{
}

#endif

/***************************************************************************
*      関数定義
*      Function Definition
***************************************************************************/

/* --- end of file --- */
