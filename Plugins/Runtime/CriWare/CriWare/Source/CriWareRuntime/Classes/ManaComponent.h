/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2015-2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Mana Playback Control Component
 * File     : ManaComponent.h
 *
 ****************************************************************************/

/* 多重定義防止 */
#pragma once

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
#include <utility>
#include "ManaMovie.h"
#include "ManaPlaylist.h"
#include "ManaTexture.h"
#include "AtomComponent.h"
#include "Components/ActorComponent.h"
#include "ManaComponent.generated.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/

// (Debug) Timers only based on time from sofdec2 - not re-syncronized with frame rendering.
// - may result to out-of-sync event points and timers.
#define CRIWARE_MANA_PLAYBACKTIMER_MANATIME_DEBUG (0)
// (Debug) Frame dropping detection based on Unreal game time - not with sofdec2 time.
// - may result to playback delay with 'fixed framerate' in project settings.
#define CRIWARE_MANA_FRAMEDROP_GAMETIME_DEBUG (0)

/***************************************************************************
 *      処理マクロ
 *      Macro Functions
 ***************************************************************************/

/***************************************************************************
 *      データ型宣言
 *      Data Type Declarations
 ***************************************************************************/
UENUM(BlueprintType)
enum class EManaComponentStatus : uint8
{
	Stop,				/** 停止中 */
	DecHdr,				/** ヘッダ解析中 */
	WaitPrep,			/** バッファリング開始停止中、および描画リソース準備中 */
	Prep,				/** 再生準備中 */
	Ready,				/** 再生準備完了 */
	Playing,			/** 再生中 */
	PlayEnd,			/** 再生終了 */
	Error,				/** エラー */
	StopProcessing		/** 停止処理中 */
};

UENUM(BlueprintType)
enum class EManaSubtitlesEncoding : uint8
{
	/** Default encoding is utf-8. */
	Default		UMETA(DisplayName = "Default"),
	/** ANSI - System encoded. */
	ANSI		UMETA(DisplayName = "ANSI"),
	/** Unicode utf-8 encoded. */
	Utf8		UMETA(DisplayName = "utf-8"),
	/** Unicode utf-16 encoded. */
	Utf16		UMETA(DisplayName = "utf-16")
};

UENUM(BlueprintType)
enum class EManaFrameAction : uint8
{
	/** Default action is Hold. */
	Default		UMETA(DisplayName = "Default"),
	/** Hold last frame. */
	Hold		UMETA(DisplayName = "Hold"),
	/** Clear video texture to its ClearColor (default = black). */
	Clear		UMETA(DisplayName = "Clear"),
};

UENUM(BlueprintType)
enum class EManaPlaybackTimer : uint8
{
	/** Default timer is Real Time. */
	Default		UMETA(DisplayName = "Default"),
	/** Real Time timer: playback time advances in real time and synchronized with video speed. */
	RealTime	UMETA(DisplayName = "Real Time"),
	/** Frame Time timer: playback time advances when a new video frame has been decoded. */
	FrameTime	UMETA(DisplayName = "Frame Time"),
	/** Manual timer: playback time advances with time given manually with AdvanceTime() function. */
	Manual		UMETA(DisplayName = "Manual"),
//	/** Mana Time timer:  playback time advances in real time form Mana native libray but never re-synchronized, may result in bad time (Debug)*/
//	ManaTime	UMETA(DisplayName = "Mana Time (Debug)"),
};

UENUM(BlueprintType)
enum class EManaMaxFrameDrop : uint8
{
	/** Disable frame dropping. */
	Disabled	UMETA(DisplayName = "Disabled"),
	/** A maximum of 1 frame can be dropped. */
	One			UMETA(DisplayName = "1"),
	/** A maximum of 2 successive frames can be dropped. */
	Two			UMETA(DisplayName = "2 (Default)"),
	/** A maximum of 3 successive frames can be dropped. */
	Three		UMETA(DisplayName = "3"),
	/** A maximum of 4 successive frames can be dropped. */
	For			UMETA(DisplayName = "4"),
	/** A maximum of 5 successive frames can be dropped. */
	Five		UMETA(DisplayName = "5"),
	/** A maximum of 6 successive frames can be dropped. */
	Six			UMETA(DisplayName = "6"),
	/** A maximum of 7 successive frames can be dropped. */
	Seven		UMETA(DisplayName = "7"),
	/** A maximum of 8 successive frames can be dropped. */
	Eight		UMETA(DisplayName = "8"),
	/** A maximum of 9 successive frames can be dropped. */
	Nine		UMETA(DisplayName = "9"),
	/** A maximum of 10 successive frames can be dropped. */
	Ten			UMETA(DisplayName = "10"),
	/** All frames are dropped until playback is re-synchronized. */
	Infinite	UMETA(DisplayName = "Infinite")
};

