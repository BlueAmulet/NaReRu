/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2015-2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Asset Factory of ManaTexture
 * File     : ManaTextureFactory.h
 *
 ****************************************************************************/

/* 多重定義防止 */
#pragma once

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
 #include "Factories/Factory.h"
#include "ManaTextureFactory.generated.h"

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

class UManaSource;

UCLASS(HideCategories = Object)
class UManaTextureFactory : public UFactory
{
	GENERATED_BODY()

public:
	UManaTextureFactory(const FObjectInitializer& ObjectInitializer);

	/** Mana movie source to set in texture. */
	UPROPERTY()
	UManaSource* InitialMovieSource;

	//~ Begin UFactory Interface
	virtual UObject* FactoryCreateNew(
		UClass* Class, 
		UObject* InParent, 
		FName Name, 
		EObjectFlags Flags, 
		UObject* Context, 
		FFeedbackContext* Warn) override;
	//~ End UFactory Interface
};

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/

/* --- end of file --- */
