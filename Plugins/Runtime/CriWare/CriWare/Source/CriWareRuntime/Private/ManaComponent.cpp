/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2015 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Mana Playback Control Component
 * File     : ManaComponent.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* モジュールヘッダ */
#include "ManaComponent.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareRuntimePrivatePCH.h"
#include "ManaTexture.h"
#include "ManaTextureResource.h"
#include "ManaPlaylist.h"

/* Unreal */
#include "Async/Async.h"
#if WITH_EDITOR
#include "Logging/MessageLog.h"
#endif

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/
#define LOCTEXT_NAMESPACE "ManaComponent"

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
/* Manaライブラリの終了判定用 */
extern "C" void    CRIAPI criMana_SetDecodeSkipThresholdFactor(CriFloat32 factor);

/***************************************************************************
 *      変数定義
 *      Variable Definition
 ***************************************************************************/

/***************************************************************************
 *      クラス定義
 *      Class Definition
 ***************************************************************************/
UManaComponent::UManaComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
, Movie(nullptr)
, ManaPlayer(nullptr)
, Status(EManaComponentStatus::Stop)
, PreviousStatus(EManaComponentStatus::Stop)
, RequiredStatus(EManaComponentStatus::Stop)
, PlayerStatus(CRIMANAPLAYER_STATUS_STOP)
, Source(nullptr)
, Playlist(nullptr)
, PlaylistIndex(0)
, SeamlessPlaylistIndex(0)
, bLoop(false)
, AudioTrack(0)
, SubAudioTrack(CRIMANA_AUDIO_TRACK_OFF)
, ExtraAudioTrack(CRIMANA_AUDIO_TRACK_OFF)
, bSubtitlesEnabled(false)
, SubtitlesChannel(0)
, SubtitlesEncoding(EManaSubtitlesEncoding::Default)
, EndFrameAction(EManaFrameAction::Default)
, PlaybackTimer(EManaPlaybackTimer::Default)
, MaxFrameDrop(EManaMaxFrameDrop::Two)
, CachedData(nullptr)
, CachedDataSize(0)
, RequestedSeekFrameNumber(-1)
, RequestedSeekIndex(-1)
, Speed(1.0f)
, RequestedSpeed(0.0f)
, bRequestPauseAfterSeek(false)
, bIsMetaDataAllocatorSet(false)
, DroppedFrameCount(0)
, bManaStarted(false)
, bSeamlessEnabled(false)
, bManaLoop(false)
#if STATS
, Stat_ReferFrameCount(0)
#endif
#ifdef CRIWARE_ENABLE_MANA_PLAYBACK_START_LATENCY_LOG
, PlayerStatusLapTime(CRIMANAPLAYER_STATUS_ERROR + 1)
, LastStatus(CRIMANAPLAYER_STATUS_STOP)
#endif
{
#if WITH_EDITOR
	/* Localization of unreal properties metadata with LOCTEXT markups and reflection */
	CRI_LOCCLASS(GetClass());
#endif

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	bTickInEditor = true;

	/* 不要な機能を無効化 */
	bNeverNeedsRenderUpdate = true;

	/* Initialize/UninitializeComponentが呼ばれるように設定する */
	bWantsInitializeComponent = true;
}

void UManaComponent::InitializeComponent()
{
	Super::InitializeComponent();

	check(!ManaPlayer);

	InitializeManaPlayer();
}

void UManaComponent::UninitializeComponent()
{
	if (ManaPlayer != nullptr) {

		// Platform specific module: ManaComponent uninitializer.
		auto PlatformMana = UCriWareInitializer::Platform.Mana();
		if (PlatformMana) {
			PlatformMana->UninitializeManaComponent(*this);
		}

		if (Movie != nullptr) {
			// clear texture if not destroyed
			if (Movie->IsValidLowLevel() && !Movie->HasAnyFlags(RF_BeginDestroyed | RF_FinishDestroyed)) {
				Movie->UpdateResource();
			}
			Movie = nullptr;
		}

		// ManaThread
		auto InManaPlayer = ManaPlayer;
		AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [InManaPlayer]()
		{
			if (criMana_IsInitialized() != CRI_FALSE) {
				criManaPlayer_StopAndWaitCompletion(InManaPlayer);

				// defer clean until pending draw operation finishes
				BeginCleanup(new FManaPlayerDeferredCleanup(InManaPlayer));
			}
		});

		/* パラメータのクリア */
		ManaPlayer = nullptr;
		Status = EManaComponentStatus::Stop;
		PlayerStatus = CRIMANAPLAYER_STATUS_STOP;
		RequestedSeekFrameNumber = -1;
		RequestedSeekIndex = -1;
		bRequestPauseAfterSeek = false;
		bIsMetaDataAllocatorSet = false;
		bManaStarted = false;
		bSeamlessEnabled = false;
		bManaLoop = false;
	}

	Super::UninitializeComponent();
}

// FDeferredCleanupInterface
FManaPlayerDeferredCleanup::~FManaPlayerDeferredCleanup()
{
	/* ハンドルの破棄 */
	/* 注意）GC処理の順序によっては、ライブラリ終了処理後にここに来る可能性がある。 */
	if (criMana_IsInitialized() != CRI_FALSE) {
		criManaPlayer_Destroy(ManaPlayer);
	}

	/* パラメータのクリア */
	ManaPlayer = nullptr;
}

#if WITH_EDITOR
void UManaComponent::CheckAlreadyUsedManaTexture(UManaTexture* InTexture)
{
	if (!InTexture || !InTexture->IsValidLowLevelFast()) {
		return;
	}

	if (IsBeingDestroyed() || HasAnyFlags(RF_BeginDestroyed | RF_FinishDestroyed)) {
		return;
	}

	for (TObjectIterator<UManaComponent> Itr; Itr; ++Itr) {
		// Access the subclass instance with the * or -> operators.
		UManaComponent *Component = *Itr;
		if (Component && Component->IsValidLowLevel() 
			&& Component != this && Component->Movie == InTexture 
			&& Component->HasBeenCreated() 
			&& !Component->IsBeingDestroyed() 
			&& !Component->HasAnyFlags(RF_BeginDestroyed | RF_FinishDestroyed)) {
			FFormatNamedArguments Arguments;
			Arguments.Add(TEXT("Object"), FText::FromString(Component->GetName()));
			auto InfoText = FText::Format(
				LOCTEXT("AlreadyReferencedManaTextureWarning", "ManaTexture is already referenced by another ManaComponent: {Object}\nA ManaTexture can only be used by a single ManaComponent at same time.")
				, Arguments
			);
			auto NotifyText = LOCTEXT("AlreadyReferencedManaTextureNotify", "ManaTexture is already referenced.");
			FMessageLog("CriWare").Info(InfoText);
			FMessageLog("CriWare").Notify(NotifyText);
			return;
		}
	}
}

void UManaComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property != nullptr) {
		FName PropertyName = PropertyChangedEvent.Property->GetFName();
		
		if (PropertyName == GET_MEMBER_NAME_CHECKED(UManaComponent, Movie)) {
			// check if used twice
			CheckAlreadyUsedManaTexture(Movie);
		}
	}
}
#endif

void UManaComponent::Prepare()
{
	check(ManaPlayer);

	/* Prepare可能な状態でなければエラーを出力 */
	if (Status != EManaComponentStatus::Stop &&
		Status != EManaComponentStatus::PlayEnd) {
		UE_LOG(LogCriWareRuntime, Error, TEXT("CriManaPlayer status is not ready to prepare. Please prepare when status is Stop or PlayEnd."));
		return;
	}

	/* 再生準備 */
	PrepareManaPlayer();
}

void UManaComponent::PreparePlaylistIndex(int32 Index)
{
	check(ManaPlayer);

	/* Prepare可能な状態でなければエラーを出力 */
	if (Status != EManaComponentStatus::Stop &&
		Status != EManaComponentStatus::PlayEnd) {
		UE_LOG(LogCriWareRuntime, Error, TEXT("CriManaPlayer status is not ready to prepare. Please prepare when status is Stop or PlayEnd."));
		return;
	}

	/* 再生準備 */
	PrepareManaPlayer(Index);
}

