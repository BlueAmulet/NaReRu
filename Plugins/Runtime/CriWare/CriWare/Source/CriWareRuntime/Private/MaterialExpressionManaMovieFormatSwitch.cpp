#include "MaterialExpressionManaMovieFormatSwitch.h"

#include "CriWareRuntimePrivatePCH.h"

#include "MaterialCompiler.h"

#if WITH_EDITORONLY_DATA
#include "Materials/HLSLMaterialTranslator.h"
#endif

#define LOCTEXT_NAMESPACE "ManaMaterialExpressionMovieFormatSwitch"

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION < 24
#define UE_ARRAY_COUNT( array ) (ARRAY_COUNT(array))
#endif

#if WITH_EDITORONLY_DATA
namespace MaterialExpressionManaMovieFormatSwitch
{
	class FManaHLSLMaterialTranslatorPlatform : public FHLSLMaterialTranslator
	{
	public:
		EShaderPlatform GetPlatform() { return Platform; }
	};
}

using namespace MaterialExpressionManaMovieFormatSwitch;
#endif

UMaterialExpressionManaMovieFormatSwitch::UMaterialExpressionManaMovieFormatSwitch(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
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
#endif
}

#if WITH_EDITOR
int32 UMaterialExpressionManaMovieFormatSwitch::Compile(class FMaterialCompiler* Compiler, int32 OutputIndex)
{
#if	WITH_EDITORONLY_DATA
	auto Translator = static_cast<MaterialExpressionManaMovieFormatSwitch::FManaHLSLMaterialTranslatorPlatform*>(Compiler);
	
	if (Translator != nullptr)
	{
		const EShaderPlatform Platform = Translator->GetPlatform();

		if (Platform == SP_PS4
			|| Platform == SP_METAL || Platform == SP_METAL_MRT
			|| Platform == SP_METAL_SM5 || Platform == SP_METAL_SM5_NOTESS
			|| Platform == SP_METAL_MACES3_1 || Platform == SP_METAL_MRT_MAC
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 24
			|| Platform == SP_METAL_MACES2
#endif
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 21
			|| Platform == SP_METAL_TVOS || Platform == SP_METAL_MRT_TVOS
#endif
			)
		{
			// Compile NV12 Input only.
			return Inputs[static_cast<uint8>(EManaMovieFormat::NV12)].Compile(Compiler);
		}
		if (Platform == SP_SWITCH || Platform == SP_SWITCH_FORWARD)
		{
			return Inputs[static_cast<uint8>(EManaMovieFormat::RGB)].Compile(Compiler);
		}
	}
#endif

	// Compile YV12 Input only.
	return Inputs[static_cast<uint8>(EManaMovieFormat::YV12)].Compile(Compiler);
}

void UMaterialExpressionManaMovieFormatSwitch::GetCaption(TArray<FString>& OutCaptions) const
{
	OutCaptions.Add(FString(TEXT("Mana Movie Format Switch")));
}

const TArray<FExpressionInput*> UMaterialExpressionManaMovieFormatSwitch::GetInputs()
{
	TArray<FExpressionInput*> OutInputs;

	for (int32 InputIndex = 0; InputIndex < UE_ARRAY_COUNT(Inputs); InputIndex++)
	{
		OutInputs.Add(&Inputs[InputIndex]);
	}

	return OutInputs;
}

FExpressionInput* UMaterialExpressionManaMovieFormatSwitch::GetInput(int32 InputIndex)
{
	return &Inputs[InputIndex];
}

#endif // WITH_EDITOR

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 19
#if WITH_EDITOR
FName UMaterialExpressionManaMovieFormatSwitch::GetInputName(int32 InputIndex) const
{
	return FName(InputIndex == 0 ? TEXT("NV12 (YUV 4:2:0 bi-planar)") : 
				 InputIndex == 1 ? TEXT("YV12 (YUV 4:2:0 tri-planar)") :
				 TEXT("RGB (RGB 4:4:4 packed)"));
}
#endif
#else
FString UMaterialExpressionManaMovieFormatSwitch::GetInputName(int32 InputIndex) const
{
	return FString(InputIndex == 0 ? TEXT("NV12 (YUV 4:2:0 bi-planar)") : TEXT("YV12 (YUV 4:2:0 tri-planar)"));
}
#endif

#if WITH_EDITOR
bool UMaterialExpressionManaMovieFormatSwitch::IsResultMaterialAttributes(int32 OutputIndex)
{
	check(OutputIndex == 0);
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
