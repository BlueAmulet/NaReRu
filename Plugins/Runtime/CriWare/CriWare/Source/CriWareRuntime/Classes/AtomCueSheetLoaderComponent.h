/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Atom Cue Sheet Loader Component
 * File     : AtomCueSheetLoaderComponent.h
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
#include "Components/SceneComponent.h"
#include "Engine/LatentActionManager.h"

/* CRIWAREプラグインヘッダ */
#include "SoundAtomCueSheet.h"
#include "CriFsBinderComponent.h"

/* モジュールヘッダ */
#include "AtomCueSheetLoaderComponent.generated.h"

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
enum class EAtomCueSheetLoaderComponentStatus : uint8
{
	Stop,				/** 停止中 */
	Loading,			/** ロード中 */
	LoadEnd,			/** ロード完了 */
	Serializing,		/** シリアライズ中 */
	SerializeEnd,		/** シリアライズ完了 */
	Binding,			/** バインド中 */
	BindEnd,			/** バインド完了 */
	LoadCueSheet,		/** LoadCueSheet */
	Complete,			/** アクティベート完了 */
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
	meta=(BlueprintSpawnableComponent, DisplayName = "Atom Cue Sheet Loader", ToolTip = "Atom Cue Sheet Loader Component"))
class CRIWARERUNTIME_API UAtomCueSheetLoaderComponent : public USceneComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnAtomCueSheetLoaded, class USoundAtomCueSheet*, Loaded);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadCompleted);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLoadCompletedNative, class UAtomCueSheetLoaderComponent*);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadError);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLoadErrorNative, class UAtomCueSheetLoaderComponent*);

public:
	// Sets default values for this component's properties
	UAtomCueSheetLoaderComponent();

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = "LatentInfo", WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "AtomCueSheetLoaderComponent")
	static void LoadAtomCueSheet(UObject* WorldContextObject, TSoftObjectPtr<USoundAtomCueSheet> Asset, FOnAtomCueSheetLoaded OnLoaded, FLatentActionInfo LatentInfo);

	/* 音声データ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowedClasses="SoundAtomCueSheet", ToolTip = "Sound data."))
	FSoftObjectPath CueSheetReference;

	/* バインダ */
	UPROPERTY(Transient)
	UCriFsBinderComponent* BinderComponent;

	/* ロード完了時に呼び出されるデリゲート */
	UPROPERTY(BlueprintAssignable, Category="AtomCueSheetLoaderComponent")
	FOnLoadCompleted OnLoadCompleted;

	/* ロード失敗時に呼び出されるデリゲート */
	UPROPERTY(BlueprintAssignable, Category="AtomCueSheetLoaderComponent")
	FOnLoadError OnLoadError;

	/* ロードの開始 */
	UFUNCTION(BlueprintCallable, Category="AtomCueSheetLoaderComponent")
	void Load();

	/* ロードの終了 */
	UFUNCTION(BlueprintCallable, Category="AtomCueSheetLoaderComponent")
	USoundAtomCueSheet* GetAtomCueSheet();

	/* ステータスの取得 */
	UFUNCTION(BlueprintCallable, Category="AtomCueSheetLoaderComponent")
	EAtomCueSheetLoaderComponentStatus GetStatus() { return Status; }

	FOnLoadCompletedNative OnLoadCompletedNative;
	FOnLoadErrorNative OnLoadErrorNative;

public:
	// Begin UObject interface.
	virtual void PostLoad() override;
	virtual void BeginDestroy() override;
	// End of UObject interface

	// UActorComponent interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// End of UActorComponent interface

private:
	/* キューシート */
	UPROPERTY(Transient)
	USoundAtomCueSheet* CueSheet;

	/* ステータス */
	EAtomCueSheetLoaderComponentStatus Status;

	FSoftObjectPath LocalizedSoftObjectPath;

	/* 更新処理 */
	bool Process();
	bool OnLoading();
	bool OnLoadEnd();
	bool OnSerializing();
	bool OnSerializeEnd();
	bool OnBinding();
	bool OnBindEnd();
	bool OnLoadCueSheet();

	/* ステータスの更新 */
	void SetStatus(EAtomCueSheetLoaderComponentStatus NewStatus);
};

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/

/* --- end of file --- */
