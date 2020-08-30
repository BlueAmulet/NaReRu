/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2013-2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Listener
 * File     : AtomListener.h
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
#include "AtomAudioVolume.h"
#include "AtomAudioVolumeHandlerForListener.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareApi.h"

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
class APlayerController;

/***************************************************************************
 *      変数宣言
 *      Prototype Variables
 ***************************************************************************/

/***************************************************************************
 *      クラス宣言
 *      Prototype Classes
 ***************************************************************************/
class CRIWARERUNTIME_API FAtomListener
{
public:
	/* コンストラクタ */
	FAtomListener();

	/* デストラクタ */
	~FAtomListener();

	/* 全てのリスナの状態を更新 */
	static void UpdateAllListeners();

	/* 全てのリスナのDistance Factor を更新 */
	static void SetDistanceFactorForAllListeners(float InDistanceFactor);

	/* リスナの状態を更新 */
	void UpdateListener(int32 PlayerIndex);

	/* 距離係数の設定 */
	void SetDistanceFactor(float InDistanceFactor);

	/* 距離係数の取得 */
	float GetDistanceFactor();

	/* 有効／無効の切り替え */
	void SetAutoUpdateEnabled(bool bEnabled);

	/* リスナ位置の指定 */
	void SetListenerLocation(FVector Location);

	/* リスナの向きの指定 */
	void SetListenerRotation(FRotator Rotation);

	/* リスナ位置の取得 */
	FVector GetListenerLocation();

	/* リスニングポイント（フォーカスポイントを加味した位置）の取得 */
	FVector GetListeningPoint();

	/* リスナハンドルの取得 */
	CriAtomEx3dListenerHn GetListenerHandle(void);

	/* Listener位置の明示的な更新用の関数 */
	void UpdateListenerTransform(const FVector &location, const FRotator &rotation);

	/* 指定したプレーヤのリスナを取得 */
	static FAtomListener* GetListener(int32 PlayerIndex = 0);

	/* 指定したプレーヤコントローラIDに対応するリスナを取得 */
	static FAtomListener* GetListenerByPlayerControllerID(int32 PlayerControllerID);

	/* 最も距離の近いリスナを取得 */
	static FAtomListener* GetNearestListener(FVector Location);

	/* リスナの数を取得 */
	static int32 GetNumListener();

	static void ResetEntranceVolumeArray();

#if WITH_EDITOR
	/* AnimationEditor中でのカメラ位置を取得・更新するための関数 */
	void UpdatePreviewListenerTransform(const FVector &location, const FRotator &rotation);

	/* EditorPreviewモード時かどうかをAtomListenerに指定するための関数 */
	void SetEditorPreviewMode(bool bIsPreview) { bIsEditorPreview = bIsPreview;}
#endif

	/* リスナに関連付けられたプレーヤコントローラのID */
	int32 PlayerControllerID;

	/* リスナに関連付けられたPawnのID */
	uint32 PawnID;

	/* ビューターゲットのID */
	uint32 ViewTargetID;

	FAtomAudioVolumeHandlerForListener* AtomAudioVolumeHandler;

private:
	static double previous_time;
	static double elapsed_time;

	/* 直前のフレームにおける座標 */
	CriAtomExVector PreviousListenerLocation;

	/* リスナリスト */
	static TArray<FAtomListener*> Listeners;

	/* リスナ */
	CriAtomEx3dListenerHn ListenerHandle;

	/* 距離係数 */
	float DistanceFactor;

	/* リスナを自動で操作するかどうか */
	bool bAutoUpdateEnabled;

	/* リスナの位置 */
	FVector ListenerLocation;

	/* リスナの向き */
	FRotator ListenerRotation;
	FQuat ListenerRotation_Quat;

	/* プレーヤコントローラの取得 */
	APlayerController* GetPlayerController(int32 PlayerIndex);

#if WITH_EDITOR
	/* PreviewWorld中での再生かどうかを確認するためのフラグ（AtomComponent内で判定する） */
	bool bIsEditorPreview;
	
	/* AnimationEditor中でのSoundPreview時のカメラロケーション */
	FVector PreviewLocation;

	/* AnimationEditor中でのSoundPreview時のカメラローテーション */
	FRotator PreviewRotation;
#endif
};

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/

/* --- end of file --- */