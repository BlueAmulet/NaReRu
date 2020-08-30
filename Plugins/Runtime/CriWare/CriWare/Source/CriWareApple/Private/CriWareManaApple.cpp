/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE Apple plugin for Unreal Engine 4
 * Module   : Sofdec2 Mana for Apple
 * File     : CriWareManaApple.cpp
 *
 ****************************************************************************/

#include "CriWareManaApple.h"
// Unreal
#include "RHI.h"
#include "Misc/AssertionMacros.h"
// CriWare
#include "CriWareApi.h"
#include "CriWareInitializer.h"
#include "CriWarePluginSettings.h"
#include "ManaTextureResource.h"
// staticaly linked on ios - direct call possible
#if defined(XPT_TGT_IOS)
#include "cri_mana_ios.h"
#endif

/**
 * Passes a CV*TextureRef or CVPixelBufferRef through to the RHI to wrap in an RHI texture without traversing system memory.
 * @see FAvfTexture2DResourceWrapper & FMetalSurface::FMetalSurface
 */
class FCriWareManaAppleTexture2DResourceWrapper
: public FResourceBulkDataInterface
{
public:
	
	FCriWareManaAppleTexture2DResourceWrapper(CFTypeRef InImageBuffer)
	: ImageBuffer(InImageBuffer)
	{
		check(ImageBuffer);
		CFRetain(ImageBuffer);
	}
	
	virtual ~FCriWareManaAppleTexture2DResourceWrapper()
	{
		CFRelease(ImageBuffer);
		ImageBuffer = nullptr;
	}
	
public:
	
	//~ FResourceBulkDataInterface interface
	
	virtual void Discard() override
	{
		delete this;
	}
	
	virtual const void* GetResourceBulkData() const override
	{
		return ImageBuffer;
	}
	
	virtual uint32 GetResourceBulkDataSize() const override
	{
		return ImageBuffer ? ~0u : 0;
	}
	
	virtual EBulkDataType GetResourceType() const override
	{
		return EBulkDataType::MediaTexture;
	}
	
	CFTypeRef ImageBuffer;
};

/**
 * Allows for direct GPU mem allocation for texture resource from a CVImageBufferRef's system memory backing store.
 */
class FCriWareManaAppleTexture2DResourceMem
: public FResourceBulkDataInterface
{
public:
	FCriWareManaAppleTexture2DResourceMem(CVImageBufferRef InImageBuffer)
	: ImageBuffer(InImageBuffer)
	{
		check(ImageBuffer);
		CFRetain(ImageBuffer);
	}
	
	/**
	 * @return ptr to the resource memory which has been preallocated
	 */
	virtual const void* GetResourceBulkData() const override
	{
		CVPixelBufferLockBaseAddress(ImageBuffer, kCVPixelBufferLock_ReadOnly);
		return CVPixelBufferGetBaseAddress(ImageBuffer);
	}
	
	/**
	 * @return size of resource memory
	 */
	virtual uint32 GetResourceBulkDataSize() const override
	{
		int32 Pitch = CVPixelBufferGetBytesPerRow(ImageBuffer);
		int32 Height = CVPixelBufferGetHeight(ImageBuffer);
		uint32 Size = (Pitch * Height);
		
		return Size;
	}
	
	/**
	 * Free memory after it has been used to initialize RHI resource
	 */
	virtual void Discard() override
	{
		CVPixelBufferUnlockBaseAddress(ImageBuffer, kCVPixelBufferLock_ReadOnly);
		delete this;
	}
	
	virtual ~FCriWareManaAppleTexture2DResourceMem()
	{
		CFRelease(ImageBuffer);
		ImageBuffer = nullptr;
	}
	
	CVImageBufferRef ImageBuffer;
};

// ctor
FCriWareManaApple::FCriWareManaApple()
	: ICriWarePlatformMana()
{
	// Register this class to CriWareRuntime module for Mana platform specific.
	UCriWareInitializer::Platform.RegisterPlatformManaInterface(this);
}

// dtor
FCriWareManaApple::~FCriWareManaApple()
{
	UCriWareInitializer::Platform.UnregisterPlatformManaInterface();
}