/***************************************************************************
 *      変数宣言
 *      Prototype Variables
 ***************************************************************************/

/***************************************************************************
 *      クラス宣言
 *      Prototype Classes
 ***************************************************************************/

/** Class to hold ManaPlayer alive until cleanup time. 
 * NOTE: ManaPlayer is used by the rendering thread. When the game thread attempts to destroy it, 
 * FDeferredCleanupInterface will keep the object around until the ManaTextureResource is done accessing it.
 */
class FManaPlayerDeferredCleanup : public FDeferredCleanupInterface
{
public:
	FManaPlayerDeferredCleanup(CriManaPlayerHn InManaPlayer) : ManaPlayer(InManaPlayer) {}

	virtual ~FManaPlayerDeferredCleanup();

private:
	// ManaPlayer
	CriManaPlayerHn ManaPlayer;
};

/** CriManaPlayerHnへのBlueprint/C++ APIを提供するコンポーネント */
UCLASS(ClassGroup = Common, BlueprintType, meta = (BlueprintSpawnableComponent, ToolTip = "ManaComponent class."))
class CRIWARERUNTIME_API UManaComponent : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEventPoint, FManaEventPointInfo, EventPointInfo);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSubtitleChanged, FText, Subtitle);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatusChanged, EManaComponentStatus, Status, UManaComponent*, ManaComponent);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMovieChanged, UManaMovie*, Movie, UManaComponent*, ManaComponent);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSeekCompleted, int32, FrameNumber, UManaComponent*, ManaComponent);

	// TEMP
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnRequestData, UManaComponent*, ManaComponent);

