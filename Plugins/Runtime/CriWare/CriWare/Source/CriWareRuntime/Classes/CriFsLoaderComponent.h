/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2017-2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Base CriFsLoader Component
 * File     : CriFsLoaderComponent.h
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
#include "UObject/Object.h"
#include "UObject/ScriptMacros.h"
#include "GameFramework/Actor.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareApi.h"

/* モジュールヘッダ */
#include "CriFsLoaderComponent.generated.h"

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
UENUM()
enum class ECriFsLoaderStatus : uint8
{
	Stop,				/** 停止中 */
	Loading,			/** ロード中 */
	Complete,			/** ロード完了 */
	Error				/** エラー */
};

/***************************************************************************
 *      変数宣言
 *      Prototype Variables
 ***************************************************************************/

/***************************************************************************
 *      クラス宣言
 *      Prototype Classes
 ***************************************************************************/
UCLASS(ClassGroup=CRIWARE, BlueprintType,
	meta=(BlueprintSpawnableComponent, DisplayName = "Cri Fs Loader", ToolTip = "Cri Fs Loader Component"))
class CRIWARERUNTIME_API UCriFsLoaderComponent : public USceneComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadCompleted);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadError);

public:
	UCriFsLoaderComponent(const FObjectInitializer& ObjectInitializer);

	/* ファイルパス */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FilePath")
	FString FilePath;

	/* ロード完了時に呼び出されるデリゲート */
	UPROPERTY(BlueprintAssignable, Category="CriFsLoaderComponent")
	FOnLoadCompleted OnLoadCompleted;

	/* ロード失敗時に呼び出されるデリゲート */
	UPROPERTY(BlueprintAssignable, Category="CriFsLoaderComponent")
	FOnLoadError OnLoadError;

	/* ロード */
	UFUNCTION(BlueprintCallable, Category="CriFsLoaderComponent")
	void Load();

	/* ステータスの取得 */
	UFUNCTION(BlueprintCallable, Category="CriFsLoaderComponent")
	ECriFsLoaderStatus GetStatus();

	/* ロード済みデータの取得 */
	UFUNCTION(BlueprintCallable, Category="CriFsLoaderComponent")
	const TArray<uint8>& GetDataArray();

	// Begin UObject interface.
	virtual void PostLoad() override;
	virtual void BeginDestroy() override;
	// End of UObject interface

	// UActorComponent interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// End of UActorComponent interface

private:
	/* バインダハンドル */
	CriFsBinderHn Binder;

	/* ローダハンドル */
	CriFsLoaderHn Loader;

	/* バインドID */
	CriFsBindId BindId;

	/* ファイルのフルパス */
	FString FullPath;

	/* ファイルサイズ */
	int64 FileSize;

	/* ロード中かどうか */
	ECriFsLoaderStatus Status;

	/* データ配列 */
	TArray<uint8> DataArray;

	/* ステータスの更新 */
	void SetStatus(ECriFsLoaderStatus NewStatus);
};

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/

/* --- end of file --- */
