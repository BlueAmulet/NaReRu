/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2015-2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Mana Movie Component Texture
 * File     : ManaComponentTexture.h
 *
 ****************************************************************************/
#pragma once

#include "TextureResource.h"
#include "Engine/Texture.h"
#include "../Launch/Resources/Version.h"
#include "ManaComponentTexture.generated.h"

class UManaTexture;
class UManaComponentTexture;

/** Component texture type enumeration. */
UENUM(BlueprintType)
enum class EManaComponentTextureType : uint8
{
	Texture_Y = 0,
	Texture_U,
	Texture_V,
	Texture_A,
	Texture_UV,
	Texture_RGB,
	Texture_External
};

/** A component texture resource. */
class FManaComponentTextureResource : public FTextureResource
{
public:
	/** Initialization constructor. */
	FManaComponentTextureResource(class UManaComponentTexture* InOwner);

	/** Returns the width of the texture in pixels. */
	virtual uint32 GetSizeX() const override;

	/** Returns the height of the texture in pixels. */
	virtual uint32 GetSizeY() const override;

	/** Called when the resource is initialized. This is only called by the rendering thread. */
	virtual void InitRHI() override;

	/** Called when the resource is released. This is only called by the rendering thread. */
	virtual void ReleaseRHI() override;

	/** Returns the Texture2DRHI, which can be used for locking/unlocking the mips. */
	FTexture2DRHIRef GetTexture2DRHI();

	/**
	 * Set the owner's texture reference to the given texture.
	 *
	 * @param NewTexture The texture to set.
	 */
	void UpdateTextureReference(FRHITexture2D* NewTexture);
	
	/**
	 *
	 */
	void RegisterExternalTexture(FTextureRHIRef &InTextureRHIRef, FSamplerStateRHIRef &InSamplerStateRHI, const FLinearColor &InCoordinateScaleRotation = FLinearColor(1, 0, 0, 1), const FLinearColor &InCoordianteOffset = FLinearColor(0, 0, 0, 0));

private:
	/** The owner of this resource. */
	class UManaComponentTexture* Owner;
	/** Texture2D reference, used for locking/unlocking the mips. */
	FTexture2DRHIRef Texture2DRHI;
	/** The current Guid used with external texture. */
	FGuid CurrentGuid;
};

UCLASS(hidecategories = Object, MinimalAPI, BlueprintType)
class UManaComponentTexture : public UTexture
{
	GENERATED_UCLASS_BODY()

	/** The width of the texture. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana)
	int32 SizeX;

	/** The height of the texture. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana)
	int32 SizeY;

	/** The format of the texture. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana)
	TEnumAsByte<enum EPixelFormat> Format;

	/** The component type of the texture */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana, meta = (DisplayName = "Component"))
	EManaComponentTextureType ComponentType;

	/** The addressing mode used for the X axis of this texture. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana, meta = (DisplayName = "X-axis Tiling Method"), AssetRegistrySearchable, AdvancedDisplay)
	TEnumAsByte<enum TextureAddress> AddressX;

	/** The addressing mode used for the Y axis of this texture. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana, meta = (DisplayName = "Y-axis Tiling Method"), AssetRegistrySearchable, AdvancedDisplay)
	TEnumAsByte<enum TextureAddress> AddressY;

public:
	/** The number of mip-maps in the texture. */
	int32 NumMips;

	/** The plane index of the Texture. */
	uint32 PlaneIndex;

	/** Whether the texture can be used as a resolve target. */
	uint32 bIsResolveTarget : 1;

	/** The color used to clear the texture if no video data is drawn. */
	FColor ClearColor;

private:
	/** The last Guid that was rendered and registered in the render command*/
	FGuid CurrentRenderedGuid;

	/** Critical section to protect last rendered guid since it can be read from anywhere. */
	mutable FCriticalSection CriticalSection;

public:
	//~ Begin UTexture Interface.
	virtual FTextureResource* CreateResource() override;

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION > 17
	virtual EMaterialValueType GetMaterialType() const override;
#else
	virtual EMaterialValueType GetMaterialType() override;
#endif

	virtual float GetSurfaceWidth() const override;
	virtual float GetSurfaceHeight() const override;
	virtual FGuid GetExternalTextureGuid() const override;
	//~ End UTexture Interface.

	void SetRenderedExternalTextureGuid(const FGuid& InNewGuid);

	//~ Begin UObject Interface.
	virtual void Serialize(FArchive& Ar) override;
	//~ End UObject Interface.

	/**
	* Initializes the texture with 1 mip-level and creates the render resource.
	*
	* @param InSizeX			- Width of the texture, in texels
	* @param InSizeY			- Height of the texture, in texels
	* @param InFormat			- Format of the texture, defaults to PF_G8
	* @param InIsResolveTarget	- Whether the texture can be used as a resolve target
	*/
	void CRIWARERUNTIME_API Init(int32 InSizeX, int32 InSizeY, EPixelFormat InFormat = PF_G8, bool InIsResolveTarget = false);

	void CRIWARERUNTIME_API UpdateSizeAndFormat(EManaComponentTextureType ComponentTextureType, int32 InSizeX, int32 InSizeY, EPixelFormat InFormat = PF_G8);

	void CRIWARERUNTIME_API UpdateClearColor(const FLinearColor& InRGBClearColor);

	/** Creates and initializes a new Texture2DDynamic with the requested settings */
	static CRIWARERUNTIME_API UManaComponentTexture* Create(UManaTexture* Owner, EManaComponentTextureType ComponentTextureType, int32 InSizeX, int32 InSizeY, EPixelFormat InFormat = PF_G8);

	/** Convert EManaComponentTextureType to its corresponding plane index */
	static CRIWARERUNTIME_API int32 GetPlaneIndexFromType(EManaComponentTextureType ComponentTextureType);
};
