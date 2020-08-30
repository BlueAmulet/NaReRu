/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Mana play list asset
 * File     : ManaPlaylist.cpp
 *
 ****************************************************************************/
#include "ManaPlaylist.h"

#include "CriWareRuntimePrivatePCH.h"

#include "UObject/Package.h"
#include "Serialization/CustomVersion.h"
#include "Misc/Paths.h"

#define LOCTEXT_NAMESPACE "ManaPlaylist"

//----------------------------------------
// FManaPlaylistCustomVersion struct
//----------------------------------------

/* ManaTextureパッケージのバージョン情報 */
struct FManaPlaylistCustomVersion
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

const FGuid FManaPlaylistCustomVersion::GUID(0x122C2078, 0x957272B6, 0xBA457845, 0xA1295529);

/* シリアライズ時に最新バージョンを埋め込み */
FCustomVersionRegistration GRegisterManaPlaylistCustomVersion(FManaPlaylistCustomVersion::GUID, FManaPlaylistCustomVersion::LatestVersion, TEXT("ManaPlaylistVer"));

//----------------------------------------
// UManaPlaylist class
//----------------------------------------
UManaPlaylist::UManaPlaylist(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bIsAlpha(false)
	, bIsMixedTypes(false)
	, bIsSeamless(false)
{
#if WITH_EDITOR
	// Localization of unreal properties metadata with LOCTEXT markups and reflection
	CRI_LOCCLASS(GetClass());
#endif
}

/* UObject  overrides
*****************************************************************************/

#if WITH_EDITOR
void UManaPlaylist::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property != nullptr) {
		FName PropertyName = PropertyChangedEvent.Property->GetFName();

		if (PropertyName == GET_MEMBER_NAME_CHECKED(UManaPlaylist, Movies)) {
			UpdatePlaylistInfo();
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UManaPlaylist::PostLoad()
{
	UpdatePlaylistInfo();

#if WITH_EDITORONLY_DATA
	bool bUpdated = false;
	/* 最後にSerializeしたときのバージョン番号を取得 */
	const auto ManaTextureVer = GetLinkerCustomVersion(FManaPlaylistCustomVersion::GUID);

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

void UManaPlaylist::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	/* 書き込み時に最新のバージョン番号を埋め込み */
	Ar.UsingCustomVersion(FManaPlaylistCustomVersion::GUID);
}

UManaMovie* UManaPlaylist::Get(int32 Index)
{
	if (!Movies.IsValidIndex(Index))
	{
		return nullptr;
	}

	return Movies[Index];
}

UManaMovie* UManaPlaylist::GetNext(int32& InOutIndex)
{
	if (Num() == 0) {
		InOutIndex = INDEX_NONE;
		return nullptr;
	}

	InOutIndex = (InOutIndex != INDEX_NONE) ? (InOutIndex + 1) % Num() : 0;
	return Get(InOutIndex);
}

UManaMovie* UManaPlaylist::GetPrevious(int32& InOutIndex)
{
	if (Num() == 0) {
		InOutIndex = INDEX_NONE;
		return nullptr;
	}

	InOutIndex = (InOutIndex != INDEX_NONE) ? (InOutIndex + Num() - 1) % Num() : 0;
	return Get(InOutIndex);
}

UManaMovie* UManaPlaylist::GetRandom(int32& InOutIndex)
{
	if (Num() == 0) {
		InOutIndex = INDEX_NONE;
		return nullptr;
	}

	InOutIndex = FMath::RandHelper(Num() - 1);
	return Get(InOutIndex);
}

void UManaPlaylist::Add(UManaMovie* ManaMovie)
{
	Movies.Add(ManaMovie);
	UpdatePlaylistInfo();
}

void UManaPlaylist::Insert(UManaMovie* ManaMovie, int32 Index)
{
	Movies.Insert(ManaMovie, Index);
	UpdatePlaylistInfo();
}

void UManaPlaylist::Remove(UManaMovie* ManaMovie)
{
	Movies.Remove(ManaMovie);
	UpdatePlaylistInfo();
}

void UManaPlaylist::RemoveAt(int32 Index)
{
	Movies.RemoveAt(Index);
	UpdatePlaylistInfo();
}

void UManaPlaylist::UpdatePlaylistInfo()
{
	bIsAlpha = CheckAlpha();
	bIsMixedTypes = CheckMixedTypes();
	bIsSeamless = CheckSeamless();
}

bool UManaPlaylist::CheckAlpha()
{
	for (auto* Movie : Movies)
	{
		if (Movie && Movie->IsAlpha())
		{
			return true;
		}
	}
	return false;
}

bool UManaPlaylist::CheckMixedTypes()
{
	if (Movies.Num() > 1)
	{
		auto* PrevMovie = Movies[0];
		for (int32 i = 1; i < Movies.Num(); i++)
		{
			auto* NextMovie = Movies[i];
			if (PrevMovie == nullptr) {
				PrevMovie = NextMovie;
				continue;
			}
			if (NextMovie == nullptr) {
				continue;
			}
		
			if (PrevMovie->GetNumVideoTracks() > 0 &&
				NextMovie->GetNumVideoTracks() > 0 &&
				PrevMovie->GetVideoTrackType(0) != NextMovie->GetVideoTrackType(0)) {
				return true;
			}
			PrevMovie = NextMovie;
		}
	}
	return false;
}

bool UManaPlaylist::CheckSeamless()
{
	if (Movies.Num() > 1)
	{
		auto* PrevMovie = Movies[0];
		for (int32 i = 1; i < Movies.Num(); i++)
		{
			auto* NextMovie = Movies[i];
			if (PrevMovie == nullptr) {
				PrevMovie = NextMovie;
				continue;
			}
			if (NextMovie == nullptr) {
				continue;
			}

			if (PrevMovie->GetNumVideoTracks() > 0 &&
				NextMovie->GetNumVideoTracks() > 0)
			{
				// check seamless compatiblity parameters
				if (PrevMovie->GetVideoTrackType(0) != NextMovie->GetVideoTrackType(0) ||
					PrevMovie->IsVideoTrackAlpha(0) != NextMovie->IsVideoTrackAlpha(0) ||
					PrevMovie->GetVideoTrackTextureDimensions(0) != NextMovie->GetVideoTrackTextureDimensions(0) ||
					PrevMovie->GetVideoTrackFrameRate(0) != NextMovie->GetVideoTrackFrameRate(0) ||
					PrevMovie->GetNumAudioTracks() != NextMovie->GetNumAudioTracks()) {
					return false;
				}
				// audio tracks
				for (int32 Track = 0; Track < PrevMovie->GetNumAudioTracks(); Track++) {
					if (PrevMovie->GetAudioTrackNumChannels(Track) != NextMovie->GetAudioTrackNumChannels(Track) ||
						PrevMovie->GetAudioTrackSamplingRate(Track) != NextMovie->GetAudioTrackSamplingRate(Track)) {
						return false;
					}
				}
			}
			PrevMovie = NextMovie;
		}
	}
	return true;
}

#undef LOCTEXT_NAMESPACE
