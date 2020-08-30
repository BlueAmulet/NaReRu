/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Mana movie asset
 * File     : ManaMovie.cpp
 *
 ****************************************************************************/
#include "ManaMovie.h"

#include "CriWareRuntimePrivatePCH.h"

#include "UObject/Package.h"
#include "Serialization/CustomVersion.h"
#include "Misc/Paths.h"

#if WITH_EDITORONLY_DATA
#include "EditorFramework/AssetImportData.h"
#endif


#define LOCTEXT_NAMESPACE "ManaMovie"

 //----------------------------------------
 // FManaMovieCustomVersion struct
 //----------------------------------------

 /* ManaTextureパッケージのバージョン情報 */
struct FManaMovieCustomVersion
{
	enum Type
	{
		// ManaTextureにバージョン番号が付与される前
		BeforeCustomVersionWasAdded = 0,

		// 最新バージョンの番号を取得するための番兵
		VersionPlusOne,

		// 最新バージョンの番号を指す
		LatestVersion = VersionPlusOne - 1,
	};

	static const FGuid GUID;
};

const FGuid FManaMovieCustomVersion::GUID(0x122C2078, 0x957272B6, 0xB166BB9A, 0xF49AD29E);

/* シリアライズ時に最新バージョンを埋め込み */
FCustomVersionRegistration GRegisterManaMovieCustomVersion(FManaMovieCustomVersion::GUID, FManaMovieCustomVersion::LatestVersion, TEXT("ManaMovieVer"));

//----------------------------------------
// FManaVideoTrackInfo struct
//----------------------------------------
FManaVideoTrackInfo::FManaVideoTrackInfo()
	:TextureDimensions(0, 0)
	, DisplayDimensions(0, 0)
	, FrameRate(0)
	, TotalFrames(0)
	, bIsAlpha(false)
	, Type(EManaMovieType::Unknown)
{
#if WITH_EDITOR
	/* Localization of unreal properties metadata with LOCTEXT markups and reflection */
	CRI_LOCSTRUCT(StaticStruct());
#endif
}

//----------------------------------------
// FManaAudioTrackInfo struct
//----------------------------------------
FManaAudioTrackInfo::FManaAudioTrackInfo()
	:NumChannels(1)
	, SamplingRate(44100)
	, TotalSamples(0)
	, bIsAmbisonics(false)
	, Type(EManaSoundType::Unknown)
{
#if WITH_EDITOR
	/* Localization of unreal properties metadata with LOCTEXT markups and reflection */
	CRI_LOCSTRUCT(StaticStruct());
#endif
}

//----------------------------------------
// UManaMovie class
//----------------------------------------

UManaMovie::UManaMovie(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bIsAlpha(false)
	, bIsRequestingMovieInfo(false)
	, ManaPlayer(nullptr)
{
#if WITH_EDITOR
	// Localization of unreal properties metadata with LOCTEXT markups and reflection
	CRI_LOCCLASS(GetClass());
#endif
}

/* UObject  overrides
*****************************************************************************/

#if WITH_EDITOR
void UManaMovie::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UManaMovie::PostInitProperties()
{
#if WITH_EDITORONLY_DATA
	if (!HasAnyFlags(RF_ClassDefaultObject | RF_NeedLoad)) {
		AssetImportData = NewObject<UAssetImportData>(this, TEXT("AssetImportData"));
	}
#endif
	Super::PostInitProperties();
}

void UManaMovie::PostLoad()
{
	// try reload movie only if needed
	if (Validate() == false) {
		bool bMovieInfoReloadSucceed = ReloadMovieInfo();
		if (!bMovieInfoReloadSucceed) {
			UE_LOG(LogCriWareRuntime, Warning, TEXT("UManaMovie::PostLoad: Failed to parse movie infomation. Please check the movie path."));
		}
	}

#if WITH_EDITORONLY_DATA
	if (AssetImportData == nullptr) {
		AssetImportData = NewObject<UAssetImportData>(this, TEXT("AssetImportData"));
	}

	bool bUpdated = false;
	/* 最後にSerializeしたときのバージョン番号を取得 */
	const auto ManaTextureVer = GetLinkerCustomVersion(FManaMovieCustomVersion::GUID);

	/* - UObjectBaseUtility::MarkPackageDirty関数を参考。エディタ上でも保存できるようにする。 */
	if (bUpdated) {
		auto Package = GetOutermost();
		if (Package) {
			Package->SetDirtyFlag(true);
			Package->PackageMarkedDirtyEvent.Broadcast(Package, true);
		}
	}
#endif

	Super::PostLoad();
}

void UManaMovie::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	/* 書き込み時に最新のバージョン番号を埋め込み */
	Ar.UsingCustomVersion(FManaMovieCustomVersion::GUID);
}

