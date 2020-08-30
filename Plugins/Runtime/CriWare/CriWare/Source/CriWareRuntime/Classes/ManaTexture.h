/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2015-2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Mana Movie Texture Asset
 * File     : ManaTexture.h
 *
 ****************************************************************************/

/* 多重定義防止 */
#pragma once

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
#include "Tickable.h"
#include "CriWareApi.h"
#include "ManaComponentTexture.h"
#include "ManaMovie.h"
#include "ManaTexture.generated.h"

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
class UManaSource;

/** Movie Frame consumed by UManaTexture */
struct FManaFrame
{
	CriManaFrameInfo Info;
	CriManaTextureBuffer Buffers[4];

	FManaFrame();
	FManaFrame(const CriManaFrameInfo& Info);

	void Invalidate(); // set in an invalid status
	bool IsValid();
};

/* Movie Formats */
enum class EManaMovieFormat : uint8
{
	/** YUV 4:2:0 bi-planar */
	NV12,
	/** YUV 4:2:0 tri-planar */
	YV12,
	/** RGB 4:4:4 packed */
	RGB,
	/** OpenGL External Texture */
	ExternalTexture,
	Num,

	PRIME = YV12,
	DEFAULT = YV12,
};

UCLASS(hidecategories = (Object, Texture, Adjustments, Compression, TextureRenderTarget2D, LevelOfDetail, Compositing), MinimalAPI, meta = (ToolTip = "ManaTexture class."))
class UManaTexture : public UTexture, public FTickableGameObject
{
	GENERATED_UCLASS_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTextureUpdated, UManaTexture*, ManaTexture);

	// Friends.
	friend class FManaPlayerMovieStreamer;

public:
#if WITH_EDITOR
	/** USMファイルパスがエディタ上で変更されたときの処理 */
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	/* */
	virtual void PostLoad() override;

	/* アセットの互換性維持 */
	virtual void Serialize(FArchive& Ar) override;

	// FTickableGameObject interface
	void Tick(float DeltaTime) override;
	bool IsTickable() const override { return bIsValidatingCurrentMovie != 0; }
	bool IsTickableInEditor() const override { return true; }
	bool IsTickableWhenPaused() const override { return true; }
	TStatId GetStatId() const override { return TStatId(); }
	//~ FTickableGameObject interface

	/** 描画用のリソースを作成して返す。 */
	virtual FTextureResource* CreateResource() override;
	virtual void UpdateResource() override;

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION > 17
	virtual EMaterialValueType GetMaterialType() const override;
#else
	virtual EMaterialValueType GetMaterialType() override;
#endif

	virtual float GetSurfaceWidth() const override;
	virtual float GetSurfaceHeight() const override;

	/**
	* Gets the width and height of the texture (in pixels).
	*
	* @return Texture dimensions.
	*/
	FIntPoint GetDimensions() const
	{
		return CachedDimensions;
	}

	/**
	* Gets the texture's pixel format.
	*
	* @return Pixel format (always PF_B8G8R8A8 for all movie textures).
	*/
	TEnumAsByte<enum EPixelFormat> GetFormat() const
	{
		if (OverrideFormat == PF_Unknown) {
			return bHDR ? PF_FloatRGBA : PF_B8G8R8A8;
		}
		return OverrideFormat;
	}

	/**
	* Gets the output movie's pixel format from mana decoder.
	* Some hardware decoders outputs only to specific pixel format.
	*
	* @return Output Movie Pixel format (may be different by platform except for Prime movies that always output to YV12).
	*/
	EManaMovieFormat GetOutputMovieFormat() const
	{
		return OutputMovieFormat;
	}

	/* UManaTexture固有API */

	/** アルファムービかどうかを返す */
	CRIWARERUNTIME_API bool IsAlphaMovie() const { return bIsAlpha; }

	/** 描画準備が完了しているかどうか */
	CRIWARERUNTIME_API bool IsRenderResourcePrepared() const;

	/** */
	UFUNCTION(BlueprintCallable, Category = Mana, meta = (ToolTip = "Sets the ManaSource for this texture."))
	CRIWARERUNTIME_API void SetMovieSource(UManaSource* InSource);

	/** */
	CRIWARERUNTIME_API UManaSource* GetMovieSource() const { return MovieSource; }

	/** */
	CRIWARERUNTIME_API int32 GetNumComponentTextures() const { return ComponentTextures.Num(); }

	/** */
	CRIWARERUNTIME_API UManaComponentTexture* GetComponentTexture(int32 Index) const 
	{ 
		return ComponentTextures.IsValidIndex(Index) ? ComponentTextures[Index] : nullptr;
	}