void UManaComponent::Play()
{
	if (ManaPlayer == nullptr) {
		UE_LOG(LogCriWareRuntime, Error, TEXT("Play function is not available until ManaComponent gets initialized."));
		return;
	}

	// Remove pause if paused (Play works like Pause(false)).
	if (IsPaused()) {
		if (RequestedSeekFrameNumber >= 0) {
			bRequestPauseAfterSeek = false;
		} else {
			criManaPlayer_Pause(ManaPlayer, CRI_OFF);
		}
	}

	// No-op if already playing.
	if (Status == EManaComponentStatus::Playing ||
		RequiredStatus == EManaComponentStatus::Playing) {
		return;
	}

	/* 再生可能な状態でなければエラーを出力 */
	if (!(RequiredStatus == EManaComponentStatus::Ready /* Prepareをかけている場合はOK */
		|| Status == EManaComponentStatus::Stop
		|| Status == EManaComponentStatus::PlayEnd)) { /* 現在の状態がStopの場合はOK */
		UE_LOG(LogCriWareRuntime, Error, TEXT("CriManaPlayer status is not ready to start. Please play when status is Stop, PlayEnd or after a call to Prepare()."));
		return;
	}

	/* Prepareしていなければ、再生準備をする */
	if (RequiredStatus == EManaComponentStatus::Stop || Status == EManaComponentStatus::PlayEnd) {
		PrepareManaPlayer();
	}

	/* 再生要求を記憶 */
	RequiredStatus = EManaComponentStatus::Playing;

	// audio mixing settings
	SetAudioTrack(AudioTrack < 0 ? CRIMANA_AUDIO_TRACK_OFF : AudioTrack);
	SetSubAudioTrack(SubAudioTrack < 0 ? CRIMANA_AUDIO_TRACK_OFF : SubAudioTrack);
	SetExtraAudioTrack(ExtraAudioTrack < 0 ? CRIMANA_AUDIO_TRACK_OFF : ExtraAudioTrack);

	// audio category settings
	SetAudioCategory(AudioCategoryName);  // Main
	SetSubAudioCategory(SubAudioCategoryName); // Music
	SetExtraAudioCategory(ExtraAudioCategoryName); // Empty -> unset

	// subtitles settings
	ChangeSubtitlesChannel(SubtitlesChannel);

	// source streamer
	SourceStreamer.Empty();
	if (Source != nullptr) {
		SourceStreamer.AddSource(Source);
	}

	// timers
	int32 Frame = -1;
	if (RequestedSeekFrameNumber > 0) {
		Frame = RequestedSeekFrameNumber;
	}
	SourceStreamer.Reset(Frame);

#if STATS
	/* 統計情報をリセット */
	Stat_LastDecodedFrameNumber = -1;
#endif

#ifdef CRIWARE_ENABLE_MANA_PLAYBACK_START_LATENCY_LOG
	/* 再生開始レイテンシ計測のため、再生開始時刻をFManaTextureResourceに通知 */
	static_cast<FManaTextureResource*>(Movie->Resource)->SetPlayRequestTime(FPlatformTime::Cycles());

	/* 状態遷移時刻を計測するため、再生開始時刻をPlayerStatusLapTimeに記録 */
	PlayerStatusLapTime.Enqueue(std::make_pair(CRIMANAPLAYER_STATUS_STOP, FPlatformTime::Cycles()));
	LastStatus = CRIMANAPLAYER_STATUS_STOP;
#endif
}

void UManaComponent::Pause(bool bPause /*=true*/)
{
	if (ManaPlayer == nullptr) {
		UE_LOG(LogCriWareRuntime, Error, TEXT("Pause function is not available until ManaComponent gets initialized."));
		return;
	}

	// pause requested same frame with seek, set the pause flag
	if (RequestedSeekFrameNumber >= 0) {
		bRequestPauseAfterSeek = bPause;
		return;
	}

	/* 再生可能な状態でなければエラーを出力 */
	if (RequiredStatus != EManaComponentStatus::Playing &&
		RequiredStatus != EManaComponentStatus::DecHdr &&
		RequiredStatus != EManaComponentStatus::Prep &&
		RequiredStatus != EManaComponentStatus::Ready) {
		UE_LOG(LogCriWareRuntime, Warning, TEXT("CriManaPlayer status is not ready to pause. Please pause when status is Playing or after call to Prepare()."));
		return;
	}

	if (PlaybackTimer != EManaPlaybackTimer::Manual) {
		criManaPlayer_Pause(ManaPlayer, bPause ? CRI_ON : CRI_OFF);
	} else {
		UE_LOG(LogCriWareRuntime, Warning, TEXT("Pause is not available when playback timer is set to Manual. You can pause playback by do not calling AdvanceTime()."));
	}
}

void UManaComponent::Loop(bool bInLoop /*=true*/)
{
	if (bLoop != bInLoop) {
		bLoop = bInLoop;
		if (ManaPlayer) {
			/* ループを設定 */
			if (bLoop && (Playlist == nullptr || Playlist->Num() == 1)) {
				criManaPlayer_Loop(ManaPlayer, CRI_ON);
				bManaLoop = true;
			}
			else {
				criManaPlayer_Loop(ManaPlayer, CRI_OFF);
				bManaLoop = false;
			}
		}
	}
}

bool UManaComponent::IsPaused() const
{
	if (ManaPlayer == nullptr) {
		UE_LOG(LogCriWareRuntime, Error, TEXT("IsPaused function is not available until ManaComponent gets initialized."));
		return false;
	}

	return (criManaPlayer_IsPaused(ManaPlayer) == CRI_ON);
}

bool UManaComponent::IsPlaying() const
{
	if (ManaPlayer == nullptr) {
		UE_LOG(LogCriWareRuntime, Error, TEXT("IsPlaying function is not available until ManaComponent gets initialized."));
		return false;
	}

	return (Status == EManaComponentStatus::Playing);
}

bool UManaComponent::IsPreparing() const
{
	if (ManaPlayer == nullptr) {
		UE_LOG(LogCriWareRuntime, Error, TEXT("IsPreparing function is not available until ManaComponent gets initialized."));
		return false;
	}

	return ((Status == EManaComponentStatus::DecHdr) ||
			(Status == EManaComponentStatus::WaitPrep) ||
			(Status == EManaComponentStatus::Prep));
}

bool UManaComponent::IsReady() const
{
	if (ManaPlayer == nullptr) {
		UE_LOG(LogCriWareRuntime, Error, TEXT("IsReady function is not available until ManaComponent gets initialized."));
		return false;
	}

	return (Status == EManaComponentStatus::Ready);
}

bool UManaComponent::PrepareManaPlayer(int32 Index /* = 0 */)
{
	/* USMファイルを開く */
	bool Cached = false;
	if (Movie && Movie->GetMovieSource()) {
		// check source type
		if (Movie->GetMovieSource()->GetClass() == UFileManaMovie::StaticClass()) {
			auto FileSource = Cast<UFileManaMovie>(Movie->GetMovieSource());
			Source = FileSource;
			OnMovieChanged.Broadcast(Source, this);
			criManaPlayer_SetFile(ManaPlayer, nullptr, TCHAR_TO_UTF8(*(FileSource->GetUrl().RightChop(7))));
		} else if (Movie->GetMovieSource()->GetClass() == UManaPlaylist::StaticClass()) {
			// get playlist
			auto SourcePlaylist = Cast<UManaPlaylist>(Movie->GetMovieSource());
			Playlist = SourcePlaylist;
			// check index
			if (Index < 0 || Index >= Playlist->Num()) {
				UE_LOG(LogCriWareRuntime, Error, TEXT("Playlist index is out of range."));
				return false;
			}
			// set index
			PlaylistIndex = Index;
			// Get movie at index
			auto MovieSource = SourcePlaylist->Get(PlaylistIndex);
			if (MovieSource && MovieSource->Validate()) {
				// set movie to texture
				Movie->SetPlaylistMovie(PlaylistIndex);
				// if file movie
				if (MovieSource->GetClass() == UFileManaMovie::StaticClass()) {
					auto FileSource = Cast<UFileManaMovie>(MovieSource);
					Source = FileSource;
					OnMovieChanged.Broadcast(Source, this);
					criManaPlayer_SetFile(ManaPlayer, nullptr, TCHAR_TO_UTF8(*(FileSource->GetUrl().RightChop(7))));
				}
				// TODO: if data movie
			} else {
				UE_LOG(LogCriWareRuntime, Error, TEXT("Movie is not specified at playlist index (%d)."), PlaylistIndex);
				return false;
			}
			// enable seamless
			if (Playlist->IsSeamless()) {
				EnableSeamless(true);
			}
		}
	} else if (Movie && CachedData && (CachedDataSize > 0)) {
		/* データがメモリにロード済みの場合 */
		criManaPlayer_SetData(ManaPlayer, CachedData, CachedDataSize);
		Cached = true;
	}
	if (!Source && !Cached) {
		/* 再生する動画ファイルが指定されていなければエラーを出力 */
		UE_LOG(LogCriWareRuntime, Error, TEXT("Movie Texture or Source is not specified."));
		return false;
	}

	// set Master Time Settings
	if (PlaybackTimer == EManaPlaybackTimer::Manual && Source != nullptr) {
		// 外部時間に従って時間を進めるため、Manualモードを使用する
		static const CriUint64 UnitN = 1000;
		const CriUint64 UnitD = Source->GetVideoTrackFrameRate(0) * UnitN;
		criManaPlayer_SetMasterTimerType(ManaPlayer, CriManaTimerType::CRIMANAPLAYER_TIMER_MANUAL);
		criManaPlayer_SetManualTimerUnit(ManaPlayer, UnitN, UnitD);
	} else {
		// Atom(音声)かSystemの時間に従って時間を進めるため、Audioモードを使用する
		criManaPlayer_SetMasterTimerType(ManaPlayer, CriManaTimerType::CRIMANAPLAYER_TIMER_AUDIO);
	}

	/* ループを設定 */
	if (bLoop && (Playlist == nullptr || Playlist->Num() == 1)) {
		criManaPlayer_Loop(ManaPlayer, CRI_ON);
		bManaLoop = true;
	} else {
		criManaPlayer_Loop(ManaPlayer, CRI_OFF);
		bManaLoop = false;
	}

	/* 再生管理用パラメータのリセット */
	LoopCount = 0;

	/* 再生開始パラメータのリセット */
	bManaStarted = false;

	/* 再生準備の開始 */
	criManaPlayer_Prepare(ManaPlayer);

	/* DecHdrへ遷移 */
	UpdateManaPlayer();
	if (PlayerStatus == CRIMANAPLAYER_STATUS_ERROR) {
		ChangeStatus(EManaComponentStatus::Error);
	} else {
		ChangeStatus(EManaComponentStatus::DecHdr);
	}

	/* Tick開始 */
	Activate(true);

	/* Prepareしたことを覚えておく */
	RequiredStatus = EManaComponentStatus::Ready;

	return true;
}

