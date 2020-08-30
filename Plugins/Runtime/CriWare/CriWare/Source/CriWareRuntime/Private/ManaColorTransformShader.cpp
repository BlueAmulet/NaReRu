/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2015-2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : YUVToRGB Color Transform Shader
 * File     : ManaColorTransformShader.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* モジュールヘッダ */
#include "ManaColorTransformShader.h"

/* Unreal Engine 4関連ヘッダ */
#include "CriWareRuntimePrivatePCH.h"

/* CRIWAREプラグインヘッダ */
#include "UniformBuffer.h"
#include "RHIStaticStates.h"
#include "ShaderParameterUtils.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/

/***************************************************************************
 *      処理マクロ
 *      Macro Functions
 ***************************************************************************/

/***************************************************************************
 *      データ型宣言
 *      Data Type Declarations
 ***************************************************************************/
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 22
 /* YUV平面をシェーダーへ渡すための構造体を定義（アルファなし） */
BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FManaUniformBufferYUV, )
SHADER_PARAMETER(float, GammaCorrection)
SHADER_PARAMETER_TEXTURE(Texture2D, Texture_Y)
SHADER_PARAMETER_TEXTURE(Texture2D, Texture_U)
SHADER_PARAMETER_TEXTURE(Texture2D, Texture_V)
SHADER_PARAMETER_SAMPLER(SamplerState, LumaSampler)
SHADER_PARAMETER_SAMPLER(SamplerState, CbSampler)
SHADER_PARAMETER_SAMPLER(SamplerState, CrSampler)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FManaUniformBufferYUV, "ManaUniformBufferYUV");

/* YUV平面をシェーダーへ渡すための構造体を定義（アルファあり） */
BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FManaUniformBufferYUVA, )
SHADER_PARAMETER(float, GammaCorrection)
SHADER_PARAMETER_TEXTURE(Texture2D, Texture_Y)
SHADER_PARAMETER_TEXTURE(Texture2D, Texture_U)
SHADER_PARAMETER_TEXTURE(Texture2D, Texture_V)
SHADER_PARAMETER_TEXTURE(Texture2D, Texture_A)
SHADER_PARAMETER_SAMPLER(SamplerState, LumaSampler)
SHADER_PARAMETER_SAMPLER(SamplerState, CbSampler)
SHADER_PARAMETER_SAMPLER(SamplerState, CrSampler)
SHADER_PARAMETER_SAMPLER(SamplerState, AlphaSampler)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FManaUniformBufferYUVA, "ManaUniformBufferYUVA");

/* YUV平面をシェーダーへ渡すための構造体を定義（アルファなし） */
BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FManaUniformBufferNV12, )
SHADER_PARAMETER(float, GammaCorrection)
SHADER_PARAMETER_TEXTURE(Texture2D, Texture_Y)
SHADER_PARAMETER_TEXTURE(Texture2D, Texture_UV)
SHADER_PARAMETER_SAMPLER(SamplerState, LumaSampler)
SHADER_PARAMETER_SAMPLER(SamplerState, CbCrSampler)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FManaUniformBufferNV12, "ManaUniformBufferNV12");

/* YUV平面をシェーダーへ渡すための構造体を定義（アルファあり） */
BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FManaUniformBufferNV12A, )
SHADER_PARAMETER(float, GammaCorrection)
SHADER_PARAMETER_TEXTURE(Texture2D, Texture_Y)
SHADER_PARAMETER_TEXTURE(Texture2D, Texture_UV)
SHADER_PARAMETER_TEXTURE(Texture2D, Texture_A)
SHADER_PARAMETER_SAMPLER(SamplerState, LumaSampler)
SHADER_PARAMETER_SAMPLER(SamplerState, CbCrSampler)
SHADER_PARAMETER_SAMPLER(SamplerState, AlphaSampler)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FManaUniformBufferNV12A, "ManaUniformBufferNV12A");

/* RGB平面をシェーダーへ渡すための構造体を定義（アルファなし） */
BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FManaUniformBufferRGB, )
SHADER_PARAMETER(float, GammaCorrection)
SHADER_PARAMETER_TEXTURE(Texture2D, Texture_RGB)
SHADER_PARAMETER_SAMPLER(SamplerState, ColorSampler)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FManaUniformBufferRGB, "ManaUniformBufferRGB");