// FTickableGameObject interface

void UManaMovie::Tick(float /*DeltaTime*/)
{
	if (bIsRequestingMovieInfo) {
		if (IsManaPlayerDecodingHeader(ManaPlayer)) {
			criMana_ExecuteMain();
		}
		else {
			bIsRequestingMovieInfo = false; // stop tick
			CriManaMovieInfo MovieInfo = GetMovieInfo(ManaPlayer, EventPoints);
			DestroyManaPlayer(ManaPlayer);
			ManaPlayer = nullptr;
			LoadMovieInfo(MovieInfo);
		}
	}
}

//~ FTickableGameObject interface

bool UManaMovie::LoadMovieInfo(const CriManaMovieInfo& MovieInfo)
{
	if (MovieInfo.is_playable == 0) {
		return false;
	}

	/* トラック情報を格納する配列を確保 */
	VideoTracks.SetNumUninitialized(MovieInfo.num_video_streams);

	/* 動画トラック情報を取得 */
	const CriManaVideoInfo* ManaVideoInfo = MovieInfo.video_prm;
	for (auto& VideoInfo : VideoTracks) {
		VideoInfo.TextureDimensions = FIntPoint(ManaVideoInfo->width, ManaVideoInfo->height);
		VideoInfo.DisplayDimensions = FIntPoint(ManaVideoInfo->disp_width, ManaVideoInfo->disp_height);
		VideoInfo.FrameRate = ManaVideoInfo->framerate / 1000.0f;
		VideoInfo.TotalFrames = ManaVideoInfo->total_frames;
		VideoInfo.bIsAlpha = (MovieInfo.num_alpha_streams > 0);	/* 各トラックにアルファ、非アルファムービが混在しない、と前提 */
		switch (ManaVideoInfo->codec_type) {
		case CRIMANA_VIDEO_CODEC_SOFDEC_PRIME: VideoInfo.Type = EManaMovieType::SofdecPrime; break;
		case CRIMANA_VIDEO_CODEC_H264: VideoInfo.Type = EManaMovieType::H264; break;
		case CRIMANA_VIDEO_CODEC_VP9: VideoInfo.Type = EManaMovieType::VP9; break;
		case CRIMANA_VIDEO_CODEC_UNKNOWN:
		default: VideoInfo.Type = EManaMovieType::Unknown;
		}

		/* 次の動画トラック情報を参照 */
		++ManaVideoInfo;
	}

	if (VideoTracks.Num() > 0) {
		bIsAlpha = VideoTracks[0].bIsAlpha;
	}

	// audio
	AudioTracks.SetNumUninitialized(MovieInfo.num_audio_streams);

	const CriManaAudioInfo* ManaAudioInfo = MovieInfo.audio_prm;
	for (auto& AudioInfo : AudioTracks) {
		AudioInfo.NumChannels = ManaAudioInfo->num_channels;
		AudioInfo.SamplingRate = ManaAudioInfo->sampling_rate;
		AudioInfo.TotalSamples = ManaAudioInfo->total_samples;
		AudioInfo.bIsAmbisonics = (ManaAudioInfo->ambisonics > 0);
		switch (ManaAudioInfo->codec_type) {
		case CRIMANA_AUDIO_CODEC_ADX: AudioInfo.Type = EManaSoundType::Adx; break;
		case CRIMANA_AUDIO_CODEC_HCA: AudioInfo.Type = EManaSoundType::HCA; break;
		case CRIMANA_AUDIO_CODEC_UNKNOWN:
		default: AudioInfo.Type = EManaSoundType::Unknown;
		}

		++ManaAudioInfo;
	}

	// subtitles
	NumSubtitleChannels = (int32)MovieInfo.num_subtitle_channels;
	MaxSubtitleSize = MovieInfo.max_subtitle_size;

	return true;
}