void UManaComponent::Stop()
{
	check(ManaPlayer);

	/* 再生停止 */
	criManaPlayer_Stop(ManaPlayer);
	bManaStarted = false;

	UpdateManaPlayer();

	if (PlayerStatus == CRIMANAPLAYER_STATUS_STOP) {
		ChangeStatus(EManaComponentStatus::Stop);
	} else {
		ChangeStatus(EManaComponentStatus::StopProcessing);
	}

	/* 再生要求をクリア */
	RequiredStatus = EManaComponentStatus::Stop;

	// Disable seek re-play request.
	if (RequestedSeekFrameNumber >= 0) {
		RequestedSeekFrameNumber = -1;
	}

#ifdef CRIWARE_ENABLE_MANA_PLAYBACK_START_LATENCY_LOG
	OutputPlaybackStartLatencyLog();
#endif

	// clear texture if requested
	if (Movie != nullptr && EndFrameAction == EManaFrameAction::Clear) {
		Movie->UpdateResource();
	}
}

#ifdef CRIWARE_ENABLE_MANA_PLAYBACK_START_LATENCY_LOG
void UManaComponent::OutputPlaybackStartLatencyLog()
{
	/* エンジン終了時にはMovieがnullptrになっている場合がある。その場合、再生開始時刻のリセットは不要。*/
	if (Movie) {
		/* 再生開始時刻をリセット */
		static_cast<FManaTextureResource*>(Movie->Resource)->SetPlayRequestTime(0);
	}

	/* 再生開始時の各状態にかかった時間をログ出力 */
	UE_LOG(LogCriWareRuntime, Log, TEXT("Mana Playback Start Latency: Player Status Lap Time ========="));
	PlayerStatusTime LapTime;
	uint32 LastTime = 0;
	while (PlayerStatusLapTime.Dequeue(LapTime)) {
		CriManaPlayerStatus CheckpointPlayerStatus = LapTime.first;
		float Duration = FPlatformTime::ToMilliseconds(LapTime.second - LastTime);
		switch (CheckpointPlayerStatus) {
		case CRIMANAPLAYER_STATUS_STOP:
			/* switch文から抜けたところで、LastTimeを初期化 */
			break;
		case CRIMANAPLAYER_STATUS_DECHDR:
			UE_LOG(LogCriWareRuntime, Log, TEXT("-    DECHDR: %5.2f"), Duration);
			break;
		case CRIMANAPLAYER_STATUS_WAIT_PREP:
			UE_LOG(LogCriWareRuntime, Log, TEXT("- WAIT_PREP: %5.2f"), Duration);
			break;
		case CRIMANAPLAYER_STATUS_PREP:
			UE_LOG(LogCriWareRuntime, Log, TEXT("-      PREP: %5.2f"), Duration);
			break;
		case CRIMANAPLAYER_STATUS_READY:
			UE_LOG(LogCriWareRuntime, Log, TEXT("-     READY: %5.2f"), Duration);
			break;
		default:
			break;
		}
		LastTime = LapTime.second;
	}
}
#endif

void UManaComponent::SeekToPosition(int32 FrameNumber)
{
	if (ManaPlayer == nullptr) {
		UE_LOG(LogCriWareRuntime, Error, TEXT("SeekToPosition function is not available until ManaComponent gets initialized."));
		return;
	}

	if (RequiredStatus != EManaComponentStatus::Playing &&
		RequiredStatus != EManaComponentStatus::Prep &&
		RequiredStatus != EManaComponentStatus::Ready &&
		RequiredStatus != EManaComponentStatus::Stop &&
		RequiredStatus != EManaComponentStatus::PlayEnd) {
		UE_LOG(LogCriWareRuntime, Error, TEXT("CriManaPlayer status is not ready to seek. Please seek when status is Playing or Stop or after a call to Prepare()."));
		return;
	}

	// sets allocator and enable collection of meta data if not yet.
	StartCollectMetaData();

	// Frame number bounds to zero
	if (FrameNumber < 0) {
		FrameNumber = 0;
	}

	// Documentation simply say criManaPlayer_SetSeekPosition() should called before
	// any call to criManaPlayer_Prepare().
	// In reality, DecHdr and even WaitPrep are also valable statuses,
	// because the seek value is only read and used when ManaPlayer goes to Prepare status.
	// Since user can call SeekToPosition at anytime in UE, we also set DecHdr as valid status.
	// This avoid some useless double calls to criManaPlayer_prepare() when call is combined with 
	// other functions like ToIndex(), Play() or Prepare().
	if (Status == EManaComponentStatus::Stop ||
		Status == EManaComponentStatus::PlayEnd ||
		Status == EManaComponentStatus::DecHdr) {
		criManaPlayer_SetSeekPosition(ManaPlayer, static_cast<CriSint32>(FrameNumber));
		if (RequestedSeekFrameNumber < 0) {
			// broadcast event if seek completed 
			OnSeekCompleted.Broadcast(FrameNumber, this);
		}
	} else {
		bRequestPauseAfterSeek = IsPaused();
		Stop();
		RequestedSeekFrameNumber = FrameNumber;
		RequestedSeekIndex = PlaylistIndex;
	}
}

void UManaComponent::Seek(FTimespan Time)
{
	if (ManaPlayer == nullptr || Source == nullptr) {
		UE_LOG(LogCriWareRuntime, Error, TEXT("Seek function is not available until ManaComponent gets initialized and movie is open."));
		return;
	}

	double FrameDuration = 1.0 / Source->GetVideoTrackFrameRate(0);
	int32 FrameNumber = FMath::RoundHalfFromZero(Time.GetTotalSeconds() * Source->GetVideoTrackFrameRate(0));
	SeekToPosition(FrameNumber);
}

void UManaComponent::SetAudioTrack(int32 Track)
{
	AudioTrack = Track;
	check(ManaPlayer);
	criManaPlayer_SetAudioTrack(ManaPlayer, Track);
}

void UManaComponent::SetSubAudioTrack(int32 Track)
{
	SubAudioTrack = Track;
	check(ManaPlayer);
	criManaPlayer_SetSubAudioTrack(ManaPlayer, Track);
}

void UManaComponent::SetSubAudioSendLevel(int32 Channel, EAtomSpeakerID SpeakerId, float Volume)
{
	check(ManaPlayer);
	criManaPlayer_SetSubAudioSendLevel(
		ManaPlayer,
		Channel,						 /* チャンネルの番号。1ch音源の場合は必ず0 */
		(CriAtomSpeakerId)SpeakerId,	 /* センタースピーカーから鳴らす */
		Volume);						 /* ボリューム */
}

void UManaComponent::SetExtraAudioTrack(int32 Track)
{
	ExtraAudioTrack = Track;
	check(ManaPlayer);
	criManaPlayer_SetExtraAudioTrack(ManaPlayer, Track);
}

void UManaComponent::SetVolume(float Volume)
{
	check(ManaPlayer);
	criManaPlayer_SetVolume(ManaPlayer, Volume);
}

float UManaComponent::GetVolume() const
{
	check(ManaPlayer);
	return criManaPlayer_GetVolume(ManaPlayer);
}

void UManaComponent::SetSubVolume(float Volume)
{
	check(ManaPlayer);
	criManaPlayer_SetSubAudioVolume(ManaPlayer, Volume);
}

float UManaComponent::GetSubVolume() const
{
	check(ManaPlayer);
	return criManaPlayer_GetSubAudioVolume(ManaPlayer);
}

void UManaComponent::SetExtraVolume(float Volume)
{
	check(ManaPlayer);
	criManaPlayer_SetExtraAudioVolume(ManaPlayer, Volume);
}

float UManaComponent::GetExtraVolume() const
{
	check(ManaPlayer);
	return criManaPlayer_GetExtraAudioVolume(ManaPlayer);
}