/* RGB平面をシェーダーへ渡すための構造体を定義（アルファあり） */
BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FManaUniformBufferRGBA, )
SHADER_PARAMETER(float, GammaCorrection)
SHADER_PARAMETER_TEXTURE(Texture2D, Texture_RGB)
SHADER_PARAMETER_TEXTURE(Texture2D, Texture_A)
SHADER_PARAMETER_SAMPLER(SamplerState, ColorSampler)
SHADER_PARAMETER_SAMPLER(SamplerState, AlphaSampler)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FManaUniformBufferRGBA, "ManaUniformBufferRGBA");
#elif ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 20
/* YUV平面をシェーダーへ渡すための構造体を定義（アルファなし） */
BEGIN_UNIFORM_BUFFER_STRUCT(FManaUniformBufferYUV, )
UNIFORM_MEMBER(float, GammaCorrection)
UNIFORM_MEMBER_TEXTURE(Texture2D, Texture_Y)
UNIFORM_MEMBER_TEXTURE(Texture2D, Texture_U)
UNIFORM_MEMBER_TEXTURE(Texture2D, Texture_V)
UNIFORM_MEMBER_SAMPLER(SamplerState, LumaSampler)
UNIFORM_MEMBER_SAMPLER(SamplerState, CbSampler)
UNIFORM_MEMBER_SAMPLER(SamplerState, CrSampler)
END_UNIFORM_BUFFER_STRUCT(FManaUniformBufferYUV)

IMPLEMENT_UNIFORM_BUFFER_STRUCT(FManaUniformBufferYUV, TEXT("ManaUniformBufferYUV"));

/* YUV平面をシェーダーへ渡すための構造体を定義（アルファあり） */
BEGIN_UNIFORM_BUFFER_STRUCT(FManaUniformBufferYUVA, )
UNIFORM_MEMBER(float, GammaCorrection)
UNIFORM_MEMBER_TEXTURE(Texture2D, Texture_Y)
UNIFORM_MEMBER_TEXTURE(Texture2D, Texture_U)
UNIFORM_MEMBER_TEXTURE(Texture2D, Texture_V)
UNIFORM_MEMBER_TEXTURE(Texture2D, Texture_A)
UNIFORM_MEMBER_SAMPLER(SamplerState, LumaSampler)
UNIFORM_MEMBER_SAMPLER(SamplerState, CbSampler)
UNIFORM_MEMBER_SAMPLER(SamplerState, CrSampler)
UNIFORM_MEMBER_SAMPLER(SamplerState, AlphaSampler)
END_UNIFORM_BUFFER_STRUCT(FManaUniformBufferYUVA)

IMPLEMENT_UNIFORM_BUFFER_STRUCT(FManaUniformBufferYUVA, TEXT("ManaUniformBufferYUVA"));

/* YUV平面をシェーダーへ渡すための構造体を定義（アルファなし） */
BEGIN_UNIFORM_BUFFER_STRUCT(FManaUniformBufferNV12, )
UNIFORM_MEMBER(float, GammaCorrection)
UNIFORM_MEMBER_TEXTURE(Texture2D, Texture_Y)
UNIFORM_MEMBER_TEXTURE(Texture2D, Texture_UV)
UNIFORM_MEMBER_SAMPLER(SamplerState, LumaSampler)
UNIFORM_MEMBER_SAMPLER(SamplerState, CbCrSampler)
END_UNIFORM_BUFFER_STRUCT(FManaUniformBufferNV12)

IMPLEMENT_UNIFORM_BUFFER_STRUCT(FManaUniformBufferNV12, TEXT("ManaUniformBufferNV12"));

/* YUV平面をシェーダーへ渡すための構造体を定義（アルファあり） */
BEGIN_UNIFORM_BUFFER_STRUCT(FManaUniformBufferNV12A, )
UNIFORM_MEMBER(float, GammaCorrection)
UNIFORM_MEMBER_TEXTURE(Texture2D, Texture_Y)
UNIFORM_MEMBER_TEXTURE(Texture2D, Texture_UV)
UNIFORM_MEMBER_TEXTURE(Texture2D, Texture_A)
UNIFORM_MEMBER_SAMPLER(SamplerState, LumaSampler)
UNIFORM_MEMBER_SAMPLER(SamplerState, CbCrSampler)
UNIFORM_MEMBER_SAMPLER(SamplerState, AlphaSampler)
END_UNIFORM_BUFFER_STRUCT(FManaUniformBufferNV12A)

