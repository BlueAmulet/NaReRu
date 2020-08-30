/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2015-2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Mana Movie Texture Render Resource Manager
 * File     : ManaTextureResource.h
 *
 ****************************************************************************/

/* 多重定義防止 */
#pragma once

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
#include "RenderResource.h"
#include "RHIDefinitions.h"
#include "TextureResource.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "ManaComponentTexture.h"
#include "Containers/CircularQueue.h"
#include "Templates/Function.h"
#include "cri_mana.h"
#include <array>
#ifdef CRIWARE_ENABLE_MANA_PLAYBACK_START_LATENCY_LOG
#include <atomic>
#endif

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

/***************************************************************************
 *      変数宣言
 *      Prototype Variables
 ***************************************************************************/

/***************************************************************************
 *      クラス宣言
 *      Prototype Classes
 ***************************************************************************/
class UManaTexture;
class UManaComponentTexture;
struct FManaFrame;
enum class EManaMovieFormat : uint8;
enum class EManaMovieType : uint8;

class FManaTextureResource : public FTextureResource, public FRenderTarget
{
public:
	FManaTextureResource(const UManaTexture* InOwner);

	FORCEINLINE FLinearColor GetClearColor()
	{
		return ClearColor;
	}

	/* テクスチャの書き換え処理 */
	void UpdateTexture(FRHICommandList& RHICmdList);

	/* メインスレッドで動画フレームを書き込むためのバッファを返す
	 * @return 書き込み可能なバッファがない場合、CriManaTextureBuffer.imagebufに nullptr を入れて返す */
	FManaFrame GetWritableFrame();

	/* メインスレッドが書き込んだバッファを受け取る */
	void SubmitFrame(const FManaFrame& InManaTextureBuffer);

	/* Iterate over free frame queue. */
	uint32 IterateWritableFrames(TFunctionRef<void(FManaFrame& ManaFrame)> Iter);

	/* Reset frames queues. */
	void ResetFrameQueues();

	/* FRenderResource 仮想関数をオーバーライド */
	virtual void InitRHI() override;
	virtual void InitDynamicRHI() override;
	virtual void ReleaseDynamicRHI() override;

	virtual uint32 GetSizeX() const override
	{
		return Dimension.X;
	}

	virtual uint32 GetSizeY() const override
	{
		return Dimension.Y;
	}

	/* FRenderTargetインターフェースを実装 */
	virtual FIntPoint GetSizeXY() const override
	{
		return Dimension;
	}

	/**
	* Render target resource should be sampled in linear color space
	*
	* @return display gamma expected for rendering to this render target
	*/
	virtual float GetDisplayGamma() const override;

	/* リソースの準備が完了しているかを返す */
	bool IsPrepared() const { return bIsPrepared; }

#ifdef CRIWARE_ENABLE_MANA_PLAYBACK_START_LATENCY_LOG
	/* 性能測定のため、再生リクエストの時刻を覚えておく */
	void SetPlayRequestTime(uint32 InPlayRequestTime);
#endif

	ERHIFeatureLevel::Type GetFeatureLevel() const { return FeatureLevel; }

protected:
	void SetFeatureLevel(ERHIFeatureLevel::Type InFeatureLevel)
	{
		FeatureLevel = InFeatureLevel;
	}

private:
	const UManaTexture* Owner;

	/* YUVA各平面のデコード結果を書き込むテクス
	
	チャ */
	std::array<FTexture2DRHIRef, 4> ManaTextureRHI;

	/** Feature level that this script is representing. */
	ERHIFeatureLevel::Type FeatureLevel;

	/* バッファの個数 */
	uint32 NumBuffers;

	/* メインスレッドから動画フレームを書き込むためのバッファ領域 */
	TArray<uint8> ManaTextureData;

	/* 描画済みのバッファを格納するFIFOキュー。
	 * メインスレッド	: Dequeueのみ
	 * レンダースレッド	: Enqueueのみ */
	TCircularQueue<FManaFrame> FreeQueue;

	// in display frames
	TCircularQueue<FManaFrame> HoldQueue;

	/* 動画フレーム供給済みのバッファを格納するFIFOキュー。
	 * メインスレッド	: Enqueueのみ
	 * レンダースレッド	: Dequeueのみ */
	TCircularQueue<FManaFrame> DataQueue;

	/** テクスチャの縦横ピクセルサイズ */
	FIntPoint Dimension;

	/** the color the texture is cleared to */
	FLinearColor ClearColor;

	/** the pixel format the render target uses  */
	EPixelFormat Format;

	/** the vertex buffer holding the quad vertices when rendering to render target */
	FVertexBufferRHIRef RenderTargetVertexBufferRHI;

	/** 描画リソースの準備が完了しているかどうか */
	bool bIsPrepared;

	/** If texture update comes from texture streaming. */
	bool bIsStreamed;

	/** Allocate and initilaize render target texture if requested. */
	void InitRenderTargetTexture(uint32 SizeX, uint32 SizeY);

	/** Render current component textures to render target texture if requested. */
	void RenderToTarget(FRHICommandList& RHICmdList, EManaMovieFormat MovieFormat, bool IsAlpha);

	/** Create a vextex buffer containing a normalized quad. */
	FVertexBufferRHIRef CreateQuadVertices();

	/* ======= 性能測定用 ======= */
#ifdef CRIWARE_ENABLE_MANA_PLAYBACK_START_LATENCY_LOG
	/* 再生リクエスト時刻（再生開始レイテンシ計測のため） */
	std::atomic<uint32> PlayRequestTime;
#endif

public:
	typedef TFunction<FRHITexture2D*(	const UManaTexture& InOwner,
										const CriManaTextureBuffer& InManaTextureBuffer, 
										EManaComponentTextureType InComponentType, 
										const FIntPoint& InDimension, 
										FRHITexture2D* InTexture
									)> FUpdateTextureFunction;

	static inline void RegisterUpdateTextureFunction(FUpdateTextureFunction Function)
	{
		UpdateTextureFunction = Function;
	}

private:
	/**/
	static FUpdateTextureFunction UpdateTextureFunction;
};

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/

/* --- end of file --- */