void UManaComponent::SetAudioCategory(const FString& CategoryName)
{
	check(ManaPlayer);
	AudioCategoryName = CategoryName;
	if (CategoryName.IsEmpty()) {
		RemoveAudioCategory();
	} else {
		criManaPlayer_SetSoundCategoryByName(ManaPlayer, 0, TCHAR_TO_ANSI(*CategoryName));
	}
}

void UManaComponent::SetSubAudioCategory(const FString& CategoryName)
{
	check(ManaPlayer);
	SubAudioCategoryName = CategoryName;
	if (CategoryName.IsEmpty()) {
		RemoveSubAudioCategory();
	}
	else {
		criManaPlayer_SetSoundCategoryByName(ManaPlayer, 1, TCHAR_TO_ANSI(*CategoryName));
	}
}

void UManaComponent::SetExtraAudioCategory(const FString& CategoryName)
{
	check(ManaPlayer);
	ExtraAudioCategoryName = CategoryName;
	if (CategoryName.IsEmpty()) {
		RemoveExtraAudioCategory();
	} else {
		criManaPlayer_SetSoundCategoryByName(ManaPlayer, 2, TCHAR_TO_ANSI(*CategoryName));
	}
}

void UManaComponent::RemoveAudioCategory()
{
	check(ManaPlayer);
	AudioCategoryName.Empty();
	criManaPlayer_UnsetSoundCategory(ManaPlayer, 0);
}

void UManaComponent::RemoveSubAudioCategory()
{
	check(ManaPlayer);
	SubAudioCategoryName.Empty();
	criManaPlayer_UnsetSoundCategory(ManaPlayer, 1);
}

void UManaComponent::RemoveExtraAudioCategory()
{
	check(ManaPlayer);
	ExtraAudioCategoryName.Empty();
	criManaPlayer_UnsetSoundCategory(ManaPlayer, 2);
}

FTimespan UManaComponent::GetTime() const
{
	return SourceStreamer.GetPlaybackTime();
}

int32 UManaComponent::GetFrameNumber() const
{
	if (!ManaPlayer) {
		return -1;
	}
	/*  再生時刻の取得 */
	int32 FrameNumber = static_cast<int32>(criManaPlayer_CalcFrameNoFromTime(
		ManaPlayer,
		SourceStreamer.GetPlaybackTime().GetTicks(),
		10000000UL));
	return (FrameNumber < 0 ? -1 : FrameNumber);
}

FTimespan UManaComponent::GetMovieTime() const
{
	return SourceStreamer.GetMoviePlaybackTime();
}

int32 UManaComponent::GetPosition() const
{
	if (!ManaPlayer) {
		return -1;
	}
	/*  再生時刻の取得 */
	int32 FrameNumber = static_cast<int32>(criManaPlayer_CalcFrameNoFromTime(
		ManaPlayer,
		SourceStreamer.GetMoviePlaybackTime().GetTicks(),
		10000000UL));
	return (FrameNumber < 0 ? -1 : FrameNumber);
}

void UManaComponent::EnableSubtitles(bool bEnable)
{
	if (bSubtitlesEnabled != bEnable) {
		bSubtitlesEnabled = bEnable;
		ChangeSubtitlesChannel(SubtitlesChannel);
	}
}

void UManaComponent::ChangeSubtitlesChannel(int32 InChannel)
{
	SubtitlesChannel = InChannel;
	// disable
	int32 SubChannel = CRIMANA_SUBTITLE_CHANNEL_OFF;
	// or use subtitle, if enabled and in-range
	int32 NumSubtitleChannels = 0;
	int32 MaxSubtitleSize = 0;
	if (Source != nullptr) {
		NumSubtitleChannels = Source->GetNumSubtitleChannels();
		MaxSubtitleSize = Source->GetMaxSubtitleSize();
	}

	if (bSubtitlesEnabled && Movie != nullptr &&
		InChannel >= 0 && InChannel < NumSubtitleChannels) {
		SubChannel = InChannel;
	}
	if (ManaPlayer != nullptr) {
		criManaPlayer_SetSubtitleChannel(ManaPlayer, SubChannel);
		// reset
		for (auto& Subtitle : Subtitles) {
			Subtitle.Empty(MaxSubtitleSize);
			Subtitle.AddZeroed(MaxSubtitleSize);
		}
		// fire event
		OnSubtitleChanged.Broadcast(FText::GetEmpty());
	}
}

void UManaComponent::ChangeSubtitlesEncoding(EManaSubtitlesEncoding Encoding)
{
	SubtitlesEncoding = Encoding;
}

EManaComponentStatus UManaComponent::GetStatus() const
{
	return Status;
}

UManaMovie* UManaComponent::GetSource() const
{
	return Source;
}

void UManaComponent::SetTexture(UManaTexture* Texture)
{
	Movie = Texture;
	// clear texture
	if (Movie != nullptr) {
		Movie->UpdateResource();
	}
}

UManaTexture* UManaComponent::GetTexture() const
{
	return Movie;
}

void UManaComponent::SetCachedData(const TArray<uint8>& DataArray)
{
	CachedData = DataArray.GetData();
	CachedDataSize = DataArray.Num();
}

void UManaComponent::ResetCachedData()
{
	CachedData = nullptr;
	CachedDataSize = 0;
}

CriManaPlayerHn UManaComponent::GetManaPlayer()
{
	return ManaPlayer;
}

void UManaComponent::StartCollectMetaData()
{
	check(ManaPlayer);

	LLM_SCOPE(ELLMTag::Audio);

	if (!bIsMetaDataAllocatorSet) {
		criManaPlayer_SetMetaDataWorkAllocator(ManaPlayer,
			[](void *, CriUint32 size) { return FMemory::Malloc(size); },
			[](void *, void* ptr) { return FMemory::Free(ptr); },
			nullptr,
			CRIMANA_META_FLAG_ALL
		);
		bIsMetaDataAllocatorSet = true;
	}
}

void UManaComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ManaPlayer == nullptr) {
		return;
	}

	switch (Status) {
	case EManaComponentStatus::Stop: CRIWARE_MANACOMPONENT_CASE_STOP:
		/* Play要求が来るまで待つ */
		if (RequestedSeekFrameNumber >= 0) {
			// store and remove request pause after seek status
			bool bRequestPause = bRequestPauseAfterSeek;
			bRequestPauseAfterSeek = false;
			// seek mana player while stop status
			SeekToPosition(RequestedSeekFrameNumber);
			// prepare movie if seek to another movie
			if (RequestedSeekIndex > 0) {
				PrepareManaPlayer(RequestedSeekIndex);
				RequestedSeekIndex = -1;
			}
			// restart playback with timer set to seek position frame number. 
			Play();
			// store and remove request for seek
			int32 FrameNumber = RequestedSeekFrameNumber;
			RequestedSeekFrameNumber = -1;
			// pause after seek if requested
			if (bRequestPause) {
				Pause();
			}
			// broadcast event
			OnSeekCompleted.Broadcast(FrameNumber, this);
		}
		break;
	case EManaComponentStatus::DecHdr:
		UpdateManaPlayer();
		if (PlayerStatus == CRIMANAPLAYER_STATUS_WAIT_PREP) {
			/* WaitPrep状態へ遷移 */
			ChangeStatus(EManaComponentStatus::WaitPrep);
			goto CRIWARE_MANACOMPONENT_CASE_WAIT_PREP;
		}
		/* WaitPrepになるまで待つ */

		// Possible status after a defered level load. (Boot videos)
		if (PlayerStatus == CRIMANAPLAYER_STATUS_PREP) {
			ChangeStatus(EManaComponentStatus::Prep);
			goto CRIWARE_MANACOMPONENT_CASE_PREP;
		}
		if (Movie->IsRenderResourcePrepared() &&
			PlayerStatus == CRIMANAPLAYER_STATUS_READY) {
			ChangeStatus(EManaComponentStatus::Ready);
			goto CRIWARE_MANACOMPONENT_CASE_READY;
		}
		break;
	case EManaComponentStatus::WaitPrep: CRIWARE_MANACOMPONENT_CASE_WAIT_PREP:
		if (RequiredStatus == EManaComponentStatus::Playing
			&& Movie->IsRenderResourcePrepared()) {
			Movie->ResetFrameQueues();
			criManaPlayer_Start(ManaPlayer);
			bManaStarted = true;
		}
		/* Prep状態へ遷移 */
		UpdateManaPlayer();
		ChangeStatus(EManaComponentStatus::Prep);
		goto CRIWARE_MANACOMPONENT_CASE_PREP;
		break;
	case EManaComponentStatus::Prep: CRIWARE_MANACOMPONENT_CASE_PREP:
		if (!Movie->IsRenderResourcePrepared()) {
			/* 描画リソースの準備完了まで待つ */
			break;
		} else {
			UpdateSubtitle();
			UpdateManaPlayer();
			if (PlayerStatus == CRIMANAPLAYER_STATUS_READY) {
				ChangeStatus(EManaComponentStatus::Ready);
				goto CRIWARE_MANACOMPONENT_CASE_READY;
			} else if (PlayerStatus == CRIMANAPLAYER_STATUS_PLAYING) {
				ChangeStatus(EManaComponentStatus::Playing);
				goto CRIWARE_MANACOMPONENT_CASE_PLAYING;
			} else {
				/* ManaPlayerの状態がREADYになるまで待つ */
				break;
			}
		}
		break;
	case EManaComponentStatus::Ready: CRIWARE_MANACOMPONENT_CASE_READY:
		if (RequiredStatus == EManaComponentStatus::Playing) {
			// if already started in wait_prep, do not call cri_start again
			if (!bManaStarted) {
				Movie->ResetFrameQueues();
				/* 再生開始してPlayingへ遷移 */
				criManaPlayer_Start(ManaPlayer);
				bManaStarted = true;
			}

			ChangeStatus(EManaComponentStatus::Playing);
			goto CRIWARE_MANACOMPONENT_CASE_PLAYING;
		}
		/* 再生要求が来るまで待つ */
		break;
	case EManaComponentStatus::Playing: CRIWARE_MANACOMPONENT_CASE_PLAYING:
		UpdateSubtitle();
		UpdateManaPlayer();
		UpdatePlaybackTime();

		if (PlayerStatus == CRIMANAPLAYER_STATUS_PLAYING) {
			/* デコードしたムービフレームをテクスチャリソースへ提供 */
			UpdateFrame();
		} else if (PlayerStatus == CRIMANAPLAYER_STATUS_PLAYEND) {
			// if playlist, try to play next movie or stay to playend status
			if (Playlist != nullptr) {
				if (Playlist->IsSeamless()) {
					EnableSeamless(false);
				} else if (bLoop || PlaylistIndex + 1 < Playlist->Num()) {
					// stops and prepare next index movie in playlist
					Next();
					break;
				}
			}
			// else set clear color if requested
			if (Movie != nullptr && EndFrameAction == EManaFrameAction::Clear) {
				Movie->UpdateResource();
			}

			// broadcast PlayEnd status only if a standalone a single movie or a complete playlist ended,
			// movies in non-seamless playlist do not trigger PlayEnd but the Stop status when movies change.
			RequiredStatus = EManaComponentStatus::PlayEnd;
			ChangeStatus(EManaComponentStatus::PlayEnd);
			goto CRIWARE_MANACOMPONENT_CASE_PLAYEND;
		}
		/* PLAYENDになるまで待つ */
		break;
	case EManaComponentStatus::PlayEnd: CRIWARE_MANACOMPONENT_CASE_PLAYEND:
		/* なにもしない */
		break;
	case EManaComponentStatus::StopProcessing:
		UpdateManaPlayer();
		if (PlayerStatus == CRIMANAPLAYER_STATUS_STOP) {
			ChangeStatus(EManaComponentStatus::Stop);
			/* Tick停止 */
			Deactivate();
			goto CRIWARE_MANACOMPONENT_CASE_STOP;
		}
		/* STOPになるまで待つ */
		break;
	case EManaComponentStatus::Error:
		/* なにもしない */
		break;
	default:
		break;
	}

	if (PlayerStatus == CRIMANAPLAYER_STATUS_ERROR) {
		ChangeStatus(EManaComponentStatus::Error);
	}

#ifdef CRIWARE_ENABLE_MANA_PLAYBACK_START_LATENCY_LOG
	{
		CriManaPlayerStatus ManaPlayerStatus = criManaPlayer_GetStatus(ManaPlayer);
		/* 再生開始レイテンシ計測のため、状態遷移時に現在時刻を記録しておく */
		if (ManaPlayerStatus != LastStatus) {
			PlayerStatusLapTime.Enqueue(std::make_pair(LastStatus, FPlatformTime::Cycles()));
			LastStatus = ManaPlayerStatus;
		}
	}
#endif
}

void UManaComponent::ChangeStatus(EManaComponentStatus NewStatus)
{
	Status = NewStatus;
	// status event
	if (PreviousStatus != Status) {
		PreviousStatus = Status;
		OnStatusChanged.Broadcast(Status, this);
	}
}

void UManaComponent::InitializeManaPlayer()
{
	// ManaPlayerを作成
	//ManaPlayer = criManaPlayer_Create(nullptr, 0);
	CriManaPlayerConfig Config;
	criManaPlayer_SetDefaultHandleConfig(&Config);
	Config.use_atomex_player = CRI_TRUE;
	ManaPlayer = criManaPlayer_CreateWithConfig(&Config, nullptr, 0);

	if (ManaPlayer != nullptr) {
		// Mana player settings
		const uint32 NumberOfFramePools = 5;
		criManaPlayer_SetNumberOfFramePools(ManaPlayer, NumberOfFramePools);
		criManaPlayer_SetBufferingTime(ManaPlayer, 5.0f);
		criMana_SetDecodeSkipThresholdFactor(5.0f);

		// Platform specific module: ManaComponent initializer.
		auto PlatformMana = UCriWareInitializer::Platform.Mana();
		if (PlatformMana) {
			PlatformMana->InitializeManaComponent(*this);
		}
	} else {
		UE_LOG(LogCriWareRuntime, Error, TEXT("Failed to create a Mana player. Please check Mana settings."));
	}

	// clear texture if possible
	if (Movie != nullptr) {
		Movie->UpdateResource();
	}
}

void UManaComponent::UpdateManaPlayer()
{
	check(ManaPlayer);

	/* ManaPlayerの状態を更新 */
	criManaPlayer_SyncMasterTimer(ManaPlayer);
	criManaPlayer_ExecuteMain(ManaPlayer);
	PlayerStatus = criManaPlayer_GetStatus(ManaPlayer);
}

void UManaComponent::UpdateSubtitle()
{
	check(ManaPlayer);

	int32 NumSubtitleChannels = 0;
	int32 MaxSubtitleSize = 0;
	if (Source != nullptr) {
		NumSubtitleChannels = Source->GetNumSubtitleChannels();
		MaxSubtitleSize = Source->GetMaxSubtitleSize();
	}

	if (bSubtitlesEnabled && Movie != nullptr && NumSubtitleChannels > 0) {
		auto Buffer = Subtitles[!bCurSubtitleIndex].GetData();
		if (Buffer != nullptr) {
			CriUint32 Size = criManaPlayer_GetSubtitleOnTime(ManaPlayer, reinterpret_cast<CriUint8*>(Buffer), MaxSubtitleSize);

			// different string
			if (Subtitles[bCurSubtitleIndex] != Subtitles[!bCurSubtitleIndex]) {
				// swap text buffer
				bCurSubtitleIndex = !bCurSubtitleIndex;
				// convert text if needed
				FText Subtitle;
				switch (SubtitlesEncoding) {
				case EManaSubtitlesEncoding::ANSI:
					Subtitle = FText::FromString(ANSI_TO_TCHAR((ANSICHAR*)Buffer));
					break;
				case EManaSubtitlesEncoding::Default:
				case EManaSubtitlesEncoding::Utf8:
					Subtitle = FText::FromString(UTF8_TO_TCHAR((ANSICHAR*)Buffer));
					break;
				case EManaSubtitlesEncoding::Utf16:
					Subtitle = FText::FromString((TCHAR*)Buffer);
					break;
				}
				// fire subtitle text changed event
				OnSubtitleChanged.Broadcast(Subtitle);
			}
		}
	}
}

void UManaComponent::UpdatePlaybackTime()
{
	// update playback timer
	auto PlaybackDeltaTime = SourceStreamer.Update(this, ManaPlayer);
	// trigger new event points
	auto EventPoints = SourceStreamer.GetEventPoints(SourceStreamer.GetMoviePlaybackTime(), -PlaybackDeltaTime);
	for (auto EventPointInfo : EventPoints) {
		OnEventPoint.Broadcast(EventPointInfo);
	}
}

