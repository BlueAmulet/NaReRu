/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2015-2016 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : YUVToRGB Color Transform Shader
 * File     : ManaColorTransformShader.h
 *
 ****************************************************************************/

/* 多重定義防止 */
#pragma once

#include "GlobalShader.h"
#include "RHICommandList.h"
#include "RHIResources.h"
#include "RenderResource.h"
#include "Runtime/Launch/Resources/Version.h"
#include <array>

#if	ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >=25
#define MINIMUM_RHI_FEATURE_LEVEL ERHIFeatureLevel::ES3_1
#else 
#define MINIMUM_RHI_FEATURE_LEVEL ERHIFeatureLevel::ES2
#endif

/* YUVをRGBへ色変換するピクセルシェーダ（アルファなし） */
class FManaColorTransformPixelShader_YUV_to_RGB : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FManaColorTransformPixelShader_YUV_to_RGB, Global);

public:
	FManaColorTransformPixelShader_YUV_to_RGB() {}
	explicit FManaColorTransformPixelShader_YUV_to_RGB(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
	}

	static bool ShouldCache(EShaderPlatform Platform) { return IsFeatureLevelSupported(Platform, MINIMUM_RHI_FEATURE_LEVEL); }

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 19
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, MINIMUM_RHI_FEATURE_LEVEL);
	}
#endif

	//virtual bool Serialize(FArchive& Ar) override
	//{
	//	bool bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);		
	//	return bShaderHasOutdatedParameters;
	//}

	/* ピクセルシェーダに渡すYUVテクスチャを設定 */
	void SetParameters(FRHICommandList& RHICmdList, const std::array<FTexture2DRHIRef, 4>& YUVTextures, float GammaCorrection);

private:
	/* YUVテクスチャ配列にアクセスする際のインデックス */
	static const size_t PlaneIndex_Y = 0;
	static const size_t PlaneIndex_U = 1;
	static const size_t PlaneIndex_V = 2;
};

/* YUVAをRGBAへ色変換するピクセルシェーダ（アルファあり） */
class FManaColorTransformPixelShader_YUVA_to_RGBA : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FManaColorTransformPixelShader_YUVA_to_RGBA, Global);

public:
	FManaColorTransformPixelShader_YUVA_to_RGBA() {}
	explicit FManaColorTransformPixelShader_YUVA_to_RGBA(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
	}

	static bool ShouldCache(EShaderPlatform Platform) { return IsFeatureLevelSupported(Platform, MINIMUM_RHI_FEATURE_LEVEL); }

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 19
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, MINIMUM_RHI_FEATURE_LEVEL);
	}
#endif

	//virtual bool Serialize(FArchive& Ar) override
	//{
	//	bool bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);		
	//	return bShaderHasOutdatedParameters;
	//}

	/* ピクセルシェーダに渡すYUVテクスチャを設定 */
	void SetParameters(FRHICommandList& RHICmdList, const std::array<FTexture2DRHIRef, 4>& YUVTextures, float GammaCorrection);

private:
	/* YUVAテクスチャ配列にアクセスする際のインデックス */
	static const size_t PlaneIndex_Y = 0;
	static const size_t PlaneIndex_U = 1;
	static const size_t PlaneIndex_V = 2;
	static const size_t PlaneIndex_A = 3;
};

/* NV12をRGBへ色変換するピクセルシェーダ（アルファなし） */
class FManaColorTransformPixelShader_NV12_to_RGB : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FManaColorTransformPixelShader_NV12_to_RGB, Global);

public:
	FManaColorTransformPixelShader_NV12_to_RGB() {}
	explicit FManaColorTransformPixelShader_NV12_to_RGB(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
	}

	static bool ShouldCache(EShaderPlatform Platform) { return IsFeatureLevelSupported(Platform, MINIMUM_RHI_FEATURE_LEVEL); }

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 19
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, MINIMUM_RHI_FEATURE_LEVEL);
	}
#endif

	//virtual bool Serialize(FArchive& Ar) override
	//{
	//	bool bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);
	//	return bShaderHasOutdatedParameters;
	//}

	/* ピクセルシェーダに渡すYUVテクスチャを設定 */
	void SetParameters(FRHICommandList& RHICmdList, const std::array<FTexture2DRHIRef, 4>& YUVTextures, float GammaCorrection);

private:
	/* NV12テクスチャ配列にアクセスする際のインデックス */
	static const size_t PlaneIndex_Y = 0;
	static const size_t PlaneIndex_UV = 1;
};

/* NV12AをRGBAへ色変換するピクセルシェーダ（アルファなし） */
class FManaColorTransformPixelShader_NV12A_to_RGBA : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FManaColorTransformPixelShader_NV12A_to_RGBA, Global);