void FCriWareManaApple::InitializeManaLibrary()
{
	// Load CriWare Settings if not already loaded
	GetMutableDefault<UCriWarePluginSettings>()->LoadConfig();
	// get settings
	auto Settings = GetDefault<UCriWarePluginSettings>();

	if (Settings->InitializeMana && Settings->UseH264Decoder) {
#if defined(XPT_TGT_IOS)
		// Set up H.264 decoder for IOS
		CriManaVideoToolboxH264DecoderConfig_IOS H264DecoderConfig;
		H264DecoderConfig.decoder_output = CRIMANAVIDEOTOOLBOXH264IOS_DECODE_TO_METAL_TEXTURES;
		id<MTLDevice> Device = (id<MTLDevice>)GDynamicRHI->RHIGetNativeDevice();
		check(Device);
		H264DecoderConfig.render_context = Device;
		
		criMana_SetupVideoToolboxH264Decoder_IOS(&H264DecoderConfig, nullptr, 0);
		
#elif defined(XPT_TGT_MACOSX)
		// Set up H.264 decoder for Mac
		CriManaVideoToolboxH264DecoderConfig_MACOSX H264DecoderConfig;

#if COREVIDEO_SUPPORTS_METAL
		H264DecoderConfig.decoder_output = CRIMANAVIDEOTOOLBOXH264MACOSX_DECODE_TO_METAL_NV12_TEXTURES;
		id<MTLDevice> Device = (id<MTLDevice>)GDynamicRHI->RHIGetNativeDevice();
		check(Device);
		H264DecoderConfig.render_context = Device;
#else
		H264DecoderConfig.decoder_output = CRIMANAVIDEOTOOLBOXH264MACOSX_DECODE_TO_IMAGE_BUFFER;
		// tips: with Image Buffer output mode, if render_context is not null, it overwrites
		// the output type. (default is kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange)
		H264DecoderConfig.render_context = nullptr;
#endif

		criMana_SetupVideoToolboxH264Decoder_MACOSX(&H264DecoderConfig, nullptr, 0);
#endif
	}
}

void FCriWareManaApple::FinalizeManaLibrary()
{
	auto Settings = GetDefault<UCriWarePluginSettings>();

	if (Settings->InitializeMana && Settings->UseH264Decoder) {
#if defined(XPT_TGT_IOS) || defined(XPT_TGT_MACOSX)
#endif
	}
}

void FCriWareManaApple::InitializeManaComponent(UManaComponent& ManaComponent)
{
	auto Settings = GetDefault<UCriWarePluginSettings>();

	if (Settings->UseH264Decoder) {
		// add component extension object for Apple
		auto ComponentApple = NewObject<UManaComponentApple>(&ManaComponent);
		if (ComponentApple != nullptr) {
			ComponentApple->AddToRoot();
			ComponentApple->ManaApple = this;
			ComponentApple->ManaComponent = &ManaComponent;
			ManaComponent.OnStatusChanged.AddDynamic(ComponentApple, &UManaComponentApple::OnManaComponentStatusChanged);
			ComponentApple->InitializeResources();
			// add to map
			ManaComponents.Add(&ManaComponent, ComponentApple);
		}
	}
}

void FCriWareManaApple::UninitializeManaComponent(UManaComponent& ManaComponent)
{
	auto Settings = GetDefault<UCriWarePluginSettings>();

	if (Settings->UseH264Decoder) {
		auto ComponentApple = GetManaComponentApple(&ManaComponent);
		if (ComponentApple != nullptr) {
			// remove ComponentApple
			ComponentApple->RemoveFromRoot();
			// remove delegate
			ManaComponent.OnStatusChanged.RemoveDynamic(ComponentApple, &UManaComponentApple::OnManaComponentStatusChanged);
			// remove from map
			ManaComponents.Remove(&ManaComponent);
		}
	}
}

void UManaComponentApple::InitializeResources()
{
	auto* ManaTexture = ManaComponent->GetTexture();
	if (ManaTexture == nullptr) { return; }

	auto* MovieSource = ManaComponent->GetSource();

	if (MovieSource != nullptr && MovieSource->GetNumVideoTracks() > 0) {
		CurrentMovieType = MovieSource->GetVideoTrackType(0);
	} else {
		CurrentMovieType = EManaMovieType::Unknown;
	}

	if (CurrentMovieType == EManaMovieType::H264) {
		ManaTexture->SetStreamed(true);
		VideoTextures.Init(nullptr, 3);
	} else {
		ManaTexture->SetStreamed(false);
		VideoTextures.Empty();
	}
}