void UManaComponent::UpdateFrame()
{
#if STATS
	/* ReferFrameの動作間隔を計測 */
	DECLARE_FLOAT_COUNTER_STAT(TEXT("Mana ReferFrame Call Interval"), STAT_ManaReferInterval, STATGROUP_CriWare);
	uint32 CurrentTime = FPlatformTime::Cycles();
	SET_FLOAT_STAT(STAT_ManaReferInterval, FPlatformTime::ToMilliseconds(CurrentTime - Stat_LastReferCallTime));
	Stat_LastReferCallTime = CurrentTime;

	/* ManaPlayerの時刻が単調増加しているか確認 */
	CriUint64 count, unit;
	criManaPlayer_GetTime(ManaPlayer, &count, &unit);
	float ManaPlayerTime = count / (float)unit;
	DECLARE_FLOAT_COUNTER_STAT(TEXT("Mana GetTime Delta"), STAT_ManaGetTimeDelta, STATGROUP_CriWare);
	SET_FLOAT_STAT(STAT_ManaGetTimeDelta, ManaPlayerTime - Stat_LastManaPlayerTime);
	Stat_LastManaPlayerTime = ManaPlayerTime;
#endif

	check(ManaPlayer);

	// discard unused frames
	Movie->IterateWritableFrames([&](auto& ManaFrame) {
		if (ManaFrame.IsValid()) {
			if (ManaFrame.Info.image_info[0].imageptr != nullptr) {
				criManaPlayer_DiscardFrame(ManaPlayer, &ManaFrame.Info);
				ManaFrame.Info.image_info[0].imageptr = nullptr;
			}
		}
	});

	// try to get a new FrameInfo
	CriManaFrameInfo FrameInfo = { 0 };
	bool bSearchNextFrame = CRI_TRUE;
	bool bDrawFrame = false;
	bool bNextFrame = true;
	while (bNextFrame)
	{
		if (criManaPlayer_ReferFrame(ManaPlayer, &FrameInfo) == CRI_TRUE) {
#if STATS
			/* デバッグ向け：Tickが安定しているか確認するため、OnTimeになるまでのReferFrameの回数を計測 */
			Stat_ReferFrameCount++;
#endif
			if (criManaPlayer_IsFrameOnTime(ManaPlayer, &FrameInfo) == CRI_TRUE) {
				// keep frame
				criManaPlayer_KeepFrame(ManaPlayer, &FrameInfo);

				// re-synchronize playback timer with frames
				bool bIsInSync;
				auto PlaybackDeltaTime = SourceStreamer.SynchronizeWithFrame(this, FrameInfo, bIsInSync);
				// trigger new event points
				auto EventPoints = SourceStreamer.GetEventPoints(SourceStreamer.GetMoviePlaybackTime(), -PlaybackDeltaTime);
				for (auto EventPointInfo : EventPoints) {
					OnEventPoint.Broadcast(EventPointInfo);
				}
				// if out-of-sync with ManaTime, decide or not to drop the frame
				if (!bIsInSync &&
					static_cast<int32>(MaxFrameDrop) > 0 &&
					(DroppedFrameCount < static_cast<int32>(MaxFrameDrop) || static_cast<int32>(MaxFrameDrop) > 10)) {
					// discard frame immediatly and loop again to get a new one
					criManaPlayer_DiscardFrame(ManaPlayer, &FrameInfo);
					DroppedFrameCount++;
#if STATS
					/* フレームドロップ数をUE4プロファイラに通知 */
					DECLARE_DWORD_ACCUMULATOR_STAT(TEXT("Mana Dropped Frames"), STAT_ManaDroppedFrames, STATGROUP_CriWare);
					INC_DWORD_STAT_BY(STAT_ManaDroppedFrames, 1);
#endif
					continue;
				}

				// we have a frame to draw
				bDrawFrame = true;
				DroppedFrameCount = 0;
				break;
			} // !frame on time
			break;
		} else { // !refer frame
#if STATS
			/* デコードが間に合わなかった回数を測定 */
			DECLARE_DWORD_ACCUMULATOR_STAT(TEXT("Mana ReferFrame failed"), STAT_ManaReferFailed, STATGROUP_CriWare);
			INC_DWORD_STAT_BY(STAT_ManaReferFailed, 1);
#endif
			break;
		}
	}

	if (bDrawFrame)
	{
		// try get a new frame
		/* テクスチャに動画フレームを提供 */
		auto ManaFrame = Movie->GetWritableFrame();

		if (ManaFrame.IsValid()) {

			// assign new frame info to frame info buffer
			ManaFrame.Info = FrameInfo;

			// assign data
			for (int ImageBufferIndex = 0, DestIndex = 0; ImageBufferIndex < 4; ImageBufferIndex++) {
				if (ManaFrame.Info.image_info[ImageBufferIndex].imageptr != nullptr) {
					ManaFrame.Buffers[DestIndex].imagebuf = ManaFrame.Info.image_info[ImageBufferIndex].imageptr;
					ManaFrame.Buffers[DestIndex].pitch = ManaFrame.Info.image_info[ImageBufferIndex].line_pitch;
					ManaFrame.Buffers[DestIndex].bufsize = ManaFrame.Info.image_info[ImageBufferIndex].bufsize;
					DestIndex++;
				}
			}

			/*
			if (ManaFrame.Info.frame_no == 0) {
				UE_LOG(LogTemp, Log, TEXT("==== ManaFrame[0] ====\n  width=%d height=%d\n  disp_width=%d disp_height=%d\n"), ManaFrame.Info.width, ManaFrame.Info.height, ManaFrame.Info.disp_width, ManaFrame.Info.disp_height);
				UE_LOG(LogTemp, Log, TEXT("-- Texture Y:\n  line_size=%d\n  line_pitch=%d\n  num_lines=%d\n  bufsize=%d\n"), ManaFrame.Info.image_info[0].line_size, ManaFrame.Info.image_info[0].line_pitch, ManaFrame.Info.image_info[0].num_lines, ManaFrame.Info.image_info[0].bufsize);
				UE_LOG(LogTemp, Log, TEXT("-- Texture U:\n  line_size=%d\n  line_pitch=%d\n  num_lines=%d\n  bufsize=%d\n"), ManaFrame.Info.image_info[1].line_size, ManaFrame.Info.image_info[1].line_pitch, ManaFrame.Info.image_info[1].num_lines, ManaFrame.Info.image_info[1].bufsize);
				UE_LOG(LogTemp, Log, TEXT("-- Texture V:\n  line_size=%d\n  line_pitch=%d\n  num_lines=%d\n  bufsize=%d\n"), ManaFrame.Info.image_info[2].line_size, ManaFrame.Info.image_info[2].line_pitch, ManaFrame.Info.image_info[2].num_lines, ManaFrame.Info.image_info[2].bufsize);
				UE_LOG(LogTemp, Log, TEXT("-- Texture A:\n  line_size=%d\n  line_pitch=%d\n  num_lines=%d\n  bufsize=%d\n"), ManaFrame.Info.image_info[3].line_size, ManaFrame.Info.image_info[3].line_pitch, ManaFrame.Info.image_info[3].num_lines, ManaFrame.Info.image_info[3].bufsize);
			}*/

			// store frame to display queue
			Movie->SubmitFrame(ManaFrame);
#if STATS
			/* フレームスキップ数をUE4プロファイラに通知 */
			DECLARE_DWORD_COUNTER_STAT(TEXT("Mana Skipped Frames"), STAT_ManaSkippedFrames, STATGROUP_CriWare);
			SET_DWORD_STAT(STAT_ManaSkippedFrames, FrameInfo.cnt_skipped_frames);

			/* 全フレームドロップ数をUE4プロファイラに通知 */
			DECLARE_DWORD_ACCUMULATOR_STAT(TEXT("Mana Total Dropped Frames"), STAT_ManaTotalDroppedFrames, STATGROUP_CriWare);
			INC_DWORD_STAT_BY(STAT_ManaTotalDroppedFrames, FrameInfo.frame_no - (Stat_LastDecodedFrameNumber + 1));

			/* フレーム番号が１ずつ増加しなかった回数をUE4プロファイラに通知 */
			DECLARE_DWORD_ACCUMULATOR_STAT(TEXT("Mana Discontinuous Frames"), STAT_ManaDiscontinuousFrames, STATGROUP_CriWare);
			if (FrameInfo.frame_no != Stat_LastDecodedFrameNumber + 1) {
				INC_DWORD_STAT_BY(STAT_ManaDiscontinuousFrames, 1);
			}

			/* OnTimeになるまでのReferFrameの回数をUE4プロファイラに通知 */
			DECLARE_DWORD_COUNTER_STAT(TEXT("Mana ReferFrame Count until FrameOnTime"), STAT_ManaReferCount, STATGROUP_CriWare);
			SET_DWORD_STAT(STAT_ManaReferCount, Stat_ReferFrameCount);
			Stat_ReferFrameCount = 0;

			/* UE4プロファイラ向けに統計情報を更新 */
			Stat_LastDecodedFrameNumber = FrameInfo.frame_no;
#endif
		} else {
			// No frame in free queue was available: immediate discard of the current frame.
			/* 書き込み可能なバッファがなかった。
			* レンダリングが追いついていないので、このフレームは破棄する */
			criManaPlayer_DiscardFrame(ManaPlayer, &FrameInfo);
			UE_LOG(LogCriWareRuntime, Warning, TEXT("Video frame dropped by insufficient buffer."));
		}
	}
#if STATS
	/* 関数の時間をUE4プロファイラに通知 */
	uint32 CurrentTime2 = FPlatformTime::Cycles();
	DECLARE_FLOAT_COUNTER_STAT(TEXT("Mana UpdateFrame Process"), STAT_UpdateFrame, STATGROUP_CriWare);
	SET_FLOAT_STAT(STAT_UpdateFrame, FPlatformTime::ToMilliseconds(CurrentTime2 - CurrentTime));
#endif
}

// Playlist