void UManaMovie::AsyncLoadMovie(CriManaPlayerHn InManaPlayer)
{
	ManaPlayer = InManaPlayer;
	ManaPlayerDecodeHeader(ManaPlayer);
	bIsRequestingMovieInfo = true; // start tick
}

void UManaMovie::Clear()
{
	// clear all info
	VideoTracks.Empty();
	AudioTracks.Empty();
	EventPoints.Empty();
	bIsAlpha = false;
	NumSubtitleChannels = 0;
	MaxSubtitleSize = 0;
}

/* ムービーの情報を取得するため、ムービーのヘッダーを解析する */
CriManaPlayerHn UManaMovie::CreateManaPlayer()
{
	/* Manaを利用するかどうかチェック */
	if (criMana_IsInitialized() == CRI_FALSE) {
		return nullptr;
	}

	/* ヘッダーをデコード */
	CriManaPlayerHn ManaPlayer = criManaPlayer_Create(NULL, 0);
	if (!ManaPlayer) {
		return nullptr;
	}

	criManaPlayer_SetMetaDataWorkAllocator(ManaPlayer,
		[](void *, CriUint32 size) { return FMemory::Malloc(size); },
		[](void *, void* ptr) { return FMemory::Free(ptr); },
		nullptr,
		CRIMANA_META_FLAG_ALL
	);

	return ManaPlayer;
}

/* ムービーヘッダーのデコード中かどうかを返す */
bool UManaMovie::IsManaPlayerDecodingHeader(CriManaPlayerHn ManaPlayer)
{
	return criManaPlayer_GetStatus(ManaPlayer) < CRIMANAPLAYER_STATUS_WAIT_PREP;
}


void UManaMovie::ManaPlayerDecodeHeader(CriManaPlayerHn ManaPlayer)
{
	if (ManaPlayer == nullptr) { return; }

	criManaPlayer_DecodeHeader(ManaPlayer);
}

CriManaMovieInfo UManaMovie::GetMovieInfo(CriManaPlayerHn ManaPlayer, TArray<FManaEventPointInfo>& EventPoints)
{
	/* 失敗時には、再生不可能なムービー情報として返す。 */
	CriManaMovieInfo MovieInfo;
	MovieInfo.is_playable = 0;

	if (ManaPlayer == nullptr) { return MovieInfo; }

	/* ムービー情報を取得 */
	criManaPlayer_GetMovieInfo(ManaPlayer, &MovieInfo);

	/* キューポイント情報を取得 */
	const CriManaCuePointInfo* CuePointInfo = criManaPlayer_GetCuePointInfo(ManaPlayer);
	if ((CuePointInfo != nullptr) && (CuePointInfo->num_eventpoint > 0)) {
		/* イベントポイント情報を格納する配列を確保 */
		EventPoints.SetNumZeroed(CuePointInfo->num_eventpoint);

		/* イベントポイント情報の保存 */
		const CriManaEventPoint* EventPoint = CuePointInfo->eventtable;
		for (auto& EventPointInfo : EventPoints) {
			EventPointInfo.Name = ((EventPoint->cue_name != nullptr) ? (UTF8_TO_TCHAR(EventPoint->cue_name)) : TEXT(""));
			EventPointInfo.Time = ((EventPoint->tunit > 0) ? ((float)EventPoint->time / (float)EventPoint->tunit) : 0.0f);
			EventPointInfo.Type = EventPoint->type;
			EventPointInfo.Parameter = ((EventPoint->param_string != nullptr) ? (UTF8_TO_TCHAR(EventPoint->param_string)) : TEXT(""));
			++EventPoint;
		}
	}

	return MovieInfo;
}

void UManaMovie::ManaPlayerSetFile(CriManaPlayerHn ManaPlayer, const FString& FilePath)
{
	if (ManaPlayer == nullptr) { return; }
	criManaPlayer_SetFile(ManaPlayer, NULL, TCHAR_TO_UTF8(*FilePath));
}

void UManaMovie::ManaPlayerSetData(CriManaPlayerHn ManaPlayer, const TArray<uint8>& DataArray)
{
	if (ManaPlayer == nullptr) { return; }
	criManaPlayer_SetData(ManaPlayer, DataArray.GetData(), DataArray.Num());
}