protected:
	/** Mana Source */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mana, meta = (ToolTip = "Movie source."))
	UManaSource* MovieSource;

private:
	//~ Deprecated
	/** USMファイルへの相対パス DEPRECATED */
	UPROPERTY()
	FString MovieFilePath_DEPRECATED;
	//~ End Deprecated

public:
	/** Render movie to single RGBA texture with render target. May slow with 2k/4k movies. You shouln't use this for VR movies.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mana)
	uint32 bRenderToTexture : 1;

	//~ Begin Render to Texture sub menu
	/** Gamma adjustment (raises linear-space RGB color to the specified power.) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mana|Render to Texture", meta = (DisplayName = "Gamma Adjustment"))
	float TargetGamma;

	/** Whether to support storing HDR values, which requires more memory. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mana|Render to Texture", meta = (DisplayName = "HDR"), AssetRegistrySearchable)
	uint32 bHDR : 1;
	//~ End Render to Texture sub menu

	/** The addressing mode to use for the X axis. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mana, meta = (DisplayName = "Address X"), AssetRegistrySearchable, AdvancedDisplay)
	TEnumAsByte<enum TextureAddress> AddressX;

	/** The addressing mode to use for the Y axis. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mana, meta = (DisplayName = "Address Y"), AssetRegistrySearchable, AdvancedDisplay)
	TEnumAsByte<enum TextureAddress> AddressY;

	/** The color used to clear the texture if no video data is drawn. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mana, meta = (DisplayName = "Clear Color"), AssetRegistrySearchable, AdvancedDisplay)
	FLinearColor ClearColor;

	/** The format of the texture data.
	 *
	 * Normally the format is derived from bHDR, this allows code to set the format explicitly. 
	 */
	UPROPERTY()
	TEnumAsByte<enum EPixelFormat> OverrideFormat;

	/** Event for updates. */
	UPROPERTY(BlueprintAssignable)
	FOnTextureUpdated OnTextureUpdated;

protected:
	/** Component texures of the movie. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana, AdvancedDisplay)
	TArray<UManaComponentTexture*> ComponentTextures;

public:
	/** init */
	virtual void Init(const FString& InMovieFilePath = "");

	/** set current index movie from play list and update texture. */
	bool SetPlaylistMovie(int32 Index);

	/** currently used ManaMovie by ManaTexture */
	FORCEINLINE UManaMovie* GetMovie() const { return CurrentMovie; }

	/** ManaTextureResourceから、書き込み先フレームを取得 */
	FManaFrame GetWritableFrame();

	/** ManaTextureResourceへ、書き込み済みフレームを登録 */
	void SubmitFrame(const FManaFrame& InManaFrame);

	/* Iterate over free frame queue. */
	uint32 IterateWritableFrames(TFunctionRef<void(FManaFrame& ManaFrame)> Iter);

	/** Clear and reset queued frames. */
	void ResetFrameQueues();

	/** Check if the texture is streamed from movie source. */
	FORCEINLINE bool IsStreamed() const { return bIsStreamed; }

	/** Flag the texture if streamed from movie source. */
	void SetStreamed(bool IsStreamed) { bIsStreamed = IsStreamed; }

private:
	static const FIntPoint DefaultDimension;

	/** The texture's cached width and height (in pixels). */
	FIntPoint CachedDimensions;

	/** If the movie use alpha. */
	uint32 bIsAlpha : 1;

	/** If the texture use texture streaming from decoder. */
	uint32 bIsStreamed : 1;

	/** Format used by decoder to output the movie. */
	EManaMovieFormat OutputMovieFormat;

	/** update needed format to display the movie */
	void UpdateOutputMovieFormat(UManaMovie* Movie);

	/** Create or update component textures to the desired movie format */
	void UpdateComponentTextures(EManaMovieFormat InMovieFormat, FIntPoint InSize);

	/** Current playback Movie from ManaSource */
	UManaMovie* CurrentMovie;

	/** Current Index in play list used */
	int32 CurrentMovieIndex;

	/** If awaiting current movie information */
	uint32 bIsValidatingCurrentMovie;
    
	/** Internal array of with fixed slots for component texures.
	 *
	 * This ensures that recycled textures keep type when movie source
	 * or platform changes and permits consistant reference for materials.
	 * [0] -> Y / RGB, [1] -> U / UV, [2] -> V, [3] -> Alpha.
	 */
	TArray<UManaComponentTexture*, TFixedAllocator<4>> ComponentTextureSlots;

	virtual bool CanBeInCluster() const override { return false; }
};

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/

/* --- end of file --- */