public:
	// Friends.
	friend class FManaPlayerMovieStreamer;

	UManaComponent(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	// Playback

	/**
	 * ムービの再生準備をします。
	 * ムービの再生は開始せず、ヘッダ解析と再生準備だけ行って待機するための関数です。
	 * この関数を使用して再生準備を事前に済ませることにより、ムービ再生開始の
	 * タイミングを細かく制御することができます。
	 * （再生準備をせずにPlay関数を呼び出した場合、実際の再生開始までタイムラグが発生します。）
	 *
	 * このメンバ関数を呼び出す場合は、状態が EManaComponentStatus::Stop である必要があります。
	 *
	 * 本メンバ関数を呼び出し後、状態が EManaComponentStatus::Ready のときに
	 * Play 関数を呼ぶことで即座に再生を開始することができます。
	 */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Prepare movie for play."))
	void Prepare();

	/**
	 * ムービの再生準備をします。
	 * ムービの再生は開始せず、ヘッダ解析と再生準備だけ行って待機するための関数です。
	 * この関数を使用して再生準備を事前に済ませることにより、ムービ再生開始の
	 * タイミングを細かく制御することができます。
	 * （再生準備をせずにPlay関数を呼び出した場合、実際の再生開始までタイムラグが発生します。）
	 *
	 * このメンバ関数を呼び出す場合は、状態が EManaComponentStatus::Stop である必要があります。
	 *
	 * 本メンバ関数を呼び出し後、状態が EManaComponentStatus::Ready のときに
	 * Play 関数を呼ぶことで即座に再生を開始することができます。
	 */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Prepare movie for play fron index in play list."))
	void PreparePlaylistIndex(int32 Index);

	/**
	 * ムービの再生に必要なリソースを割り当て、再生開始します。
	 * Prepare の呼び出し無しでこのメンバ関数を呼び出す場合は、
	 * 状態が EManaComponentStatus::Stop である必要があります。
	 * Prepare 関数を呼ばずに本メンバ関数を呼び出した場合は、ムービの解析と再生準備を行うため、
	 * 実際にムービの再生が始まるまでにタイムラグが発生します。
	 * Prepare 関数を先に呼び出して、状態が EManaComponentStatus::Ready になっていれば、
	 * 本メンバ関数を呼び出してすぐに再生が始まります。
	 * 注意:本関数を実行する前に、あらかじめManaComponentに対し再生するムービのテクスチャを指定しておく必要があります。
	 */
	UFUNCTION(BlueprintCallable, Category = "Mana")
	void Play();

	/**
	 * ムービ再生のポーズ切り替えを行います。
	 *
	 * @param bPause 引数にtrueを指定すると動画が一時停止します。<br>引数にfalseを指定すると動画の一時停止が解除されます。
	 */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Pause/unpause playback."))
	void Pause(bool bPause = true);

	/** ループ再生するかどうか (true: ループする false: ループしない)
	 *  default: false (ループしない)
	 *  暫定：再生中のループ変更は未対応
	 */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Enable looping of movie playback.", ScriptName = "LoopMethod"))
	void Loop(bool bInLoop = true);

	/**
	 * Checks whether playback is looping.
	 *
	 * @return true if looping, false otherwise.
	 * @see SetLooping
	 */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Is playback looping?"))
	bool IsLooping() const { return bLoop; }

	/**
	 * Checks whether playback is currently paused.
	 *
	 * @return true if playback is paused, false otherwise.
	 * @see CanPause, IsPlaying, IsReady, Pause
	 */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Is playback paused?"))
	bool IsPaused() const;

	/**
	 * Checks whether playback has started.
	 *
	 * @return true if playback has started, false otherwise.
	 * @see CanPlay, IsPaused, IsReady, Play
	 */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Is playback playing?"))
	bool IsPlaying() const;

	/**
	 * Checks whether the media is currently opening or buffering.
	 *
	 * @return true if playback is being prepared, false otherwise.
	 * @see CanPlay, IsPaused, IsReady, Play
	 */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Is playback opening or buffering?"))
	bool IsPreparing() const;

	/**
	 * Checks whether media is ready for playback.
	 *
	 * A player is ready for playback if it has a media source opened that
	 * finished preparing and is not in an error state.
	 *
	 * @return true if media is ready, false otherwise.
	 * @see IsPaused, IsPlaying, Stop
	 */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Is ready for playback?"))
	bool IsReady() const;

	/**
	 * ムービの再生を停止する要求を出します。（即時復帰）
	 * 本メンバ関数内で全ての停止処理が実行されるわけではありません。
	 * 本メンバ関数の実行後、状態が EManaComponentStatus::StopProcessing に遷移した場合は、
	 * 状態が EManaComponentStatus::Stop に自動遷移したことを確認後、次の再生制御を行ってください。
	 * また、状態が EManaComponentStatus::Error になった場合は、本メンバ関数を呼び出して
	 * EManaComponentStatus::Stop に遷移するのを待ってください。
	 */
	UFUNCTION(BlueprintCallable, Category = "Mana")
	void Stop();

	/**
	 * ムービのシーク位置を、ムービのフレーム番号で指定します。
	 * 例えば「100」を指定すると、ムービの先頭フレームから数えて 100 フレーム目に再生開始位置を移動できます。
	 * ムービ再生中に本関数を呼ぶと、指定のシーク位置から再生が続行されます。
	 */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Seek to a frame number."))
	void SeekToPosition(int32 FrameNumber);

	/**
	 * ムービのシーク位置を FTimespan で指定します。
	 * 例えば「10 秒」を指定すると、ムービの先頭時刻から数えて 10 秒時点に再生開始位置を移動できます。
	 * ムービ再生中に本関数を呼ぶと、指定のシーク位置から再生が続行されます。
	 */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Seek to a time."))
	void Seek(FTimespan Time);

	// Audio

	/**
	 * ムービのメインオーディオトラックの出力音声ボリュームを変更します。
	 * 
	 * @param Volume 0.0f 〜 1.0f の範囲でボリューム値を指定します。ボリューム値は音声データの振幅に対する倍率です（単位はデシベルではありません）。
	 */
	UFUNCTION(BlueprintCallable, Category = "Mana")
	void SetVolume(float Volume);

	/**
	 * ムービーのメインオーディオトラックの出力音声ボリュームを取得します。
	 *
	 * @return ボリュームが返ります。値域は 0.0 - 1.0 です。 
	 */
	UFUNCTION(BlueprintCallable, Category = "Mana")
	float GetVolume() const;

	/** Set main audio track index. */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Set main audio track index."))
	void SetAudioTrack(int32 Track);
	
	/** Set sub audio track index. */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Set sub audio track index."))
	void SetSubAudioTrack(int32 Track);
	
	/** Set sub audio send level. */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Set sub audio send level."))
	void SetSubAudioSendLevel(int32 Channel, EAtomSpeakerID SpeakerId, float Volume);

	/** Set extra audio track index. */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Set extra audio track index."))
	void SetExtraAudioTrack(int32 Track);

	/**
	 * Get the current main audio track index.
	 *
	 * @return The audio track index.
	 */
	FORCEINLINE int32 GetAudioTrack() const { return AudioTrack; }

	/**
	 * Get the current sub audio track index.
	 *
	 * @return The audio track index.
	 */
	FORCEINLINE int32 GetSubAudioTrack() const { return SubAudioTrack; }
	
	/**
	 * Get the current extra audio track index.
	 *
	 * @return The audio track index.
	 */
	FORCEINLINE int32 GetExtraAudioTrack() const { return ExtraAudioTrack; }

	/** Set volume of sub audio track. */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Set volume of sub audio track."))
	void SetSubVolume(float Volume);
	
	/** Set volume of extra audio track. */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Set volume of extra audio track."))
	void SetExtraVolume(float Volume);

	/** Get current volume of sub audio track. */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Get current volume of sub audio track."))
	float GetSubVolume() const;
	
	/** Get current volume of extra audio track. */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Get current volume of extra audio track."))
	float GetExtraVolume() const;

	/**
	 * Get the current Atom category assigned to main audio track.
	 *
	 * @return The Atom category.
	 */
	FORCEINLINE FString GetAudioCategory() const { return AudioCategoryName; }

	/**
	 * Get the current Atom category assigned to sub audio track.
	 *
	 * @return The Atom category.
	 */
	FORCEINLINE FString GetSubAudioCategory() const { return SubAudioCategoryName; }

	/**
	 * Get the current Atom category assigned to extra audio track.
	 *
	 * @return The Atom category.
	 */
	FORCEINLINE FString GetExtraAudioCategory() const { return ExtraAudioCategoryName; }

	/** Assign Atom category to main audio track. */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Assign Atom category to main audio track."))
	void SetAudioCategory(const FString& CategoryName);

	/** Assign Atom category to main audio track. */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Assign Atom category to sub audio track."))
	void SetSubAudioCategory(const FString& CategoryName);

	/** Assign Atom category to main audio track. */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Assign Atom category to extra audio track."))
	void SetExtraAudioCategory(const FString& CategoryName);

	/** Remove Atom category assigned to main audio track. */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Remove Atom category assigned to main audio track."))
	void RemoveAudioCategory();

	/** Remove Atom category assigned to sub audio track. */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Remove Atom category assigned to sub audio track."))
	void RemoveSubAudioCategory();

	/** Remove Atom category assigned to extra audio track. */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Remove Atom audio category assigned to extra audio track."))
	void RemoveExtraAudioCategory();

	// subtitles

	/** Enable or disable subtitles. */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Enable or disable subtitles."))
	void EnableSubtitles(bool bEnable);

	/** Select a subtitles channel. */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Select a subtitles channel."))
	void ChangeSubtitlesChannel(int32 Channel);

	/** Select a subtitles encoding. */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Select a subtitles encoding."))
	void ChangeSubtitlesEncoding(EManaSubtitlesEncoding Encoding);

	/**
	 * ムービーの再生時刻(秒)を取得します。
	 *
	 * @return ムービーの再生時刻(秒)が返ります。
	 */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Get playback time."))
	FTimespan GetTime() const;

	/**
	 * ムービーの再生フレームを取得します。
	 *
	 * @return ムービーの再生フレームが返ります。<br>ムービーが再生されていない場合は-1が返ります。
	 */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Get playback frame number."))
	int32 GetFrameNumber() const;

	/** Get movie Playbacktime time (no loop, no seamless) */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Get current movie playback time."))
	FTimespan GetMovieTime() const;

	/** 再生位置（ループを含めない）の取得 */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Get current movie playback frame number."))
	int32 GetPosition() const;

	/** Set playback speed rate. */
	//UFUNCTION(BlueprintCallable, Category="Mana", meta = (ToolTip = "Set playback speed rate."))
	//void SetRate(float Speed);

	/** Get playback speed rate。 */
	//UFUNCTION(BlueprintCallable, Category="Mana", meta = (ToolTip = "Get playback speed rate."))
	//float GetRate();

	/**
	 * ムービの再生状態を取得します。
	 * 返り値はEManaComponentStatus列挙型で、以下の値を取ります。
	 * ・Stop : 停止中
	 * ・DecHdr : ヘッダ解析中
	 * ・WaitPrep : バッファリング開始停止中、および描画リソース準備中
	 * ・Prep : 再生準備中
	 * ・Ready : 再生準備完了
	 * ・Playing : 再生中
	 * ・PlayEnd : 再生終了
	 * ・Error : エラー
	 * ・StopProcessing : 停止処理中
	 * 
	 * @return ムービの再生状態が返ります。
	 */
	UFUNCTION(BlueprintCallable, Category = "Mana")
	EManaComponentStatus GetStatus() const;

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Get Mana movie source."))
	UManaMovie* GetSource() const;

	/** 再生するムービを指定します。 */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Set Mana movie texture."))
	void SetTexture(UManaTexture* Texture);

	/** ムービフレームをテクスチャとして取得します。 */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Get Mana movie texture."))
	UManaTexture* GetTexture() const;

	// cached data

	/** メモリにロード済みのムービデータを指定します。 */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Set Mana movie data to stream."))
	void SetCachedData(const TArray<uint8>& DataArray);

	/** キャッシュの参照を停止 */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Reset and disable usage of Mana movie data."))
	void ResetCachedData();

	// native handle

	/** Manaプレーヤハンドルの取得 */
	CriManaPlayerHn GetManaPlayer();

	// deprecated

	/** 再生するムービを指定します。
	 *  再生準備や再生開始した後は、再生終了または再生停止するまでムービの変更は反映されません。 */
	UPROPERTY(Category = Rendering, EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Movie Texture", ToolTip = "Mana Movie texture."))
	UManaTexture* Movie;

	// others tracks (events, subtitiles ...)

	/** イベントポイント到達時に呼び出されるデリゲート */
	UPROPERTY(BlueprintAssignable, Category = "Mana")
	FOnEventPoint OnEventPoint;

	/** Event for subtitles. */
	UPROPERTY(BlueprintAssignable, Category = "Mana")
	FOnSubtitleChanged OnSubtitleChanged;

	/** Event for status. */
	UPROPERTY(BlueprintAssignable, Category = "Mana")
	FOnStatusChanged OnStatusChanged;

	/** Event for movies. */
	UPROPERTY(BlueprintAssignable, Category = "Mana")
	FOnMovieChanged OnMovieChanged;

	/** Event for seeking. */
	UPROPERTY(BlueprintAssignable, Category = "Mana")
	FOnSeekCompleted OnSeekCompleted;

	// component behaviour

	/** 毎Vの動画フレーム取得処理 */
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	/* コンポーネントの初期化時に再生に必要なリソースを確保する */
	virtual void InitializeComponent() override;

	/* 再生に使用したリソースを解放する */
	virtual void UninitializeComponent() override;

	// Playlist control

	/** Stops and jumps to next movie in playlist. */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Stops and jumps to next movie in playlist."))
	bool Next();

	/** Stops and jumps to previous movie in playlist. */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Stops and jumps to previous movie in playlist."))
	bool Previous();

	/** Stops and jumps to desired index in playlist. */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Stops and jumps to movie at desired index in playlist."))
	bool ToIndex(int32 Index);

	/**
	 * Get the currently active play list.
	 *
	 * @return The play list, if any.
	 */
	FORCEINLINE UManaPlaylist* GetPlaylist() const { return Playlist; }

	/**
	 * Get the current play list index.
	 *
	 * @return Play list index.
	 */
	FORCEINLINE int32 GetPlaylistIndex() const { return PlaylistIndex; }

	/**
	* Get the current subtitles channel index.
	*
	* @return Play list index.
	*/
	FORCEINLINE int32 GetSubtitlesChannel() const { return SubtitlesChannel; }

	/**
	* Get the current subtitles channel encoding.
	*
	* @return Play list index.
	*/
	FORCEINLINE EManaSubtitlesEncoding GetSubtitlesEncoding() const { return SubtitlesEncoding; }

	/**
	 * Get the current playback timer.
	 *
	 * @return Play list index.
	 */
	EManaPlaybackTimer GetPlaybackTimer();

	/**
	 * Set the current playback timer.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Set the current playback timer."))
	void SetPlaybackTimer(EManaPlaybackTimer InPlaybackTimer);

	/**
	 * 指定した時間の分だけムービの再生時刻を進めます。
	 * この関数を呼び出す ManaComponent オブジェクトは、PlaybackTimer が Manual に設定されている必要があります。
	 * 例えば DeltaTime で 16 ミリ秒を指定すると、再生状態のムービは 16 ミリ秒分だけ時刻が進みます。
	 * ユーザアプリケーション側で独自管理するゲーム内時刻に同期してムービ再生を行いたい場合、この関数を使います。
	 */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Advance playback time of movie when playback timer is set to Manual."))
	void AdvanceTime(FTimespan DeltaTime);

	/**
	 * Get the maximum of droppable frames when movie is out-of-sync.
	 *
	 * @return Play list index.
	 */
	FORCEINLINE EManaMaxFrameDrop GetMaxDropFrames() const { return MaxFrameDrop; }

	/**
	 * Set the current playback timer.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Set the maximum of droppable frames when movie is out-of-sync."))
	void SetMaxDropFrames(EManaMaxFrameDrop InMaxDropFrames);

	// SEAMLESS

	/** TEMP Enable or disable seamless. */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Enable or disable seamless playback."))
	void EnableSeamless(bool bEnable);

	/** TEMP Callback for seamless */
	UPROPERTY(BlueprintReadWrite, Category = "Mana")
	FOnRequestData OnRequestData;

	/** TEMP Set file directly to Mana Player - Use this with attention, if ManaTexture is not set with same video information it may result to a crash */
	UFUNCTION(BlueprintCallable, Category = "Mana", meta = (ToolTip = "Force set file (for seamless)."))
	void SetFile(const FString &MovieFilePath);

protected:
	/* Init */
	void InitializeManaPlayer();

	/* 描画リソースへムービフレームを提供する */
	void UpdateFrame();

	/* サブタイトルの文字列を更新する */
	void UpdateSubtitle();

	/* 再生の時間を更新し、エベントポイントを確認する */
	void UpdatePlaybackTime();

	/* ManaPlayerの状態を更新する */
	void UpdateManaPlayer();

	/* 再生準備をする */
	bool PrepareManaPlayer(int32 Index = 0);

	/* Allocate buffer and collect meta data for seek. */
	void StartCollectMetaData();

	/* Change internal status and broadcast OnStatusChanged if new status is different. */
	void ChangeStatus(EManaComponentStatus NewStatus);

	/* Seamless callback */
	static void OnDataRequestCallback(void* ManaComponent, CriManaPlayerHn ManaPlayer);

	/* On Movie changed internally by Mana player. */
	void OnMovieChangedByManaPlayer();

#if WITH_EDITOR
	/* Check if the used mana texture is already used by another ManaComponent.*/
	void CheckAlreadyUsedManaTexture(UManaTexture* InTexture);
#endif

	/* ManaPlayer */
	CriManaPlayerHn ManaPlayer;

	/* ManaComponentの再生状態を保持する */
	EManaComponentStatus Status;

	/* ManaComponentの再生状態を保持する */
	EManaComponentStatus PreviousStatus;

	/* アプリ側からどの状態へ遷移するよう要求されているか */
	EManaComponentStatus RequiredStatus;

	/* CriManaPlayerHnの状態 */
	CriManaPlayerStatus PlayerStatus;

	/* Subtitle strings (current and prev). */
	TArray<uint8> Subtitles[2];

	/* Current subtitle String index. */
	bool bCurSubtitleIndex;

	/* Current Source (from Texture) */
	UManaMovie* Source;

	/** The play list to use, if any. */
	UPROPERTY(BlueprintReadOnly, transient, Category = "Mana")
	UManaPlaylist* Playlist;

	/** The current index of the source in the play list being played. */
	UPROPERTY(BlueprintReadOnly, Category = "Mana")
	int32 PlaylistIndex;

	/** The current index used for next Mana seamless callback. */
	int32 SeamlessPlaylistIndex;

	//--- EditDefaultsOnly

	/** ループ再生するかどうか (true: ループする false: ループしない) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mana", meta = (ToolTip = "Enable looping of movie playback."))
	bool bLoop;

	/** The current used main audio track. Negative value means unused.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mana|Audio", meta = (UIMin = -1, ClampMin = -1, ClampMax = 31, ToolTip = "Audio track index used for main audio."))
	int32 AudioTrack;

	/** The current used sub audio track. Negative value means unused.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mana|Audio", meta = (UIMin = -1, ClampMin = -1, ClampMax = 31, ToolTip = "Audio track index used for sub audio mixing."))
	int32 SubAudioTrack;

	/** The current used extra audio track. Negative value means unused.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mana|Audio", meta = (UIMin = -1, ClampMin = -1, ClampMax = 31, ToolTip = "Audio track index used for extra audio mixing."))
	int32 ExtraAudioTrack;

	/** The current Atom category assigned to main audio. Empty means no category assined. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mana|Audio", meta = (ToolTip = "Atom category assigned to main audio."))
	FString AudioCategoryName;
	
	/** The current Atom category assigned to sub audio. Empty means no category assined. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mana|Audio", meta = (ToolTip = "Atom category assigned to sub audio."))
	FString SubAudioCategoryName;
	
	/** The current Atom category assigned to extra audio. Empty means no category assined. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mana|Audio", meta = (ToolTip = "Atom category assigned to extra audio."))
	FString ExtraAudioCategoryName;
	
	/** Enable subtitles of movie playback. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mana|Subtitles", meta = (ToolTip = "Enable subtitles of movie playback."))
	bool bSubtitlesEnabled;

	/** Select a subtitle channel of movie playback. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mana|Subtitles", meta = (UIMin = 0, ClampMin = 0, ClampMax = 15, ToolTip = "Select a subtitle channel of movie playback."))
	int32 SubtitlesChannel;

	/** Select a subtitle channel of movie playback. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mana|Subtitles", meta = (ToolTip = "Used character encoding of the subtitles."))
	EManaSubtitlesEncoding SubtitlesEncoding;

	/** Play End Actions */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mana", AdvancedDisplay, meta = (ToolTip = "Action with last dipslayed frame when movie playback is finished."))
	EManaFrameAction EndFrameAction;

	/** The type of timer to use with playback function based on timings (times, event points...)*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mana", AdvancedDisplay)
	EManaPlaybackTimer PlaybackTimer;

	/** The maxinum number of decoded frames that can be dropped. This is different than the Mana FrameSkipping set in CriWare plugin settings. Frame was decoded but will immediatly discarded and not be drawn. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mana", AdvancedDisplay, meta = (ToolTip = "Maxinum number of decoded frames that can be dropped. This is different than the Mana FrameSkipping set in CriWare plugin settings. Frame was decoded but will immediatly discarded and not be drawn."))
	EManaMaxFrameDrop MaxFrameDrop;

	//--- ~EditDefaultsOnly

	/* キャッシュ */
	const void *CachedData;

	/* キャッシュサイズ */
	int32 CachedDataSize;

	/* Frame number for seek, when >= 0 waiting to stop, seek and replay to this frame else -1 when no request. */
	int32 RequestedSeekFrameNumber;

	/* Playlist index for seek, the index of the movie to replay when seeking. */
	int32 RequestedSeekIndex;

	/* Current playback speed [0.0, 2.5]. 0.0 is pause. */
	float Speed;

	/* Speed to set, when > 0.0 waiting to stop, set speed, seek and replay to current frame. */
	float RequestedSpeed;

	/* Is needed to re-pause after seek (Stop will remove the pause) */
	uint32 bRequestPauseAfterSeek : 1;

	/* Is alloctor set for meta data */
	uint32 bIsMetaDataAllocatorSet : 1;

	/* ループ回数 */
	int32 LoopCount;

	/* Dropped frame counter */
	int32 DroppedFrameCount;

	/* Mana player started */
	bool bManaStarted;

	/* Seamless callback attached */
	bool bSeamlessEnabled;

	/* Mana player loop status */
	bool bManaLoop;

	/* sub class for source streaming */
	class FManaSourceStreamer
	{
	public:
		FManaSourceStreamer();

		TArray<FManaEventPointInfo> GetEventPoints(FTimespan Time, FTimespan Duration);
		void AddSource(UManaMovie* InSource);
		void AdvanceToNextSource(bool InManaLoop);
		void Empty();
		UManaMovie* GetCurrentSource();

		void Reset(int32 Frame);
		FTimespan Update(UManaComponent* Owner, CriManaPlayerHn InManaPlayer);
		FTimespan SynchronizeWithFrame(UManaComponent* Owner, const CriManaFrameInfo& FrameInfo, bool& bIsInSync);
		void AdvanceTime(FTimespan ManaDeltaTime, FTimespan PlaybackDeltaTime);
		FTimespan GetPlaybackTime() const { return PlaybackTime; }
		FTimespan GetMoviePlaybackTime() const { return MoviePlaybackTime; }
		FTimespan GetManaTime() const { return ManaTime; }

	private:
		typedef TDoubleLinkedList<UManaMovie*> SourceList;

		SourceList::TDoubleLinkedListNode* ReadHead;
		SourceList Sources;
		bool bDisableCuePoint;

		/* Current playback times */
		FTimespan PlaybackTime;
		FTimespan MoviePlaybackTime;
		FTimespan ManaTime;
		FTimespan MaxPlaybackTime;
		/* (unused yet) */
		int32 LoopCount;
#if CRIWARE_MANA_PLAYBACKTIMER_MANATIME_DEBUG
		uint32 FrameCount;
#endif
#if CRIWARE_MANA_FRAMEDROP_GAMETIME_DEBUG
		FTimespan GameTime;
#endif
	};

	/* The source streamer for time synchronization and event points. */
	FManaSourceStreamer SourceStreamer;

#if STATS
	/* ======= デバッグ用の統計情報 ======= */

	/* 定期処理の開始時刻 */
	uint32 Stat_LastReferCallTime;

	/* 最後にデコード完了したフレーム番号 */
	int32 Stat_LastDecodedFrameNumber;

	/* FrameOnTimeがTrueになるまでにReferFrameした回数 */
	int32 Stat_ReferFrameCount;

	/* GetTimeの差分を見るため、前回のGetTimeの結果を格納 */
	float Stat_LastManaPlayerTime;
#endif

#ifdef CRIWARE_ENABLE_MANA_PLAYBACK_START_LATENCY_LOG
	/* Playの呼び出しからManaPlayerの各状態に遷移するまでにかかった時間を記録 */
	void OutputPlaybackStartLatencyLog();

	/* ManaPlayerの各状態が終わった時刻を記録しておく */
	typedef std::pair<CriManaPlayerStatus, uint32> PlayerStatusTime;
	TCircularQueue<PlayerStatusTime> PlayerStatusLapTime{32};

	/* 前回に時刻を記録したときのManaPlayerの状態を保持 */
	CriManaPlayerStatus LastStatus;
#endif
};

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/

/* --- end of file --- */