bool UManaComponent::Next()
{
	// change movie from jukebox
	if (Playlist != nullptr) {
		int32 Index = PlaylistIndex + 1;
		bool End = false;
		// check if over
		if (Index >= Playlist->Num()) {
			Index = 0;
			End = !bLoop;
		}
		// update next movie in Texture and set data to mana player
		if (!End) {
			return ToIndex(Index);
		}
	}
	return false;
}

bool UManaComponent::Previous()
{
	// change movie from jukebox
	if (Playlist != nullptr) {
		int32 Index = PlaylistIndex - 1;
		bool End = false;
		// check if over
		if (Index < 0) {
			Index = Playlist->Num() - 1;
			End = !bLoop;
		}
		// update next movie in Texture and set data to mana player
		if (!End) {
			return ToIndex(Index);
		}
	}
	return false;
}

bool UManaComponent::ToIndex(int32 Index)
{
	bool bWasPlaying = IsPlaying();
	Stop();
	if (PrepareManaPlayer(Index)) {
		PlaylistIndex = Index;
		if (bSeamlessEnabled) {
			SeamlessPlaylistIndex = Index;
		}
		if (bWasPlaying) {
			Play();
		}
		return true;
	}
	return false;
}


// TEMP seamless
void UManaComponent::EnableSeamless(bool bEnable)
{
	if (ManaPlayer != nullptr && Source != nullptr && bEnable != bSeamlessEnabled) {
		// default maximum picture data size (0)
		uint32 MaxPictureDataSize = 0;
		if (bEnable) {
			// compute maximum picture data size based on movie size for h264 seamless. (w*h*0.75)
			if (Source->GetVideoTrackType(0) == EManaMovieType::H264) {
				static const float Factor = 0.75f;
				FIntPoint Dimensions = Source->GetVideoTrackTextureDimensions(0);
				MaxPictureDataSize = static_cast<uint32>(Dimensions.X * Dimensions.Y * Factor);
			}
			SeamlessPlaylistIndex = PlaylistIndex;
			// Link seamless callback.
			criManaPlayer_SetDataRequestCallback(ManaPlayer, OnDataRequestCallback, this);
		} else {
			SeamlessPlaylistIndex = 0;
			// Disable seamless.
			criManaPlayer_SetDataRequestCallback(ManaPlayer, nullptr, nullptr);
		}
		// set maximum picture data size
		criManaPlayer_SetMaxPictureDataSize(ManaPlayer, MaxPictureDataSize);

		bSeamlessEnabled = bEnable;
	}
}

// TEMP seamless
void UManaComponent::SetFile(const FString &MovieFilePath)
{
	if (ManaPlayer == nullptr) {
		UE_LOG(LogCriWareRuntime, Error, TEXT("SetFile function is not available until ManaComponent gets initialized."));
		return;
	}

	FString FilePath;
	if (FPaths::IsRelative(MovieFilePath)) {
		if (FilePath.StartsWith(TEXT("./"))) {
			FilePath = FPaths::ConvertRelativePathToFull(UCriWareInitializer::GetContentDir(), MovieFilePath.RightChop(2));
		} else {
			FilePath = FPaths::ConvertRelativePathToFull(UCriWareInitializer::GetContentDir(), MovieFilePath);
		}
	} else {
		FilePath = MovieFilePath;
	}

	if (FPaths::FileExists(FilePath)) {
		criManaPlayer_SetFile(ManaPlayer, nullptr, TCHAR_TO_UTF8(*FilePath));
	} else {
		UE_LOG(LogCriWareRuntime, Error, TEXT("Movie file path is not valid or does not exist: %s"), *FilePath);
	}
}

void UManaComponent::OnDataRequestCallback(void* ManaComponent, CriManaPlayerHn ManaPlayer)
{
	auto* Self = reinterpret_cast<UManaComponent*>(ManaComponent);

	// change movie from jukebox
	if (Self->Playlist != nullptr) {
		Self->SeamlessPlaylistIndex++;
		bool End = false;
		// check if over
		if (Self->SeamlessPlaylistIndex >= Self->Playlist->Num()) {
			Self->SeamlessPlaylistIndex = 0;
			End = !Self->bLoop;
		}
		// update next movie in Texture and set data to mana player
		if (!End) {
			auto MovieSource = Self->Playlist->Get(Self->SeamlessPlaylistIndex);
			if (MovieSource && MovieSource->Validate()) {
				Self->SourceStreamer.AddSource(MovieSource);
				if (MovieSource->GetClass() == UFileManaMovie::StaticClass()) {
					auto FileSource = Cast<UFileManaMovie>(MovieSource);
					criManaPlayer_SetFile(ManaPlayer, nullptr, TCHAR_TO_UTF8(*(FileSource->GetUrl().RightChop(7))));
				}
			} else {
				UE_LOG(LogCriWareRuntime, Error, TEXT("Movie is not specified at playlist index (%d)."), Self->SeamlessPlaylistIndex);
			}
		}
	}

	// broadcast user event
	if (Self->OnRequestData.IsBound()) {
		Self->OnRequestData.Execute(Self);
		//	criManaPlayer_DeferCallback(ManaPlayer);
	}
}

void UManaComponent::OnMovieChangedByManaPlayer()
{
	if (bSeamlessEnabled) {
		// if playlist used
		if (Playlist != nullptr) {
			PlaylistIndex++;
			if (PlaylistIndex >= Playlist->Num()) {
				PlaylistIndex = 0;
			}
			// propage index to texture for update 
			Movie->SetPlaylistMovie(PlaylistIndex);
			// update the current movie source
			auto MovieSource = Playlist->Get(PlaylistIndex);
			if (MovieSource && MovieSource->Validate() && MovieSource->GetClass() == UFileManaMovie::StaticClass()) {
				Source = Cast<UFileManaMovie>(MovieSource);
			}
		}
	} else if (bManaLoop) {
		LoopCount++;
	}

	OnMovieChanged.Broadcast(Source, this);
}

void UManaComponent::SetPlaybackTimer(EManaPlaybackTimer InPlaybackTimer)
{
	PlaybackTimer = InPlaybackTimer;
}

EManaPlaybackTimer UManaComponent::GetPlaybackTimer()
{
	return PlaybackTimer;
}

void UManaComponent::AdvanceTime(FTimespan DeltaTime)
{
	if (PlaybackTimer == EManaPlaybackTimer::Manual && Source != nullptr && !IsPaused()) {
		// ManaPlayer も進行させる
		static const uint64 UnitN = 1000;
		const uint64 UnitD = Source->GetVideoTrackFrameRate(0) * UnitN;
		FTimespan AdvancedTime = FTimespan(UnitN * 10000000UL / UnitD);
		FTimespan TargetTime = SourceStreamer.GetManaTime() + DeltaTime;
		FTimespan PlaybackDeltaTime = FTimespan::Zero();
		for (int i = 1; TargetTime >= SourceStreamer.GetPlaybackTime() + AdvancedTime; i++, AdvancedTime = FTimespan(UnitN * i * 10000000UL / UnitD)) {
			criManaPlayer_AdvanceManualTimer(ManaPlayer);
			PlaybackDeltaTime = AdvancedTime;
		}

		SourceStreamer.AdvanceTime(DeltaTime, PlaybackDeltaTime);
	}
}

void UManaComponent::SetMaxDropFrames(EManaMaxFrameDrop InMaxDropFrames)
{
	MaxFrameDrop = InMaxDropFrames;
}

//----------------------------------------
//  FManaSourceStreamer class
//----------------------------------------

UManaComponent::FManaSourceStreamer::FManaSourceStreamer()
	: ReadHead(nullptr)
	, bDisableCuePoint(false)
	, PlaybackTime(-1)
	, MoviePlaybackTime(-1)
	, ManaTime(-1)
	, MaxPlaybackTime(-1)
	, LoopCount(0)
#if CRIWARE_MANA_PLAYBACKTIMER_MANATIME_DEBUG
	, FrameCount(0)
#endif
{}

TArray<FManaEventPointInfo> UManaComponent::FManaSourceStreamer::GetEventPoints(FTimespan Time, FTimespan Duration)
{
	TArray<FManaEventPointInfo> Results;

	// イベントポイント検索が無効（キューポイント無効フラグがtrueの場合）、何も検索せずに返る
	if (bDisableCuePoint || Duration.GetTicks() == 0) {
		return Results;
	}

	FTimespan OffsetDuration(FTimespan::Zero());
	// get offset from previous sources
	for (auto Node = Sources.GetTail(); Node != ReadHead; Node = Node->GetPrevNode()) {
		OffsetDuration += FTimespan::FromSeconds(Node->GetValue()->GetVideoTrackTotalFrames(0) / Node->GetValue()->GetVideoTrackFrameRate(0));
	}

	// loop on sources in queue from tail to read head
	for (auto Node = Sources.GetTail(); Node != nullptr && Node->GetNextNode() != ReadHead; Node = Node->GetPrevNode()) {
		auto NodeSource = Node->GetValue();

		if (Node == ReadHead) {
			OffsetDuration = FTimespan::Zero();
		} else if (Node != Sources.GetTail()) {
			OffsetDuration -= FTimespan::FromSeconds(NodeSource->GetVideoTrackTotalFrames(0) / NodeSource->GetVideoTrackFrameRate(0));
		}
		// loop on every event points and collect them if in range
		for (int32 i = 0; i < NodeSource->GetNumEventPoints(); i++) {
			auto EventPointInfo = NodeSource->GetEventPointInfo(i);
			auto EventPointTime = FTimespan::FromSeconds(EventPointInfo.Time) - OffsetDuration;

			if (Duration.GetTicks() > 0) {
				if (EventPointTime >= Time && EventPointTime < Time + Duration) {
					Results.Add(EventPointInfo);
				}
			} else if (EventPointTime <= Time && EventPointTime > Time + Duration) {
				Results.Add(EventPointInfo);
			}
		}
	}
	return Results;
}

