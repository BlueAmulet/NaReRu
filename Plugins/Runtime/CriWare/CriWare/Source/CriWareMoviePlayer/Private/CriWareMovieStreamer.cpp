/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2017-2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : MovieStreamer Class for CriWareMoviePlayer module
 * File     : CriWareMovieStreamer.cpp
 *
 ****************************************************************************/
#include "CriWareMovieStreamer.h"

#include "CriWareApi.h"
#include "ManaComponent.h"
#include "ManaTexture.h"
#include "ManaMovie.h"

DEFINE_LOG_CATEGORY_STATIC(LogCriWareMoviePlayer, Log, All);

#define MOVIE_FILE_EXTENSION TEXT("usm")


FManaPlayerMovieStreamer::FManaPlayerMovieStreamer()
	: ManaPlayer(nullptr)
	, ManaTexture(nullptr)
	, ManaMovie(nullptr)
	, StreamDimensions({0, 0})
	, StreamFormat(EManaMovieFormat::DEFAULT)
	, StreamIsAlpha(false)
	, bWaitForPlay(false)
{
	UE_LOG(LogCriWareMoviePlayer, Log, TEXT("CriWareMoviePlayer ctor..."));

	//
	// Allocate resources here whose livetime spans the life of this streamer instance
	//
	MovieViewport = MakeShareable(new FMovieViewport());
}

FManaPlayerMovieStreamer::~FManaPlayerMovieStreamer()
{
	// Release all texture resources.
	ReleaseAllResources();
}

bool FManaPlayerMovieStreamer::Init(const TArray<FString>& MoviePaths, TEnumAsByte<EMoviePlaybackType> inPlaybackType)
{
	// NOTE: Called from main thread
	// Initializes the streamer for audio and video playback of the given path(s).
	// NOTE: If multiple paths are provided, it is expect that they be played back seamlessly.
	UE_LOG(LogCriWareMoviePlayer, Warning, TEXT("FManaPlayerMoviePlayer init. Path count = %d..."), MoviePaths.Num());

	// Add the given paths to the movie queue
	MovieQueue.Append(MoviePaths);

	// Forced initialize of ManaPlayer (UManaComponent)
	if (!ManaPlayer) {
		ManaPlayer = NewObject<UManaComponent>((UObject*)GetTransientPackage(), "CriWareMoviePlayer", RF_NoFlags);
		ManaPlayer->bRegistered = true;
		ManaPlayer->InitializeManaPlayer();
		ManaPlayer->AddToRoot();
	}

	// Create a Mana texture.
	if (!ManaTexture) {
		ManaTexture = NewObject<UManaTexture>((UObject*)GetTransientPackage(), "CriWareMovieTexture", RF_NoFlags);
		ManaTexture->bRenderToTexture = true;
		ManaTexture->TargetGamma = 0.454545f; // no gamma post process for Slice, we need to compensate
		// Attach to ManaPlayer
		ManaPlayer->Movie = ManaTexture;
		ManaTexture->AddToRoot();
	}

	// Create a Mana movie
	if (!ManaMovie) {
		ManaMovie = NewObject<UFileManaMovie>((UObject*)GetTransientPackage(), "CriWareMovieSource", RF_NoFlags);
		ManaMovie->AddToRoot();
	}

	// No movie set to Mana player yet. We will just let Tick() load the first movie. 
	bWaitForPlay = false;

	return true;
}

