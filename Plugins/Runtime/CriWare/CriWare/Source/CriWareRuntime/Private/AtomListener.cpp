/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2013-2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Listener
 * File     : AtomListener.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* モジュールヘッダ */
#include "AtomListener.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareRuntimePrivatePCH.h"
#include "AtomListenerFocusPoint.h"

/* Unreal Engine 4関連ヘッダ */
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "HeadMountedDisplay.h"
#include "HeadMountedDisplayFunctionLibrary.h"

#if WITH_EDITOR
#include "Settings/SkeletalMeshEditorSettings.h"
#include "Editor/Persona/Private/AnimationEditorViewportClient.h"
#endif

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
TArray<FAtomListener*> FAtomListener::Listeners;
double FAtomListener::previous_time = 0.0;
double FAtomListener::elapsed_time = 0.0;

/***************************************************************************
 *      クラス定義
 *      Class Definition
 ***************************************************************************/
/* コンストラクタ */
FAtomListener::FAtomListener()
{
	/* リスナハンドルを作成 */
	ListenerHandle = criAtomEx3dListener_Create(NULL, NULL, 0);

	/* 距離係数の保存 */
	DistanceFactor = 1.0f;

	/* リスナを自動で操作するよう設定 */
	bAutoUpdateEnabled = true;

	/* リスナ位置の初期化 */
	ListenerLocation = FVector::ZeroVector;
	ListenerRotation = FRotator::ZeroRotator;

	PreviousListenerLocation.x = ListenerLocation.X;
	PreviousListenerLocation.y = ListenerLocation.Y;
	PreviousListenerLocation.z = ListenerLocation.Z;

	/* プレーヤコントローラ、Pawnとの紐づけをクリア */
	PlayerControllerID = INDEX_NONE;
	PawnID = 0;
	ViewTargetID = 0;

	AtomAudioVolumeHandler = new FAtomAudioVolumeHandlerForListener();

	/* リストに登録 */
	Listeners.Add(this);

#if WITH_EDITOR
	bIsEditorPreview = false;
	PreviewLocation = FVector::ZeroVector;
	PreviewRotation = FRotator::ZeroRotator;
#endif
}

/* デストラクタ */
FAtomListener::~FAtomListener()
{
	/* 備考）リスナハンドルは破棄しない。				*/
	/* 　　　AtomExプレーヤより先に破棄できないので、	*/
	/* 　　　Finalizeで自動的に破棄されるのを待つ。		*/

	/* リストから破棄 */
	Listeners.Remove(this);

	delete AtomAudioVolumeHandler;
}

void FAtomListener::ResetEntranceVolumeArray()
{
	for (FAtomListener* listener : Listeners) {
		listener->AtomAudioVolumeHandler->ResetEntranceVolumeArray();
	}
}

/* 全てのリスナの状態を更新 */
void FAtomListener::UpdateAllListeners()
{
	/* 初期化済みかどうかチェック */
	if (criAtomEx_IsInitialized() == CRI_FALSE) {
		return;
	}

#if WITH_EDITOR
	/* プレビュー実行中かどうかチェック */
	bool bIsDebugging = (GEditor ? (GEditor->PlayWorld != nullptr) : false);
	bool bIsEditorPreviewMode = false;
	for (FAtomListener* Listener : Listeners) {
		if (Listener->bIsEditorPreview) {
			/* PreviewWorldで再生中のSoundがあるかどうかの確認              */
			/* 存在している場合はbIsEditorPreviewがtrueになっているものがある */
			/* 一つでもbIsEditorPreviewがtrueならリスナーの更新を行う        */
			bIsEditorPreviewMode = true;
			break;
		}
	}
	if ((GIsEditor != false) && (bIsDebugging == false) && !bIsEditorPreviewMode) {
		/* エディタ上ではプレビュー中のみリスナの更新を行う */
		return;
	}
#endif

	double current_time = FPlatformTime::Seconds();
	elapsed_time = current_time - previous_time;

	/* リスナの更新 */
	int32 PlayerIndex = 0;
	for (FAtomListener* Listener : Listeners) {
		if (!Listener) {
			continue;
		}
#if WITH_EDITOR
		if (Listener->bIsEditorPreview) {
			Listener->UpdateListenerTransform(Listener->PreviewLocation, Listener->PreviewRotation);
			continue;
		}
#endif
		Listener->UpdateListener(PlayerIndex);
		PlayerIndex++;
	}

	previous_time = current_time;
}

