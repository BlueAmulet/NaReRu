#pragma once

#include "ManaTexture.h"
#include "Materials/MaterialExpression.h"
#include "Runtime/Launch/Resources/Version.h"
#include "MaterialExpressionManaColorSpaceConverter.generated.h"

UCLASS(collapsecategories, hidecategories = Object, MinimalAPI)
class UMaterialExpressionManaColorSpaceConverter : public UMaterialExpression
{
	GENERATED_UCLASS_BODY()
	
	UPROPERTY(Meta = (RequiredInput = "true", Tooltip = "Luma Component Channel - (Used as RGB with packed RGB format.)"))
	FExpressionInput Luma;

	UPROPERTY(Meta = (RequiredInput = "true", Tooltip = "Chroma Blue-difference Component Channel - (Used as UV with NV12 format.)"))
	FExpressionInput ChromaB;

	UPROPERTY(Meta = (RequiredInput = "true", Tooltip = "Chroma Red-difference Component Channel"))
	FExpressionInput ChromaR;

	UPROPERTY(Meta = (RequiredInput = "true", Tooltip = "Alpha Channel"))
	FExpressionInput Alpha;

	UPROPERTY(Meta = (RequiredInput = "false", ToolTip = "Gamma Exponent - Defaults to sRGB-to-linear function or 'Const Gamma' if not hooked up."))
	FExpressionInput Gamma;

	/** Enable different pixel format by platform for H264 movies. This functionality is now provided with MovieType. */
	UPROPERTY()
	bool bH264_DEPRECATED;

	/** Inform the input movie type used for platform dependent pixel format conversion. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MaterialExpressionManaColorSpaceConverter, DisplayName = "Movie Type", Meta = (ToolTip = "Inform the input movie type used for platform dependent pixel format conversion."))
	EManaMovieType MovieType;

	/** Color Space to use when converting Y'CrCb to Linear. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MaterialExpressionManaColorSpaceConverter, DisplayName = "Color Space", Meta = (ToolTip = "Color Space to use when converting Y'CrCb to Linear."))
	EManaColorSpace ColorSpace;

	/** Only used if Gamma is not hooked up. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MaterialExpressionManaColorSpaceConverter, DisplayName = "Use Gamma Exponent", Meta = (ToolTip = "Only used if Gamma is not hooked up. If false, default Gamma correction function is used."))
	bool bUseGammaExponent;

	/** Only used if Gamma is not hooked up. */
	UPROPERTY(EditAnywhere, Category = MaterialExpressionManaColorSpaceConverter, meta = (OverridingInputProperty = "Gamma", Tooltip = "Only used if Gamma is not hooked up.", EditCondition = "bUseGammaExponent"))
	float ConstGamma;

	//~ Begin UObject Interface.
	virtual void PostLoad() override;
	virtual void Serialize(FStructuredArchive::FRecord Record) override;
	//~ End UObject Interface.

	//~ Begin UMaterialExpression Interface
#if WITH_EDITOR
	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
	virtual int32 GetWidth() const;
#endif // WITH_EDITOR
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 19
#if WITH_EDITOR
	virtual FName GetInputName(int32 InputIndex) const override;
#endif
#else 
	virtual FString GetInputName(int32 InputIndex) const override;
#endif
#if WITH_EDITOR
	virtual bool IsResultMaterialAttributes(int32 OutputIndex) override;
	virtual uint32 GetInputType(int32 InputIndex) override { return InputIndex < 2 ? MCT_Float3 : MCT_Float1; }
	virtual uint32 GetOutputType(int32 OutputIndex) override { return MCT_Float; }
#endif // WITH_EDITOR
	//~ End UMaterialExpression Interface
};