void FManaPlayerMovieStreamer::UpdateTextureMovie()
{
	if (ManaTexture && ManaMovie && ManaMovie->GetNumVideoTracks() > 0) {
		ManaTexture->MovieSource = ManaMovie;
		ManaTexture->CurrentMovie = ManaMovie;
		ManaTexture->CurrentMovieIndex = 0;
		ManaTexture->CachedDimensions = ManaMovie->GetVideoTrackTextureDimensions(0);
		ManaTexture->bIsAlpha = ManaMovie->IsVideoTrackAlpha(0);

		// Select the output format that we use in ManaTexture.
		switch (ManaMovie->GetVideoTrackType(0)) {
		case EManaMovieType::SofdecPrime:
			ManaTexture->OutputMovieFormat = EManaMovieFormat::PRIME;
			break;
		case EManaMovieType::VP9:
#if defined(XPT_TGT_SWITCH)
			ManaTexture->OutputMovieFormat = EManaMovieFormat::RGB;
#else
			ManaTexture->OutputMovieFormat = EManaMovieFormat::PRIME;
#endif
			break;
		case EManaMovieType::H264:
#if defined(XPT_TGT_PC)
			ManaTexture->OutputMovieFormat = EManaMovieFormat::YV12;
#elif defined(XPT_TGT_PS4)
			ManaTexture->OutputMovieFormat = EManaMovieFormat::NV12;
#elif defined(XPT_TGT_XBOXONE)
			ManaTexture->OutputMovieFormat = EManaMovieFormat::YV12;
#elif defined(XPT_TGT_SWITCH)
			ManaTexture->OutputMovieFormat = EManaMovieFormat::RGB;
#elif defined(XPT_TGT_MACOSX) || defined(XPT_TGT_IOS)
			ManaTexture->OutputMovieFormat = EManaMovieFormat::NV12;
#else
			ManaTexture->OutputMovieFormat = EManaMovieFormat::DEFAULT;
#endif
			break;
		default:
			ManaTexture->OutputMovieFormat = EManaMovieFormat::DEFAULT;
		}
	}
}

void FManaPlayerMovieStreamer::UpdateTextures()
{
	struct TextureInfo {
		EManaComponentTextureType Type;
		EPixelFormat Format;
	};
	typedef TArray<TextureInfo> ComponentTextureList;

	// select component texture set we want for each format
	ComponentTextureList DesiredTextures;

	switch (StreamFormat) {
	case EManaMovieFormat::YV12:
	{
		DesiredTextures.Add({ EManaComponentTextureType::Texture_Y, PF_G8 });
		DesiredTextures.Add({ EManaComponentTextureType::Texture_U, PF_G8 });
		DesiredTextures.Add({ EManaComponentTextureType::Texture_V, PF_G8 });
		break;
	}
	case EManaMovieFormat::NV12:
	{
		DesiredTextures.Add({ EManaComponentTextureType::Texture_Y, PF_G8 });
		DesiredTextures.Add({ EManaComponentTextureType::Texture_UV, PF_R8G8 });
		break;
	}
	case EManaMovieFormat::RGB:
	{
		DesiredTextures.Add({ EManaComponentTextureType::Texture_RGB, PF_R8G8B8A8 });
		break;
	}
	}
	if (StreamIsAlpha) {
		DesiredTextures.Add({ EManaComponentTextureType::Texture_A, PF_G8 });
	}

	// sort existing textures
	ManaTexture->ComponentTextures.Sort([](const UManaComponentTexture& a, const UManaComponentTexture& b) {
		TCHAR la = a.GetName()[a.GetName().Len() - 1];
		TCHAR lb = b.GetName()[b.GetName().Len() - 1];
		int aX = la == 'Y' ? 0 : la == 'U' ? 1 : la == 'V' ? 2 : la == 'A' ? 3 : -1;
		int bX = lb == 'Y' ? 0 : lb == 'U' ? 1 : lb == 'V' ? 2 : lb == 'A' ? 3 : -1;
		return aX < bX;
	});

	// create textures or update existing textures
	int32 TextureIndex = 0;
	for (auto Info : DesiredTextures) {
		if (TextureIndex < ManaTexture->ComponentTextures.Num()) {
			auto ComponentTexture = ManaTexture->ComponentTextures[TextureIndex];
			ComponentTexture->Resource->ReleaseResource();
			ComponentTexture->UpdateSizeAndFormat(Info.Type, StreamDimensions.X, StreamDimensions.Y, Info.Format);
			ComponentTexture->Resource->InitResource();
		}
		else {
			auto ComponentTexture = UManaComponentTexture::Create(ManaTexture, Info.Type, StreamDimensions.X, StreamDimensions.Y, Info.Format);
			ManaTexture->ComponentTextures.Add(ComponentTexture);
			ComponentTexture->AddToRoot();
		}

		TextureIndex++;
	}

	// clear unused textures if exist
	if (TextureIndex < ManaTexture->ComponentTextures.Num()) {
		while (TextureIndex < ManaTexture->ComponentTextures.Num()) {
			ManaTexture->ComponentTextures[TextureIndex]->RemoveFromRoot();
			TextureIndex++;
		}

		const int32 NumManaTextures = DesiredTextures.Num();
		ManaTexture->ComponentTextures.RemoveAt(NumManaTextures, ManaTexture->ComponentTextures.Num() - NumManaTextures, true);
	}
}

