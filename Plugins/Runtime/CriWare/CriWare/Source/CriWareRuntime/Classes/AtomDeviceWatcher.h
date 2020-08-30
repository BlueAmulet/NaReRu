/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Atom Device Watcher
 * File     : AtomDeviceWatcher.h
 *
 ****************************************************************************/

/* 多重定義防止 */
#pragma once

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* Unreal Engine 4関連ヘッダ */
#include "CoreMinimal.h"

/* モジュールヘッダ */
#include "AtomDeviceWatcher.generated.h"

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
class FAtomDeviceWatcher;

/***************************************************************************
 *      変数宣言
 *      Prototype Variables
 ***************************************************************************/

/***************************************************************************
 *      クラス宣言
 *      Prototype Classes
 ***************************************************************************/
UCLASS(BlueprintType)
class CRIWARERUNTIME_API UAtomDeviceWatcher : public UObject
{
	GENERATED_UCLASS_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeviceUpdated);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnDeviceUpdatedNative, class UAtomDeviceWatcher*);

public:
	UPROPERTY(BlueprintAssignable)
	FOnDeviceUpdated OnDeviceUpdated;

	UFUNCTION(BlueprintCallable, Category = "AtomDeviceWatcher")
	FString GetDeviceName();

	UFUNCTION(BlueprintCallable, Category = "AtomDeviceWatcher")
	int32 GetDeviceChannelCount();

	FOnDeviceUpdatedNative OnDeviceUpdatedNative;

	static FAtomDeviceWatcher* DeviceWatcherClass;
	static TArray<UAtomDeviceWatcher*> DeviceWatcherObjects;

	// Begin UObject interface.
	virtual void PostInitProperties() override;
	virtual void BeginDestroy() override;
	// End UObject interface.
};

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/

/* --- end of file --- */
