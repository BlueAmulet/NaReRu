/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2015-2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Mana Movie Component Texture
 * File     : ManaComponentTexture.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* モジュールヘッダ */
#include "ManaComponentTexture.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareRuntimePrivatePCH.h"
#include "ManaTexture.h"

/* Unreal Engine 4関連ヘッダ */
#include "DeviceProfiles/DeviceProfile.h"
#include "DeviceProfiles/DeviceProfileManager.h"
#include "ExternalTexture.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/
#define LOCTEXT_NAMESPACE "ManaComponentTexture"

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

/***************************************************************************
 *      変数定義
 *      Variable Definition
 ***************************************************************************/

/***************************************************************************
 *      クラス定義
 *      Class Definition
 ***************************************************************************/

typedef EManaComponentTextureType EComponent;

namespace
{
	FColor RGBtoYUV_BT601(const FLinearColor& RGB)
	{
		// Linear to sRGB
		auto Color = RGB.ToFColor(true);

		return FColor(
			(( 66  * Color.R + 129 * Color.G + 25  * Color.B) >> 8) + 16,
			((-38  * Color.R - 74  * Color.G + 112 * Color.B) >> 8) + 128,
			(( 112 * Color.R - 94  * Color.G - 18  * Color.B) >> 8) + 128,
			Color.A
		);
	}

	void RHIFillTexture2D(FRHITexture2D* Texture2DRHI, const FColor& ClearColor)
	{
		uint32 Stride;
		uint8* TextureBuffer = reinterpret_cast<uint8*>(RHILockTexture2D(Texture2DRHI, 0, RLM_WriteOnly, Stride, false));
		
		uint32 Length = Texture2DRHI->GetSizeX() * Texture2DRHI->GetSizeY();
		uint32 BytesPerPixel = Stride / Texture2DRHI->GetSizeX();
		if (BytesPerPixel == 1) {
			FMemory::Memset(TextureBuffer, ClearColor.R, Length);
		} else {
			uint32 Color = 0;
			switch (Texture2DRHI->GetFormat()) {
				case PF_R8G8B8A8: Color = ClearColor.ToPackedABGR(); break;
				case PF_B8G8R8A8: Color = ClearColor.ToPackedARGB(); break;
				default: Color = ClearColor.ToPackedRGBA();
			}
			for (uint32 i = 0; i < Length; i++, TextureBuffer += BytesPerPixel) {
				FMemory::Memcpy(TextureBuffer, &Color, BytesPerPixel);
			}
		}

		RHIUnlockTexture2D(Texture2DRHI, 0, false);
	}
}

/*-----------------------------------------------------------------------------
FManaComponentTextureResource
-----------------------------------------------------------------------------*/
/** Initialization constructor. */
FManaComponentTextureResource::FManaComponentTextureResource(UManaComponentTexture* InOwner)
	: Owner(InOwner)
{
}

/** Returns the width of the texture in pixels. */
uint32 FManaComponentTextureResource::GetSizeX() const
{
	return Owner->SizeX;
}

/** Returns the height of the texture in pixels. */
uint32 FManaComponentTextureResource::GetSizeY() const
{
	return Owner->SizeY;
}

/** Called when the resource is initialized. This is only called by the rendering thread. */
void FManaComponentTextureResource::InitRHI()
{
	// Create the sampler state RHI resource.
	FSamplerStateInitializerRHI SamplerStateInitializer
	(
		(ESamplerFilter)UDeviceProfileManager::Get().GetActiveProfile()->GetTextureLODSettings()->GetSamplerFilter(Owner),
		Owner->AddressX == TA_Wrap ? AM_Wrap : (Owner->AddressX == TA_Clamp ? AM_Clamp : AM_Mirror),
		Owner->AddressY == TA_Wrap ? AM_Wrap : (Owner->AddressY == TA_Clamp ? AM_Clamp : AM_Mirror),
		AM_Wrap
	);
	SamplerStateRHI = RHICreateSamplerState(SamplerStateInitializer);
	
	uint32 Flags = TexCreate_Dynamic;
	if (Owner->bIsResolveTarget)
	{
		Flags |= TexCreate_ResolveTargetable;
		bIgnoreGammaConversions = true;		// Note, we're ignoring Owner->SRGB (it should be false).
	}
	else if (Owner->SRGB)
	{
		Flags |= TexCreate_SRGB;
	}
	if (Owner->bNoTiling)
	{
		Flags |= TexCreate_NoTiling;
	}
	FRHIResourceCreateInfo CreateInfo = FRHIResourceCreateInfo(FClearValueBinding(Owner->ClearColor));
	if (Owner->ComponentType != EComponent::Texture_External)
	{
		Texture2DRHI = RHICreateTexture2D(GetSizeX(), GetSizeY(), Owner->Format, Owner->NumMips, 1, Flags, CreateInfo);
		TextureRHI = Texture2DRHI;
	}
	else
	{
		Texture2DRHI = GDynamicRHI->RHICreateTextureExternal2D(GetSizeX(), GetSizeY(), Owner->Format, Owner->NumMips, 1, Flags, CreateInfo);
		TextureRHI = Texture2DRHI;
		CurrentGuid = FGuid::NewGuid();
		//FSamplerStateInitializerRHI SamplerStateInitializer(SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp);
		//FSamplerStateRHIRef SamplerStateRHI = RHICreateSamplerState(SamplerStateInitializer);
		FExternalTextureRegistry::Get().RegisterExternalTexture(CurrentGuid, TextureRHI, SamplerStateRHI, FLinearColor(1.0f, 0.0f, 0.0f, 1.0f), FLinearColor(0.0f, 0.0f, 0.0f, 0.0f));
		Owner->SetRenderedExternalTextureGuid(CurrentGuid);
	}

	// Fill Texture with ClearColor
	::RHIFillTexture2D(Texture2DRHI, Owner->ClearColor);

	RHIUpdateTextureReference(Owner->TextureReference.TextureReferenceRHI, TextureRHI);
}	