IMPLEMENT_UNIFORM_BUFFER_STRUCT(FManaUniformBufferNV12A, TEXT("ManaUniformBufferNV12A"));

/* RGB平面をシェーダーへ渡すための構造体を定義（アルファなし） */
BEGIN_UNIFORM_BUFFER_STRUCT(FManaUniformBufferRGB, )
UNIFORM_MEMBER(float, GammaCorrection)
UNIFORM_MEMBER_TEXTURE(Texture2D, Texture_RGB)
UNIFORM_MEMBER_SAMPLER(SamplerState, ColorSampler)
END_UNIFORM_BUFFER_STRUCT(FManaUniformBufferRGB)

IMPLEMENT_UNIFORM_BUFFER_STRUCT(FManaUniformBufferRGB, TEXT("ManaUniformBufferRGB"));

/* RGB平面をシェーダーへ渡すための構造体を定義（アルファあり） */
BEGIN_UNIFORM_BUFFER_STRUCT(FManaUniformBufferRGBA, )
UNIFORM_MEMBER(float, GammaCorrection)
UNIFORM_MEMBER_TEXTURE(Texture2D, Texture_RGB)
UNIFORM_MEMBER_TEXTURE(Texture2D, Texture_A)
UNIFORM_MEMBER_SAMPLER(SamplerState, ColorSampler)
UNIFORM_MEMBER_SAMPLER(SamplerState, AlphaSampler)
END_UNIFORM_BUFFER_STRUCT(FManaUniformBufferRGBA)

IMPLEMENT_UNIFORM_BUFFER_STRUCT(FManaUniformBufferRGBA, TEXT("ManaUniformBufferRGBA"));
#else 
 /* YUV平面をシェーダーへ渡すための構造体を定義（アルファなし） */
BEGIN_UNIFORM_BUFFER_STRUCT(FManaUniformBufferYUV, )
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER(float, GammaCorrection)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_TEXTURE(Texture2D, Texture_Y)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_TEXTURE(Texture2D, Texture_U)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_TEXTURE(Texture2D, Texture_V)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_SAMPLER(SamplerState, LumaSampler)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_SAMPLER(SamplerState, CbSampler)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_SAMPLER(SamplerState, CrSampler)
END_UNIFORM_BUFFER_STRUCT(FManaUniformBufferYUV)

IMPLEMENT_UNIFORM_BUFFER_STRUCT(FManaUniformBufferYUV, TEXT("ManaUniformBufferYUV"));

/* YUV平面をシェーダーへ渡すための構造体を定義（アルファあり） */
BEGIN_UNIFORM_BUFFER_STRUCT(FManaUniformBufferYUVA, )
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER(float, GammaCorrection)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_TEXTURE(Texture2D, Texture_Y)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_TEXTURE(Texture2D, Texture_U)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_TEXTURE(Texture2D, Texture_V)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_TEXTURE(Texture2D, Texture_A)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_SAMPLER(SamplerState, LumaSampler)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_SAMPLER(SamplerState, CbSampler)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_SAMPLER(SamplerState, CrSampler)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_SAMPLER(SamplerState, AlphaSampler)
END_UNIFORM_BUFFER_STRUCT(FManaUniformBufferYUVA)

IMPLEMENT_UNIFORM_BUFFER_STRUCT(FManaUniformBufferYUVA, TEXT("ManaUniformBufferYUVA"));

/* YUV平面をシェーダーへ渡すための構造体を定義（アルファなし） */
BEGIN_UNIFORM_BUFFER_STRUCT(FManaUniformBufferNV12, )
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER(float, GammaCorrection)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_TEXTURE(Texture2D, Texture_Y)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_TEXTURE(Texture2D, Texture_UV)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_SAMPLER(SamplerState, LumaSampler)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_SAMPLER(SamplerState, CbCrSampler)
END_UNIFORM_BUFFER_STRUCT(FManaUniformBufferNV12)

IMPLEMENT_UNIFORM_BUFFER_STRUCT(FManaUniformBufferNV12, TEXT("ManaUniformBufferNV12"));

