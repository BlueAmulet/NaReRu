/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Atom Device Watcher
 * File     : AtomDeviceWatcher.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* モジュールヘッダ */
#include "AtomDeviceWatcher.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareRuntimePrivatePCH.h"

/* Unreal Engine 4関連ヘッダ */
#include "Tickable.h"

#if defined(XPT_TGT_PC)
#include "Windows/WindowsHWrapper.h"
#include "Windows/AllowWindowsPlatformTypes.h"
THIRD_PARTY_INCLUDES_START
#include <cri_atom_wasapi.h>
THIRD_PARTY_INCLUDES_END
#include "Windows/HideWindowsPlatformTypes.h"
#elif defined(XPT_TGT_SWITCH)
#include <cri_atom_switch.h>
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
class FAtomDeviceWatcher : public FTickableGameObject
{
public:
	FAtomDeviceWatcher();
	~FAtomDeviceWatcher();

	// Begin FTickableObjectBase Interface
	virtual void Tick(float DeltaTime) override;
	// End FTickableObjectBase Interface

	// Begin FTickableGameObject Interface
	virtual bool IsTickableInEditor() const override;
	// End FTickableGameObject Interface

private:
	// Begin FTickableObjectBase Interface
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	// End FTickableObjectBase Interface
};

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/
namespace AtomDeviceWatcher {
	void CRIAPI OnDeviceUpdated(void* Object);
	FThreadSafeBool bIsUpdated;
};

/***************************************************************************
 *      変数定義
 *      Variable Definition
 ***************************************************************************/
FAtomDeviceWatcher* UAtomDeviceWatcher::DeviceWatcherClass = nullptr;
TArray<UAtomDeviceWatcher*> UAtomDeviceWatcher::DeviceWatcherObjects;

/***************************************************************************
 *      クラス定義
 *      Class Definition
 ***************************************************************************/
UAtomDeviceWatcher::UAtomDeviceWatcher(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UAtomDeviceWatcher::PostInitProperties()
{
	Super::PostInitProperties();

	/* デフォルトオブジェクトの場合は何もしない */
	if (HasAnyFlags(RF_ClassDefaultObject)) {
		return;
	}

	/* 初回オブジェクト作成時にFAtomDeviceWatcherを作成 */
	if (UAtomDeviceWatcher::DeviceWatcherClass == nullptr) {
		UAtomDeviceWatcher::DeviceWatcherClass = new FAtomDeviceWatcher();
	}

	/* 自身をリストに登録 */
	UAtomDeviceWatcher::DeviceWatcherObjects.Add(this);
}

void UAtomDeviceWatcher::BeginDestroy()
{
	/* リストから登録を抹消 */
	if (UAtomDeviceWatcher::DeviceWatcherObjects.Contains(this)) {
		UAtomDeviceWatcher::DeviceWatcherObjects.Remove(this);
	}

	/* 最終オブジェクト破棄時にFAtomDeviceWatcherを破棄 */
	if ((DeviceWatcherObjects.Num() == 0)
		&& (UAtomDeviceWatcher::DeviceWatcherClass != nullptr)) {
		delete UAtomDeviceWatcher::DeviceWatcherClass;
		UAtomDeviceWatcher::DeviceWatcherClass = nullptr;
	}

	Super::BeginDestroy();
}

FString UAtomDeviceWatcher::GetDeviceName()
{
#if defined(XPT_TGT_SWITCH)
	/* デバイス名の取得 */
	CriAtomDeviceName_SWITCH DeviceName;
	bool bResult = criAtom_GetDeviceName_SWITCH(&DeviceName);
	if (bResult != false) {
		return DeviceName.name;
	}
#endif

	/* 未対応の機種では常にDefaultを返す */
	return "Default";
}

int32 UAtomDeviceWatcher::GetDeviceChannelCount()
{
#if defined(XPT_TGT_SWITCH)
	return criAtom_GetDeviceChannelCount_SWITCH();
#else
	return criAtomExAsrRack_GetOutputChannels(0);
#endif
}

FAtomDeviceWatcher::FAtomDeviceWatcher()
{
	/* デバイス変更通知コールバックの登録 */
#if defined(XPT_TGT_PC)
	criAtom_SetDeviceUpdateCallback_WASAPI(AtomDeviceWatcher::OnDeviceUpdated, this);
#elif defined(XPT_TGT_SWITCH)
	criAtom_SetDeviceUpdateCallback_SWITCH(AtomDeviceWatcher::OnDeviceUpdated, this);
#endif
}

FAtomDeviceWatcher::~FAtomDeviceWatcher()
{
}

void FAtomDeviceWatcher::Tick(float DeltaTime)
{
	/* 初期化済みかどうかチェック */
	if (criAtomEx_IsInitialized() == CRI_FALSE) {
		return;
	}

	/* デバイスの更新チェック */
	bool bResult = AtomDeviceWatcher::bIsUpdated.AtomicSet(false);
	if (bResult == false) {
		return;
	}

	/* 全てのオブジェクトのイベントをトリガ */
	for (UAtomDeviceWatcher* Object : UAtomDeviceWatcher::DeviceWatcherObjects) {
		/* 参照が有効かどうか念のためチェック */
		if ((Object == nullptr) || (!Object->IsValidLowLevel())) {
			continue;
		}

		/* イベントのトリガ */
		if (Object->OnDeviceUpdated.IsBound()) {
			Object->OnDeviceUpdated.Broadcast();
		}
		if (Object->OnDeviceUpdatedNative.IsBound()) {
			Object->OnDeviceUpdatedNative.Broadcast(Object);
		}
	}
}

bool FAtomDeviceWatcher::IsTickableInEditor() const
{
	return true;
}

bool FAtomDeviceWatcher::IsTickable() const
{
	return true;
}

TStatId FAtomDeviceWatcher::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FAtomDeviceWatcher, STATGROUP_Tickables);
}

void CRIAPI AtomDeviceWatcher::OnDeviceUpdated(void* Object)
{
	AtomDeviceWatcher::bIsUpdated.AtomicSet(true);
}

/* --- end of file --- */
