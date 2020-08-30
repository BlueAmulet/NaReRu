/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2014-2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Atom Statics
 * File     : AtomStatics.h
 *
 ****************************************************************************/

/* 多重定義防止 */
#pragma once

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* Unreal Engine 4関連ヘッダ */
#include "Kismet/BlueprintFunctionLibrary.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareApi.h"
#include "SoundAtomCue.h"
#include "AtomComponent.h"
#include "AtomComponentPool.h"
#include "SoundAtomConfig.h"

/* モジュールヘッダ */
#include "AtomStatics.generated.h"

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
UCLASS(meta=(ToolTip = "AtomComponent class."))
class CRIWARERUNTIME_API UAtomStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UAtomStatics();

	/**
	 * 指定された位置に音源を配置し、キューを再生します。
	 * ループ音を再生すると停止できないため、ワンショット音にのみ使用してください。
	 * ループ音の再生には SpawnSoundAtLocation 関数を使用してください。
	 * 本関数で配置した音源はどのActorにも追従しません。
	 * UE4標準オーディオのUGameplayStatics::PlaySoundAtLocation互換のAPIです。
	 * （再生する音声がAtomCueアセットである点が異なります。）
	 * 各引数の意味はUGamePlayStatics::SpawnSoundAtLocation関数に従います。
	 *
	 * @param WorldContextObject WorldContextObjectを指定します。
	 * @param Sound Atomキューを指定します。
	 * @param Location 音源を配置する座標を指定します。
	 * @param Rotation 回転を指定します。
	 * @param VolumeMultiplier 音量の倍率を指定します。
	 * @param PitchMultiplier ピッチの倍率を指定します。
	 * @param StartTime ここに開始時間(秒)を指定することで、音声データを途中から再生することも可能です。
	 * @param AttenuationSettings Attenuationの設定を指定します。
	 * @param ConcurrencySettings Concurrencyの設定を指定します。
	 */
	UFUNCTION(BlueprintCallable, Category="Atom", meta=(WorldContext="WorldContextObject", AdvancedDisplay = "3", UnsafeDuringActorConstruction = "true"))
	static void PlaySoundAtLocation(UObject* WorldContextObject, class USoundAtomCue* Sound, FVector Location, FRotator Rotation = FRotator::ZeroRotator, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, class USoundAttenuation* AttenuationSettings = NULL, class USoundConcurrency* ConcurrencySettings = nullptr);

	/** PlaySoundAtLocation関数からRotator引数を省いたバージョン。 */
	static void PlaySoundAtLocation(UObject* WorldContextObject, class USoundAtomCue* Sound, FVector Location, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, class USoundAttenuation* AttenuationSettings = NULL, class USoundConcurrency* ConcurrencySettings = nullptr);

	/**
	 * UGameplayStatics::SpawnSound2D互換のAPIです。
	 * （再生する音声がAtomCueアセットである点が異なります。）
	 *
	 * @param WorldContextObject WorldContextObjectを指定します。
	 * @param Sound Atomキューを指定します。
	 * @param PitchMultiplier ピッチの倍率を指定します。
	 * @param StartTime ここに開始時間(秒)を指定することで、音声データを途中から再生することも可能です。
	 * @param bPersistAcrossLevelTransition パーシスタントレベルをまたいで生存するかどうか指定します。
	 * @param bAutoDestroy 再生が完了したとき自動的にオブジェクトを削除するかどうか指定します。
	 * @return 指定したキューを再生するAtomComponentのリファレンスが返ります。
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Atom", meta = (WorldContext = "WorldContextObject", AdvancedDisplay = "2", UnsafeDuringActorConstruction = "true", Keywords = "Play"))
	static class UAtomComponent* SpawnSound2D(const UObject* WorldContextObject, USoundAtomCue* Sound, float PitchMultiplier = 1.f, float StartTime = 0.f, bool bPersistAcrossLevelTransition = false, bool bAutoDestroy = true);

	/**
	 * レベルがアンロードされても破棄されないAtomComponentを作成します。
	 * AISACコントロール値の指定など、発音開始前に設定が必要な音声をレベルをまたいで再生したい場合に使用します。
	 * （UAtomStatics::SpawnSound2Dとの違いは、コンポーネントの作成と同時に自動で発音を開始しない点のみです。）
	 *
	 * @param WorldContextObject WorldContextObjectを指定します。
	 * @param bAutoDestroy 再生が完了したとき自動的にオブジェクトを削除するかどうか指定します。
	 * @return 指定したキューを再生するAtomComponentのリファレンスが返ります。
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Atom", meta = (WorldContext = "WorldContextObject", AdvancedDisplay = "2", UnsafeDuringActorConstruction = "true", Keywords = "Create"))
	static class UAtomComponent* CreateRootedAtomComponent(const UObject* WorldContextObject, bool bAutoDestroy = true);

	/**
	 * 指定された位置に音源を配置し、キューを再生します。
	 * 再生中のAtomComponentを返すため、ループ音を再生しても停止することが可能です。
	 * 本関数で配置した音源はどのActorにも追従しません。
	 * UE4標準オーディオのUGameplayStatics::SpawnSoundAtLocation互換のAPIです。
	 * （再生する音声がAtomCueアセットである点が異なります。）
	 * 各引数の意味はUGamePlayStatics::SpawnSoundAtLocation関数に従います。
	 *
	 * @param WorldContextObject WorldContextObjectを指定します。
	 * @param Sound Atomキューを指定します。
	 * @param Location 音源を配置する座標を指定します。
	 * @param Rotation 回転を指定します。
	 * @param VolumeMultiplier 音量の倍率を指定します。
	 * @param PitchMultiplier ピッチの倍率を指定します。
	 * @param StartTime ここに開始時間(秒)を指定することで、音声データを途中から再生することも可能です。
	 * @param AttenuationSettings Attenuationの設定を指定します。
	 * @param ConcurrencySettings Concurrencyの設定を指定します。
	 * @param bAutoDestroy 再生が完了したとき自動的にオブジェクトを削除するかどうか指定します。
	 * @return 指定したキューを再生するAtomComponentのリファレンスが返ります。
	 */
	UFUNCTION(BlueprintCallable, Category="Atom", meta=(WorldContext="WorldContextObject", AdvancedDisplay = "3", UnsafeDuringActorConstruction = "true", Keywords = "play"))
	static class UAtomComponent* SpawnSoundAtLocation(const UObject* WorldContextObject, class USoundAtomCue* Sound, FVector Location, FRotator Rotation = FRotator::ZeroRotator, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, class USoundAttenuation* AttenuationSettings = NULL, class USoundConcurrency* ConcurrencySettings = nullptr, bool bAutoDestroy = true);

	/**
	 * 指定されたコンポーネントに追従する音源を配置し、キューを再生します。
	 * UE4標準オーディオのUGameplayStatics::SpawnSoundAttached互換のAPIです。
	 * （再生する音声がAtomCueアセットである点が異なります。）
	 * 各引数の意味はUGamePlayStatics::SpawnSoundAttached関数に従います。
	 *
	 * @param Sound Atomキューを指定します。
	 * @param AttachToComponent ここで指定したコンポーネントに追従します。
	 * @param AttachPointName アタッチするボーンの名前を指定します。
	 * @param Location 音源を配置する座標を指定します。
	 * @param Rotation 回転を指定します。
	 * @param LocationType ロケーションタイプを指定します。KeepWorldPosition又はKeepRelativeOffsetのみ指定できます。
	 * @param bStopWhenAttachedToDestroyed 追従しているコンポーネントが破棄されたとき音声を停止するかどうか指定します。
	 * @param VolumeMultiplier 音量の倍率を指定します。
	 * @param PitchMultiplier ピッチの倍率を指定します。
	 * @param StartTime ここに開始時間(秒)を指定することで、音声データを途中から再生することも可能です。
	 * @param AttenuationSettings Attenuationの設定を指定します。
	 * @param ConcurrencySettings 利用できません（内部で無視されます）
	 * @param bAutoDestroy 再生が完了したとき自動的にオブジェクトを削除するかどうか指定します。
	 * @return 指定したキューを再生するAtomComponentのリファレンスが返ります。
	 */
	UFUNCTION(BlueprintCallable, Category="Atom", meta=(AdvancedDisplay = "2", UnsafeDuringActorConstruction = "true"))
	static class UAtomComponent* SpawnSoundAttached(class USoundAtomCue* Sound, class USceneComponent* AttachToComponent, FName AttachPointName = NAME_None, FVector Location = FVector(ForceInit), FRotator Rotation = FRotator::ZeroRotator, EAttachLocation::Type LocationType = EAttachLocation::KeepRelativeOffset, bool bStopWhenAttachedToDestroyed = false, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, class USoundAttenuation* AttenuationSettings = NULL, class USoundConcurrency* ConcurrencySettings = nullptr, bool bAutoDestroy = true);

	/** SpawnSoundAttached関数からRotator引数を省いたバージョン。 */
	static class UAtomComponent* SpawnSoundAttached(class USoundAtomCue* Sound, class USceneComponent* AttachToComponent, FName AttachPointName = NAME_None, FVector Location = FVector(ForceInit), EAttachLocation::Type LocationType = EAttachLocation::KeepRelativeOffset, bool bStopWhenAttachedToDestroyed = false, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, class USoundAttenuation* AttenuationSettings = NULL, class USoundConcurrency* ConcurrencySettings = nullptr, bool bAutoDestroy = true);

	/**
	 * 指定されたコンポーネントに追従する音源を配置し、キューを再生します。
	 * 各引数の意味はUGamePlayStatics::SpawnSoundAttached関数に従います。
	 * ※本関数は将来のバージョンで削除される予定です。SpawnSoundAttached関数へ移行をお願いいたします。
	 */
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 22
	UE_DEPRECATED(4.23, "UAtomStatics::PlaySoundAttached has been deprecated. This function will be removed in UE4.25. Please use UAtomStatics::SpawnSoundAttached instead.")