public:
	FManaColorTransformPixelShader_NV12A_to_RGBA() {}
	explicit FManaColorTransformPixelShader_NV12A_to_RGBA(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
	}

	static bool ShouldCache(EShaderPlatform Platform) { return IsFeatureLevelSupported(Platform, MINIMUM_RHI_FEATURE_LEVEL); }

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 19
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, MINIMUM_RHI_FEATURE_LEVEL);
	}
#endif

	//virtual bool Serialize(FArchive& Ar) override
	//{
	//	bool bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);
	//	return bShaderHasOutdatedParameters;
	//}

	/* ピクセルシェーダに渡すYUVテクスチャを設定 */
	void SetParameters(FRHICommandList& RHICmdList, const std::array<FTexture2DRHIRef, 4>& YUVTextures, float GammaCorrection);

private:
	/* NV12Aテクスチャ配列にアクセスする際のインデックス */
	static const size_t PlaneIndex_Y = 0;
	static const size_t PlaneIndex_UV = 1;
	static const size_t PlaneIndex_A = 2;
};

/* RGBをRGBへ色変換するピクセルシェーダ（アルファなし） */
class FManaColorTransformPixelShader_RGB_to_RGB : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FManaColorTransformPixelShader_RGB_to_RGB, Global);

public:
	FManaColorTransformPixelShader_RGB_to_RGB() {}
	explicit FManaColorTransformPixelShader_RGB_to_RGB(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
	}

	static bool ShouldCache(EShaderPlatform Platform) { return IsFeatureLevelSupported(Platform, MINIMUM_RHI_FEATURE_LEVEL); }

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 19
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, MINIMUM_RHI_FEATURE_LEVEL);
	}
#endif

	//virtual bool Serialize(FArchive& Ar) override
	//{
	//	bool bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);
	//	return bShaderHasOutdatedParameters;
	//}

	/* ピクセルシェーダに渡すRGBテクスチャを設定 */
	void SetParameters(FRHICommandList& RHICmdList, const std::array<FTexture2DRHIRef, 4>& RGBTextures, float GammaCorrection);

private:
	/* RGBテクスチャ配列にアクセスする際のインデックス */
	static const size_t PlaneIndex_RGB = 0;
};

/* RGBAをRGBAへ色変換するピクセルシェーダ（アルファなし） */
class FManaColorTransformPixelShader_RGBA_to_RGBA : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FManaColorTransformPixelShader_RGBA_to_RGBA, Global);

public:
	FManaColorTransformPixelShader_RGBA_to_RGBA() {}
	explicit FManaColorTransformPixelShader_RGBA_to_RGBA(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
	}

	static bool ShouldCache(EShaderPlatform Platform) { return IsFeatureLevelSupported(Platform, MINIMUM_RHI_FEATURE_LEVEL); }

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 19
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, MINIMUM_RHI_FEATURE_LEVEL);
	}
#endif

	//virtual bool Serialize(FArchive& Ar) override
	//{
	//	bool bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);
	//	return bShaderHasOutdatedParameters;
	//}

	/* ピクセルシェーダに渡すRGBテクスチャを設定 */
	void SetParameters(FRHICommandList& RHICmdList, const std::array<FTexture2DRHIRef, 4>& RGBTextures, float GammaCorrection);

private:
	/* RGBAテクスチャ配列にアクセスする際のインデックス */
	static const size_t PlaneIndex_RGB = 0;
	static const size_t PlaneIndex_A = 1;
};

/* 頂点情報をパススルーする頂点シェーダ */
class FManaPaththroughVertexShader : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FManaPaththroughVertexShader, Global);

public:
	FManaPaththroughVertexShader() {}
	explicit FManaPaththroughVertexShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{}

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 19
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, MINIMUM_RHI_FEATURE_LEVEL);
	}
#endif

	static bool ShouldCache(EShaderPlatform Platform) { return true; }
};

/* 頂点シェーダに渡す、全画面の四角形を表すデータ */
struct FManaElementVertex
{
	FVector4 Position;
	FVector2D UV;

	FManaElementVertex() { }

	FManaElementVertex(const FVector4& InPosition, const FVector2D& InTextureCoordinate)
		: Position(InPosition)
		, UV(InTextureCoordinate)
	{ }
};

/* 頂点シェーダへ渡すパラメータ */
class FManaVertexDeclaration : public FRenderResource
{
public:
	FVertexDeclarationRHIRef VertexDeclarationRHI;

	virtual void InitRHI() override
	{
		FVertexDeclarationElementList Elements;
		uint32 Stride = sizeof(FManaElementVertex);
		Elements.Add(FVertexElement(0, STRUCT_OFFSET(FManaElementVertex, Position), VET_Float4, 0, Stride));
		Elements.Add(FVertexElement(0, STRUCT_OFFSET(FManaElementVertex, UV), VET_Float2, 1, Stride));
		VertexDeclarationRHI = RHICreateVertexDeclaration(Elements);
	}

	virtual void ReleaseRHI() override
	{
		VertexDeclarationRHI.SafeRelease();
	}
};

/* --- end of file --- */