/* YUV平面をシェーダーへ渡すための構造体を定義（アルファあり） */
BEGIN_UNIFORM_BUFFER_STRUCT(FManaUniformBufferNV12A, )
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER(float, GammaCorrection)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_TEXTURE(Texture2D, Texture_Y)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_TEXTURE(Texture2D, Texture_UV)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_TEXTURE(Texture2D, Texture_A)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_SAMPLER(SamplerState, LumaSampler)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_SAMPLER(SamplerState, CbCrSampler)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_SAMPLER(SamplerState, AlphaSampler)
END_UNIFORM_BUFFER_STRUCT(FManaUniformBufferNV12A)

IMPLEMENT_UNIFORM_BUFFER_STRUCT(FManaUniformBufferNV12A, TEXT("ManaUniformBufferNV12A"));

/* RGB平面をシェーダーへ渡すための構造体を定義（アルファなし） */
BEGIN_UNIFORM_BUFFER_STRUCT(FManaUniformBufferRGB, )
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER(float, GammaCorrection)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_TEXTURE(Texture2D, Texture_RGB)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_SAMPLER(SamplerState, ColorSampler)
END_UNIFORM_BUFFER_STRUCT(FManaUniformBufferRGB)

IMPLEMENT_UNIFORM_BUFFER_STRUCT(FManaUniformBufferRGB, TEXT("ManaUniformBufferRGB"));

/* RGB平面をシェーダーへ渡すための構造体を定義（アルファあり） */
BEGIN_UNIFORM_BUFFER_STRUCT(FManaUniformBufferRGBA, )
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER(float, GammaCorrection)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_TEXTURE(Texture2D, Texture_RGB)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_TEXTURE(Texture2D, Texture_A)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_SAMPLER(SamplerState, ColorSampler)
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_SAMPLER(SamplerState, AlphaSampler)
END_UNIFORM_BUFFER_STRUCT(FManaUniformBufferRGBA)

IMPLEMENT_UNIFORM_BUFFER_STRUCT(FManaUniformBufferRGBA, TEXT("ManaUniformBufferRGBA"));
#endif

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
void FManaColorTransformPixelShader_YUV_to_RGB::SetParameters(
	FRHICommandList& RHICmdList,
	const std::array<FTexture2DRHIRef, 4>& YUVTextures, 
	float GammaCorrection)
{
	FManaUniformBufferYUV UniformBuffer;
	UniformBuffer.Texture_Y = YUVTextures[PlaneIndex_Y];
	UniformBuffer.Texture_U = YUVTextures[PlaneIndex_U];
	UniformBuffer.Texture_V = YUVTextures[PlaneIndex_V];
	UniformBuffer.LumaSampler = TStaticSamplerState<SF_Bilinear>::GetRHI();
	UniformBuffer.CbSampler = TStaticSamplerState<SF_Bilinear>::GetRHI();
	UniformBuffer.CrSampler = TStaticSamplerState<SF_Bilinear>::GetRHI();
	UniformBuffer.GammaCorrection = GammaCorrection;

	auto Data = TUniformBufferRef<FManaUniformBufferYUV>::CreateUniformBufferImmediate(UniformBuffer, UniformBuffer_SingleFrame);
#if	ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 25
	TShaderMapRef<FManaColorTransformPixelShader_YUV_to_RGB> PixelShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	SetUniformBufferParameter(RHICmdList, PixelShader.GetPixelShader(), GetUniformBufferParameter<FManaUniformBufferYUV>(), Data);
#else
	SetUniformBufferParameter(RHICmdList, GetPixelShader(), GetUniformBufferParameter<FManaUniformBufferYUV>(), Data);
#endif
}

void FManaColorTransformPixelShader_YUVA_to_RGBA::SetParameters(
	FRHICommandList& RHICmdList,
	const std::array<FTexture2DRHIRef, 4>& YUVTextures
	, float GammaCorrection)
{
	FManaUniformBufferYUVA UniformBuffer;
	UniformBuffer.Texture_Y = YUVTextures[PlaneIndex_Y];
	UniformBuffer.Texture_U = YUVTextures[PlaneIndex_U];
	UniformBuffer.Texture_V = YUVTextures[PlaneIndex_V];
	UniformBuffer.Texture_A = YUVTextures[PlaneIndex_A];
	UniformBuffer.LumaSampler = TStaticSamplerState<SF_Bilinear>::GetRHI();
	UniformBuffer.CbSampler = TStaticSamplerState<SF_Bilinear>::GetRHI();
	UniformBuffer.CrSampler = TStaticSamplerState<SF_Bilinear>::GetRHI();
	UniformBuffer.AlphaSampler = TStaticSamplerState<SF_Bilinear>::GetRHI();
	UniformBuffer.GammaCorrection = GammaCorrection;

	auto Data = TUniformBufferRef<FManaUniformBufferYUVA>::CreateUniformBufferImmediate(UniformBuffer, UniformBuffer_SingleFrame);
#if	ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 25
	TShaderMapRef<FManaColorTransformPixelShader_YUVA_to_RGBA> PixelShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	SetUniformBufferParameter(RHICmdList, PixelShader.GetPixelShader(), GetUniformBufferParameter<FManaUniformBufferYUVA>(), Data);
#else
	SetUniformBufferParameter(RHICmdList, GetPixelShader(), GetUniformBufferParameter<FManaUniformBufferYUVA>(), Data);
#endif
}

