
#include "PlatformManaMovie.h"

#include "CriWareRuntimePrivatePCH.h"

#include "UObject/SequencerObjectVersion.h"
#if WITH_EDITOR
#include "Interfaces/ITargetPlatform.h"
#endif

#define LOCTEXT_NAMESPACE "ManaMovie"


UPlatformManaMovie::UPlatformManaMovie(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	// Localization of unreal properties metadata with LOCTEXT markups and reflection
	CRI_LOCCLASS(GetClass());
#endif
}

/* UObject overrides
*****************************************************************************/

#if WITH_EDITOR
void UPlatformManaMovie::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property != nullptr) {
		FName PropertyName = PropertyChangedEvent.Property->GetFName();

		if (PropertyName == GET_MEMBER_NAME_CHECKED(UPlatformManaMovie, PlatformManaMovies)) {
			ReloadMovieInfo();
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UPlatformManaMovie::PostLoad()
{
	Super::PostLoad();

	// update for current platform
	ReloadMovieInfo();
}


/* UManaMovie interface
 *****************************************************************************/

FString UPlatformManaMovie::GetUrl() const
{
	// Guard against reentrant calls.
	static bool bIsGettingUrl = false;
	if (bIsGettingUrl)
	{
		UE_LOG(LogCriWareRuntime, Warning, TEXT("UPlatformManaMovie::GetUrl - Reentrant calls are not supported. Asset: %s"), *GetPathName());
		return FString();
	}
	TGuardValue<bool> GettingUrlGuard(bIsGettingUrl, true);

	UManaMovie* PlatformManaMovie = GetManaMovie();
	return (PlatformManaMovie != nullptr) ? PlatformManaMovie->GetUrl() : FString();
}


void UPlatformManaMovie::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar.UsingCustomVersion(FSequencerObjectVersion::GUID);
	auto CustomVersion = Ar.CustomVer(FSequencerObjectVersion::GUID);

	if (Ar.IsLoading() && (CustomVersion < FSequencerObjectVersion::RenameMediaSourcePlatformPlayers))
	{
		FString DummyDefaultSource;
		Ar << DummyDefaultSource;

#if WITH_EDITORONLY_DATA
		Ar << PlatformManaMovies;
#endif
	}
	else
	{
#if WITH_EDITORONLY_DATA
		if (Ar.IsFilterEditorOnly())
		{
			if (Ar.IsSaving())
			{
				UManaMovie** PlatformManaMovie = PlatformManaMovies.Find(Ar.CookingTarget()->IniPlatformName());
				ManaMovie = (PlatformManaMovie != nullptr) ? *PlatformManaMovie : nullptr;
			}

			Ar << ManaMovie;
		}
		else
		{
			Ar << PlatformManaMovies;
		}
#else
		Ar << ManaMovie;
#endif
	}
}


bool UPlatformManaMovie::Validate() const
{
	// Guard against reentrant calls.
	static bool bIsValidating = false;
	if (bIsValidating)
	{
		UE_LOG(LogCriWareRuntime, Warning, TEXT("UPlatformManaMovie::Validate - Reentrant calls are not supported. Asset: %s"), *GetPathName());
		return false;
	}
	TGuardValue<bool> ValidatingGuard(bIsValidating, true);

#if WITH_EDITORONLY_DATA
	for (auto PlatformNameManaMoviePair : PlatformManaMovies)
	{
		UManaMovie* PlatformManaMovie = PlatformNameManaMoviePair.Value;

		if (PlatformManaMovie != nullptr && PlatformManaMovie->Validate() == false)
		{
			return false;
		}
	}
#endif

	UManaMovie* PlatformManaMovie = GetManaMovie();
	return (PlatformManaMovie != nullptr && PlatformManaMovie->Validate() && VideoTracks.Num() > 0);
}

bool UPlatformManaMovie::ReloadMovieInfo() 
{
	// Guard against reentrant calls.
	static bool bIsReloadingMovieInfo = false;
	if (bIsReloadingMovieInfo)
	{
		UE_LOG(LogCriWareRuntime, Warning, TEXT("UPlatformManaMovie::bIsReloadMovieInfo - Reentrant calls are not supported. Asset: %s"), *GetPathName());
		return false;
	}
	TGuardValue<bool> ValidatingGuard(bIsReloadingMovieInfo, true);

	// clear info
	Clear();

	UManaMovie* PlatformManaMovie = GetManaMovie();
	if (PlatformManaMovie)
	{
		// video
		VideoTracks.Append(PlatformManaMovie->VideoTracks);
		// audio
		AudioTracks.Append(PlatformManaMovie->AudioTracks);
		// events
		EventPoints.Append(PlatformManaMovie->EventPoints);
		// subtitles
		MaxSubtitleSize = PlatformManaMovie->MaxSubtitleSize;
		NumSubtitleChannels = PlatformManaMovie->NumSubtitleChannels;
		// alpha
		bIsAlpha = PlatformManaMovie->bIsAlpha;

		return true;
	}

	return false;
}


/* UPlatformManaMovie implementation
 *****************************************************************************/

UManaMovie* UPlatformManaMovie::GetManaMovie() const
{
#if WITH_EDITORONLY_DATA
	const FString RunningPlatformName(FPlatformProperties::IniPlatformName());
	UManaMovie*const* PlatformManaMovie = PlatformManaMovies.Find(RunningPlatformName);

	if (PlatformManaMovie == nullptr)
	{
		return nullptr;
	}

	return *PlatformManaMovie;
#else
	return ManaMovie;
#endif
}

#undef LOCTEXT_NAMESPACE
