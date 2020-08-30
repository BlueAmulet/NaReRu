/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2015 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Mana Movie Texture Render Resource Manager
 * File     : ManaTextureResource.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* モジュールヘッダ */
#include "ManaTextureResource.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareRuntimePrivatePCH.h"
#include "ManaComponentTexture.h"
#include "ManaTexture.h"
#include "ManaColorTransformShader.h"

/* Unreal Engine 4関連ヘッダ */
#include "DeviceProfiles/DeviceProfile.h"
#include "DeviceProfiles/DeviceProfileManager.h"
#include "RHIStaticStates.h"
#include "RHIDefinitions.h"
#include "RHICommandList.h"
#include "RenderResource.h"
#include "StaticBoundShaderState.h"
#include "SceneUtils.h"
#include "PipelineStateCache.h"
#include "../Launch/Resources/Version.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/
#ifndef GETSAFERHISHADER_PIXEL
#define GETPIXELSHADERRHI(PixelShader)		((PixelShader).GetPixelShader())
#else
#define GETPIXELSHADERRHI(PixelShader)		(GETSAFERHISHADER_PIXEL(*PixelShader))
#endif
#ifndef GETSAFERHISHADER_VERTEX
#define GETVERTEXSHADERRHI(VertexShader)	((VertexShader).GetVertexShader())
#else
#define GETVERTEXSHADERRHI(PixelShader)		(GETSAFERHISHADER_VERTEX(*PixelShader))
#endif

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
namespace FManaTextureResource_Utility {
	/* デコードした動画バッファをGPUへ転送 */
	FRHITexture2D* UpdateManaTexture(const UManaTexture& Owner, const CriManaTextureBuffer& ManaTextureBuffer,
		EManaComponentTextureType ComponentType, const FIntPoint& Dimension, FRHITexture2D* Texture)
	{
		check(ManaTextureBuffer.imagebuf != nullptr);

		if (ManaTextureBuffer.imagebuf == nullptr || Texture == nullptr) {
			return Texture;
		}

#if (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 18) || \
    defined(XPT_TGT_PC) || \
	(defined(XPT_TGT_XBOXONE) && \
		(ENGINE_MAJOR_VERSION == 4) && ((ENGINE_MINOR_VERSION <= 12) || (ENGINE_MINOR_VERSION >= 15)) \
	) // a bug in 4.13 & 4.14 XboxOneRHI cause XB1 to reboot
		/* DirectX系ではID3D11DeviceContext::UpdateSubresourceで転送する */
		RHIUpdateTexture2D(
			Texture,
			0,
			FUpdateTextureRegion2D(
				0, 0, 0, 0,
				Dimension.X,
				Dimension.Y),
			ManaTextureBuffer.pitch,
			ManaTextureBuffer.imagebuf);
#else
		/* OpenGL系ではRHIUpdateTexture2Dがサポートされていないため、代わりにLock Unlockを使う */
		uint32 Stride;
		uint8* TextureBuffer = reinterpret_cast<uint8*>(RHILockTexture2D(Texture, 0, RLM_WriteOnly, Stride, false));
		auto ImageBuffer = ManaTextureBuffer.imagebuf;
		for (int32 Line = 0; Line < Dimension.Y; Line++) {
			FMemory::Memcpy(TextureBuffer, ImageBuffer, Dimension.X);
			TextureBuffer += Stride;
			ImageBuffer += ManaTextureBuffer.pitch;
		}
		RHIUnlockTexture2D(Texture, 0, false);
#endif

		return Texture;
	}
}

FManaTextureResource::FUpdateTextureFunction FManaTextureResource::UpdateTextureFunction = FManaTextureResource_Utility::UpdateManaTexture;

FManaTextureResource::FManaTextureResource(const UManaTexture* InOwner)
	:	Owner(InOwner)
	,	FeatureLevel(GMaxRHIFeatureLevel)
	,	NumBuffers(3)
	,	FreeQueue(NumBuffers + 2)
	,	HoldQueue(2 + 2)
	,	DataQueue(NumBuffers + 2)
	,	ClearColor(InOwner->ClearColor)
	,	Format(InOwner->GetFormat())
	,	RenderTargetVertexBufferRHI(nullptr)
	,	bIsPrepared(false)
	,	bIsStreamed(false)
{
	Dimension = Owner->GetDimensions();
	ManaTextureRHI[0] = nullptr;

	auto PlatformMana = UCriWareInitializer::Platform.Mana();
	if (PlatformMana != nullptr) {
		UpdateTextureFunction = [PlatformMana](
			const UManaTexture& InManaTexture,
			const CriManaTextureBuffer& InManaTextureBuffer,
			EManaComponentTextureType InComponentType,
			const FIntPoint& InDimension,
			FRHITexture2D* InTexture) {
			return PlatformMana->UpdateManaTexture(
				InManaTexture,
				InManaTextureBuffer,
				InComponentType,
				InDimension,
				InTexture);
		};
	}
}

void FManaTextureResource::InitRHI()
{

}

void FManaTextureResource::InitDynamicRHI()
{
	SetFeatureLevel(GMaxRHIFeatureLevel);

	Dimension = Owner->GetDimensions();
	for (int i = 0; i < 4; i++) {
		ManaTextureRHI[i] = nullptr;
	}

	if (Owner->bRenderToTexture) {
		InitRenderTargetTexture(Dimension.X, Dimension.Y);
	} else {
		InitRenderTargetTexture(1, 1);
	}

	ResetFrameQueues();

	bIsPrepared = true;
}

void FManaTextureResource::ResetFrameQueues()
{
	DataQueue.Empty();
	HoldQueue.Empty();
	FreeQueue.Empty();
	for (uint32 BufferIndex = 0; BufferIndex < (NumBuffers + (Owner->IsStreamed() ? 1 : 0)); ++BufferIndex) {
		// 描画済みキューに入れておく
		FreeQueue.Enqueue(FManaFrame());
	}
}

void FManaTextureResource::ReleaseDynamicRHI()
{
	ReleaseRHI();

	RHIUpdateTextureReference(Owner->TextureReference.TextureReferenceRHI, nullptr);
	RenderTargetTextureRHI.SafeRelease();

	for (auto& Texture : ManaTextureRHI) {
		Texture.SafeRelease();
	}

	if (RenderTargetVertexBufferRHI) {
		RenderTargetVertexBufferRHI.SafeRelease();
	}

	bIsPrepared = false;
}

FManaFrame FManaTextureResource::GetWritableFrame()
{
	FManaFrame ManaFrame;
	bool IsDequeueSucceed = FreeQueue.Dequeue(ManaFrame);

	//UE_LOG(LogTemp, Log, TEXT("num Free frame %d (%d)\n"), FreeQueue.Count(), IsDequeueSucceed);

	if (!IsDequeueSucceed) {
		ManaFrame.Invalidate(); 
	}

	//UE_LOG(LogTemp, Log, TEXT("GET   in%d  out%d->\n"), DataQueue.Count(), FreeQueue.Count());

	return ManaFrame;
}

uint32 FManaTextureResource::IterateWritableFrames(TFunctionRef<void(FManaFrame& ManaFrame)> Iter)
{
	FManaFrame ManaFrame;
	uint32 Count = 0;
	while (Count < FreeQueue.Count()) {
		if (FreeQueue.Dequeue(ManaFrame)) {
			Iter(ManaFrame);
			FreeQueue.Enqueue(ManaFrame);
			Count++;
		} else {
			break;
		}
	}
	return Count;
}

void FManaTextureResource::SubmitFrame(const FManaFrame& InManaFrame)
{
	// Initialize table to component RHI textures
	if (ManaTextureRHI[0] == nullptr) {
		/* ムービのデコード結果を書き込むテクスチャを作成 */
		/* アルファムービでなければ、A平面の初期化処理をスキップする */
		for (int32 TextureIndex = 0, TextureRHIIndex = 0; TextureIndex < Owner->GetNumComponentTextures(); TextureIndex++) {
			auto Resource = Owner->GetComponentTexture(TextureIndex)->Resource;
			if (Resource != nullptr) {
				ManaTextureRHI[TextureRHIIndex++] = Resource->TextureRHI->GetTexture2D();
			}
		}
	}

	DataQueue.Enqueue(InManaFrame);
	//UE_LOG(LogTemp, Log, TEXT("SET ->in%d  out%d\n"), DataQueue.Count(), FreeQueue.Count());

	/* レンダースレッドに、テクスチャを更新する指示をキューイングしておく */
	ENQUEUE_RENDER_COMMAND(UpdateTexture)([this](FRHICommandList& RHICmdList) {
		UpdateTexture(RHICmdList);
	});
}