bool FManaPlayerMovieStreamer::StartNextMovie()
{
	bool bDidStartMovie = false;

	// Use the engine's file system to read the movie content
	FString MoviePath = /*FPaths::GameContentDir() +*/ FString::Printf(TEXT("Movies/%s.%s"), *(MovieQueue[0]), MOVIE_FILE_EXTENSION);
	MovieQueue.RemoveAt(0);

	if (ManaTexture && ManaMovie) {
		// set a movie
		ManaMovie->SetFilePath(MoviePath);

		// Note: Actually, we cannot use ManaTexture->SetMovieSource(ManaMovie) from render thread.
		
		// update manually internal texture info
		UpdateTextureMovie();
		
		// update manually the internal resources
		FIntPoint NewDimensions = ManaTexture->GetDimensions();
		EManaMovieFormat Format = ManaTexture->GetOutputMovieFormat();
		bool IsAlpha = ManaTexture->IsAlphaMovie();

		bool bStreamChanged = (NewDimensions != StreamDimensions 
								|| Format != StreamFormat
								|| IsAlpha != StreamIsAlpha);

		if (bStreamChanged) {
			StreamDimensions = NewDimensions;
			StreamFormat = Format;
			StreamIsAlpha = IsAlpha;

			if (ManaTexture->Resource) {
				ManaTexture->Resource->ReleaseResource();
			}

			UpdateTextures();
				
			if (!ManaTexture->Resource) {
				ManaTexture->Resource = ManaTexture->CreateResource();
			}

			ManaTexture->Resource->InitResource();

			MovieViewport->SetTexture(nullptr);

			if (!SlateVideoTexture.IsValid()) {
				SlateVideoTexture = MakeShareable(new FSlateTexture2DRHIRef(nullptr, 0, 0));
			}
			SlateVideoTexture->SetRHIRef(nullptr, 0, 0);
		}

		// Play
		ManaPlayer->Play();

		UE_LOG(LogCriWareMoviePlayer, Log, TEXT("Queued movie path: [ %s ]"), *MoviePath);
		bDidStartMovie = true;
	}

	return bDidStartMovie;
}

void FManaPlayerMovieStreamer::ForceCompletion()
{
	// NOTE: Called from main thread
	// WARNING: Assumes that the Tick() function will never be called again
	// The canonical implementation for this shuts down playback before returning

	// Stop the player and make sure it doesn't attempt to start the next movie
	MovieQueue.Empty();

	if (ManaPlayer != nullptr) {
		ManaPlayer->Stop();
	}

	// Release all texture resources.
	ReleaseAllResources();
}