void FManaColorTransformPixelShader_NV12_to_RGB::SetParameters(
	FRHICommandList& RHICmdList,
	const std::array<FTexture2DRHIRef, 4>& YUVTextures
	, float GammaCorrection)
{
	FManaUniformBufferNV12 UniformBuffer;
	UniformBuffer.Texture_Y = YUVTextures[PlaneIndex_Y];
	UniformBuffer.Texture_UV = YUVTextures[PlaneIndex_UV];
	UniformBuffer.LumaSampler = TStaticSamplerState<SF_Bilinear>::GetRHI();
	UniformBuffer.CbCrSampler = TStaticSamplerState<SF_Bilinear>::GetRHI();
	UniformBuffer.GammaCorrection = GammaCorrection;

	auto Data = TUniformBufferRef<FManaUniformBufferNV12>::CreateUniformBufferImmediate(UniformBuffer, UniformBuffer_SingleFrame);
#if	ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 25
	TShaderMapRef<FManaColorTransformPixelShader_NV12_to_RGB> PixelShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	SetUniformBufferParameter(RHICmdList, PixelShader.GetPixelShader(), GetUniformBufferParameter<FManaUniformBufferNV12>(), Data);
#else
	SetUniformBufferParameter(RHICmdList, GetPixelShader(), GetUniformBufferParameter<FManaUniformBufferNV12>(), Data);
#endif
}

void FManaColorTransformPixelShader_NV12A_to_RGBA::SetParameters(
	FRHICommandList& RHICmdList,
	const std::array<FTexture2DRHIRef, 4>& YUVTextures
	, float GammaCorrection)
{
	FManaUniformBufferNV12A UniformBuffer;
	UniformBuffer.Texture_Y = YUVTextures[PlaneIndex_Y];
	UniformBuffer.Texture_UV = YUVTextures[PlaneIndex_UV];
	UniformBuffer.Texture_A = YUVTextures[PlaneIndex_A];
	UniformBuffer.LumaSampler = TStaticSamplerState<SF_Bilinear>::GetRHI();
	UniformBuffer.CbCrSampler = TStaticSamplerState<SF_Bilinear>::GetRHI();
	UniformBuffer.AlphaSampler = TStaticSamplerState<SF_Bilinear>::GetRHI();
	UniformBuffer.GammaCorrection = GammaCorrection;

	auto Data = TUniformBufferRef<FManaUniformBufferNV12A>::CreateUniformBufferImmediate(UniformBuffer, UniformBuffer_SingleFrame);
#if	ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 25
	TShaderMapRef<FManaColorTransformPixelShader_NV12A_to_RGBA> PixelShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	SetUniformBufferParameter(RHICmdList, PixelShader.GetPixelShader(), GetUniformBufferParameter<FManaUniformBufferNV12A>(), Data);
#else
	SetUniformBufferParameter(RHICmdList, GetPixelShader(), GetUniformBufferParameter<FManaUniformBufferNV12A>(), Data);
#endif
}