void FManaTextureResource::UpdateTexture(FRHICommandList& RHICmdList)
{
	FManaFrame ManaFrame;
	bool IsDequeueSucceed = DataQueue.Dequeue(ManaFrame);
	if (!IsDequeueSucceed) {
		return;
	}

	bool IsStreamed = Owner->IsStreamed();

	// set oldest previous hold to free
	FManaFrame PrevManaFrame;
	if ((!IsStreamed || HoldQueue.Count() >= 3) && HoldQueue.Dequeue(PrevManaFrame)) {
		FreeQueue.Enqueue(PrevManaFrame);
	}

	bool IsAlpha = Owner->IsAlphaMovie();

	// デコードした動画バッファをGPUへ転送
	for (int32 TextureIndex = 0, TextureRHIIndex = 0; TextureIndex < Owner->GetNumComponentTextures(); TextureIndex++) {
		auto ComponentTexture = Owner->GetComponentTexture(TextureIndex);
		if (ComponentTexture->Resource == nullptr) {
			continue;
		}
		if (ManaTextureRHI[TextureRHIIndex] == nullptr) {
			break;
		}
		auto NewTextureRHI = UpdateTextureFunction(
			*Owner,
			ManaFrame.Buffers[TextureRHIIndex],
			ComponentTexture->ComponentType,
			FIntPoint(ComponentTexture->SizeX, ComponentTexture->SizeY),
			ManaTextureRHI[TextureRHIIndex]
		);
		if (NewTextureRHI != ManaTextureRHI[TextureRHIIndex]) {
			// overwrite texture to use
			ManaTextureRHI[TextureRHIIndex] = NewTextureRHI;
			// update ComponentTexture owner
			static_cast<FManaComponentTextureResource*>(ComponentTexture->Resource)->UpdateTextureReference(NewTextureRHI);
		}
		TextureRHIIndex++;
	}

	// 転送完了。描画済みキューへ積む
	HoldQueue.Enqueue(ManaFrame);

	//UE_LOG(LogTemp, Log, TEXT("DIS   in%d->out%d\n"), DataQueue.Count(), FreeQueue.Count());

	// Check if we use RT to texture
	if (Owner->bRenderToTexture) {
		RenderToTarget(RHICmdList, Owner->GetOutputMovieFormat(), IsAlpha);
	}

	/* 再生開始レイテンシの計測結果をコンソールへ出力 */
#ifdef CRIWARE_ENABLE_MANA_PLAYBACK_START_LATENCY_LOG
	uint32 PlayRequestTime_local = PlayRequestTime.load();
	if (PlayRequestTime_local > 0) {
		DECLARE_FLOAT_COUNTER_STAT(TEXT("Mana Playback Start Latency (ms)"), STAT_ManaPlaybackLatency, STATGROUP_CriWare);
		SET_FLOAT_STAT(STAT_ManaPlaybackLatency, FPlatformTime::ToMilliseconds(FPlatformTime::Cycles() - PlayRequestTime_local));
		PlayRequestTime.store(0);
	}
#endif
}

void FManaTextureResource::InitRenderTargetTexture(uint32 SizeX, uint32 SizeY)
{
	bool bCreateSRGB = true;
	// if render target gamma used was 1.0 then disable SRGB for the static texture
	if (FMath::Abs(GetDisplayGamma() - 1.0f) < KINDA_SMALL_NUMBER) {
		bCreateSRGB = false;
	}

	// Create the RHI texture. Only one mip is used and the texture is targetable for resolve.
	// YUV->RGBシェーダ色変換の結果を保持するテクスチャ（レンダーターゲット）を作成
	uint32 RenderTargetFlags = bCreateSRGB ? TexCreate_SRGB : 0;
	RenderTargetFlags |= (TexCreate_ShaderResource | TexCreate_RenderTargetable | TexCreate_Dynamic);

	ClearColor = Owner->ClearColor;
	FRHIResourceCreateInfo CreateInfo = FRHIResourceCreateInfo(FClearValueBinding(ClearColor));

	RenderTargetTextureRHI = RHICreateTexture2D(
		SizeX,
		SizeY,
		Format,
		1,
		1,
		RenderTargetFlags,
		CreateInfo);

	// Set resource texture to the render target texture.
	TextureRHI = RenderTargetTextureRHI;

	// Draw the clear color.
	ENQUEUE_RENDER_COMMAND(ClearRenderTarget)([this, SizeX, SizeY](FRHICommandList& RHICmdList) {
		FRHIRenderPassInfo RPInfo(TextureRHI, ERenderTargetActions::Clear_Store);
		RHICmdList.BeginRenderPass(RPInfo, TEXT("ClearManaTexture"));
		RHICmdList.EndRenderPass();
		RHICmdList.SetViewport(0, 0, 0.0f, SizeX, SizeY, 1.0f);
		//RHICmdList.CopyToResolveTarget(TextureRHI, TextureRHI, FResolveParams());
		RHICmdList.TransitionResource(EResourceTransitionAccess::EReadable, TextureRHI);
	});

	/* FTextureRenderTarget2DResource::InitDynamicRHIを参考 */
	// Create the sampler state RHI resource.
	FSamplerStateInitializerRHI SamplerStateInitializer
	(
		(ESamplerFilter)UDeviceProfileManager::Get().GetActiveProfile()->GetTextureLODSettings()->GetSamplerFilter(Owner),
		Owner->AddressX == TA_Wrap ? AM_Wrap : (Owner->AddressX == TA_Clamp ? AM_Clamp : AM_Mirror),
		Owner->AddressY == TA_Wrap ? AM_Wrap : (Owner->AddressY == TA_Clamp ? AM_Clamp : AM_Mirror),
		AM_Wrap
	);
	SamplerStateRHI = RHICreateSamplerState(SamplerStateInitializer);

	// レンダーターゲットをUManaTextureに紐付け
	RHIUpdateTextureReference(Owner->TextureReference.TextureReferenceRHI, (FTextureRHIRef&)RenderTargetTextureRHI);

	// Create quad
	RenderTargetVertexBufferRHI = CreateQuadVertices();
}