void FAtomListener::SetDistanceFactorForAllListeners(float InDistanceFactor)
{
	/* 初期化済みかどうかチェック */
	if (criAtomEx_IsInitialized() == CRI_FALSE) {
		return;
	}

#if WITH_EDITOR
	/* プレビュー実行中かどうかチェック */
	bool bIsDebugging = (GEditor ? (GEditor->PlayWorld != nullptr) : false);
	if ((GIsEditor != false) && (bIsDebugging == false)) {
		/* エディタ上ではプレビュー中のみリスナの更新を行う */
		return;
	}
#endif

	/* リスナの更新 */
	int32 PlayerIndex = 0;
	for (FAtomListener* Listener : Listeners) {
		if (Listener != nullptr) {
			Listener->SetDistanceFactor(InDistanceFactor);
			PlayerIndex++;
		}
	}
}

/* 距離係数の設定 */
void FAtomListener::SetDistanceFactor(float InDistanceFactor)
{
	if (InDistanceFactor <= 0.0f) {
		UE_LOG(LogCriWareRuntime, Error, TEXT("Invalid distance factor."));
		return;
	}

	/* 距離係数の保存 */
	DistanceFactor = InDistanceFactor;
}

/* 距離係数の取得 */
float FAtomListener::GetDistanceFactor()
{
	return DistanceFactor;
}

/* 有効／無効の切り替え */
void FAtomListener::SetAutoUpdateEnabled(bool bEnabled)
{
	bAutoUpdateEnabled = bEnabled;
}

/* リスナ位置の指定 */
void FAtomListener::SetListenerLocation(FVector Location)
{
	ListenerLocation = Location;
}

/* リスナの向きの指定 */
void FAtomListener::SetListenerRotation(FRotator Rotation)
{
	ListenerRotation = Rotation;
	ListenerRotation_Quat = ListenerRotation.Quaternion();
}

/* リスナ位置の取得 */
FVector FAtomListener::GetListenerLocation()
{
	return ListenerLocation;
}

/* リスニングポイントの取得 */
FVector FAtomListener::GetListeningPoint()
{
	CriAtomExVector pos;
	FVector ListeningPoint;
	float level;

	/* フォーカスポイントの取得 */
	criAtomEx3dListener_GetFocusPoint(ListenerHandle, &pos);

	/* Distance Focus Levelの取得 */
	level = criAtomEx3dListener_GetDistanceFocusLevel(ListenerHandle);

	/* リスニングポイントの計算 */
	ListeningPoint.X = pos.x / DistanceFactor;
	ListeningPoint.Y = pos.y / DistanceFactor;
	ListeningPoint.Z = pos.z / DistanceFactor;

	/* リスニングポイントの計算 */
	FVector listener_location = ListenerLocation;
#if WITH_EDITOR
	listener_location = FAtomListener::bIsEditorPreview ? PreviewLocation : ListenerLocation;
#endif

	ListeningPoint = ListeningPoint * level + listener_location * (1.0f - level);

	return ListeningPoint;
}

/* リスナハンドルの取得 */
CriAtomEx3dListenerHn FAtomListener::GetListenerHandle(void)
{
	return ListenerHandle;
}

#if WITH_EDITOR
void FAtomListener::UpdatePreviewListenerTransform(const FVector &location, const FRotator &rotation)
{
	PreviewLocation = location;
	PreviewRotation = rotation;
}
#endif

void FAtomListener::UpdateListenerTransform(const FVector &location, const FRotator &rotation)
{
	if (ListenerHandle == NULL) {
		return;
	}

	/* リスナの座標を設定 */
	CriAtomExVector pos, front, top;
	pos.x = location.X * DistanceFactor;
	pos.y = location.Y * DistanceFactor;
	pos.z = location.Z * DistanceFactor;
	criAtomEx3dListener_SetPosition(ListenerHandle, &pos);

	/* リスナの向きを設定 */
	/* クオータニオンから前方ベクトルと上方ベクトルを取得 */
	FQuat rotation_quat = rotation.Quaternion();
	FVector FrontVector = rotation_quat.GetForwardVector();
	FVector TopVector = rotation_quat.GetUpVector();
	front.x = FrontVector.X;
	front.y = FrontVector.Y;
	front.z = FrontVector.Z;
	top.x = TopVector.X;
	top.y = TopVector.Y;
	top.z = TopVector.Z;
	criAtomEx3dListener_SetOrientation(ListenerHandle, &front, &top);

	/* Listenerの速度更新 */
	FVector CurrentListenerLocation = GetListeningPoint();
	CriAtomExVector listener_pos;
	listener_pos.x = CurrentListenerLocation.X * DistanceFactor;
	listener_pos.y = CurrentListenerLocation.Y * DistanceFactor;
	listener_pos.z = CurrentListenerLocation.Z * DistanceFactor;
	CriAtomExVector listener_velocity;
	listener_velocity.x = (listener_pos.x - PreviousListenerLocation.x) / elapsed_time;
	listener_velocity.y = (listener_pos.y - PreviousListenerLocation.y) / elapsed_time;
	listener_velocity.z = (listener_pos.z - PreviousListenerLocation.z) / elapsed_time;
	criAtomEx3dListener_SetVelocity(ListenerHandle, &listener_velocity);
	PreviousListenerLocation.x = listener_pos.x;
	PreviousListenerLocation.y = listener_pos.y;
	PreviousListenerLocation.z = listener_pos.z;

	/* リスナの更新 */
	criAtomEx3dListener_Update(ListenerHandle);
}

