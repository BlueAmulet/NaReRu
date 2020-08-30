/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Base CriFsBinder Component
 * File     : CriFsBinderComponent.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* モジュールヘッダ */
#include "CriFsBinderComponent.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareRuntimePrivatePCH.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/
#define LOCTEXT_NAMESPACE "CriFsBinderComponent"

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
UCriFsBinderComponent::UCriFsBinderComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	bTickInEditor = true;
	bNeverNeedsRenderUpdate = true;
	bAutoActivate = true;

	Binder = nullptr;
	BindId = CRIFSBINDER_BID_NULL;
	Status = ECriFsBinderStatus::Stop;
}

void UCriFsBinderComponent::PostLoad()
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

	/* バインドの開始 */
	if (bAutoActivate) {
		Bind();
	}
}

void UCriFsBinderComponent::BeginDestroy()
{
	/* バインダの破棄 */
	/* 注意）GC処理の順序によっては、ライブラリ終了処理後にここに来る可能性がある。 */
	if (criFs_IsInitialized() != CRI_FALSE) {
		Stop();
	}

	Super::BeginDestroy();
}

void UCriFsBinderComponent::Bind()
{
	/* 多重バインドは許可しない */
	if (Status == ECriFsBinderStatus::Binding) {
		UE_LOG(LogCriWareRuntime, Error,
			TEXT("UCriFsBinderComponent::Bind is not allowed while binding."));
		return;
	}

	/* 処理を中止 */
	if (Status != ECriFsBinderStatus::Stop) {
		Stop();
	}

	/* パスが指定されているかどうかチェック */
	if (FilePath.IsEmpty()) {
		UE_LOG(LogCriWareRuntime, Error, TEXT("File path not specified."));
		SetStatus(ECriFsBinderStatus::Error);
		return;
	}

	/* Non-Asset Content Directoryからのパスに変換 */
	/* 注意）既にフルパスになっている場合は変換をスキップ。 */
	if (FilePath.StartsWith(FPaths::ProjectContentDir()) == false) {
		FullPath = UCriWareInitializer::GetContentDir() + FilePath;
	} else {
		FullPath = FilePath;
	}

	/* バインド開始の通知 */
	SetStatus(ECriFsBinderStatus::Binding);
}

void UCriFsBinderComponent::Stop()
{
	SetStatus(ECriFsBinderStatus::Stop);
}

ECriFsBinderStatus UCriFsBinderComponent::GetStatus()
{
	return Status;
}

CriFsBinderHn UCriFsBinderComponent::MoveHandle()
{
	CriFsBinderHn Result = Binder;

	/* メンバ変数のクリア */
	Binder = NULL;
	BindId = CRIFSBINDER_BID_NULL;

	return Result;
}

void UCriFsBinderComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/* バインド中かどうかチェック */
	if (Status != ECriFsBinderStatus::Binding) {
		/* バインド中以外は何もしない */
		return;
	}

	CriError Error;

	/* CriFsBinderの有無をチェック */
	if (Binder == nullptr) {
		/* バインダの作成 */
		Error = criFsBinder_Create(&Binder);
		if (Error != CRIERR_OK) {
			SetStatus(ECriFsBinderStatus::Error);
			return;
		}

		/* バインドの開始 */
		Error = criFsBinder_BindFile(
			Binder, nullptr, TCHAR_TO_UTF8(*FullPath), nullptr, 0, &BindId);
		if (Error != CRIERR_OK) {
			SetStatus(ECriFsBinderStatus::Error);
			return;
		}
	} else {
		/* バインド状態の取得 */
		CriFsBinderStatus BinderStatus;
		Error = criFsBinder_GetStatus(BindId, &BinderStatus);
		if ((Error != CRIERR_OK) || (BinderStatus == CRIFSBINDER_STATUS_ERROR)) {
			SetStatus(ECriFsBinderStatus::Error);
			return;
		}

		/* 完了チェック */
		if (BinderStatus == CRIFSBINDER_STATUS_COMPLETE) {
			/* ステータスの更新 */
			SetStatus(ECriFsBinderStatus::Complete);
			return;
		}
	}
}

void UCriFsBinderComponent::SetStatus(ECriFsBinderStatus NewStatus)
{
	/* ステータス変更の有無をチェック */
	if (Status == NewStatus) {
		return;
	}

	/* バインド完了時以外はバインド済みデータも破棄 */
	/* 備考）バインダの数に限りがあるので、         */
	/* 　　　バインド成功時以外はハンドルを破棄。   */
	if ((NewStatus == ECriFsBinderStatus::Stop)
		|| (NewStatus == ECriFsBinderStatus::Error)) {
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
	}

	/* ステータスの更新 */
	Status = NewStatus;

	/* イベントをトリガ */
	switch (NewStatus) {
		case ECriFsBinderStatus::Complete:
		OnBindCompleted.Broadcast();
		break;

		case ECriFsBinderStatus::Error:
		OnBindError.Broadcast();
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
