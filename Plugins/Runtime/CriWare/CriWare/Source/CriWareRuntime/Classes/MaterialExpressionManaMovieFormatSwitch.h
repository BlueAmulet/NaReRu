#pragma once

#include "ManaTexture.h"
#include "Materials/MaterialExpression.h"
#include "Runtime/Launch/Resources/Version.h"
#include "MaterialExpressionManaMovieFormatSwitch.generated.h"

UCLASS(collapsecategories, hidecategories = Object, MinimalAPI)
class UMaterialExpressionManaMovieFormatSwitch : public UMaterialExpression
{
	GENERATED_UCLASS_BODY()

	UPROPERTY()
	FExpressionInput Inputs[static_cast<uint8>(EManaMovieFormat::Num)];

	//~ Begin UMaterialExpression Interface
#if WITH_EDITOR
	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
	virtual const TArray<FExpressionInput*> GetInputs() override;
	virtual FExpressionInput* GetInput(int32 InputIndex) override;
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
	virtual uint32 GetInputType(int32 InputIndex) override { return MCT_Unknown; }
	virtual uint32 GetOutputType(int32 OutputIndex) override { return MCT_Unknown; }
#endif // WITH_EDITOR
	//~ End UMaterialExpression Interface
};