/** Called when the resource is released. This is only called by the rendering thread. */
void FManaComponentTextureResource::ReleaseRHI()
{
	if (CurrentGuid.IsValid()) {
		FExternalTextureRegistry::Get().UnregisterExternalTexture(CurrentGuid);
		CurrentGuid.Invalidate();
	}

	RHIUpdateTextureReference(Owner->TextureReference.TextureReferenceRHI, nullptr);
	FTextureResource::ReleaseRHI();
	Texture2DRHI.SafeRelease();
}

/** Returns the Texture2DRHI, which can be used for locking/unlocking the mips. */
FTexture2DRHIRef FManaComponentTextureResource::GetTexture2DRHI()
{
	return Texture2DRHI;
}

/** Set the owner's texture reference to the given texture. */
void FManaComponentTextureResource::UpdateTextureReference(FRHITexture2D* NewTexture)
{
	TextureRHI = NewTexture;
	//Texture2DRHI = NewTexture;
	RHIUpdateTextureReference(Owner->TextureReference.TextureReferenceRHI, NewTexture);
}

/** Set the owner's texture reference to the given external texture. */
void FManaComponentTextureResource::RegisterExternalTexture(FTextureRHIRef &InTextureRHIRef, FSamplerStateRHIRef &InSamplerStateRHI, const FLinearColor &InCoordinateScaleRotation, const FLinearColor &InCoordianteOffset)
{
	FGuid PreviousGuid = CurrentGuid;

	CurrentGuid = FGuid::NewGuid();
	FExternalTextureRegistry::Get().RegisterExternalTexture(CurrentGuid, InTextureRHIRef, InSamplerStateRHI, InCoordinateScaleRotation, InCoordianteOffset);

	if (PreviousGuid.IsValid()) {
		FExternalTextureRegistry::Get().UnregisterExternalTexture(PreviousGuid);
	}

	//Update usable Guid for the RenderThread
	Owner->SetRenderedExternalTextureGuid(CurrentGuid);
}

/*-----------------------------------------------------------------------------
UManaComponentTexture
-----------------------------------------------------------------------------*/
UManaComponentTexture::UManaComponentTexture(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	/* Localization of unreal properties metadata with LOCTEXT markups and reflection */
	CRI_LOCCLASS(GetClass());
#endif

	NeverStream = true;
	Format = PF_G8;
}

void UManaComponentTexture::Init(int32 InSizeX, int32 InSizeY, EPixelFormat InFormat/*=27*/, bool InIsResolveTarget/*=false*/)
{
	SizeX = InSizeX;
	SizeY = InSizeY;
	Format = (EPixelFormat)InFormat;
	NumMips = 1;
	bIsResolveTarget = InIsResolveTarget;

	// Initialize the resource.
	UpdateResource();
}

void UManaComponentTexture::UpdateSizeAndFormat(EManaComponentTextureType ComponentTextureType, int32 InSizeX, int32 InSizeY, EPixelFormat InFormat/*=27*/)
{
	EPixelFormat DesiredFormat = EPixelFormat(InFormat);
	if (InSizeX > 0 && InSizeY > 0)
	{
		PlaneIndex = GetPlaneIndexFromType(ComponentTextureType);
		ComponentType = ComponentTextureType;
		if (ComponentTextureType == EComponent::Texture_U
			|| ComponentTextureType == EComponent::Texture_V
			|| ComponentTextureType == EComponent::Texture_UV) {
			InSizeX /= 2;
			InSizeY /= 2;
		}

		SizeX = InSizeX;
		SizeY = InSizeY;
		Format = DesiredFormat;
		NumMips = 1;
		bIsResolveTarget = false;
	}
}

