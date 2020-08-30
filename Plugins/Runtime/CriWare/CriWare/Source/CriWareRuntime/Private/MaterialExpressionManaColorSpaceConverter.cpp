#include "MaterialExpressionManaColorSpaceConverter.h"

#include "CriWareRuntimePrivatePCH.h"

#include "MaterialCompiler.h"
#include "Serialization/CustomVersion.h"

#if WITH_EDITORONLY_DATA
#include "Materials/HLSLMaterialTranslator.h"
#endif

#define LOCTEXT_NAMESPACE "ManaMaterialExpressionMovieFormatSwitch"

struct FManaColorSpaceConverterCustomVersion
{
	enum Type
	{
		// Before any version changes were made in converter.
		BeforeCustomVersionWasAdded = 0,

		// Can choose which movie type source will be converted.
		UseMovieType,

		// -----<new versions can be added above this line>-------------------------------------------------
		VersionPlusOne,
		LatestVersion = VersionPlusOne - 1
	};

	static const FGuid GUID;
};

const FGuid FManaColorSpaceConverterCustomVersion::GUID(0x3D30FE9E, 0x455B11E9, 0x9DEE88B6, 0x8D2D6541);

// Register the custom version with core
FCustomVersionRegistration GRegisterManaColorSpaceConverterCustomVersion(FManaColorSpaceConverterCustomVersion::GUID, FManaColorSpaceConverterCustomVersion::LatestVersion, TEXT("ManaColorSpaceConverterVer"));

namespace MaterialExpressionManaColorSpaceConverter
{
#if WITH_EDITORONLY_DATA
	class FManaHLSLMaterialTranslatorPlatform : public FHLSLMaterialTranslator
	{
	public:
		FORCEINLINE EShaderPlatform GetPlatform() const { return Platform; }
		FORCEINLINE FMaterial* GetMaterial() const { return Material; }
	};
#endif
	const float LimitedRangeCoefs[] = { 0.0625f, 1.164383f, 0.5f, 0.5f };
	const float Rec601Coefs[] = { 1.596027f, -0.391762f, -0.812968f, 2.017232f };
	const float Rec709Coefs[] = { 1.792700f, -0.213300f, -0.532900f, 2.112400f };
}

using namespace MaterialExpressionManaColorSpaceConverter;