TGlobalResource<FManaVertexDeclaration> GManaVertexDeclaration;

void FManaTextureResource::RenderToTarget(FRHICommandList& RHICmdList, EManaMovieFormat MovieFormat, bool IsAlpha)
{
	// YUV->RGBシェーダ色変換
	SCOPED_DRAW_EVENT(RHICmdList, ManaTextureRenderToTargetResource);

	RHICmdList.TransitionResource(EResourceTransitionAccess::EWritable, RenderTargetTextureRHI);

	FRHIRenderPassInfo RPInfo(RenderTargetTextureRHI, ERenderTargetActions::Load_Store);
	RHICmdList.BeginRenderPass(RPInfo, TEXT("RenderManaTextureToTarget"));
	{
		FGraphicsPipelineStateInitializer GraphicsPSOInit;
		RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

		TShaderMapRef<FManaPaththroughVertexShader> VertexShader(GetGlobalShaderMap(GetFeatureLevel()));

		GraphicsPSOInit.BoundShaderState.VertexShaderRHI = GETVERTEXSHADERRHI(VertexShader);
		GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
		GraphicsPSOInit.RasterizerState = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();
		GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
		GraphicsPSOInit.PrimitiveType = PT_TriangleStrip;

		/* アルファチャンネルがあるかどうかでピクセルシェーダを切り替え */
		switch (MovieFormat) {
		case EManaMovieFormat::YV12:
			if (IsAlpha) {
				TShaderMapRef<FManaColorTransformPixelShader_YUVA_to_RGBA> PixelShader(GetGlobalShaderMap(GetFeatureLevel()));
				GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETPIXELSHADERRHI(PixelShader);
				GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GManaVertexDeclaration.VertexDeclarationRHI;
				SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);
				PixelShader->SetParameters(RHICmdList, ManaTextureRHI, GetDisplayGamma());
			} else {
				TShaderMapRef<FManaColorTransformPixelShader_YUV_to_RGB> PixelShader(GetGlobalShaderMap(GetFeatureLevel()));
				GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETPIXELSHADERRHI(PixelShader);
				GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GManaVertexDeclaration.VertexDeclarationRHI;
				SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);
				PixelShader->SetParameters(RHICmdList, ManaTextureRHI, GetDisplayGamma());
			}
			break;
		case EManaMovieFormat::NV12:
			if (IsAlpha) {
				TShaderMapRef<FManaColorTransformPixelShader_NV12A_to_RGBA> PixelShader(GetGlobalShaderMap(GetFeatureLevel()));
				GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETPIXELSHADERRHI(PixelShader);
				GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GManaVertexDeclaration.VertexDeclarationRHI;
				SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);
				PixelShader->SetParameters(RHICmdList, ManaTextureRHI, GetDisplayGamma());
			} else {
				TShaderMapRef<FManaColorTransformPixelShader_NV12_to_RGB> PixelShader(GetGlobalShaderMap(GetFeatureLevel()));
				GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETPIXELSHADERRHI(PixelShader);
				GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GManaVertexDeclaration.VertexDeclarationRHI;
				SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);
				PixelShader->SetParameters(RHICmdList, ManaTextureRHI, GetDisplayGamma());
			}
			break;
		case EManaMovieFormat::RGB:
			if (IsAlpha) {
				TShaderMapRef<FManaColorTransformPixelShader_RGBA_to_RGBA> PixelShader(GetGlobalShaderMap(GetFeatureLevel()));
				GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETPIXELSHADERRHI(PixelShader);
				GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GManaVertexDeclaration.VertexDeclarationRHI;
				SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);
				PixelShader->SetParameters(RHICmdList, ManaTextureRHI, GetDisplayGamma());
			} else {
				TShaderMapRef<FManaColorTransformPixelShader_RGB_to_RGB> PixelShader(GetGlobalShaderMap(GetFeatureLevel()));
				GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETPIXELSHADERRHI(PixelShader);
				GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GManaVertexDeclaration.VertexDeclarationRHI;
				SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);
				PixelShader->SetParameters(RHICmdList, ManaTextureRHI, GetDisplayGamma());
			}
			break;
		}

		//RHICmdList.SetDepthStencilState(TStaticDepthStencilState<false, CF_Always>::GetRHI());
		//RHICmdList.SetRasterizerState(TStaticRasterizerState<>::GetRHI());
		//RHICmdList.SetBlendState(TStaticBlendStateWriteMask<CW_RGBA, CW_NONE, CW_NONE, CW_NONE, CW_NONE, CW_NONE, CW_NONE, CW_NONE>::GetRHI());
		RHICmdList.SetViewport(0, 0, 0.0f, Dimension.X, Dimension.Y, 1.0f);

		// draw full size quad into render target
		RHICmdList.SetStreamSource(0, RenderTargetVertexBufferRHI, 0);