/* リスナの状態を更新 */
void FAtomListener::UpdateListener(int32 PlayerIndex)
{
	/* 初期化済みかどうかチェック */
	if (ListenerHandle == NULL) {
		return;
	}

	/* リスナ座標を自動更新するかどうかのチェック */
	if (bAutoUpdateEnabled != false) {
		/* プレーヤコントローラの取得 */
		APlayerController* PlayerController = GetPlayerController(PlayerIndex);

		/* プレーヤコントローラの有無をチェック */
		if (PlayerController != nullptr) {
			if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled()) {
				/* VR Modeの場合HMDのポジションと角度を加える */
				PlayerController->GetPlayerViewPoint(ListenerLocation, ListenerRotation);
				FVector HMDLocation;
				FRotator HMDRotation;
				UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRotation, HMDLocation);
				ListenerLocation += HMDLocation;
				ListenerRotation = HMDRotation;
			} else {
				/* プレーヤが存在する場合はプレーヤのビューポートを取得 */
				FVector FrontDir, RightDir;
				PlayerController->GetAudioListenerPosition(ListenerLocation, FrontDir, RightDir);
				ListenerRotation = FRotationMatrix::MakeFromXY(FrontDir, RightDir).Rotator();
			}

			/* ビューターゲットの取得 */
			AActor* NewViewTarget = PlayerController->GetViewTarget();
			if (NewViewTarget != nullptr) {
				uint32 NewViewTargetID = NewViewTarget->GetUniqueID();
				if (ViewTargetID != NewViewTargetID) {
					/* カメラ切り替え時はフォーカスレベルをクリア */
					criAtomEx3dListener_SetDistanceFocusLevel(ListenerHandle, 0.0f);
					criAtomEx3dListener_SetDirectionFocusLevel(ListenerHandle, 0.0f);
					ViewTargetID = NewViewTargetID;
				}

				/* 新規ビューターゲットにフォーカスポイントの更新を要請 */
				UAtomListenerFocusPoint* FocusPoint = Cast<UAtomListenerFocusPoint>(
					NewViewTarget->GetComponentByClass(UAtomListenerFocusPoint::StaticClass()));
				if (FocusPoint != nullptr) {
					FocusPoint->UpdateFocusPoint();
				}
			}

			/* プレーヤコントローラのIDを取得 */
			int32 NewPlayerControllerID = UGameplayStatics::GetPlayerControllerID(PlayerController);
			if (PlayerControllerID != NewPlayerControllerID) {
				/* プレーヤコントローラ変更時はフォーカスレベルをクリア */
				criAtomEx3dListener_SetDistanceFocusLevel(ListenerHandle, 0.0f);
				criAtomEx3dListener_SetDirectionFocusLevel(ListenerHandle, 0.0f);
				PlayerControllerID = NewPlayerControllerID;
			}

			/* PawnのIDを取得 */
			APawn* NewPawn = PlayerController->GetPawn();
			if (NewPawn != nullptr) {
				uint32 NewPawnID = NewPawn->GetUniqueID();
				if (PawnID != NewPawnID) {
					/* Possess時はフォーカスレベルをクリア */
					criAtomEx3dListener_SetDistanceFocusLevel(ListenerHandle, 0.0f);
					criAtomEx3dListener_SetDirectionFocusLevel(ListenerHandle, 0.0f);
					PawnID = NewPawnID;

					/* 新規Pawnにフォーカスポイントの更新を要請 */
					UAtomListenerFocusPoint* FocusPoint = Cast<UAtomListenerFocusPoint>(
						NewPawn->GetComponentByClass(UAtomListenerFocusPoint::StaticClass()));
					if (FocusPoint != nullptr) {
						FocusPoint->UpdateFocusPoint();
					}
				}
			}
		} else {
			/* 非アクティブであることを通知 */
			PlayerControllerID = INDEX_NONE;
			PawnID = 0;
		}
	}

	/* 非アクティブなリスナは処理しない */
	if (PlayerControllerID == INDEX_NONE) {
		return;
	}

	UpdateListenerTransform(ListenerLocation, ListenerRotation);

	/* AudioVolumeによるバスのSnapshot適用 */
	AtomAudioVolumeHandler->ProcessAudioVolume(GetListeningPoint());
}