void FManaColorTransformPixelShader_RGB_to_RGB::SetParameters(
	FRHICommandList& RHICmdList,
	const std::array<FTexture2DRHIRef, 4>& RGBTextures
	, float GammaCorrection)
{
	FManaUniformBufferRGB UniformBuffer;
	UniformBuffer.Texture_RGB = RGBTextures[PlaneIndex_RGB];
	UniformBuffer.ColorSampler = TStaticSamplerState<SF_Bilinear>::GetRHI();
	UniformBuffer.GammaCorrection = GammaCorrection;

	auto Data = TUniformBufferRef<FManaUniformBufferRGB>::CreateUniformBufferImmediate(UniformBuffer, UniformBuffer_SingleFrame);
#if	ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 25
	TShaderMapRef<FManaColorTransformPixelShader_RGB_to_RGB> PixelShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	SetUniformBufferParameter(RHICmdList, PixelShader.GetPixelShader(), GetUniformBufferParameter<FManaUniformBufferRGB>(), Data);
#else
	SetUniformBufferParameter(RHICmdList, GetPixelShader(), GetUniformBufferParameter<FManaUniformBufferRGB>(), Data);
#endif
}

void FManaColorTransformPixelShader_RGBA_to_RGBA::SetParameters(
	FRHICommandList& RHICmdList,
	const std::array<FTexture2DRHIRef, 4>& RGBTextures
	, float GammaCorrection)
{
	FManaUniformBufferRGBA UniformBuffer;
	UniformBuffer.Texture_RGB = RGBTextures[PlaneIndex_RGB];
	UniformBuffer.Texture_A = RGBTextures[PlaneIndex_A];
	UniformBuffer.ColorSampler = TStaticSamplerState<SF_Bilinear>::GetRHI();
	UniformBuffer.AlphaSampler = TStaticSamplerState<SF_Bilinear>::GetRHI();
	UniformBuffer.GammaCorrection = GammaCorrection;

	auto Data = TUniformBufferRef<FManaUniformBufferRGBA>::CreateUniformBufferImmediate(UniformBuffer, UniformBuffer_SingleFrame);
#if	ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 25
	TShaderMapRef<FManaColorTransformPixelShader_RGBA_to_RGBA> PixelShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	SetUniformBufferParameter(RHICmdList, PixelShader.GetPixelShader(), GetUniformBufferParameter<FManaUniformBufferRGBA>(), Data);
#else
	SetUniformBufferParameter(RHICmdList, GetPixelShader(), GetUniformBufferParameter<FManaUniformBufferRGBA>(), Data);
#endif
}


/* usfファイル名とシェーダー関数名を紐付け */
IMPLEMENT_SHADER_TYPE(,
					  FManaColorTransformPixelShader_YUV_to_RGB,
					  TEXT("/Plugin/CriWare/Private/ManaColorTransform.usf"),
					  TEXT("PixelShader_YUV_to_RGB"),
					  SF_Pixel);

IMPLEMENT_SHADER_TYPE(,
					  FManaColorTransformPixelShader_YUVA_to_RGBA,
					  TEXT("/Plugin/CriWare/Private/ManaColorTransform.usf"),
					  TEXT("PixelShader_YUVA_to_RGBA"),
					  SF_Pixel);

IMPLEMENT_SHADER_TYPE(,
					  FManaColorTransformPixelShader_NV12_to_RGB,
					  TEXT("/Plugin/CriWare/Private/ManaColorTransform.usf"),
					  TEXT("PixelShader_NV12_to_RGB"),
					  SF_Pixel);

IMPLEMENT_SHADER_TYPE(,
					  FManaColorTransformPixelShader_NV12A_to_RGBA,
					  TEXT("/Plugin/CriWare/Private/ManaColorTransform.usf"),
					  TEXT("PixelShader_NV12A_to_RGBA"),
					  SF_Pixel);

IMPLEMENT_SHADER_TYPE(,
					  FManaColorTransformPixelShader_RGB_to_RGB,
					  TEXT("/Plugin/CriWare/Private/ManaColorTransform.usf"),
					  TEXT("PixelShader_RGB_to_RGB"),
					  SF_Pixel);

IMPLEMENT_SHADER_TYPE(,
					  FManaColorTransformPixelShader_RGBA_to_RGBA,
					  TEXT("/Plugin/CriWare/Private/ManaColorTransform.usf"),
					  TEXT("PixelShader_RGBA_to_RGBA"),
					  SF_Pixel);

IMPLEMENT_SHADER_TYPE(,
					  FManaPaththroughVertexShader,
					  TEXT("/Plugin/CriWare/Private/ManaColorTransform.usf"),
					  TEXT("MainVertexShader"),
					  SF_Vertex);

/***************************************************************************
 *      関数定義
 *      Function Definition
 ***************************************************************************/

/* --- end of file --- */