#if	ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 22
		RHICmdList.DrawPrimitive(0, 2, 1);
#else
		RHICmdList.DrawPrimitive(PT_TriangleStrip, 0, 2, 1);
#endif
	}
	RHICmdList.EndRenderPass();
	//RHICmdList.CopyToResolveTarget(RenderTargetTextureRHI, RenderTargetTextureRHI, FResolveParams());
	RHICmdList.TransitionResource(EResourceTransitionAccess::EReadable, RenderTargetTextureRHI);
}

FVertexBufferRHIRef FManaTextureResource::CreateQuadVertices()
{
	static const FManaElementVertex QuadVertices[4] = {
		{ FVector4(-1.0f, 1.0f, 0.0f, 1.0f), FVector2D(0.0f, 0.0f) },
		{ FVector4(1.0f, 1.0f, 0.0f, 1.0f), FVector2D(1.0f, 0.0f) },
		{ FVector4(-1.0f, -1.0f, 0.0f, 1.0f), FVector2D(0.0f, 1.0f) },
		{ FVector4(1.0f, -1.0f, 0.0f, 1.0f), FVector2D(1.0f, 1.0f) }
	};
	static const uint32 Size = GetVertexCountForPrimitiveCount(2, PT_TriangleStrip) * sizeof(QuadVertices[0]);
	
	FRHIResourceCreateInfo CreateInfo;
	FVertexBufferRHIRef VertexBufferRHI = RHICreateVertexBuffer(Size, BUF_Static, CreateInfo);
	void* VoidPtr = RHILockVertexBuffer(VertexBufferRHI, 0, Size, RLM_WriteOnly);
	FPlatformMemory::Memcpy(VoidPtr, QuadVertices, Size);
	RHIUnlockVertexBuffer(VertexBufferRHI);

	return VertexBufferRHI;
}

/**
 * Render target resource should be sampled in linear color space
 *
 * @return display gamma expected for rendering to this render target
 */
float FManaTextureResource::GetDisplayGamma() const
{
	if (Owner->TargetGamma > KINDA_SMALL_NUMBER * 10.0f)
	{
		return Owner->TargetGamma;
	}
	if (Format == PF_FloatRGB || Format == PF_FloatRGBA || bSRGB == false) // linear
	{
		return 1.0;
	}
	
	return 2.2; // like in FTextureRenderTargetResource

	//return FRenderTarget::GetDisplayGamma();
}

#ifdef CRIWARE_ENABLE_MANA_PLAYBACK_START_LATENCY_LOG
void FManaTextureResource::SetPlayRequestTime(uint32 InPlayRequestTime)
{
	PlayRequestTime.store(InPlayRequestTime);
}
#endif

/***************************************************************************
 *      関数定義
 *      Function Definition
 ***************************************************************************/

#undef GETPIXELSHADERRHI
#undef GETVERTEXSHADERRHI

/* --- end of file --- */