void UManaComponent::FManaSourceStreamer::AddSource(UManaMovie* InSource)
{
	// add source to queue
	Sources.AddHead(InSource);

	if (ReadHead == nullptr) {
		ReadHead = Sources.GetTail();
		bDisableCuePoint = false;
	}
}

void UManaComponent::FManaSourceStreamer::AdvanceToNextSource(bool InManaLoop)
{
	if (ReadHead != nullptr) {
		// get next source in queue
		auto PrevNode = ReadHead->GetPrevNode();

		if (PrevNode != nullptr) {
			// remove old source to queue
			Sources.RemoveNode(ReadHead->GetNextNode());
			// set read head to next source in queue
			ReadHead = PrevNode;
		} else {
			// no more source -> disable EventPoints
			bDisableCuePoint = true;
		}
	} else {
		// set read head on tail at start
		ReadHead = Sources.GetTail();
		bDisableCuePoint = false;
	}

	// always enable if mana player is looping
	if (InManaLoop) {
		bDisableCuePoint = false;
	}
}

UManaMovie* UManaComponent::FManaSourceStreamer::GetCurrentSource()
{
	return ReadHead ? ReadHead->GetValue() : nullptr;
}

void UManaComponent::FManaSourceStreamer::Empty()
{
	// reset read head and queue
	ReadHead = nullptr;
	Sources.Empty();
	bDisableCuePoint = true;
}

void UManaComponent::FManaSourceStreamer::Reset(int32 Frame)
{
	if (Frame <= 0) {
		PlaybackTime = FTimespan(-1);
	} else {
		PlaybackTime = FTimespan::FromSeconds(Frame / GetCurrentSource()->GetVideoTrackFrameRate(0));
	}
	ManaTime = MoviePlaybackTime = MaxPlaybackTime = PlaybackTime;
	LoopCount = 0;
#if CRIWARE_MANA_PLAYBACKTIMER_MANATIME_DEBUG
	FrameCount = 0;
#endif
#if CRIWARE_MANA_FRAMEDROP_GAMETIME_DEBUG
	GameTime = PlaybackTime;
#endif
}

FTimespan UManaComponent::FManaSourceStreamer::Update(UManaComponent* Owner, CriManaPlayerHn InManaPlayer)
{
	FTimespan ManaDeltaTime = FTimespan::Zero();

	switch (Owner->PlaybackTimer) {
	case EManaPlaybackTimer::Default:
	case EManaPlaybackTimer::RealTime:
	{
		// RealTime and DeltaTime (just want delta, so use int64)
		CriUint64 count, unit;
		criManaPlayer_GetTime(InManaPlayer, &count, &unit);
		auto NewManaTime = FTimespan(count * 10000000UL / unit);
		ManaDeltaTime = NewManaTime - ManaTime;
		ManaTime = NewManaTime;
		// only if mana is playing

		// check and limit delta if too fast (after expected next frame time)
		auto NewPlaybackTime = PlaybackTime + ManaDeltaTime;
		if (NewPlaybackTime > MaxPlaybackTime) {
			NewPlaybackTime = MaxPlaybackTime;
			ManaDeltaTime = NewPlaybackTime - PlaybackTime;
		}

		PlaybackTime = NewPlaybackTime;
		MoviePlaybackTime += ManaDeltaTime;
	}
		break;
	case EManaPlaybackTimer::FrameTime:
	{
		// update ManaTime only
		CriUint64 count, unit;
		criManaPlayer_GetTime(InManaPlayer, &count, &unit);
		ManaTime = FTimespan(count * 10000000UL / unit);
	}
		break;
	case EManaPlaybackTimer::Manual:
		break;
#if CRIWARE_MANA_PLAYBACKTIMER_MANATIME_DEBUG
	case EManaPlaybackTimer::ManaTime:
	{
		// PlaybackTime and Mana delta time
		CriUint64 count, unit;
		criManaPlayer_GetTime(InManaPlayer, &count, &unit);
		auto NewPlaybackTime = FTimespan(count * 10000000UL / unit);
		ManaDeltaTime = NewPlaybackTime - PlaybackTime;
		PlaybackTime = NewPlaybackTime;

		// Durations
		float FrameRate = GetCurrentSource()->GetVideoTrackFrameRate(0);
		int32 TotalFrames = GetCurrentSource()->GetVideoTrackTotalFrames(0);
		FTimespan MovieDuration = FTimespan::FromSeconds(TotalFrames / FrameRate);
		FTimespan TotalDuration = FTimespan::FromSeconds(FrameCount / FrameRate);

		// reset frame counter
		if (TotalDuration > PlaybackTime) {
			FrameCount = 0;
			ManaDeltaTime = 0;
			TotalDuration = FTimespan::Zero();
		}

		auto NewMoviePlaybackTime = PlaybackTime - TotalDuration;
		// if movie change occur, use next source in streamer queue
		if (NewMoviePlaybackTime >= MovieDuration && (Owner->bSeamlessEnabled || Owner->bManaLoop)) {
			AdvanceToNextSource(Owner->bManaLoop);
			FrameCount += TotalFrames;
			NewMoviePlaybackTime -= MovieDuration;
			Owner->OnMovieChangedByManaPlayer();
		}

		MoviePlaybackTime = NewMoviePlaybackTime;
	}
		break;
#endif
	}
#if CRIWARE_MANA_FRAMEDROP_GAMETIME_DEBUG
	if (PlaybackTime >= FTimespan::Zero() && Owner->IsPlaying() && !Owner->IsPaused()) {
		float DeltaTime = Owner->GetWorld()->GetDeltaSeconds();
		GameTime += FTimespan::FromSeconds(DeltaTime);
	}
#endif
	return ManaDeltaTime;
}

FTimespan UManaComponent::FManaSourceStreamer::SynchronizeWithFrame(UManaComponent* Owner, const CriManaFrameInfo& FrameInfo, bool& bIsInSync)
{
	FTimespan ManaDeltaTime = FTimespan::Zero();
	bIsInSync = true;

	switch (Owner->PlaybackTimer) {
	case EManaPlaybackTimer::Default:
	case EManaPlaybackTimer::RealTime:
	case EManaPlaybackTimer::FrameTime:
	case EManaPlaybackTimer::Manual:
	{
		// compute frame and next frame time. Use double -> best precision for multiply than int32.
		double FrameDuration = static_cast<double>(FrameInfo.framerate_d) / FrameInfo.framerate_n;
		auto NewPlaybackTime = FTimespan::FromSeconds(FrameInfo.frame_no * FrameDuration);
		auto NewMoviePlaybackTime = FTimespan::FromSeconds(FrameInfo.frame_no_per_file * FrameDuration);
		auto FrameDurationTime = FTimespan::FromSeconds(FrameDuration);

		ManaDeltaTime = NewPlaybackTime - PlaybackTime;

		// time fence
		MaxPlaybackTime = NewPlaybackTime + FrameDurationTime - FTimespan(1);

		// check if movie reloop or changed
		if (NewMoviePlaybackTime < MoviePlaybackTime) {
			AdvanceToNextSource(Owner->bManaLoop);
			NewMoviePlaybackTime = FTimespan::Zero();
			Owner->OnMovieChangedByManaPlayer();
		}

		PlaybackTime = NewPlaybackTime;
		MoviePlaybackTime = NewMoviePlaybackTime;

		auto DropTime = PlaybackTime + FrameDurationTime;
		// check if mana is away
		if (ManaTime >= DropTime) { // ManaTime sync
#if CRIWARE_MANA_FRAMEDROP_GAMETIME_DEBUG
			if (GameTime >= DropTime) // GameTime Sync - may result to playback delay with 'fixed framerate'
#endif
				bIsInSync = false;
		}
	}
		break;
	}

	return ManaDeltaTime;
}

void UManaComponent::FManaSourceStreamer::AdvanceTime(FTimespan ManaDeltaTime, FTimespan PlaybackDeltaTime)
{
	ManaTime += ManaDeltaTime;
	PlaybackTime += PlaybackDeltaTime;
	MoviePlaybackTime += PlaybackDeltaTime;
}

/***************************************************************************
 *      関数定義
 *      Function Definition
 ***************************************************************************/
#undef LOCTEXT_NAMESPACE

/* --- end of file --- */