void UManaComponentTexture::UpdateClearColor(const FLinearColor& InRGBClearColor)
{
	if (ComponentType == EComponent::Texture_RGB) {
		ClearColor = InRGBClearColor.ToFColor(true);
		ClearColor.A = 255;
	} else {
		auto YUVClearColor = ::RGBtoYUV_BT601(InRGBClearColor);
		ClearColor = (ComponentType == EComponent::Texture_Y ? FColor(YUVClearColor.R, YUVClearColor.R, YUVClearColor.R, YUVClearColor.R) :
			ComponentType == EComponent::Texture_U ? FColor(YUVClearColor.G, YUVClearColor.G, YUVClearColor.G, YUVClearColor.G) :
			ComponentType == EComponent::Texture_V ? FColor(YUVClearColor.B, YUVClearColor.B, YUVClearColor.B, YUVClearColor.B) :
			ComponentType == EComponent::Texture_UV ? FColor(YUVClearColor.G, YUVClearColor.B, YUVClearColor.G, YUVClearColor.B) :
			FColor(YUVClearColor.A, YUVClearColor.A, YUVClearColor.A, YUVClearColor.A));
	}
}

FTextureResource* UManaComponentTexture::CreateResource()
{
	if (SizeX == 0 || SizeY == 0 || NumMips == 0 || Format == PF_Unknown) {
		return nullptr;
	}

	return new FManaComponentTextureResource(this);
}

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION > 17
EMaterialValueType UManaComponentTexture::GetMaterialType() const
#else
EMaterialValueType UManaComponentTexture::GetMaterialType()
#endif
{
	if (ComponentType == EComponent::Texture_External) {
		return MCT_TextureExternal;
	}
	return MCT_Texture2D;
}

float UManaComponentTexture::GetSurfaceWidth() const
{
	return SizeX;
}

float UManaComponentTexture::GetSurfaceHeight() const
{
	return SizeY;
}

FGuid UManaComponentTexture::GetExternalTextureGuid() const
{
	FScopeLock Lock(&CriticalSection);
	return CurrentRenderedGuid;
}

void UManaComponentTexture::SetRenderedExternalTextureGuid(const FGuid& InNewGuid)
{
	check(IsInRenderingThread());

	FScopeLock Lock(&CriticalSection);
	CurrentRenderedGuid = InNewGuid;
}

void UManaComponentTexture::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
}

UManaComponentTexture* UManaComponentTexture::Create(UManaTexture* Owner, EManaComponentTextureType ComponentTextureType, int32 InSizeX, int32 InSizeY, EPixelFormat InFormat/*=27*/)
{
	EPixelFormat DesiredFormat = EPixelFormat(InFormat);
	if (InSizeX > 0 && InSizeY > 0)
	{
		if (ComponentTextureType == EComponent::Texture_U
			|| ComponentTextureType == EComponent::Texture_V
			|| ComponentTextureType == EComponent::Texture_UV) {
			InSizeX /= 2;
			InSizeY /= 2;
		}

		int32 PlaneIndex = GetPlaneIndexFromType(ComponentTextureType);
		FName Name = *(Owner->GetName() + (
			PlaneIndex == static_cast<uint32>(EComponent::Texture_Y) ? TEXT("_Y") :
			PlaneIndex == static_cast<uint32>(EComponent::Texture_U) ? TEXT("_U") :
			PlaneIndex == static_cast<uint32>(EComponent::Texture_V) ? TEXT("_V") :
			PlaneIndex == static_cast<uint32>(EComponent::Texture_A) ? TEXT("_A") :
			TEXT("_X")));

		auto NewTexture = NewObject<UManaComponentTexture>(Owner, UManaComponentTexture::StaticClass(), Name, RF_Public);
		if (NewTexture != NULL)
		{
			// Disable compression
			NewTexture->CompressionSettings = TC_Default;
#if WITH_EDITORONLY_DATA
			NewTexture->CompressionNone = true;
			NewTexture->MipGenSettings = TMGS_NoMipmaps;
			NewTexture->CompressionNoAlpha = true;
			NewTexture->DeferCompression = false;
#endif // #if WITH_EDITORONLY_DATA
			NewTexture->bNoTiling = true;
			NewTexture->SRGB = false;
			NewTexture->Init(InSizeX, InSizeY, DesiredFormat, false);
			NewTexture->PlaneIndex = PlaneIndex;
			NewTexture->ComponentType = ComponentTextureType;
			NewTexture->UpdateClearColor(Owner->ClearColor);
		}

		return NewTexture;
	}
	else
	{
		UE_LOG(LogTexture, Warning, TEXT("Invalid parameters specified for UManaComponentTexture::Create()"));
		return NULL;
	}
}

int32 UManaComponentTexture::GetPlaneIndexFromType(EManaComponentTextureType ComponentTextureType)
{
	return ComponentTextureType == EComponent::Texture_UV ? 1 :
		   ComponentTextureType == EComponent::Texture_RGB ? 0 :
		   ComponentTextureType == EComponent::Texture_External ? 0 :
		   static_cast<int32>(ComponentTextureType);
}

/***************************************************************************
 *      関数定義
 *      Function Definition
 ***************************************************************************/



#undef LOCTEXT_NAMESPACE

/* --- end of file --- */
