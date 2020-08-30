/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2017-2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Base CriFsLoader Component
 * File     : CriFsLoaderComponent.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* モジュールヘッダ */
#include "CriFsLoaderComponent.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareRuntimePrivatePCH.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/
#define LOCTEXT_NAMESPACE "CriFsLoaderComponent"

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
/* File Systemライブラリの終了判定用 */
extern "C" CriBool CRIAPI criFs_IsInitialized(void);

/***************************************************************************
 *      変数定義
 *      Variable Definition
 ***************************************************************************/

/***************************************************************************
 *      クラス定義
 *      Class Definition
 ***************************************************************************/
UCriFsLoaderComponent::UCriFsLoaderComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	bTickInEditor = true;
	bNeverNeedsRenderUpdate = true;
	bAutoActivate = true;

	Binder = nullptr;
	Loader = nullptr;
	BindId = CRIFSBINDER_BID_NULL;
	FileSize = -1;
	Status = ECriFsLoaderStatus::Stop;
	DataArray.Empty();
}

void UCriFsLoaderComponent::PostLoad()
{
	Super::PostLoad();

	/* デフォルトオブジェクトかどうかチェック */
	if (HasAnyFlags(RF_ClassDefaultObject)) {
		/* デフォルトオブジェクトの場合は何もしない */
		return;
	}

#if WITH_EDITOR
	/* ゲーム実行中かどうかチェック */
	/* 注意）マップロード時は処理をスキップ。 */
	UWorld* World = GetWorld();
	bool bIsGameWorld = (World ? World->IsGameWorld() : false);
	if ((GIsEditor != false) && (bIsGameWorld == false)) {
		return;
	}
#endif

	/* ロードの開始 */
	if (bAutoActivate) {
		Load();
	}
}

void UCriFsLoaderComponent::BeginDestroy()
{
	/* ローダの破棄 */
	/* 注意）GC処理の順序によっては、ライブラリ終了処理後にここに来る可能性がある。 */
	if (criFs_IsInitialized() != CRI_FALSE) {
		SetStatus(ECriFsLoaderStatus::Stop);
	}

	/* メモリの解放 */
	DataArray.Empty();

	Super::BeginDestroy();
}

void UCriFsLoaderComponent::Load()
{
	/* 多重ロードは許可しない */
	if (Status == ECriFsLoaderStatus::Loading) {
		UE_LOG(LogCriWareRuntime, Error,
			TEXT("UCriFsLoaderComponent::Load is not allowed while loading."));
		return;
	}

	/* ステータスを一旦リセット */
	if (Status != ECriFsLoaderStatus::Stop) {
		SetStatus(ECriFsLoaderStatus::Stop);
	}

	/* パスが指定されているかどうかチェック */
	if (FilePath.IsEmpty()) {
		UE_LOG(LogCriWareRuntime, Error, TEXT("File path not specified."));
		SetStatus(ECriFsLoaderStatus::Error);
		return;
	}

	/* Non-Asset Content Directoryからのパスに変換 */
	/* 注意）既にフルパスになっている場合は変換をスキップ。 */
	if (FilePath.StartsWith(FPaths::ProjectContentDir()) == false) {
		FullPath = UCriWareInitializer::GetContentDir() + FilePath;
	} else {
		FullPath = FilePath;
	}

	/* ロード開始の通知 */
	SetStatus(ECriFsLoaderStatus::Loading);
}

ECriFsLoaderStatus UCriFsLoaderComponent::GetStatus()
{
	return Status;
}

const TArray<uint8>& UCriFsLoaderComponent::GetDataArray()
{
	return DataArray;
}

void UCriFsLoaderComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/* ロード中かどうかチェック */
	if (Status != ECriFsLoaderStatus::Loading) {
		/* ロード中以外は何もしない */
		return;
	}

	CriError Error;

	/* ファイルサイズの有無をチェック */
	if (FileSize < 0) {
		/* CriFsBinderの有無をチェック */
		if (Binder == nullptr) {
			/* バインダの作成 */
			Error = criFsBinder_Create(&Binder);
			if (Error != CRIERR_OK) {
				SetStatus(ECriFsLoaderStatus::Error);
				return;
			}

			/* バインドの開始 */
			Error = criFsBinder_BindFile(
				nullptr, nullptr, TCHAR_TO_UTF8(*FullPath), nullptr, 0, &BindId);
			if (Error != CRIERR_OK) {
				SetStatus(ECriFsLoaderStatus::Error);
				return;
			}
		} else {
			/* バインド状態の取得 */
			CriFsBinderStatus BinderStatus;
			Error = criFsBinder_GetStatus(BindId, &BinderStatus);
			if ((Error != CRIERR_OK) || (BinderStatus == CRIFSBINDER_STATUS_ERROR)) {
				SetStatus(ECriFsLoaderStatus::Error);
				return;
			}

			/* 完了チェック */
			if (BinderStatus == CRIFSBINDER_STATUS_COMPLETE) {
				/* ファイルサイズの取得 */
				CriSint64 NewFileSize;
				Error = criFsBinder_GetFileSize(Binder, TCHAR_TO_UTF8(*FullPath), &NewFileSize);
				if ((Error != CRIERR_OK) || (NewFileSize < 0)) {
					SetStatus(ECriFsLoaderStatus::Error);
					return;
				}

				/* ファイルサイズの上限チェック */
				if (NewFileSize > MAX_int32) {
					UE_LOG(LogCriWareRuntime, Error, TEXT("File size must be less than 2GB."));
					SetStatus(ECriFsLoaderStatus::Error);
					return;
				}

				/* ファイルサイズの保存 */
				FileSize = NewFileSize;
			}
		}
	}

	if (FileSize >= 0) {
		/* CriFsLoaderの有無をチェック */
		if (Loader == nullptr) {
			/* ローダの作成 */
			Error = criFsLoader_Create(&Loader);
			if (Error != CRIERR_OK) {
				SetStatus(ECriFsLoaderStatus::Error);
				return;
			}

			/* ロードに必要なメモリを確保 */
			DataArray.SetNumUninitialized((int32)FileSize);

			/* ロードの開始 */
			Error = criFsLoader_Load(
				Loader, Binder, TCHAR_TO_UTF8(*FullPath), 0, FileSize, DataArray.GetData(), FileSize);
			if (Error != CRIERR_OK) {
				SetStatus(ECriFsLoaderStatus::Error);
				return;
			}
		} else {
			/* ロード状態の取得 */
			CriFsLoaderStatus LoaderStatus;
			Error = criFsLoader_GetStatus(Loader, &LoaderStatus);
			if ((Error != CRIERR_OK) || (LoaderStatus == CRIFSLOADER_STATUS_ERROR)) {
				SetStatus(ECriFsLoaderStatus::Error);
				return;
			}

			/* 完了チェック */
			if (LoaderStatus == CRIFSLOADER_STATUS_COMPLETE) {
				/* ステータスの更新 */
				SetStatus(ECriFsLoaderStatus::Complete);
				return;
			}
		}
	}
}

void UCriFsLoaderComponent::SetStatus(ECriFsLoaderStatus NewStatus)
{
	/* ステータス変更の有無をチェック */
	if (Status == NewStatus) {
		return;
	}

	/* 備考）ステータスを更新する際にはリソースを一旦破棄する。 */
	/* 　　　→バインダやローダの数に限りがあるので、           */
	/* 　　　　ロード処理中以外はハンドルを解放する。           */

	/* ローダの破棄 */
	if (Loader != nullptr) {
		criFsLoader_Destroy(Loader);
		Loader = nullptr;
	}

	/* アンバインド */
	if (BindId != CRIFSBINDER_BID_NULL) {
		criFsBinder_Unbind(BindId);
		BindId = CRIFSBINDER_BID_NULL;
	}

	/* バインダの破棄 */
	if (Binder != nullptr) {
		criFsBinder_Destroy(Binder);
		Binder = nullptr;
	}

	/* ロード完了時以外はロード済みデータも破棄 */
	if (NewStatus != ECriFsLoaderStatus::Complete) {
		FileSize = -1;
		DataArray.Empty();
	}

	/* ステータスの更新 */
	Status = NewStatus;

	/* イベントをトリガ */
	switch (NewStatus) {
		case ECriFsLoaderStatus::Complete:
		OnLoadCompleted.Broadcast();
		break;

		case ECriFsLoaderStatus::Error:
		OnLoadError.Broadcast();
		break;

		default:
		break;
	}
}

/***************************************************************************
 *      関数定義
 *      Function Definition
 ***************************************************************************/
#undef LOCTEXT_NAMESPACE

/* --- end of file --- */
