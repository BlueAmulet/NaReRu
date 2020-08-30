/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2015-2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Mana Movie Texture Asset
 * File     : ManaTexture.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* モジュールヘッダ */
#include "ManaTexture.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareRuntimePrivatePCH.h"
#include "ManaTextureResource.h"
#include "ManaMovie.h"
#include "ManaPlaylist.h"

/* Unreal Engine 4関連ヘッダ */
#include "UObject/Package.h"
#include "Serialization/CustomVersion.h"
#include "Misc/Paths.h"

#if WITH_EDITORONLY_DATA
#include "ICriWareEditor.h"
#endif

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/

/***************************************************************************
 *      処理マクロ
 *      Macro Functions
 ***************************************************************************/
#define LOCTEXT_NAMESPACE "ManaTexture"

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

/***************************************************************************
 *      変数定義
 *      Variable Definition
 ***************************************************************************/

/***************************************************************************
 *      クラス定義
 *      Class Definition
 ***************************************************************************/
/* ManaTextureパッケージのバージョン情報 */
struct FManaTextureCustomVersion
{
	enum Type
	{
		// ManaTextureにバージョン番号が付与される前
		BeforeCustomVersionWasAdded = 0,

		// FManaVideoTrackInfo::bIsAlphaが追加されたバージョン
		AddIsAlpha,

		// 4.9向けにbUseLegacyGammaを無効にしたバージョン
		DisableLegacyGamma,

		// Render with Coponent textures disable render target
		RenderComponentTextures,

		// イベントポイント情報を含めたバージョン
		IncludeEventPointInfo,

		// Render to HDR texture
		RenderHDRTargetTexture,

		// ManaSource support
		UseManaSource,

		// 最新バージョンの番号を取得するための番兵
		VersionPlusOne,

		// 最新バージョンの番号を指す
		LatestVersion = VersionPlusOne - 1,
	};

	static const FGuid GUID;
};

const FGuid FManaTextureCustomVersion::GUID(0x842A207C, 0x958442B6, 0xB456876A, 0xF49CD28E);

/* シリアライズ時に最新バージョンを埋め込み */
FCustomVersionRegistration GRegisterManaTextureCustomVersion(FManaTextureCustomVersion::GUID, FManaTextureCustomVersion::LatestVersion, TEXT("ManaTextureVer"));

//----------------------------------------
// FManaFrame struct
//----------------------------------------
FManaFrame::FManaFrame() : Info({0}), Buffers{ {0} }
{
}

FManaFrame::FManaFrame(const CriManaFrameInfo& FrameInfo) : Info(FrameInfo), 
Buffers{ {0} }
{
}

void FManaFrame::Invalidate()
{
	Info.frame_no = -1;
}

bool FManaFrame::IsValid()
{
	return (Info.frame_no >= 0);
}

//----------------------------------------
// UManaTexture class
//----------------------------------------
const FIntPoint UManaTexture::DefaultDimension = FIntPoint(2, 2);