UMaterialExpressionManaColorSpaceConverter::UMaterialExpressionManaColorSpaceConverter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bH264_DEPRECATED(false)
	, MovieType(EManaMovieType::Unknown)
	, ColorSpace(EManaColorSpace::Rec601)
	, bUseGammaExponent(false)
	, ConstGamma(2.2f)
{
#if WITH_EDITOR
	/* Localization of unreal properties metadata with LOCTEXT markups and reflection */
	CRI_LOCCLASS(GetClass());
#endif

	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		FText NAME_Mana;
		FConstructorStatics()
			: NAME_Mana(LOCTEXT("Mana", "Mana"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

#if WITH_EDITORONLY_DATA
	MenuCategories.Add(ConstructorStatics.NAME_Mana);

	Outputs.Reset();
	Outputs.Add(FExpressionOutput(TEXT(""), 1, 1, 1, 1, 0));
	Outputs.Add(FExpressionOutput(TEXT(""), 1, 1, 0, 0, 0));
	Outputs.Add(FExpressionOutput(TEXT(""), 1, 0, 1, 0, 0));
	Outputs.Add(FExpressionOutput(TEXT(""), 1, 0, 0, 1, 0));
	Outputs.Add(FExpressionOutput(TEXT(""), 1, 0, 0, 0, 1));

	bCollapsed = false;
#endif
}

void UMaterialExpressionManaColorSpaceConverter::PostLoad()
{
#if WITH_EDITORONLY_DATA
	const auto ManaColorSpaceConverterVer = GetLinkerCustomVersion(FManaColorSpaceConverterCustomVersion::GUID);

	bool bUpdated = false;

	if (ManaColorSpaceConverterVer < FManaColorSpaceConverterCustomVersion::UseMovieType) {
		if (bH264_DEPRECATED) {
			MovieType = EManaMovieType::H264;
		} else {
			MovieType = EManaMovieType::SofdecPrime;
		}

		bH264_DEPRECATED = false;
		bUpdated = true;
	}

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

void UMaterialExpressionManaColorSpaceConverter::Serialize(FStructuredArchive::FRecord Record)
{
	Super::Serialize(Record);

	Record.GetUnderlyingArchive().UsingCustomVersion(FManaColorSpaceConverterCustomVersion::GUID);
}

#if WITH_EDITOR
int32 UMaterialExpressionManaColorSpaceConverter::Compile(class FMaterialCompiler* Compiler, int32 OutputIndex)
{
	EManaMovieFormat Format = EManaMovieFormat::DEFAULT; // YV12
	bool bYUVToRGB = true;
	int32 YInput = INDEX_NONE, UInput = INDEX_NONE, VInput = INDEX_NONE, RGBOutput = INDEX_NONE;

	// select format
#if	WITH_EDITORONLY_DATA
	if (MovieType == EManaMovieType::H264 || MovieType == EManaMovieType::VP9) { // H264 and VP9 decoders have specific format by platform
		auto Translator = static_cast<MaterialExpressionManaColorSpaceConverter::FManaHLSLMaterialTranslatorPlatform*>(Compiler);
		if (Translator != nullptr) {
			const EShaderPlatform Platform = Translator->GetPlatform();

			// H264 on PS4 or METAL
            if (MovieType == EManaMovieType::H264
				&& (Platform == SP_PS4
				|| Platform == SP_METAL || Platform == SP_METAL_MRT
				|| Platform == SP_METAL_SM5 || Platform == SP_METAL_SM5_NOTESS
				|| Platform == SP_METAL_MACES3_1 || Platform == SP_METAL_MRT_MAC
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 24
				|| Platform == SP_METAL_MACES2
#endif
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 21
				|| Platform == SP_METAL_TVOS || Platform == SP_METAL_MRT_TVOS
#endif
				))
            {
				Format = EManaMovieFormat::NV12;
			} 
			// Switch H264 & VP9
			else if (Platform == SP_SWITCH || Platform == SP_SWITCH_FORWARD) {
				Format = EManaMovieFormat::RGB;
			}
		}
	}
#endif

	// get inputs
	switch (Format) {
	case EManaMovieFormat::YV12:
	{
		// YV12 Inputs
		YInput = Compiler->ComponentMask(Luma.Compile(Compiler), true, false, false, false);
		UInput = Compiler->ComponentMask(ChromaB.Compile(Compiler), true, false, false, false);
		VInput = Compiler->ComponentMask(ChromaR.Compile(Compiler), true, false, false, false);
	}
	break;
	case EManaMovieFormat::NV12:
	{
		// NV12 Inputs
		YInput = Compiler->ComponentMask(Luma.Compile(Compiler), true, false, false, false);
		int32 UVInput = ChromaB.Compile(Compiler);
		UInput = Compiler->ComponentMask(UVInput, true, false, false, false);
		VInput = Compiler->ComponentMask(UVInput, false, true, false, false);
	}
	break;
	case EManaMovieFormat::RGB:
	{
		// RGB Input
		int32 RGBAInput = Luma.Compile(Compiler);
		RGBOutput = Compiler->ComponentMask(RGBAInput, true, true, true, false);
		bYUVToRGB = false;
	}
	break;
	}

	// YUV to RGB convertion
	if (bYUVToRGB) {
		const float* ColorSpaceCoefs = (ColorSpace == EManaColorSpace::Rec601 ? Rec601Coefs : Rec709Coefs);

		int32 Y = Compiler->Mul(Compiler->Sub(YInput, Compiler->Constant(LimitedRangeCoefs[0])), Compiler->Constant(LimitedRangeCoefs[1])); // float Y = (YTex - 0.0625) * 1.164
		int32 U = Compiler->Sub(UInput, Compiler->Constant(LimitedRangeCoefs[2])); // float U = UTex - 0.5
		int32 V = Compiler->Sub(VInput, Compiler->Constant(LimitedRangeCoefs[3])); // float V = VTex - 0.5

		int32 R = Compiler->Add(Y, Compiler->Mul(V, Compiler->Constant(ColorSpaceCoefs[0]))); // float R = Y + V * 1.596
		int32 G = Compiler->Add(Y, Compiler->Add(Compiler->Mul(U, Compiler->Constant(ColorSpaceCoefs[1])), Compiler->Mul(V, Compiler->Constant(ColorSpaceCoefs[2])))); // float G = Y + U * -0.813 + V * -0.392
		int32 B = Compiler->Add(Y, Compiler->Mul(U, Compiler->Constant(ColorSpaceCoefs[3]))); // float B = Y + U * 2.017

		RGBOutput = Compiler->AppendVector(Compiler->AppendVector(R, G), B); // float3 RGB = float3(R, G, B)
	}

	// gamma correction
	if (Gamma.GetTracedInput().Expression || bUseGammaExponent) {
		// simple power
		// if the input is hooked up, use it, otherwise use the internal constant
		int32 GammaInput = Gamma.GetTracedInput().Expression ? Gamma.Compile(Compiler) : Compiler->Constant(ConstGamma);
		RGBOutput = Compiler->Power(RGBOutput, GammaInput);
	} else {
		// sRGBtoLinear
		// Color = Color > 0.04045 ? pow(Color * (1.0 / 1.055) + 0.0521327, 2.4) : Color * (1.0 / 12.92);
		RGBOutput = Compiler->Max(RGBOutput, Compiler->Constant(6.10352e-5f)); // minimum positive non-denormal (fixes black problem on DX11 AMD and NV)
		int32 AGreaterThanB = Compiler->Power(Compiler->Add(Compiler->Mul(RGBOutput, Compiler->Constant(1.0 / 1.055)), Compiler->Constant(0.0521327f)), Compiler->Constant(2.4f));
		int32 ALesserThanB = Compiler->Mul(RGBOutput, Compiler->Constant(1.0 / 12.92));
		RGBOutput = Compiler->If(RGBOutput, Compiler->Constant(0.04045f), AGreaterThanB, INDEX_NONE, ALesserThanB, Compiler->Constant(0.00001f));
	}

	// if the input is hooked up, use it, otherwise use the internal constant
	int32 AlphaInput = Alpha.GetTracedInput().Expression ? Compiler->ComponentMask(Alpha.Compile(Compiler), true, false, false, false) : Compiler->Constant(1.0f);

	return Compiler->AppendVector(RGBOutput, AlphaInput);
}

int32 UMaterialExpressionManaColorSpaceConverter::GetWidth() const
{
	return ME_STD_THUMBNAIL_SZ + (ME_STD_BORDER * 2);
}

void UMaterialExpressionManaColorSpaceConverter::GetCaption(TArray<FString>& OutCaptions) const
{
	OutCaptions.Add(FString(TEXT("Mana Color Space Converter")));
}
#endif // WITH_EDITOR

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 19
#if WITH_EDITOR
FName UMaterialExpressionManaColorSpaceConverter::GetInputName(int32 InputIndex) const
{
	return FName(InputIndex == 0 ? TEXT("Luma (Y)") :
				 InputIndex == 1 ? TEXT("Chroma b (U)") :
				 InputIndex == 2 ? TEXT("Chroma r (V)") :
				 InputIndex == 3 ? TEXT("Alpha (A)") :
				 TEXT("Gamma"));
}
#endif
#else
FString UMaterialExpressionManaColorSpaceConverter::GetInputName(int32 InputIndex) const
{
	return FString(InputIndex == 0 ? TEXT("Luma (Y)") :
				   InputIndex == 1 ? TEXT("Chroma b (U)") :
				   InputIndex == 2 ? TEXT("Chroma r (V)") :
				   InputIndex == 3 ? TEXT("Alpha (A)") :
				   TEXT("Gamma"));
}
#endif

#if WITH_EDITOR
bool UMaterialExpressionManaColorSpaceConverter::IsResultMaterialAttributes(int32 OutputIndex)
{
	//check(OutputIndex == 0);
	TArray<FExpressionInput*> ExpressionInputs = GetInputs();

	for (FExpressionInput* ExpressionInput : ExpressionInputs)
	{
		// If there is a loop anywhere in this expression's inputs then we can't risk checking them
		if (ExpressionInput->GetTracedInput().Expression && !ExpressionInput->Expression->ContainsInputLoop() && ExpressionInput->Expression->IsResultMaterialAttributes(ExpressionInput->OutputIndex))
		{
			return true;
		}
	}

	return false;
}
#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE
