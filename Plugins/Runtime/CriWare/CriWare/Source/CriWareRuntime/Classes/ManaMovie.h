/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2017-2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Mana Movie asset
 * File     : ManaMovie.h
 *
 ****************************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "Tickable.h"
#include "CriWareApi.h"
#include "ManaSource.h"
#include "ManaMovie.generated.h"

class UManaTexture;

/** Types of sofdec2 video track */
UENUM(BlueprintType)
enum class EManaMovieType : uint8
{
	/** Sofdec Prime movie */
	SofdecPrime = 0,
	/** H.264 movie */
	H264,
	/** VP9 movie */
	VP9,

	Num	UMETA(hidden),
	/** Unknown movie */
	Unknown = 0xFF
};

UENUM()
enum class EManaColorSpace : uint8
{
	/** Rec. 601 Color Space */
	Rec601 = 0,
	/** Rec. 709 Color Space */
	Rec709,

	Num UMETA(hidden),
};

/** Types of sofdec2 audio track */
UENUM(BlueprintType)
enum class EManaSoundType : uint8
{
	/** Advanced ADX sound */
	Adx = 0,
	/** High Compression Audio sound */
	HCA,

	Num	UMETA(hidden),
	/** Unknown sound */
	Unknown = 0xFF
};

/** 動画トラック情報を格納する構造体 */
USTRUCT(BlueprintType, meta = (ToolTip = "ManaVideoTrackInfo structure."))
struct FManaVideoTrackInfo
{
	GENERATED_USTRUCT_BODY()

	FManaVideoTrackInfo();

	/* ムービーの最大縦横サイズ（8の倍数） */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana, meta = (ToolTip = "Texture dimensions."))
	FIntPoint TextureDimensions;

	/* 表示する映像の縦横サイズ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana, meta = (ToolTip = "Display dimensions."))
	FIntPoint DisplayDimensions;

	/* 1秒間のフレーム数 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana, meta = (ToolTip = "Frame rate."))
	float FrameRate;

	/* 総フレーム数 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana, meta = (ToolTip = "Total frames."))
	int32 TotalFrames;

	/* アルファ（透過度）を含むムービかどうか */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana, meta = (ToolTip = "Is a video with alpha channel?"))
	bool bIsAlpha;

	/* ムービータイプ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana, meta = (ToolTip = "Type of video."))
	EManaMovieType Type;
};

/** 動画トラック情報を格納する構造体 */
USTRUCT(BlueprintType, meta = (ToolTip = "ManaAudioTrackInfo structure."))
struct FManaAudioTrackInfo
{
	GENERATED_USTRUCT_BODY()

	FManaAudioTrackInfo();

	/* オーディオチャネル数 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana, meta = (ToolTip = "Number of audio channels."))
	int32 NumChannels;

	/* サンプリング周波数 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana, meta = (ToolTip = "Sampling rate."))
	int32 SamplingRate;

	/* 総サンプル数 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana, meta = (ToolTip = "Total samples."))
	int32 TotalSamples;

	/* Ambisonics */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana, meta = (ToolTip = "Is an audio using Ambisonics?"))
	bool bIsAmbisonics;

	/* オーディオタイプ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana, meta = (ToolTip = "Type of audio."))
	EManaSoundType Type;
};

/** イベントポイント情報を格納する構造体 */
USTRUCT(BlueprintType, meta = (ToolTip = "ManaCuePoint structure."))
struct FManaEventPointInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mana)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mana)
	float Time;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mana)
	int32 Type;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mana)
	FString Parameter;
};

// forward declration
class UPlatformManaMovie;

/** ManaMovie abstract class represents a Sofdec2 Movie base for any type of Movie source */
UCLASS(Abstract, BlueprintType)
class CRIWARERUNTIME_API UManaMovie : public UManaSource, public FTickableGameObject
{
	GENERATED_UCLASS_BODY()

public:
#if WITH_EDITOR
	/** USMファイルパスがエディタ上で変更されたときの処理 */
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	/** After initializing properties. */
	virtual void PostInitProperties() override;

	/** After loading asset. */
	virtual void PostLoad() override;

	/** When serializing asset. */
	virtual void Serialize(FArchive& Ar) override;

	// FTickableGameObject interface
	void Tick(float DeltaTime) override;
	bool IsTickable() const override { return bIsRequestingMovieInfo; }
	bool IsTickableInEditor() const override { return true; }
	bool IsTickableWhenPaused() const override { return true; }
	TStatId GetStatId() const override { return TStatId(); }
	//~ FTickableGameObject interface

public:
	// UManaMovie API

