/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2017-2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Base CriFsLoader Actor
 * File     : CriFsLoader.h
 *
 ****************************************************************************/

/* 多重定義防止 */
#pragma once

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
#include "CriFsLoaderComponent.h"
#include "CriFsLoader.generated.h"

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
UCLASS(MinimalAPI)
class ACriFsLoader : public AActor
{
	GENERATED_BODY()

public:
	ACriFsLoader(const FObjectInitializer& ObjectInitializer);

	/* ロードに使用するLoaderComponent */
	UPROPERTY(Category="CriFsLoader", VisibleAnywhere, BlueprintReadOnly)
	class UCriFsLoaderComponent *LoaderComponent;

private:

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 23
	void SetHidden(bool InHidden);
	void SetCanBeDamaged(bool InCanBeDamaged);
#endif
};

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/

/* --- end of file --- */