#endif
	UFUNCTION(BlueprintCallable, Category="Atom", meta=(DeprecatedFunction, DeprecationMessage = "This function will be removed in UE4.25. Please use UAtomStatics::SpawnSoundAttached function instead.", AdvancedDisplay = "2", UnsafeDuringActorConstruction = "true"))
	static class UAtomComponent* PlaySoundAttached(class USoundAtomCue* Sound, class USceneComponent* AttachToComponent, FName AttachPointName = NAME_None, FVector Location = FVector(ForceInit), bool bStopWhenAttachedToDestroyed = false, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, class USoundAttenuation* AttenuationSettings = NULL);

	/**
	 * 3Dアニメーションに音を付けます。
	 *
	 * @param Sound Atomキューを指定します。
	 * @param AttachToComponent ここで指定したコンポーネントに追従します。
	 * @param AttachPointName アタッチするボーンの名前を指定します。
	 * @param Location 音源を配置する座標を指定します。
	 * @param bStopWhenAttachedToDestroyed 追従しているコンポーネントが破棄されたとき音声を停止するかどうか指定します。
	 * @param VolumeMultiplier 音量の倍率を指定します。
	 * @param PitchMultiplier ピッチの倍率を指定します。
	 * @param StartTime ここに開始時間(秒)を指定することで、音声データを途中から再生することも可能です。
	 * @param AttenuationSettings Attenuationの設定を指定します。
	 * @param bFollow trueを指定するとアニメーションに応じて音源が移動します。falseを指定すると音源は発音位置にとどまります。
	 * @return 指定したキューを再生するAtomComponentのリファレンスが返ります。
	 */
	UFUNCTION(BlueprintCallable, Category = "Atom", meta = (AdvancedDisplay = "2", UnsafeDuringActorConstruction = "true"))
	static class UAtomComponent* PlaySoundForAnimNotify(class USoundAtomCue* Sound, class USceneComponent* AttachToComponent, FName AttachPointName = NAME_None, FVector Location = FVector(ForceInit), bool bStopWhenAttachedToDestroyed = false, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, class USoundAttenuation* AttenuationSettings = NULL, bool bFollow = false);

	/**
	 * ゲーム中の音声全て一時停止、もしくは再開します。
	 * ポーズ中にレベルを移動した場合、レベル移動時にポーズが解除されます。 
	 *
	 * @param bPause ここにtrueを指定すると一時停止します。falseを指定すると、再生を再開します。
	 */
	UFUNCTION(BlueprintCallable, Category="Atom")
	static void PauseAudioOutput(bool bPause);

	/**
	 * ゲーム中の音声を全て停止します。
	 */
	UFUNCTION(BlueprintCallable, Category="Atom")
	static void StopAllSounds();

	/**
	 * Atom Configアセットをロードします。
	 * 既にAtom Configアセットがロード済みの場合、既存の設定をアンロードし、 [Acf Object] で指定されたものに変更します。
	 *
	 * @param AcfObject ロードしたい Atom Config アセットを指定します。
	 * @return 成功/失敗が返ります。
	 */
	UFUNCTION(BlueprintCallable, Category = "Atom", meta = (UnsafeDuringActorConstruction = "true"))
	static bool LoadAtomConfig(USoundAtomConfig* AcfObject);

	/* C++ API */
	static UAtomComponent* SpawnSoundAtLocation(const UObject* WorldContextObject, USoundAtomCueSheet* CueSheet, FString CueName, FVector Location, FRotator Rotation = FRotator::ZeroRotator, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, class USoundAttenuation* AttenuationSettings = NULL, class USoundConcurrency* ConcurrencySettings = nullptr, bool bAutoDestroy = true);
	static UAtomComponent* SpawnSoundAttached(USoundAtomCueSheet* CueSheet, FString CueName, USceneComponent* AttachToComponent, FName AttachPointName = NAME_None, FVector Location = FVector(ForceInit), FRotator Rotation = FRotator::ZeroRotator, EAttachLocation::Type LocationType = EAttachLocation::KeepRelativeOffset, bool bStopWhenAttachedToDestroyed = false, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, USoundAttenuation* AttenuationSettings = nullptr, USoundConcurrency* ConcurrencySettings = nullptr, bool bAutoDestroy = true);

	/**
	 * 距離減衰のかかり具合を再生中の全 AtomComponent と Listener について一括更新します。
	 * ※本関数は実験的な機能を提供するものであり、将来のバージョンで削除対象となっています。
	 */
	UFUNCTION(BlueprintCallable, Category = "Atom Experimental", meta = (ToolTip = "Update Distance Factor."))
	static void UpdateDistanceFactorForAllSounds(float ArgDistanceFactor);

	/**
	 * AtomPlayGate で再生された AtomComponent の ID を、AtomPlayGate と MeshComponent の識別子に紐づけて辞書に追加します。
	 * この関数は AnimNotifyState_AtomPlayGate ブループリント内でのみ使用することを想定しています。
	 * CRIWARE UE4 Plugin のユーザが本関数を呼び出すことは不正動作の原因となるため推奨しません
	 */
	UFUNCTION(BlueprintCallable, Category = "Atom")
		static void AddToAtomComponentIds(UObject* AtomPlayGateRef, UObject* SkeltalMeshComponentRef, int32 AtomComponentId);

	/**
	 * AtomPlayGate と MeshComponent に紐付いた全ての AtomComponent を停止します。
	 * この関数は AnimNotifyState_AtomPlayGate ブループリント内でのみ使用することを想定しています。
	 * CRIWARE UE4 Plugin のユーザが本関数を呼び出すことは不正動作の原因となるため推奨しません
	 */
	UFUNCTION(BlueprintCallable, Category = "Atom")
	static void StopAllSoundsForKeyFromAtomComponentIds(UObject* AtomPlayGateRef, UObject* SkeltalMeshComponentRef);

	/**
	* カリングすべきかどうか判定します。
	* @param Sound キューアセットを指定します。
	* @param Location 音源の位置を指定します。
	* @param AttenuationSettings UE4Attenuation設定を指定します。(nullptr可)
	* @param Is3DSound 3Dサウンドなのかどうか指定します。
	* @return true:カリングすべき, false:カリングすべきではない
	*/
	static bool ShouldCullSoundPlayback(USoundAtomCue* Sound, FVector Location, const FSoundAttenuationSettings* AttenuationSettings, bool Is3DSound);

	/* 現在のDistanceFactorを取得する */
	static float GetCurrentDistanceFactor();

	/* [プラグイン開発者専用]現在のDistanceFactorを設定する */
	static void SetCurrentDistanceFactor(float InDistanceFactor);

private:
	static UAtomComponent* AllocateComponent();
	static void ResumeAudioOutput(void* Object);
	/* AtomPlayGate から再生された AtomComponent の ID を管理するための辞書
	 * AtomPlayGateインスタンスとMeshComponentインスタンスのUniqueIDのペアをキーとして、AtomComponent ID を保持  */
	static TMap<TPair<int32, int32>, int32> DictForAtomComponentIdsAtomPlayGateCreated;
	static float CurrentDistanceFactor;

	/**
	 * 再生すべき音源の絶対座標を取得します。
	 * PlaySoundForAnimNotify関数内で使用します。
	 *
	 * @param AttachToComponent アタッチ先のコンポーネント
	 * @param AttachPointNameアタッチ先のボーンの名前
	 * @param Location 座標
	 * @param LocationType 座標指定タイプ
	 * @param AbsLocation [out]計算結果がここに格納されます
	 */
	static void CalculateAbsLocationToPlaySounds(USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, EAttachLocation::Type LocationType, FVector & AbsLocation);
};

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/

/* --- end of file --- */
