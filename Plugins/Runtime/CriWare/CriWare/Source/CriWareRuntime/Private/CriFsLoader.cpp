/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2017-2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Base CriFsLoader Actor
 * File     : CriFsLoader.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* モジュールヘッダ */
#include "CriFsLoader.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareRuntimePrivatePCH.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/
#define LOCTEXT_NAMESPACE "CriFsLoader"

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
ACriFsLoader::ACriFsLoader(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	LoaderComponent = CreateDefaultSubobject<UCriFsLoaderComponent>(TEXT("CriFsLoaderComponent0"));
	LoaderComponent->bAutoActivate = true;
	RootComponent = LoaderComponent;

	bReplicates = false;
	SetHidden(true);
	SetCanBeDamaged(false);
}

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 23
void ACriFsLoader::SetHidden(bool InHidden)
{
	bHidden = InHidden;
}

void ACriFsLoader::SetCanBeDamaged(bool InCanBeDamaged)
{
	bCanBeDamaged = InCanBeDamaged;
}
#endif

/***************************************************************************
 *      関数定義
 *      Function Definition
 ***************************************************************************/
#undef LOCTEXT_NAMESPACE

/* --- end of file --- */
