/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2017-2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : MovieStreamer Class for CriWareMoviePlayer module
 * File     : CriWareMovieStreamer.h
 *
 ****************************************************************************/
#pragma once

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* Unreal Engine 4関連ヘッダ */
#include "MoviePlayer.h"
#include "Slate/SlateTextures.h"

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
class UManaComponent;
class UManaTexture;
class UManaMovie;
class UFileManaMovie;
enum class EManaMovieFormat : uint8;

// The actual streamer class
class FManaPlayerMovieStreamer : public IMovieStreamer
{

public:
	FManaPlayerMovieStreamer();
	~FManaPlayerMovieStreamer();

	/**
	* Initializes this movie streamer with all the movie paths (ordered) we want to play
	* Movie paths are local to the current game's Content/Movies/ directory.
	*/
	virtual bool Init(const TArray<FString>& MoviePaths, TEnumAsByte<EMoviePlaybackType> inPlaybackType) override;

	/** Forces the movie streamer to cancel what it's streaming and close. */
	virtual void ForceCompletion() override;

	/** Code run every tick for any additional per tick handling of playing the movie. Returns true if done. */
	virtual bool Tick(float DeltaTime) override;

	/** Gets a viewport interface which will be used to draw the movie. */
	virtual TSharedPtr<class ISlateViewport> GetViewportInterface() override;

	/** Gets the aspect ratio of the movie frames being streamed. */
	virtual float GetAspectRatio() const override;

	/** returns the name of the movie currently being played */
	virtual FString GetMovieName() override;

	/** returns true if the movie being played in the last one in the play list */
	virtual bool IsLastMovieInPlaylist() override;

	/** Called to allow the movie streamer to cleanup any resources once there are no movies left to play. */
	virtual void Cleanup() override;

	virtual FTexture2DRHIRef GetTexture() override { return SlateVideoTexture.IsValid() ? SlateVideoTexture->GetRHIRef() : nullptr; }

	FOnCurrentMovieClipFinished OnCurrentMovieClipFinishedDelegate;
	virtual FOnCurrentMovieClipFinished& OnCurrentMovieClipFinished() override { return OnCurrentMovieClipFinishedDelegate; }

private:
	TSharedPtr<FSlateTexture2DRHIRef, ESPMode::ThreadSafe> SlateVideoTexture;
	TSharedPtr<FMovieViewport> MovieViewport;

	// The list of pending movies
	TArray<FString>		MovieQueue;
	
	UManaComponent*		ManaPlayer;
	
	UManaTexture*		ManaTexture;

	UFileManaMovie*		ManaMovie;

	FIntPoint			StreamDimensions;

	EManaMovieFormat	StreamFormat;

	bool				StreamIsAlpha;

	bool				bWaitForPlay;

	bool StartNextMovie();
	void CheckForNextVideoFrame(float DeltaTime);
	void UpdateTextureMovie();
	void UpdateTextures();
	void ReleaseAllResources();
};

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/

/* --- end of file --- */