void UManaMovie::DestroyManaPlayer(CriManaPlayerHn ManaPlayer)
{
	if (ManaPlayer == nullptr) { return; }
	/* ManaPlayerを破棄 */
	criManaPlayer_StopAndWaitCompletion(ManaPlayer);
	criManaPlayer_Destroy(ManaPlayer);
}

//----------------------------------------
// UFileManaMovie class
//----------------------------------------

UFileManaMovie::UFileManaMovie(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	// Localization of unreal properties metadata with LOCTEXT markups and reflection
	CRI_LOCCLASS(GetClass());
#endif
}

/* UObject  overrides
*****************************************************************************/

#if WITH_EDITOR
void UFileManaMovie::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property != nullptr) {
		FName PropertyName = PropertyChangedEvent.Property->GetFName();

#if WITH_EDITORONLY_DATA
		if (PropertyName == GET_MEMBER_NAME_CHECKED(UManaMovie, AssetImportData->SourceData)) {
			SetFilePath(AssetImportData->GetFirstFilename());
		}
#endif
		if (PropertyName == GET_MEMBER_NAME_CHECKED(UFileManaMovie, FilePath)) {
			bool bMovieInfoReloadSucceed = ReloadMovieInfo();
			if (!bMovieInfoReloadSucceed) {
				AssetImportData->Update(TEXT(""));
				UE_LOG(LogCriWareRuntime, Warning, TEXT("UManaMovie::PostEditChangeProperty: Failed to parse movie infomation. Please check the movie file path."));
			}
			else {
				AssetImportData->Update(GetFullPath());
			}
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

/* UManaMovie overrides
*****************************************************************************/

FString UFileManaMovie::GetUrl() const
{
	FString FullPath = GetFullPath();
	if (!FullPath.IsEmpty()) {
		return FString(TEXT("file://")) + FullPath;
	}
	return FullPath;
}

bool UFileManaMovie::Validate() const
{
	if (!FPaths::FileExists(GetFullPath())) {
		return false;
	}

	if (VideoTracks.Num() <= 0) {
		return false;
	}

	return true;
}

bool UFileManaMovie::ReloadMovieInfo()
{
	if (IsValidating()) { return true; }

	// clear data
	Clear();
	// check if path is present
	if (FilePath.IsEmpty()) {
		return false;
	}
	// request movie info
	FString Path = GetFullPath();
	CriManaPlayerHn Player = CreateManaPlayer();
	ManaPlayerSetFile(Player, Path);
	AsyncLoadMovie(Player);

	return true;
}


/* UFileManaMovie implementation
*****************************************************************************/

void UFileManaMovie::SetFilePath(const FString& Path)
{
	FilePath = Path;
	ReloadMovieInfo();
}

FString UFileManaMovie::GetFullPath() const
{
	if (!FPaths::IsRelative(FilePath)) {
		return FilePath;
	}

	if (FilePath.StartsWith(TEXT("./"))) {
		return FPaths::ConvertRelativePathToFull(UCriWareInitializer::GetContentDir(), FilePath.RightChop(2));
	}

	return FPaths::ConvertRelativePathToFull(UCriWareInitializer::GetContentDir(), FilePath);
}

//----------------------------------------
// UDataManaMovie class
//----------------------------------------

UDataManaMovie::UDataManaMovie(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	// Localization of unreal properties metadata with LOCTEXT markups and reflection
	CRI_LOCCLASS(GetClass());
#endif
}

bool UDataManaMovie::ReloadMovieInfo()
{
	if (IsValidating()) { return true; }

	// clear data
	Clear();
	// Check if data are present
	if (DataArray == nullptr || DataArray->Num() == 0) {
		return false;
	}
	// request movie info
	CriManaPlayerHn Player = CreateManaPlayer();
	ManaPlayerSetData(Player, *DataArray);
	AsyncLoadMovie(Player);

	return true;
}

void UDataManaMovie::SetDataArray(TArray<uint8>& InDataArray)
{
	DataArray = &InDataArray;
	ReloadMovieInfo();
}

bool UDataManaMovie::Validate() const
{
	if (DataArray == nullptr || DataArray->Num() == 0) {
		return false;
	}

	if (VideoTracks.Num() <= 0) {
		return false;
	}

	return true;
}

#undef LOCTEXT_NAMESPACE