	// tracks accessors
	/** */
	int32 GetNumVideoTracks() const { return VideoTracks.Num(); }

	/** */
	FIntPoint GetVideoTrackTextureDimensions(int32 TrackIndex) const
	{
		return VideoTracks.IsValidIndex(TrackIndex) ? VideoTracks[TrackIndex].TextureDimensions : FIntPoint();
	}

	/** */
	FIntPoint GetVideoTrackDisplayDimensions(int32 TrackIndex) const
	{
		return VideoTracks.IsValidIndex(TrackIndex) ? VideoTracks[TrackIndex].DisplayDimensions : FIntPoint();
	}

	/** */
	float GetVideoTrackFrameRate(int32 TrackIndex) const
	{
		return VideoTracks.IsValidIndex(TrackIndex) ? VideoTracks[TrackIndex].FrameRate : 0.0f;
	}

	/** */
	int32 GetVideoTrackTotalFrames(int32 TrackIndex) const
	{
		return VideoTracks.IsValidIndex(TrackIndex) ? VideoTracks[TrackIndex].TotalFrames : 0;
	}

	/** */
	bool IsVideoTrackAlpha(int32 TrackIndex) const
	{
		return VideoTracks.IsValidIndex(TrackIndex) ? VideoTracks[TrackIndex].bIsAlpha : false;
	}

	/** */
	EManaMovieType GetVideoTrackType(int32 TrackIndex) const
	{
		return VideoTracks.IsValidIndex(TrackIndex) ? VideoTracks[TrackIndex].Type : EManaMovieType::Unknown;
	}

	/** */
	int32 GetNumAudioTracks() const { return AudioTracks.Num(); }

	/** */
	int32 GetAudioTrackNumChannels(int32 TrackIndex) const
	{
		return AudioTracks.IsValidIndex(TrackIndex) ? AudioTracks[TrackIndex].NumChannels : 0;
	}

	/** */
	int32 GetAudioTrackSamplingRate(int32 TrackIndex) const
	{
		return AudioTracks.IsValidIndex(TrackIndex) ? AudioTracks[TrackIndex].SamplingRate : 0;
	}

	/** */
	int32 GetAudioTrackTotalSamples(int32 TrackIndex) const
	{
		return AudioTracks.IsValidIndex(TrackIndex) ? AudioTracks[TrackIndex].TotalSamples : 0;
	}

	/** */
	bool IsAudioTrackAmbisonics(int32 TrackIndex) const
	{
		return AudioTracks.IsValidIndex(TrackIndex) ? AudioTracks[TrackIndex].bIsAmbisonics : false;
	}

	/** */
	EManaSoundType GetAudioTrackType(int32 TrackIndex) const
	{
		return AudioTracks.IsValidIndex(TrackIndex) ? AudioTracks[TrackIndex].Type : EManaSoundType::Unknown;
	}

	/** */
	int32 GetNumEventPoints() const { return EventPoints.Num(); }

	/** */
	FManaEventPointInfo GetEventPointInfo(int32 Index) const
	{
		return EventPoints.IsValidIndex(Index) ? EventPoints[Index] : FManaEventPointInfo();
	}

	/** */
	int32 GetNumSubtitleChannels() const { return NumSubtitleChannels; }

	/** Get maximum size in subtible that is used by movie. */
	uint32 GetMaxSubtitleSize() const { return MaxSubtitleSize; }

	/** Check if pending movie validation */
	bool IsValidating() const { return bIsRequestingMovieInfo; }

	/** アルファムービかどうかを返す */
	bool IsAlpha() const { return bIsAlpha; }

	/** Validate the movie source settings (must override in child classes). */
	virtual bool Validate() const PURE_VIRTUAL(UManaMovie::Validate, return false;);

	/** Get the movie source's URL string (must override in child classes). */
	virtual FString GetUrl() const PURE_VIRTUAL(UManaMovie::GetUrl, return FString(););

protected:
	/** 動画トラック情報 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana, meta = (ToolTip = "Video track info."))
	TArray<FManaVideoTrackInfo> VideoTracks;

	/** 音楽トラック情報 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana, meta = (ToolTip = "Audio track info."))
	TArray<FManaAudioTrackInfo> AudioTracks;

	/** イベントポイント情報 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mana, meta = (ToolTip = "Event point info."))
	TArray<FManaEventPointInfo> EventPoints;

	/** Number of subtitles channels. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana, meta = (DisplayName = "Number of Subtitles Channels"))
	int32 NumSubtitleChannels;

	/** Maximum size of bytes needed for subtitle */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana, meta = (ToolTip = "Maximun subtitle size in bytes."))
	int32 MaxSubtitleSize;

	/** If the movie use alpha */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana, meta = (ToolTip = "True if movie use alpha channel."))
	uint32 bIsAlpha : 1;

#if WITH_EDITORONLY_DATA
public:
	/** Importing data and options used for this movie */
	UPROPERTY(VisibleAnywhere, Instanced, Category = ImportSettings)
	class UAssetImportData* AssetImportData;

	/** */
	UPROPERTY()
	UManaTexture* Thumbnail;

	/** Information for thumbnail rendering */
	UPROPERTY(VisibleAnywhere, Instanced, Category = Thumbnail)
	class UThumbnailInfo* ThumbnailInfo;
#endif

protected:
	/** Reload movie information from Sofdec2 movie source.
	*  @return true if information was effectively reloaded.
	*/
	virtual bool ReloadMovieInfo() PURE_VIRTUAL(UManaMovie::ReloadMovieInfo, return false;);

	/** Load from MovieInfo struct */
	bool LoadMovieInfo(const CriManaMovieInfo& MovieInfo);

	/** Async request to get MovieInfo */
	void AsyncLoadMovie(CriManaPlayerHn InManaPlayer);

	/** Clear Movie*/
	virtual void Clear();

private:
	bool bIsRequestingMovieInfo;
	CriManaPlayerHn ManaPlayer;

	friend class UPlatformManaMovie;

protected:
	/** Utilities with Sofdec2 */
	static CriManaPlayerHn CreateManaPlayer();
	static void ManaPlayerDecodeHeader(CriManaPlayerHn ManaPlayer);
	static bool IsManaPlayerDecodingHeader(CriManaPlayerHn ManaPlayer);
	static CriManaMovieInfo GetMovieInfo(CriManaPlayerHn ManaPlayer, TArray<FManaEventPointInfo>& EventPoints);
	static void ManaPlayerSetFile(CriManaPlayerHn ManaPlayer, const FString& FilePath);
	static void ManaPlayerSetData(CriManaPlayerHn ManaPlayer, const TArray<uint8>& DataArray);
	static void DestroyManaPlayer(CriManaPlayerHn ManaPlayer);
};

UCLASS(BlueprintType, hidecategories = (Object), meta = (ToolTip = "FileManaMovie class."))
class CRIWARERUNTIME_API UFileManaMovie : public UManaMovie
{
	GENERATED_UCLASS_BODY()

public:
#if WITH_EDITOR
	/** USMファイルパスがエディタ上で変更されたときの処理 */
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	/** Load entire media file into memory and play from there (if possible). */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = File, AdvancedDisplay)
	bool bPrecacheFile;

	/**
	* Get the path to the sofdec2 usm file to be played.
	*
	* @return The file path.
	* @see SetFilePath
	*/
	const FString& GetFilePath() const
	{
		return FilePath;
	}

	/**
	* Set the path to the sofdec2 usm file that this source represents.
	*
	* Automatically converts full paths to media sources that reside in the
	* Engine's or project's directory into relative paths to Criware FileSystem path settings.
	*
	* @param Path The path to set.
	* @see FilePath, GetFilePath
	*/
	UFUNCTION(BlueprintCallable, Category = "Mana|FileManaMovie")
	void SetFilePath(const FString& Path);

	// ManaMovie overrides

	bool Validate() const override;

	FString GetUrl() const override;

protected:
	/**
	* The path to the media file to be played.
	*
	* @see SetFilePath
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = File, AssetRegistrySearchable)
	FString FilePath;

	/** Reload movie information from Sofdec2 movie file.
	 *
	 *  @return true if information was reloaed
	 */
	bool ReloadMovieInfo() override;

	/** Get the full path to the file. */
	FString GetFullPath() const;
};

UCLASS(BlueprintType, hidecategories = (Object), meta = (ToolTip = "DataManaMovie class."))
class CRIWARERUNTIME_API UDataManaMovie : public UManaMovie
{
	GENERATED_UCLASS_BODY()

public:

	// ManaMovie overrides

	bool Validate() const override;

	FString GetUrl() const override { return FString(); }

	// UDataManaMovie interface

	/** */
	UFUNCTION(BlueprintCallable, Category = "Mana|DataManaMovie")
	void SetDataArray(TArray<uint8>& InDataArray);

	/** */
	UFUNCTION(BlueprintCallable, Category = "Mana|DataManaMovie")
	TArray<uint8>& GetDataArray() { return *DataArray; }

protected:
	/** Reload movie information from Sofdec2 movie file.
	*
	*  @return true if information was reloaed
	*/
	bool ReloadMovieInfo() override;

	/** */
	TArray<uint8>* DataArray;
};