UManaTexture::UManaTexture(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MovieSource(nullptr)
	, ClearColor(FLinearColor::Black)
	, OverrideFormat(PF_Unknown)
	, bIsAlpha(false)
	, bIsStreamed(false)
	, OutputMovieFormat(EManaMovieFormat::DEFAULT)
	, CurrentMovie(nullptr)
	, CurrentMovieIndex(0)
	, bIsValidatingCurrentMovie(false)
{
#if WITH_EDITOR
	/* Localization of unreal properties metadata with LOCTEXT markups and reflection */
	CRI_LOCCLASS(GetClass());
#endif
}

/* UObject  overrides
*****************************************************************************/

#if WITH_EDITOR
void UManaTexture::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property != nullptr) {
		FName PropertyName = PropertyChangedEvent.Property->GetFName();

		if (PropertyName == GET_MEMBER_NAME_CHECKED(UManaTexture, AddressX) ||
			PropertyName == GET_MEMBER_NAME_CHECKED(UManaTexture, AddressY)) {
			// sync addressing of component textures
			for (auto ComponentTexture : ComponentTextures) {
				ComponentTexture->AddressX = AddressX;
				ComponentTexture->AddressY = AddressY;
				ComponentTexture->PostEditChange();
			}
		}

		if (PropertyName == GET_MEMBER_NAME_CHECKED(UManaTexture, ClearColor))
		{
			// sync clear color of component textures
			for (auto ComponentTexture : ComponentTextures) {
				ComponentTexture->UpdateClearColor(ClearColor);
			}
		}

		if (PropertyName == GET_MEMBER_NAME_CHECKED(UManaTexture, MovieSource)) {
			SetMovieSource(MovieSource);
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UManaTexture::PostLoad()
{
	/* 最後にSerializeしたときのバージョン番号を取得 */
	const auto ManaTextureVer = GetLinkerCustomVersion(FManaTextureCustomVersion::GUID);

#if WITH_EDITORONLY_DATA
	bool bUpdated = false;

	/* UE4.9以降のプラグインではbUseLegacyGammaを無効にする */
	if (ManaTextureVer < FManaTextureCustomVersion::DisableLegacyGamma) {
		bUseLegacyGamma = false;
		bUpdated = true;
	}

	if (ManaTextureVer < FManaTextureCustomVersion::AddIsAlpha) {
		bUpdated = true;
	}

	if (ManaTextureVer < FManaTextureCustomVersion::RenderComponentTextures) {
		bRenderToTexture = true;
		bUpdated = true;
	}

	// Create a new ManaMovie Asset from deprecated MovieFilePath.
	if (ManaTextureVer < FManaTextureCustomVersion::UseManaSource) {
		if (!MovieFilePath_DEPRECATED.IsEmpty() && MovieSource == nullptr) {
			FString ResolvedSourceFilePath = MovieFilePath_DEPRECATED;
			if (FPaths::IsRelative(ResolvedSourceFilePath)) {
				ResolvedSourceFilePath = FPaths::Combine(UCriWareInitializer::GetContentDir(), ResolvedSourceFilePath);
			}

			FString MovieFilePath = ResolvedSourceFilePath;
			FPaths::MakePathRelativeTo(MovieFilePath, *UCriWareInitializer::GetContentDir());

			auto& CriWareEditor = FModuleManager::Get().LoadModuleChecked<ICriWareEditor>("CriWareEditor").Get();

			FString PackagePath = FPackageName::GetLongPackagePath(GetOutermost()->GetName());
			const FString Suffix = "_Movie";
			auto NewManaMovie = Cast<UFileManaMovie>(CriWareEditor.CreateAsset(*FPaths::GetBaseFilename(MovieFilePath_DEPRECATED) + Suffix, PackagePath, UFileManaMovie::StaticClass()));

			NewManaMovie->SetFilePath(MovieFilePath);
			NewManaMovie->AssetImportData->Update(ResolvedSourceFilePath);

			auto Package = NewManaMovie->GetOutermost();
			if (Package) {
				Package->SetDirtyFlag(true);
				Package->PackageMarkedDirtyEvent.Broadcast(Package, true);
			}

			// assign to texture
			MovieSource = NewManaMovie;
		}

		MovieFilePath_DEPRECATED.Empty();
		bUpdated = true;
	}

	/* - UObjectBaseUtility::MarkPackageDirty関数を参考。エディタ上でも保存できるようにする。 */
	if (bUpdated) {
		auto Package = GetOutermost();
		if (Package) {
			Package->SetDirtyFlag(true);
			Package->PackageMarkedDirtyEvent.Broadcast(Package, true);
		}
	}
#else
	if (ManaTextureVer < FManaTextureCustomVersion::UseManaSource)
	{
		UE_LOG(LogCriWareRuntime, Warning, TEXT("Texture UManaTexture asset '%s' with version %d detected in a cooked build (latest version is %d).  Please perform a full recook."), *GetPathName(), ManaTextureVer, (int32)FManaTextureCustomVersion::LatestVersion);
	}
#endif

	Super::PostLoad();

	// update sub-objects for current platform
	SetMovieSource(MovieSource);
}

void UManaTexture::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	/* 書き込み時に最新のバージョン番号を埋め込み */
	Ar.UsingCustomVersion(FManaTextureCustomVersion::GUID);
}

/* FTickableGameObject  overrides
*****************************************************************************/

void UManaTexture::Tick(float /*DeltaTime*/)
{
	if (bIsValidatingCurrentMovie && CurrentMovie && !CurrentMovie->IsValidating()) {
		bIsValidatingCurrentMovie = false; // stop tick

		if (CurrentMovie->Validate()) {
			CachedDimensions = CurrentMovie->GetVideoTrackTextureDimensions(0);
			UpdateOutputMovieFormat(CurrentMovie);
		}
		else {
			CachedDimensions = DefaultDimension;
			CurrentMovie = nullptr;
		}

		UpdateComponentTextures(OutputMovieFormat, CachedDimensions);
		UpdateResource();
	}
}

/* UTexture  overrides
*****************************************************************************/

FTextureResource* UManaTexture::CreateResource()
{
	LLM_SCOPE(ELLMTag::Audio);

	if ((CachedDimensions.X == 0) || (CachedDimensions.Y == 0)) {
		return nullptr;
	}

#if defined(XPT_TGT_XBOXONE)
	/* TCircularQueueのメモリアラインメント要求を満たすため、FMemory::Mallocで確保する。
	 * エンジン側によってdeleteされるが、operator deleteが対になるFMemory::Freeで
	 * エンジンによってオーバーライドされているため問題なく解放される。
	 * operator newもFMemory::Mallocでオーバーライドされているが、XboxOneのコンパイラでは検出してくれない様子。*/
	auto TextureResourceMemory = FMemory::Malloc(sizeof(FManaTextureResource));
	return new (TextureResourceMemory) FManaTextureResource(this);
#else
	return new FManaTextureResource(this);
#endif
}

void UManaTexture::UpdateResource()
{
	for (auto ComponentTexture : ComponentTextures) {
		ComponentTexture->UpdateResource();
	}

	// Call parent implementation
	Super::UpdateResource();

	OnTextureUpdated.Broadcast(this);
}

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION > 17
EMaterialValueType UManaTexture::GetMaterialType() const
#else
EMaterialValueType UManaTexture::GetMaterialType()
#endif
{
	return MCT_Texture2D;
}

float UManaTexture::GetSurfaceWidth() const
{
	return CachedDimensions.X;
}


float UManaTexture::GetSurfaceHeight() const
{
	return CachedDimensions.Y;
}

/* UTexture  functions
*****************************************************************************/

void UManaTexture::Init(const FString& InMovieFilePath /*= ""*/)
{
	// placeholder 1x1 textures.
	// todo: maybe "no movie texture" :D
	CachedDimensions = DefaultDimension;

	// create sub-textures
	UpdateComponentTextures(OutputMovieFormat, CachedDimensions);

	bRenderToTexture = false;
}

void UManaTexture::UpdateOutputMovieFormat(UManaMovie* Movie)
{
	if (Movie && Movie->GetNumVideoTracks() > 0) {
		bIsAlpha = Movie->IsVideoTrackAlpha(0);

		// Select the ouput format that we use in ManaTexture.
		switch (Movie->GetVideoTrackType(0)) {
		case EManaMovieType::SofdecPrime:
			OutputMovieFormat = EManaMovieFormat::PRIME;
			break;
		case EManaMovieType::VP9:
#if defined(XPT_TGT_SWITCH)
			OutputMovieFormat = EManaMovieFormat::RGB;
#else
			OutputMovieFormat = EManaMovieFormat::PRIME;
#endif
			break;
		case EManaMovieType::H264:
#if defined(XPT_TGT_PC)
			OutputMovieFormat = EManaMovieFormat::YV12;
#elif defined(XPT_TGT_PS4)
			OutputMovieFormat = EManaMovieFormat::NV12;
#elif defined(XPT_TGT_XBOXONE)
			OutputMovieFormat = EManaMovieFormat::YV12;
#elif defined(XPT_TGT_SWITCH)
			OutputMovieFormat = EManaMovieFormat::RGB;
#elif defined(XPT_TGT_ANDROID)
			OutputMovieFormat = EManaMovieFormat::ExternalTexture;
#elif defined(XPT_TGT_MACOSX) || defined(XPT_TGT_IOS)
			OutputMovieFormat = EManaMovieFormat::NV12;
#else
			OutputMovieFormat = EManaMovieFormat::DEFAULT;
#endif
			break;
		default:
			OutputMovieFormat = EManaMovieFormat::DEFAULT;
		}
	}
}

void UManaTexture::UpdateComponentTextures(EManaMovieFormat InMovieFormat, FIntPoint InSize)
{
	if (InSize.X == 0 || InSize.Y == 0) { return; }

	struct TextureInfo {
		EManaComponentTextureType Type;
		EPixelFormat Format;
	};
	typedef TArray<TextureInfo> ComponentTextureList;

	// select component texture set we want for each format
	ComponentTextureList DesiredTextures;

	switch (InMovieFormat) {
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
	case EManaMovieFormat::ExternalTexture:
	{
		DesiredTextures.Add({ EManaComponentTextureType::Texture_External, PF_R8G8B8A8 });
		break;
	}
	}
#if WITH_EDITOR
	// ensure always all color textures in editor
	if (DesiredTextures.Num() < 2) {
		DesiredTextures.Add({ EManaComponentTextureType::Texture_U, PF_Unknown });
		if (DesiredTextures.Num() < 3) {
			DesiredTextures.Add({ EManaComponentTextureType::Texture_V, PF_Unknown });
		}
	}
#endif
	if (IsAlphaMovie()) {
		DesiredTextures.Add({ EManaComponentTextureType::Texture_A, PF_G8 });
	}

	// prepare arrays and move assigned textures to fixed slots
	if (ComponentTextureSlots.Num() == 0) {
		ComponentTextureSlots.Init(nullptr, 4);
		if (ComponentTextures.Num() != 0) {
			for (auto ComponentTexture : ComponentTextures) {
				int32 DesiredTextureIndex = UManaComponentTexture::GetPlaneIndexFromType(ComponentTexture->ComponentType);
				ComponentTextureSlots[DesiredTextureIndex] = ComponentTexture;
			}
		}
	}
	ComponentTextures.Empty();

	// create textures or update existing textures
	for (int32 TextureIndex = 0; TextureIndex < ComponentTextureSlots.Num(); TextureIndex++) {
		bool InUse = false;
		for (auto Info : DesiredTextures) {
			int32 DesiredTextureIndex = UManaComponentTexture::GetPlaneIndexFromType(Info.Type);
			if (DesiredTextureIndex == TextureIndex) {
				if (ComponentTextureSlots[TextureIndex] != nullptr) {
					ComponentTextureSlots[TextureIndex]->UpdateSizeAndFormat(Info.Type, InSize.X, InSize.Y, Info.Format);
					ComponentTextureSlots[TextureIndex]->UpdateClearColor(ClearColor);
					ComponentTextureSlots[TextureIndex]->UpdateResource();
				}
				else {
					ComponentTextureSlots[TextureIndex] = UManaComponentTexture::Create(this, Info.Type, InSize.X, InSize.Y, Info.Format);
				}
				ComponentTextures.Add(ComponentTextureSlots[TextureIndex]);
				InUse = true;
				break;
			}
		}
		if (!InUse) {
			ComponentTextureSlots[TextureIndex] = nullptr;
		}
	}
}

FManaFrame UManaTexture::GetWritableFrame()
{
	if (!Resource) {
		/* FTextureResourceの準備が終わっていないのに呼ばれることは起こりえる。
		 * FTextureResourceの準備が終わるまでスキップする */
		FManaFrame ManaFrame;
		ManaFrame.Invalidate();
		return ManaFrame;
	}
	return static_cast<FManaTextureResource*>(Resource)->GetWritableFrame();
}

void UManaTexture::SubmitFrame(const FManaFrame& InManaFrame)
{
	if (!Resource) {
		UE_LOG(LogCriWareRuntime, Error, TEXT("UManaTexture::SubmitFrame called while FTextureResource is not ready. This should not happen, so please contact to CRI Middleware technical support."));
	}
	return static_cast<FManaTextureResource*>(Resource)->SubmitFrame(InManaFrame);
}

/* Iterate over free frame queue. */
uint32 UManaTexture::IterateWritableFrames(TFunctionRef<void(FManaFrame& ManaFrame)> Iter)
{
	if (!Resource) {
		return 0;
	}
	return static_cast<FManaTextureResource*>(Resource)->IterateWritableFrames(Iter);
}

void UManaTexture::ResetFrameQueues()
{
	if (!Resource) {
		UE_LOG(LogCriWareRuntime, Error, TEXT("UManaTexture::ResetFrameQueues called while FTextureResource is not ready. This should not happen, so please contact to CRI Middleware technical support."));
	}
	return static_cast<FManaTextureResource*>(Resource)->ResetFrameQueues();
}

bool UManaTexture::IsRenderResourcePrepared() const
{
	if (!Resource) {
		return false;
	}

	return static_cast<FManaTextureResource*>(Resource)->IsPrepared();
}

void UManaTexture::SetMovieSource(UManaSource* InMovieSource)
{
	MovieSource = InMovieSource;

	if (MovieSource != nullptr) {
		// look for a Movie info
		UManaMovie* Movie = nullptr;
		
		if (MovieSource->IsA(UManaMovie::StaticClass()))
		{
			Movie = Cast<UManaMovie>(MovieSource);
		} else if (MovieSource->GetClass() == UManaPlaylist::StaticClass()
			       && Cast<UManaPlaylist>(MovieSource)->Num() > 0)
		{
			// first movie
			Movie = Cast<UManaPlaylist>(MovieSource)->Get(0);
		}

		if (Movie != nullptr && (Movie->IsValidating() || Movie->Validate())) {
			if (Movie->Validate()) {
				CachedDimensions = Movie->GetVideoTrackTextureDimensions(0);
				CurrentMovie = Movie;
			} else {
				CachedDimensions = DefaultDimension;
				CurrentMovie = Movie;
				bIsValidatingCurrentMovie = true; // start tick
			}
		} else {
			CachedDimensions = DefaultDimension;
			CurrentMovie = nullptr;
		}
	} else {
		CachedDimensions = DefaultDimension;
		CurrentMovie = nullptr;
	}
	CurrentMovieIndex = 0;
	if (CurrentMovie) {
		UpdateOutputMovieFormat(CurrentMovie);
	}
	UpdateComponentTextures(OutputMovieFormat, CachedDimensions);
	UpdateResource();
}

bool UManaTexture::SetPlaylistMovie(int32 Index)
{
	if (MovieSource && MovieSource->GetClass() == UManaPlaylist::StaticClass()) {
		auto* Playlist = Cast<UManaPlaylist>(MovieSource);
		auto* Movie = Playlist->Get(Index);
		if (Movie && (Movie->IsValidating() || Movie->Validate())) {
			CurrentMovie = Movie;
			CurrentMovieIndex = Index;
			if (Movie->Validate()) {
				if (Playlist->IsSeamless() == false) {
					CachedDimensions = Movie->GetVideoTrackTextureDimensions(0);
					UpdateOutputMovieFormat(CurrentMovie);
					UpdateComponentTextures(OutputMovieFormat, CachedDimensions);
					UpdateResource();
				}
			} else {
				bIsValidatingCurrentMovie = true; // start tick
			}
			return true;
		}
	}
	return false;
}

/***************************************************************************
 *      関数定義
 *      Function Definition
 ***************************************************************************/

#undef LOCTEXT_NAMESPACE

/* --- end of file --- */