void UManaComponentApple::OnManaComponentStatusChanged(EManaComponentStatus Status, UManaComponent* InManaComponent)
{
	if (InManaComponent == nullptr) { return; }
	
	if (Status == EManaComponentStatus::DecHdr) {
		InitializeResources();
	}
}

// TODO: derivate FNVNSurace with video flag and create NVTextureVideo
FRHITexture2D* FCriWareManaApple::UpdateManaTexture(const UManaTexture& Owner, const CriManaTextureBuffer& ManaTextureBuffer,
	EManaComponentTextureType ComponentType, const FIntPoint& Dimension, FRHITexture2D *Texture)
{
	check(Texture != nullptr);
	check(ManaTextureBuffer.imagebuf != nullptr);

	auto MovieType = EManaMovieType::Unknown;
	if (Owner.GetMovie()) {
		MovieType = Owner.GetMovie()->GetVideoTrackType(0);
	}

	if (MovieType == EManaMovieType::H264 && ComponentType != EManaComponentTextureType::Texture_A) {
		// H264 texture - Use native - No copy
		
		FRHIResourceCreateInfo CreateInfo;
		EPixelFormat Format = Texture->GetFormat();
		uint32 TexCreateFlags = TexCreate_Dynamic | TexCreate_NoTiling | TexCreate_ShaderResource;
		FTexture2DRHIRef NewTexture = nullptr;
		
#if COREVIDEO_SUPPORTS_METAL
		// On macOS/iOS/tvOS we use the Metal texture cache
		if (IsMetalPlatform(GMaxRHIShaderPlatform)) {
			// Metal can upload directly from an IOSurface to a 2D texture, so we can just wrap it.
			CVMetalTextureRef TextureRef = reinterpret_cast<CVMetalTextureRef>(ManaTextureBuffer.imagebuf);
			CreateInfo.BulkData = new FCriWareManaAppleTexture2DResourceWrapper(TextureRef);
			CreateInfo.ResourceArray = nullptr;
		} else
			// { }  // opengl -> not supported by unreal
#endif
		{
			if (IsMetalPlatform(GMaxRHIShaderPlatform)) {
				// Metal can upload directly from an IOSurface to a 2D texture, so we can just wrap it.
				CreateInfo.BulkData = new FCriWareManaAppleTexture2DResourceWrapper(ManaTextureBuffer.imagebuf);
			} else {
				// OpenGL on Mac uploads as a TEXTURE_RECTANGLE for which we have no code, so upload via system memory.
				// Note: OpenGL is not supported by unreal engine since 4.14
				CreateInfo.BulkData = new FCriWareManaAppleTexture2DResourceMem((CVImageBufferRef)ManaTextureBuffer.imagebuf);
			}
			CreateInfo.ResourceArray = nullptr;
		}
		
		// create a texture with bulkdata.
		NewTexture = RHICreateTexture2D(Dimension.X, Dimension.Y, Format, 1, 1, TexCreateFlags, CreateInfo);
		
		// keep our new texture smart pointer in safe place for the draw
		auto ComponentApple = GetManaComponentApple(&Owner);
		if (ComponentApple != nullptr) {
			int32 PlaneIndex = UManaComponentTexture::GetPlaneIndexFromType(ComponentType);
			if (ComponentApple->VideoTextures.Num() <= PlaneIndex) {
				ComponentApple->VideoTextures.Add(NewTexture);
			} else {
				ComponentApple->VideoTextures[PlaneIndex] = NewTexture;
			}
			
			// return the new texture pointer to stream
			Texture = NewTexture;
		}
	} else {
		// Others - Update texture to GPU
		RHIUpdateTexture2D(
			Texture,
			0,
			FUpdateTextureRegion2D(
				0, 0, 0, 0,
				Dimension.X,
				Dimension.Y
			),
			ManaTextureBuffer.pitch,
			ManaTextureBuffer.imagebuf
		);
	}

	return Texture;
}