/* 指定したプレーヤのリスナを取得 */
FAtomListener* FAtomListener::GetListener(int32 PlayerIndex)
{
	/* 指定範囲のチェック */
	if (!Listeners.IsValidIndex(PlayerIndex)) {
		return nullptr;
	}

	/* リスナを返す */
	return Listeners[PlayerIndex];
}

/* 指定したプレーヤコントローラIDに対応するリスナを取得 */
FAtomListener* FAtomListener::GetListenerByPlayerControllerID(int32 PlayerControllerID)
{
	for (FAtomListener* Listener : Listeners) {
		if ((Listener != nullptr) && (Listener->PlayerControllerID != INDEX_NONE)
			&& (Listener->PlayerControllerID == PlayerControllerID)) {
			return Listener;
		}
	}

	return nullptr;
}

/* 最も距離の近いリスナを取得 */
FAtomListener* FAtomListener::GetNearestListener(FVector Location)
{
	FAtomListener* MinListener = nullptr;
	float MinDistance = 0.0f;

	/* 最小距離の計算 */
	for (FAtomListener* Listener : Listeners) {
		/* 非アクティブなリスナは無視 */
		if ((Listener == nullptr) || (Listener->PlayerControllerID == INDEX_NONE)) {
			continue;
		}

		/* リスニングポイントからの距離を計算 */
		float Distance = FVector::DistSquared(Listener->GetListeningPoint(), Location);

		/* 最小距離のリスナを選択 */
		if ((MinListener == nullptr) || (Distance < MinDistance)) {
			MinListener = Listener;
			MinDistance = Distance;
		}
	}

	return MinListener;
}

int32 FAtomListener::GetNumListener()
{
	return Listeners.Num();
}

/* プレーヤコントローラの取得 */
APlayerController* FAtomListener::GetPlayerController(int32 PlayerIndex)
{
	/* ワールドの取得 */
	UWorld* World = GWorld;

	/* プレーヤコントローラの取得 */
	APlayerController* PlayerController = nullptr;
	if (World != nullptr) {
		PlayerController = UGameplayStatics::GetPlayerController(World, PlayerIndex);
	}

#if WITH_EDITOR
	/* 備考）PIE時は上記処理でプレーヤコントローラが取得できない可能性あり */
	if (PlayerController == nullptr) {
		/* エディタワールドの取得 */
		World = (GEditor ? (GEditor->PlayWorld) : nullptr);

		/* ローカルプレーヤの取得 */
		ULocalPlayer* LocalPlayer = (GEngine ? (GEngine->GetDebugLocalPlayer()) : nullptr);
		if (LocalPlayer != nullptr) {
			/* ローカルプレーヤのワールドを取得 */
			UWorld* PlayerWorld = LocalPlayer->GetWorld();
			if (!World) {
				World = PlayerWorld;
			}
		}

		/* ワールドが取得できた場合はプレーヤコントローラを再取得 */
		if (World != nullptr) {
			PlayerController = UGameplayStatics::GetPlayerController(World, PlayerIndex);
		}
	}
#endif

	/* プレーヤコントローラが描画対象かどうかチェック */
	if (PlayerController != nullptr) {
		/* プレーヤコントローラがローカルプレーヤかどうかチェック */
		ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
		if (LocalPlayer == nullptr) {
			return nullptr;
		}

		/* ローカルプレーヤビューの描画が有効かどうかチェック */
		if ((LocalPlayer->Size.X <= 0.f) || (LocalPlayer->Size.Y <= 0.f)) {
			return nullptr;
		}
	}

	return PlayerController;
}

/***************************************************************************
 *      関数定義
 *      Function Definition
 ***************************************************************************/

/* --- end of file --- */