bool FManaPlayerMovieStreamer::Tick(float DeltaTime)
{
	// Returns false if a movie is still playing, true if the movie list has completed
	// NOTE: Movie playback ignores DeltaTime. Time is wall time

	// If we aren't fully initialized and running, pretend the movie has completed
	if (!ManaPlayer) {
		return true;
	}

	check(IsInRenderingThread());

	switch (ManaPlayer->GetStatus()) {
	case EManaComponentStatus::PlayEnd:
	case EManaComponentStatus::Stop:
		if (bWaitForPlay) {
			// Not active yet waiting new play to start
			return false;
		}

		if (MovieQueue.Num() > 0)
		{
			bWaitForPlay = StartNextMovie();
			// Still playing a movie, so return that we aren't done yet
			return false;
		}
		break;
	default:
		bWaitForPlay = false;
		CheckForNextVideoFrame(DeltaTime);
		// Still playing, so tell the caller we are not done
		return false;
	}

	// By default, we are done streaming
	if (SlateVideoTexture->IsValid()) {
		SlateVideoTexture->SetRHIRef(nullptr, 0, 0);
	}
	return true;
}

void FManaPlayerMovieStreamer::CheckForNextVideoFrame(float DeltaTime)
{
	check(IsInRenderingThread());
	
	// Call manually a Tick of ManaPlayer because UManaComponent is not a registered Component.
	if (ManaPlayer) {
		ManaPlayer->TickComponent(DeltaTime, ELevelTick::LEVELTICK_All, nullptr);
	}

	if (!SlateVideoTexture.IsValid()) {
		SlateVideoTexture = MakeShareable(new FSlateTexture2DRHIRef(nullptr, 0, 0));
		SlateVideoTexture->SetRHIRef(nullptr, 0, 0);
	}

	// Update the texture reference to use.
	if (ManaTexture && ManaTexture->IsRenderResourcePrepared()) {
		FTexture2DRHIRef Texture2DRHI = ManaTexture->Resource->TextureRHI->GetTexture2D();
		SlateVideoTexture->SetRHIRef(Texture2DRHI, Texture2DRHI->GetSizeX(), Texture2DRHI->GetSizeY());
		MovieViewport->SetTexture(SlateVideoTexture);
	}
}

TSharedPtr<class ISlateViewport> FManaPlayerMovieStreamer::GetViewportInterface()
{
	return MovieViewport;
}

float FManaPlayerMovieStreamer::GetAspectRatio() const
{
	//FIntPoint Dimensions = ManaTexture->GetDimensions();
	//return Dimensions.X / Dimensions.Y;

	return (float)MovieViewport->GetSize().X / (float)MovieViewport->GetSize().Y;
}

FString FManaPlayerMovieStreamer::GetMovieName()
{
	return MovieQueue.Num() > 0 ? MovieQueue[0] : TEXT("");
}

bool FManaPlayerMovieStreamer::IsLastMovieInPlaylist()
{
	return MovieQueue.Num() <= 1;
}

void FManaPlayerMovieStreamer::Cleanup()
{
	// Release all texture resources.
	ReleaseAllResources();
}

void FManaPlayerMovieStreamer::ReleaseAllResources()
{
	if (ManaTexture) {
		for (int32 TextureIndex = 0; TextureIndex < ManaTexture->ComponentTextures.Num(); TextureIndex++) {
			auto ComponentTexture = ManaTexture->ComponentTextures[TextureIndex];
			if (ComponentTexture->Resource) {
				BeginReleaseResource(ComponentTexture->Resource);
			}
			ComponentTexture->RemoveFromRoot();
		}
		ManaTexture->ComponentTextures.Empty();

		if (ManaTexture->Resource) {
			BeginReleaseResource(ManaTexture->Resource);
		}
		FlushRenderingCommands();

		ManaTexture->RemoveFromRoot();
	}

	if (ManaPlayer) {
		ManaPlayer->RemoveFromRoot();
	}

	if (ManaMovie) {
		ManaMovie->RemoveFromRoot();
	}

	ManaPlayer = nullptr;
	ManaTexture = nullptr;
	ManaMovie = nullptr;

	// Make sure the viewport doesn't have a lingering reference
	MovieViewport->SetTexture(nullptr);
}
