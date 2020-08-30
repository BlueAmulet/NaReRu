/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2010 CRI Middleware Co., Ltd.
 *
 * Library  : CRI Mana (Sofdec2)
 * Module   : Library User's Header
 * File     : cri_mana.h
 * Date     : 2020-03-30
 * Version  : (see CRIMANA_VERSION)
 *
 ****************************************************************************/
/*!
 *	\file		cri_mana.h
 */
 
/* 多重定義防止 */
/* Prevention of redefinition */
#ifndef CRI_INCL_CRI_MANA_H
#define CRI_INCL_CRI_MANA_H

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
#include <cri_xpt.h>
#include <cri_file_system.h>
#include <cri_atom_ex.h>

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/
/* バージョン情報 */
/* Version Number */
#define CRIMANA_VERSION				(0x02191500)
#define CRIMANA_VER_NUM				"2.19.15"
#define CRIMANA_VER_NAME			"CRI Mana"
#define CRIMANA_VER_OPTION

/* CRIMANA_MAX_VIDEO_NUM */
/*JP
 * \brief ムービーファイル内の最大ビデオストリーム数
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * 
 * \sa CriManaMovieInfo, criManaPlayer_GetMovieInfo
 */
#define CRIMANA_MAX_VIDEO_NUM			(1)

/* CRIMANA_MAX_AUDIO_NUM */
/*JP
 * \brief	ムービーファイル内の最大オーディオストリーム数
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * 
 * \sa CriManaMovieInfo, criManaPlayer_GetMovieInfo
 */
#define CRIMANA_MAX_AUDIO_NUM			(32)

/* CRIMANA_MAX_ALPHA_NUM */
/*JP
 * \brief	ムービーファイル内の最大アルファストリーム数
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * 
 * \sa CriManaMovieInfo, criManaPlayer_GetMovieInfo
 */
#define CRIMANA_MAX_ALPHA_NUM			(1)

/* CRIMANA_AUDIO_TRACK_OFF */
/*JP
 * \brief	オーディオ再生OFFの指定値
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * 
 * \sa criManaPlayer_SetAudioTrack
 */
#define CRIMANA_AUDIO_TRACK_OFF			(-1)

/* CRIMANA_AUDIO_TRACK_AUTO */
/*JP
 * \brief	オーディオトラックのデフォルト値
 * \ingroup MDL_MANALIB_PLAYER
 * \sa criManaPlayer_SetAudioTrack
 */
#define CRIMANA_AUDIO_TRACK_AUTO	(100)

/* CRIMANA_MAX_SUBTITLE_NUM */
/*JP
 * \brief   字幕の最大チャンネル数
 * \ingroup MDL_MANALIB_PLAYER
 * \sa CRIMANA_MAX_VIDEO_NUM, CRIMANA_MAX_AUDIO_NUM
 */
#define	CRIMANA_MAX_SUBTITLE_NUM	(16)

/* CRIMANA_SUBTITLE_CHANNEL_OFF */
/*JP
 * \brief	字幕再生OFFの指定値
 * \ingroup MDL_MANALIB_PLAYER
 * \sa criManaPlayer_SetSubtitleChannel
 */
#define CRIMANA_SUBTITLE_CHANNEL_OFF	(-1)

/* CRIMANA_MAX_FILE_NAME */
/*JP
 * \brief	Manaプレーヤーに指定可能なファイル名の最大長さ
 * \ingroup MDL_MANALIB_PLAYER
 * \sa criManaPlayer_SetFile
 */
#define CRIMANA_MAX_FILE_NAME			(256)

/* 機種固有フレーム情報数 */
/* Number of platform-specific frame information items */
#define CRIMANA_FRAME_DETAILS_NUM		(2)

/***************************************************************************
 *      処理マクロ
 *      Macro Functions
 ***************************************************************************/
/*==========================================================================
 *      CRI Mana API
 *=========================================================================*/
/* criMana_SetDefaultLibConfig */
/*JP
 * \brief ライブラリ初期化用コンフィグ構造体にデフォルト値をセット
 * \ingroup MDL_MANALIB_GLOBAL
 * \param[out] p_config	初期化用コンフィグ構造体へのポインタ
 * \par 説明:
 * ::criMana_Initialize 関数に設定するコンフィグ構造体( ::CriManaLibConfig ) に
 * デフォルトの値をセットします。<br>
 * \par 備考:
 * 各機種固有の関数が存在する場合があります。詳細は機種別のリファレンスを参照してください。
 * \sa
 * CriManaLibConfig
 */
#define criMana_SetDefaultLibConfig(p_config)	\
{\
	(p_config)->max_decoder_handles	= 4;\
	(p_config)->thread_model		= CRIMANA_THREAD_MODEL_MULTI;\
	(p_config)->version				= CRIMANA_VERSION;\
}

/*==========================================================================
 *      CRI Mana Player API
 *=========================================================================*/
/* criManaPlayer_SetDefaultHandleConfig */
/*JP
 * \brief プレーヤー作成用コンフィグ構造体にデフォルト値をセット
 * \ingroup MDL_MANALIB_PLAYER
 * \param[out] p_config	プレーヤー作成用コンフィグ構造体へのポインタ
 * \par 説明:
 * ::criManaPlayer_CreateWithConfig 関数に設定するコンフィグ構造体( ::CriManaPlayerConfig ) に
 * デフォルトの値をセットします。<br>
 * \sa
 * criManaPlayer_CreateWithConfig
 */
#define criManaPlayer_SetDefaultHandleConfig(p_config)	\
{\
	(p_config)->readbuf_size_byte	= 0;\
	(p_config)->use_atomex_player	= CRI_FALSE;\
	(p_config)->max_path	        = CRIMANA_MAX_FILE_NAME;\
}

/***************************************************************************
 *      列挙子宣言
 *      Enum Declarations
 ***************************************************************************/
/* CriManaPlayerStatus */
/*JP
 * \brief プレーヤステータス
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * Manaプレーヤーの再生状態を示す値です。
 * ハンドル状態は ::criManaPlayer_GetStatus 関数でいつでも取得することが出来ます。<br>
 * ハンドル作成直後は CRIMANAPLAYER_STOP 状態です。<br>
 * <br>
 * ハンドル状態は CRIMANAPLAYER_STATUS_STOP から CRIMANAPLAYER_STATUS_PLAYEND まで順に遷移していきます。<br>
 * アプリケーションがムービーを再生するにあたって、必ずしもこの全ての状態をチェックする必要はありません。<br>
 * 最低限、CRIMANAPLAYER_STATUS_STOP, CRIMANAPLAYER_STATUS_PLAYING, CRIMANAPLAYER_STATUS_PLAYEND, CRIMANAPLAYER_STATUS_ERROR さえ
 * チェックすれば、ムービーの再生を行うことができます。<br>
 * <br>
 * Manaプレーヤーを作成後、ムービーの解像度などが既に確定している場合は、アプリケーションは
 * ::criManaPlayer_Start 関数を直接呼び出すことができます。この場合、ハンドル状態は自動的に
 * CRIMANAPLAYER_STATUS_PLAYEND まで遷移していきます。<br>
 * <br>
 * 最初に ::criManaPlayer_DecodeHeader 関数を呼び出した場合は、ヘッダ解析が終了するとハンドル状態は
 * CRIMANAPLAYER_STATUS_WAIT_PREP となり、アプリケーションから ::criManaPlayer_Prepare 関数または
 * ::criManaPlayer_Start 関数が呼ばれるまで待機します。<br>
 * <br>
 * CRIMANAPLAYER_STATUS_WAIT_PREP状態以降、 ::criManaPlayer_GetMovieInfo 関数でムービー情報を取得することができます。<br>
 * ::criManaPlayer_Prepare 関数 を呼び出した場合は、ヘッダ解析およびデータのバッファリングが終わると、
 * ハンドル状態は CRIMANAPLAYER_STATUS_READY となり、アプリケーションから ::criManaPlayer_Start 関数が
 * 呼ばれるまで待機します。これによって再生開始のタイミングを調整することができます。<br>
 * <br>
 * 再生が終了すると自動的に CRIMANAPLAYER_STATUS_PLAYEND になります。<br>
 * <br>
 * ::criManaPlayer_Stop 関数を呼び出した場合は、デコーダの停止処理が終わったあとに CRIMANAPLAYER_STATUS_STOP
 * 状態になります。 ::criManaPlayer_Stop 関数終了直後に停止状態になるとは限りません。<br>
 * <br>
 * メモリ不足やデータエラーなど何らかの問題が発生した場合は CRIMANAPLAYER_STATUS_ERROR 状態となります。<br>
 * CRIMANAPLAYER_STATUS_ERROR 状態になった場合は ::criManaPlayer_Stop 関数 を呼び出してハンドル状態が
 * CRIMANAPLAYER_STATUS_STOP 状態に遷移させてください。<br>
 * <br>
 * ::criManaPlayer_Destroy 関数は CRIMANAPLAYER_STATUS_STOP, CRIMANAPLAYER_STATUS_PLAYEND の
 * いずれかの状態の時のみ呼び出すことができます。
 * 
 * \sa criManaPlayer_GetStatus
 */
typedef enum {
	/* CriManaPlayerStatus.CRIMANAPLAYER_STATUS_STOP */
	/*JP< 停止中					*/
	CRIMANAPLAYER_STATUS_STOP		= 0,

	/* CriManaPlayerStatus.CRIMANAPLAYER_STATUS_DECHDR */
	/*JP< ヘッダ解析中				*/
	CRIMANAPLAYER_STATUS_DECHDR		= 1,

	/* CriManaPlayerStatus.CRIMANAPLAYER_STATUS_WAIT_PREP */
	/*JP< バッファリング開始停止中	*/
	CRIMANAPLAYER_STATUS_WAIT_PREP	= 2,

	/* CriManaPlayerStatus.CRIMANAPLAYER_STATUS_PREP */
	/*JP< 再生準備中				*/
	CRIMANAPLAYER_STATUS_PREP		= 3,

	/* CriManaPlayerStatus.CRIMANAPLAYER_STATUS_READY */
	/*JP< 再生待機中				*/
	CRIMANAPLAYER_STATUS_READY		= 4,

	/* CriManaPlayerStatus.CRIMANAPLAYER_STATUS_PLAYING */
	/*JP< 再生中					*/
	CRIMANAPLAYER_STATUS_PLAYING	= 5,

	/* CriManaPlayerStatus.CRIMANAPLAYER_STATUS_PLAYEND */
	/*JP< 再生終了					*/
	CRIMANAPLAYER_STATUS_PLAYEND	= 6,

	/* CriManaPlayerStatus.CRIMANAPLAYER_STATUS_ERROR */
	/*JP< エラー					*/
	CRIMANAPLAYER_STATUS_ERROR		= 7,

	CRIMANAPLAYER_STATUS_ENUM_SIZE_IS_4BYTES = 0x7FFFFFFF
} CriManaPlayerStatus;

/* CriManaTimerType */
/*JP
 * \brief タイマ種別
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * Manaプレーヤーが内部の時刻として同期する参照タイマ（マスタータイマー）の種別です。
 * 
 * \sa criManaPlayer_SetMasterTimerType, criManaPlayer_GetMasterTimerType
 */
typedef enum {
	/* CriManaTimerType.CRIMANAPLAYER_TIMER_NONE */
	/*JP
	 * ビデオフレームは時刻同期をしません。<br>
	 * デコードが終わったフレームはすぐに取得することができます。
	 */
	CRIMANAPLAYER_TIMER_NONE	=0,

	/* CriManaTimerType.CRIMANAPLAYER_TIMER_SYSTEM */
	/*JP
	 * ビデオフレームは Manaプレーヤーが内部で使用しているシステム時刻
	 * に同期します。
	 */
	CRIMANAPLAYER_TIMER_SYSTEM	=1,

	/* CriManaTimerType.CRIMANAPLAYER_TIMER_AUDIO */
	/*JP
	 * ビデオフレームはメインオーディオトラックで再生しているオーディオ時刻に同期します。<br>
	 * もしもムービーデータにオーディオが含まれていない場合は、ビデオはシステム時刻に同期します。
	 */
	CRIMANAPLAYER_TIMER_AUDIO	=2,

	/* CriManaTimerType.CRIMANAPLAYER_TIMER_USER */
	/*JP
	 * ビデオフレームはアプリケーションから指定されたユーザー独自のタイマに同期します。
	 */
	CRIMANAPLAYER_TIMER_USER	=3,

	/* CriManaTimerType.CRIMANAPLAYER_TIMER_MANUAL */
	/*JP
	 * マニュアルタイマー：アプリケーションから時刻が進む単位の設定や
	 * 時刻を進めるモードです。
	 */
	CRIMANAPLAYER_TIMER_MANUAL	=4,		

	CRIMANAPLAYER_TIMER_ENUM_SIZE_IS_4BYTES = 0x7FFFFFFF
} CriManaTimerType;

/* CriManaThreadModel */
/*JP
 * \brief スレッドモデル
 * \ingroup MDL_MANALIB_GLOBAL
 * \par 説明:
 * CRI Manaライブラリがどのようなスレッドモデルで動作するかを表します。
 * 
 * \sa CriManaLibConfig, criMana_CalculateLibWorkSize, criMana_Initialize
 */
typedef enum {
	/* CriManaThreadModel.CRIMANA_THREAD_MODEL_MULTI */
	/*JP
	 * マルチスレッド動作モデル。<br>
	 * ライブラリは内部でスレッドを作成し、マルチスレッドにて動作します。
	 */
	CRIMANA_THREAD_MODEL_MULTI = 0,

	/* CriManaThreadModel.CRIMANA_THREAD_MODEL_SINGLE */
	/*JP
	 * シングルスレッド動作モデル。<br>
	 *    ライブラリは内部でスレッドを作成しません。
	 */
	CRIMANA_THREAD_MODEL_SINGLE = 1,
	
	/* Keep enum 4bytes */
	CRIMANA_THREAD_MODEL_ENUM_SIZE_IS_4BYTES = 0x7FFFFFFF
} CriManaThreadModel;

/* CriManaVideoCodecType */
/*JP
 * \brief ビデオコーデック種別
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * 再生するムービーに含まれるビデオコーデックの種別です。
 * 現状は Sofdec.Prime (従来のSofdecコーデック) のみの対応になります。
 * 
 * \sa CriManaVideoInfo, CriManaMovieInfo, criManaPlayer_GetMovieInfo
 */
typedef enum {
	/* CriManaVideoCodecType.CRIMANA_VIDEO_CODEC_UNKNOWN */
	/*JP< Unknown */
	CRIMANA_VIDEO_CODEC_UNKNOWN = 0,

	/* CriManaVideoCodecType.CRIMANA_VIDEO_CODEC_SOFDEC_PRIME */
	/*JP< Sofdec.Prime */
	CRIMANA_VIDEO_CODEC_SOFDEC_PRIME = 1,

	/* CriManaVideoCodecType.CRIMANA_VIDEO_CODEC_H264 */
	/*JP< H.264/AVC     */
	CRIMANA_VIDEO_CODEC_H264 = 5,

	/* CriManaVideoCodecType.CRIMANA_VIDEO_CODEC_VP9 */
	/*JP< VP9     */
	CRIMANA_VIDEO_CODEC_VP9 = 9,

	/* Keep enum 4bytes */
	CRIMANA_VIDEO_CODEC_ENUM_SIZE_IS_4BYTES = 0x7FFFFFFF
} CriManaVideoCodecType;

/* CriManaAudioCodecType */
/*JP
 * \brief オーディオコーデック種別
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * 再生するムービーに含まれるオーディオコーデックの種別です。
 * 現状は ADX コーデックのみの対応になります。
 * 
 * \sa CriManaAudioInfo, CriManaMovieInfo, criManaPlayer_GetMovieInfo
 */
typedef enum {
	/* CriManaAudioCodecType.CRIMANA_AUDIO_CODEC_UNKNOWN */
	/*JP< Unknown */
	CRIMANA_AUDIO_CODEC_UNKNOWN = 0,

	/* CriManaAudioCodecType.CRIMANA_AUDIO_CODEC_ADX */
	/*JP< Advanced ADX */
	CRIMANA_AUDIO_CODEC_ADX = 2,

	/* CriManaAudioCodecType.CRIMANA_AUDIO_CODEC_HCA */
	/*JP< HCA          */
	CRIMANA_AUDIO_CODEC_HCA = 4,

	/* Keep enum 4bytes */
	CRIMANA_AUDIO_CODEC_ENUM_SIZE_IS_4BYTES = 0x7FFFFFFF
} CriManaAudioCodecType;

/* CriManaAlphaType */
/*JP
 * \brief アルファムービーの合成モード
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * 
 * \sa CriManaPlaybackAlphaParams, CriManaAlphaInfo, CriManaFrameInfo, 
 * criManaPlayer_IsFrameOnTime, criManaPlayer_ReferFrame, criManaPlayer_DiscardFrame
 * criManaPlayer_CopyFrameToBufferARGB32, criManaPlayer_CopyFrameToBuffersYUV
 */
typedef enum {
	/* CriManaAlphaType.CRIMANA_COMPO_OPAQ */
	/*JP< 不透明、アルファ情報なし								*/
	CRIMANA_COMPO_OPAQ		 = 0,

	/* CriManaAlphaType.CRIMANA_COMPO_ALPHFULL */
	/*JP< フルAlpha合成（アルファ用データが8ビット）			*/
	CRIMANA_COMPO_ALPHFULL	 = 1,

	/* CriManaAlphaType.CRIMANA_COMPO_ALPH3STEP */
	/*JP< 3値アルファ											*/
	CRIMANA_COMPO_ALPH3STEP	 = 2,

	/* CriManaAlphaType.CRIMANA_COMPO_ALPH32BIT */
	/*JP< フルAlpha、（カラーとアルファデータで32ビット）		*/
	CRIMANA_COMPO_ALPH32BIT	 = 3,

	/* CriManaAlphaType.CRIMANA_COMPO_ALPH1BIT */
	/*JP< フルAlpha、（カラーとアルファで32ビット、α値は2値）	*/
	CRIMANA_COMPO_ALPH1BIT	 = 4,

	/* CriManaAlphaType.CRIMANA_COMPO_ALPH2BIT */
	/*JP< フルAlpha、（カラーとアルファで32ビット、α値は4値）	*/
	CRIMANA_COMPO_ALPH2BIT	 = 5,

	/* CriManaAlphaType.CRIMANA_COMPO_ALPH3BIT */
	/*JP< フルAlpha、（カラーとアルファで32ビット、α値は8値）	*/
	CRIMANA_COMPO_ALPH3BIT	 = 6,

	/* CriManaAlphaType.CRIMANA_COMPO_ALPH4BIT */
	/*JP< フルAlpha、（カラーとアルファで32ビット、α値は16値）	*/
	CRIMANA_COMPO_ALPH4BIT	 = 7,

	/* Keep enum 4bytes */
	CRIMANA_COMPO_ENUM_SIZE_IS_4BYTES = 0x7FFFFFFF
} CriManaAlphaType;

/* CriManaMetaFlag */
/*JP
 * \brief メタデータの使用フラグ 【現在未対応】
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * どの種類のメタデータを使用したいかを制御するためのフラグです。
 * 現状は種別ごとのフラグ制御は未対応です。シーク再生やキューポイントを使用する場合は、
 * CRIMANA_META_FLAG_ALLを指定してください。
 * 
 * \sa criManaPlayer_SetMetaDataWorkAllocator
 */
typedef enum {
	/* CriManaMetaFlag.CRIMANA_META_FLAG_NONE */
	/*JP< メタデータを使用しない */
	CRIMANA_META_FLAG_NONE	= 0x0000,

	/* CriManaMetaFlag.CRIMANA_META_FLAG_CUE */
	/*JP< 未対応 */
	CRIMANA_META_FLAG_CUE	= 0x0001,

	/* CriManaMetaFlag.CRIMANA_META_FLAG_SEEK */
	/*JP< 未対応 */
	CRIMANA_META_FLAG_SEEK	= 0x0002,

	/* CriManaMetaFlag.CRIMANA_META_FLAG_ALL */
	/*JP< 全ての形式のメタデータを使用 */
	CRIMANA_META_FLAG_ALL	= CRIMANA_META_FLAG_CUE | CRIMANA_META_FLAG_SEEK,

	/* Keep enum 4bytes */
	CRIMANA_META_FLAG_ENUM_SIZE_IS_4BYTES = 0x7FFFFFFF
} CriManaMetaFlag;

/* CriManaReferFrameResult */
/*JP
 * \brief ReferFrameのフレーム参照結果 (デバッグ情報)
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * デコード済みのビデオフレームへの参照結果を表します。
 * このパラメタはデバッグ時の参考情報として使用してください。
 *
 * \sa CriManaFrameInfo
 * criManaPlayer_IsFrameOnTime, criManaPlayer_ReferFrame, criManaPlayer_DiscardFrame, 
 * criManaPlayer_CopyFrameToBufferARGB32, criManaPlayer_CopyFrameToBuffersYUV
 */
typedef enum {
	/* CriManaReferFrameResult.CRIMANA_REFER_RESULT_OK */
	/*JP< フレーム参照成功 */
	CRIMANA_REFER_RESULT_OK				= 0,

	/* CriManaReferFrameResult.CRIMANA_REFER_RESULT_SHORT_INPUT */
	/*JP< 入力バッファー内のデータ不足 */
	CRIMANA_REFER_RESULT_SHORT_INPUT	= 1,

	/* CriManaReferFrameResult.CRIMANA_REFER_RESULT_SHORT_CPUTIME */
	/*JP< デコード処理のCPU時間不足  */
	CRIMANA_REFER_RESULT_SHORT_CPUTIME	= 2,

	/* CriManaReferFrameResult.CRIMANA_REFER_RESULT_NO_MORE_KEEP */
	/*JP< キープ可能なフレームがない */
	CRIMANA_REFER_RESULT_NO_MORE_KEEP   = 3,

	/* Keep enum 4bytes */
	CRIMANA_REFER_RESULT_ENUM_SIZE_IS_4BYTES = 0x7FFFFFFF
} CriManaReferFrameResult;

/* CriManaColorSpaceConversionType */
/*JP
 * \brief 色変換の種類
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * 
 * \sa CriManaVideoInfo,CriManaAlphaInfo,CriManaFrameInfo
 */
typedef enum {
	CRIMANA_COLORSPACE_CONVERSION_TYPE_ITU_R_BT601_LIMITED = 0,
	CRIMANA_COLORSPACE_CONVERSION_TYPE_ITU_R_BT601_FULLRANGE = 1,

	/* Keep enum 4bytes */
	CRIMANA_COLORSPACE_CONVERSION_TYPE_ENUM_SIZE_IS_4BYTES = 0x7FFFFFFF
} CriManaColorSpaceConversionType;

/* CriManaDecodeMode */
/*JP
 * \brief デコーダ動作モード
 * \ingroup MDL_MANALIB_GLOBAL
 * \par 説明:
 * デコーダの動作モードを示します
 *
 * \sa criManaPlayer_SetDecodeMode
 */
typedef enum {
	/* CriManaDecodeMode.CRIMANA_DECODE_MODE_NORMAL */
	/*JP
	 *	\brief 通常デコード処理モード
	 *	\par 説明:
	 *  通常のデコード処理を実行します。<br>
	 *  ManaライブラリおよびManaプレーヤーのデフォルト値です。
	 */
	CRIMANA_DECODE_MODE_NORMAL	= 0,		

	/* CriManaDecodeMode.CRIMANA_DECODE_MODE_B_SKIP */
	/*JP
	 *	\brief 強制Bピクチャスキップモード
	 *	\par 説明:
	 *  再生しているムービーデータ内のビデオストリームにBピクチャが含まれている場合、
	 *  Bピクチャについてはデコード処理を実行しません。<br>
	 *  ムービー再生の負荷を減らしたい場合、この動作モードを指定します。
	 *
	 *  \par 備考:
	 *  Sofdec.Primeコーデックのみ対応。
	 */
	CRIMANA_DECODE_MODE_B_SKIP	= 1,

	CRIMANA_DECODE_MODE_ENUM_SIZE_IS_4BYTES = 0x7FFFFFFF

} CriManaDecodeMode;

/* CriManaPlayerMovieEventSyncMode */
/*JP
 * \brief ムービーイベント（キューポイント、字幕）の同期モード
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * ムービーイベントの同期モードを示します。
 * 
 * \sa criManaPlayer_SetMovieEventSyncMode
 */
typedef enum {
	/* CriManaPlayerMovieEventSyncMode.CRIMANAPLAYER_EVENT_SYNC_FRAME_TIME */
	/*JP
	* ムービーイベントはフレーム時刻（ビデオフレームの取得）に同期します。
	 */
	CRIMANAPLAYER_EVENT_SYNC_FRAME_TIME		= 0,

	/* CriManaPlayerMovieEventSyncMode.CRIMANAPLAYER_EVENT_SYNC_PLAYBACK_TIME */
	/*JP
	 * ムービーイベントは再生時刻（タイマ種別に基づいた再生時刻）に同期します。
	 */
	CRIMANAPLAYER_EVENT_SYNC_PLAYBACK_TIME	= 1,

	CRIMANAPLAYER_EVENT_SYNC_ENUM_SIZE_IS_4BYTES = 0x7FFFFFFF
} CriManaPlayerMovieEventSyncMode;

/***************************************************************************
 *      データ型宣言
 *      Data Type Declarations
 ***************************************************************************/
/*==========================================================================
 *      CRI Mana API
 *=========================================================================*/
/* CriManaLibConfig */
/*JP
 * \brief Manaライブラリ初期化パラメタ
 * \ingroup MDL_MANALIB_GLOBAL
 * \par 説明:
 * CRI Manaライブラリの動作仕様を指定するための構造体です。
 * \par 備考:
 * 各機種固有の初期化パラメタが存在する場合があります。詳細は機種別のリファレンスを参照してください。
 * \sa criMana_CalculateLibWorkSize, criMana_Initialize
 */
typedef struct {
	/* CriManaLibConfig.max_decoder_handles */
	/*JP< 最大消費デコーダハンドル数。アルファムービーを再生する場合はハンドル資源を二つ消費します。 */
	CriUint32			max_decoder_handles;

	/* CriManaLibConfig.thread_model */
	/*JP< スレッドモデル */
	CriManaThreadModel	thread_model;

	/* CriManaLibConfig.version */
	/*JP< ヘッダバージョン番号 */
	CriUint32			version;
} CriManaLibConfig;

/* CriManaEventPoint */
/*JP
 * \brief イベントポイント情報
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * キューポイント機能でムービーデータに埋め込まれた個々のタイミング情報です。
 * \sa criManaPlayer_GetCuePointInfo, CriManaCuePointInfo
 */
typedef struct {
	/* CriManaEventPoint.cue_name */
	/*JP< イベントポイント名。文字コードはキューポイント情報テキストに従います。 */
	CriChar8		*cue_name;

	/* CriManaEventPoint.size_name */
	/*JP< イベントポイント名のデータサイズ */
	CriUint32		size_name;

	/* CriManaEventPoint.time */
	/*JP< タイマカウント */
	CriUint64		time;

	/* CriManaEventPoint.tunit */
	/*JP< １秒あたりのタイマカウント値。count ÷ unit で秒単位の時刻となります。 */
	CriUint64		tunit;

	/* CriManaEventPoint.type */
	/*JP< イベントポイント種別 */
	CriSint32		type;

	/* CriManaEventPoint.param_string */
	/*JP< ユーザパラメータ文字列。文字コードはキューポイント情報テキストに従います。 */
	CriChar8		*param_string;

	/* CriManaEventPoint.size_param */
	/*JP< ユーザパラメータ文字列のデータサイズ */
	CriUint32		size_param;

	/* CriManaEventPoint.cnt_callback */
	/*JP< キューポイントコールバックの呼び出しカウンタ */
	CriUint32		cnt_callback;
} CriManaEventPoint;

/* CriManaCuePointInfo */
/*JP
 * \brief キューポイント情報
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * キューポイント情報は、イベントポイントの個数と一覧です。
 * \sa criManaPlayer_GetCuePointInfo, CriManaCuePointInfo
 */
typedef struct {
	/* CriManaCuePointInfo.num_eventpoint */
	/*JP< イベントポイント個数 */
	CriUint32			num_eventpoint;

	/* CriManaCuePointInfo.eventtable */
	/*JP< イベントポイント一覧 */
	CriManaEventPoint	*eventtable;
} CriManaCuePointInfo;

/*==========================================================================
 *      CRI Mana Player API
 *=========================================================================*/
/* CriManaPlayerConfig */
/*JP
 * \brief プレーヤー作成用コンフィグ構造体
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * Manaプレーヤーがムービーを再生する際の動作仕様を指定するためのデータ型です。<br>
 * ::criManaPlayer_CreateWithConfig 関数の引数に指定します。<br>
 * <br>
 * 作成されたプレーヤーは、ハンドル作成時に本構造体で指定された設定に応じて、
 * 内部リソースを必要なだけ確保します。<br>
 * <br>
 * ハンドル作成時にコンフィグ構造体を使用しない場合、再生ワークとして再生開始
 * 時に後で内部リソースが確保されます。事前に再生するムービーのスペックが決まっている
 * 場合に限り、本構造体を使用してください。
 * \sa criManaPlayer_CreateWithConfig
 */
typedef struct {
	/* CriManaPlayerConfig.readbuf_size_byte */
	/*JP
		\brief 入力バッファサイズ (単位:バイト)
		\par 説明:
		Manaプレーヤーが再生用に内部で確保する入力バッファーのサイズを指定できます。<br>
		<br>
		0以外を指定するとハンドル作成時に指定したサイズで入力バッファーを作成します。<br>
		0を指定した場合は、ハンドル作成時には作成せず、再生ワークの確保時に入力バッファーを確保します。<br>
		\par 計算方法：
		- 入力バッファサイズ = 再生するムービーの最大ビットレート(bps) / 8 * バッファリング時間(秒)<br>
		  例: 最大2Mbpsのムービーを再生する場合 : 2*1000*1000 / 8 * 1.0f = 250000<br>
		\par 注意:
		入力バッファサイズを小さくしすぎると再生が滞る恐れがあります。バッファリング時間は少なくとも１秒
		以上を割り当ててください。<br>
		マルチストリーミング再生時には本数に応じてサイズを大目にすることを推奨します。
	*/
	CriUint32	readbuf_size_byte;

	/* CriManaPlayerConfig.use_atomex_player */
	/*JP
	    \brief サウンド再生に ::criAtomExPlayer を使用するためのフラグです
		\par 説明:
		本フラグに CRI_TRUE を指定することで、 Mana プレーヤーのサウンド再生に ::criAtomExPlayer が使用されます。
		この criAtomExPlayer ハンドルを ::criManaPlayer_GetAtomExPlayer 関数で取得することが可能です。
	*/
	CriBool use_atomex_player;

	/* CriManaPlayerConfig.max_path */
	/*JP 
		\brief 最大パス長（バイト単位）
		\par 説明:
		Manaプレーヤーに指定可能なファイルパスの最大長です。<br>
		CRIMANA_MAX_FILE_NAME を超えるファイルパス長を取り扱う場合に、想定される十分な長さを max_path に指定することで
		Manaプレーヤーが使用可能なパス長の上限を拡張できます。<br>
		\par 備考:
		CRIMANA_MAX_FILE_NAME 以下の値を指定した場合は、max_pathは CRIMANA_MAX_FILE_NAME がセットされます。<br>
		\attention
		- Manaプレーヤーの最大パス長を拡張する場合、
		  CRI File System ライブラリ側でも CriFsConfig メンバーの max_path に最大長を指定し、
		  事前に CRI File System ライブラリを初期化しておく必要があります。<br>
		- パスの最大長には、終端のNULL文字を含んだ数を指定する必要があります。
		（「文字数＋１バイト」の値を指定する必要があります。）<br>
	*/
	CriSint32 max_path;

} CriManaPlayerConfig;

struct CriManaPlayerTag;
typedef struct CriManaPlayerTag CriManaPlayerObj;
/* CriManaPlayerHn */
/*JP
 * \brief プレーヤハンドル
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * CriManaPlayerHn は、動画再生用に作られたプレーヤーを操作するためのハンドルです。<br>
 * ::criManaPlayer_Create 関数で動画再生用のプレーヤーを作成すると、
 * 関数はプレーヤー操作用に、この"Manaプレーヤハンドル"を返します。
 * <br>
 * データのセットや再生の開始、ステータスの取得等、プレーヤーに対して行う操作は、
 * 全てManaプレーヤハンドルを介して実行されます。<br>
 * \sa criManaPlayer_Create
 */
typedef CriManaPlayerObj *CriManaPlayerHn;

/*==========================================================================
 *      再生ワークサイズの調整用構造体
 *=========================================================================*/
/* CriManaPlaybackCommonParams */
/*JP
 * \brief 再生ワーク用基本パラメタ
 * \ingroup MDL_MANALIB_GLOBAL
 * \par 説明:
 * ムービー再生に必要な基本再生ワーク領域を確保する際に参照するパラメタです。
 *
 * \sa CriManaPlaybackBasicWorkConfig, criManaPlayer_GetPlaybackWorkParam, 
 * criManaPlayer_CalculatePlaybackWorkSize, criManaPlayer_SetPlaybackWork
 */
typedef struct {
	/* CriManaPlaybackCommonParams.readbuf_size_byte */
	/*JP< 入力バッファサイズ (単位:バイト) */
	CriSint32 readbuf_size_byte;

	/* CriManaPlaybackCommonParams.readbuf_size_byte */
	/*JP< ムービーに含まれるオーディオのトラック数 */
	CriUint32 max_audio_tracks;
} CriManaPlaybackCommonParams;

/* CriManaPlaybackVideoParams */
/*JP
 * \brief ビデオ関連再生パラメタ
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * ビデオの再生に必要なパラメタを表します。
 * ビデオの再生に必要なワーク領域はこのパラメタを元に計算され、最終的なワークサイズに
 * 加算されます。
 *
 * \sa CriManaPlaybackBasicWorkConfig, criManaPlayer_GetPlaybackWorkParam, 
 * criManaPlayer_CalculatePlaybackWorkSize, criManaPlayer_SetPlaybackWork
 */
typedef struct {
	/* CriManaPlaybackVideoParams.video_flag */
	/*JP< [未使用] ビデオ使用フラグ */
	CriBool		video_flag;

	/* CriManaPlaybackVideoParams.max_width */
	/*JP< ムービー最大幅（８の倍数） */
	CriSint32	max_width;

	/* CriManaPlaybackVideoParams.max_height */
	/*JP< ムービー最大高さ（８の倍数） */
	CriSint32	max_height;

	/* CriManaPlaybackVideoParams.num_frame_pools */
	/*JP< フレームプール数 */
	CriSint32	num_frame_pools;

	/* CriManaPlaybackVideoParams.codec_type */
	/*JP< コーデック種別 (CRIMANA_VIDEO_CODEC_UNKNOWNが指定された場合はSofdec.Primeとみなします) */ 
	CriManaVideoCodecType codec_type;

	/* CriManaPlaybackVideoParams.capacity_of_picsize */
	/*JP< ピクチャサイズ上限値 */
	CriSint32	capacity_of_picsize;

	/* CriManaPlaybackVideoParams.framerate_n */
	/*JP< 有理数形式フレームレート(分子) framerate [x1000] = framerate_n / framerate_d */
	CriSint32	framerate_n;

	/* CriManaPlaybackVideoParams.framerate_d */
	/*JP< 有理数形式フレームレート(分母) framerate [x1000] = framerate_n / framerate_d */
	CriSint32	framerate_d;

	// CriManaVideoLevel level;			/* 未使用 */	/* Not used */
} CriManaPlaybackVideoParams;

/* CriManaPlaybackAudioParams */
/*JP
 * \brief オーディオ関連再生パラメタ
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * オーティオの再生に必要なパラメタを表します。
 * オーディオの再生に必要なワーク領域はこのパラメタを元に計算され、最終的なワークサイズに
 * 加算されます。
 *
 * \sa CriManaPlaybackBasicWorkConfig, criManaPlayer_GetPlaybackWorkParam, 
 * criManaPlayer_CalculatePlaybackWorkSize, criManaPlayer_SetPlaybackWork
 */
typedef struct {
	/* CriManaPlaybackAudioParams.audio_flag */
	/*JP< オーディオ使用フラグ */
	CriBool		audio_flag;

	/* CriManaPlaybackAudioParams.sampling_rate */
	/*JP< サンプリング周波数（単位：Hz）*/
	CriUint32	sampling_rate;

	/* CriManaPlaybackAudioParams.num_channels */
	/*JP< チャンネル数 */
	CriUint32	num_channels;

	/* CriManaPlaybackAudioParams.output_buffer_samples */
	/*JP< 出力バッファサイズの指定（単位：サンプル数） */
	CriSint32	output_buffer_samples;

	/* CriManaPlaybackAudioParams.codec_type */
	/*JP< コーデック種別 (CRIMANA_AUDIO_CODEC_UNKNOWNが指定された場合はADXとみなします) */
	CriManaAudioCodecType codec_type;
} CriManaPlaybackAudioParams;

/* CriManaPlaybackSubtitleParams */
/*JP
 * \brief 字幕関連再生パラメタ
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * 字幕の再生に必要なパラメタを表します。
 * 字幕の再生に必要なワーク領域はこのパラメタを元に計算され、最終的なワークサイズに
 * 加算されます。
 * 
 * \sa CriManaPlaybackBasicWorkConfig, criManaPlayer_GetPlaybackWorkParam, 
 * criManaPlayer_CalculatePlaybackWorkSize, criManaPlayer_SetPlaybackWork
 */
typedef struct {
	/* CriManaPlaybackSubtitleParams.subtitle_flag */
	/*JP< 字幕使用フラグ */
	CriBool		subtitle_flag;

	/* CriManaPlaybackSubtitleParams.max_datasize */
	/*JP< 最大字幕データサイズ */
	CriSint32	max_datasize;
} CriManaPlaybackSubtitleParams;

/* CriManaPlaybackAlphaParams */
/*JP
 * \brief アルファチャンネルパラメタ
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * アルファチャンネルのデコードに必要なパラメタを表します。
 * アルファチャンネルのデコードに必要なワーク領域はこのパラメタを元に計算され、
 * 最終的なワークサイズに加算されます。
 *
 * \sa CriManaPlaybackBasicWorkConfig, criManaPlayer_GetPlaybackWorkParam, 
 * criManaPlayer_CalculatePlaybackWorkSize, criManaPlayer_SetPlaybackWork
 */
typedef struct {
	/* CriManaPlaybackAlphaParams.alpha_flag */
	/*JP< アルファチャンネル使用フラグ */
	CriBool				alpha_flag;

	/* CriManaPlaybackAlphaParams.alpha_type */
	/*JP< アルファムービーの合成モード */
	CriManaAlphaType	alpha_type;

	/* CriManaPlaybackAlphaParams.max_width */
	/*JP< ムービー最大幅（８の倍数） */
	CriSint32			max_width;

	/* CriManaPlaybackAlphaParams.max_height */
	/*JP< ムービー最大高さ（８の倍数） */
	CriSint32			max_height;

	/* CriManaPlaybackAlphaParams.codec_type */
	/*JP< コーデック種別 (CRIMANA_VIDEO_CODEC_UNKNOWNが指定された場合はSofdec.Primeとみなします) */ 
	CriManaVideoCodecType codec_type;

	/* CriManaPlaybackAlphaParams.capacity_of_picsize */
	/*JP< ピクチャサイズ上限値 */
	CriSint32			capacity_of_picsize;
} CriManaPlaybackAlphaParams;

/* CriManaPlaybackBasicWorkConfig */
/*JP
 * \brief 再生ワーク用パラメタ構造体 (基本) 
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * ムービー再生を行うために必ず必要となる再生パラメタです。
 * ::criManaPlayer_SetPlaybackWork 関数に、ワーク領域と一緒に引数として渡してください。
 *
 * \sa CriManaPlaybackBasicWorkConfig, criManaPlayer_GetPlaybackWorkParam, 
 * criManaPlayer_CalculatePlaybackWorkSize, criManaPlayer_SetPlaybackWork
 */
typedef struct {
	/* CriManaPlaybackBasicWorkConfig.common_params */
	/*JP< 基本バッファー量調整パラメタ */
	CriManaPlaybackCommonParams common_params;

	/* CriManaPlaybackBasicWorkConfig.video_params */
	/*JP< ビデオ用再生ワークパラメタ */
	CriManaPlaybackVideoParams  video_params;

	/* CriManaPlaybackBasicWorkConfig.main_audio_params */
	/*JP< メインオーディオ用再生ワークパラメタ */
	CriManaPlaybackAudioParams  main_audio_params;

	/* CriManaPlaybackBasicWorkConfig.subtitle_params */
	/*JP< 字幕用再生ワークパラメタ */
	CriManaPlaybackSubtitleParams subtitle_params;
} CriManaPlaybackBasicWorkConfig;

/* CriManaPlaybackExWorkConfig */
/*JP
 * \brief 再生ワーク用パラメタ構造体（拡張）
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * 特殊な再生を行う場合に必要となるワークバッファのサイズを調整するための拡張制御パラメタです。
 * サブオーディオ再生やアルファムービー再生などを行う場合は、このパラメタ構造体を
 * CriManaPlaybackBasicWorkConfig 構造体と一緒に ::criManaPlayer_SetPlaybackWork 関数に
 * 渡してください。
 *
 * \sa criManaPlayer_GetPlaybackWorkParam, criManaPlayer_CalculatePlaybackWorkSize, 
 * criManaPlayer_SetPlaybackWork
 */
typedef struct {
	/* CriManaPlaybackExWorkConfig.sub_audio_params */
	/*JP< SubAudio */
	CriManaPlaybackAudioParams      sub_audio_params;

	/* CriManaPlaybackExWorkConfig.extra_audio_params */
	/*JP< ExtraAudio */
	CriManaPlaybackAudioParams      extra_audio_params;

	/* CriManaPlaybackExWorkConfig.alpha_params */
	/*JP< Alpha */
	CriManaPlaybackAlphaParams      alpha_params;

	//CriManaPlaybackZmovieParams	zmovie_params;		/* Z Movie */
	//CriManaPlaybackStereo3DParams stereo3d_params;	/* Stereo 3D */
} CriManaPlaybackExWorkConfig;


/*==========================================================================
 *      ムービーファイルの詳細情報
 *=========================================================================*/
/* CriManaAudioInfo */
/*JP 
 * \brief オーディオパラメータ
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * ムービーファイルに含まれるオーディオデータの解析情報です。
 *
 * \sa CriManaMovieInfo, criManaPlayer_GetMovieInfo
 */
typedef struct {
	/* CriManaAudioInfo.codec_type */
	/*JP< コーデック種別 */
	CriManaAudioCodecType	codec_type;

	/* CriManaAudioInfo.sampling_rate */
	/*JP< サンプリング周波数 */
	CriUint32				sampling_rate;

	/* CriManaAudioInfo.num_channels */
	/*JP< オーディオチャンネル数 */
	CriUint32				num_channels;

	/* CriManaAudioInfo.total_samples */
	/*JP< 総サンプル数 */
	CriUint32				total_samples;

	/* CriManaAudioInfo.output_buffer_samples */
	/*JP< サウンド出力バッファーのサンプル数 */
	CriUint32				output_buffer_samples;

	/* CriManaAudioInfo.ambisonics */
	/*JP< Ambisonicsフラグ */
	CriUint8				ambisonics;
} CriManaAudioInfo;

/* CriManaVideoInfo */
/*JP 
 * \brief ビデオパラメータ
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * ムービーファイルに含まれるビデオデータの解析情報です。
 * 
 * \sa CriManaMovieInfo, criManaPlayer_GetMovieInfo
 */
typedef struct {
	/* CriManaVideoInfo.width */
	/*JP< ムービー最大幅（８の倍数） */
	CriUint32				width;

	/* CriManaVideoInfo.height */
	/*JP< ムービー最大高さ（８の倍数） */
	CriUint32				height;

	/* CriManaVideoInfo.disp_width */
	/*JP< 表示したい映像の横ピクセル数（左端から） */
	CriUint32				disp_width;

	/* CriManaVideoInfo.disp_height */
	/*JP< 表示したい映像の縦ピクセル数（上端から） */
	CriUint32				disp_height;

	/* CriManaVideoInfo.framerate */
	/*JP< フレームレート[x1000] */
	CriUint32				framerate;

	/* CriManaVideoInfo.framerate_n */
	/*JP< 有理数形式フレームレート(分子) framerate [x1000] = framerate_n / framerate_d */
	CriUint32				framerate_n;

	/* CriManaVideoInfo.framerate_d */
	/*JP< 有理数形式フレームレート(分母) framerate [x1000] = framerate_n / framerate_d */
	CriUint32				framerate_d;

	/* CriManaVideoInfo.total_frames */
	/*JP< 総フレーム数 */
	CriUint32				total_frames;

	/* CriManaVideoInfo.material_width */
	/*JP< エンコード前のビデオ素材の横ピクセル数 */
	CriUint32				material_width;

	/* CriManaVideoInfo.material_height */
	/*JP< エンコード前のビデオ素材の縦ピクセル数 */
	CriUint32				material_height;

	/* CriManaVideoInfo.codec_type */
	/*JP< ビデオコーデック種別 */
	CriManaVideoCodecType	codec_type;

	/* CriManaVideoInfo.color_conv */
	/*JP< 色変換種別 */
	CriManaColorSpaceConversionType color_conv;

	/* CriManaVideoInfo.max_picture_size */
	/*JP< 最大ピクチャサイズ（無効時 -1） */
	CriSint32				max_picture_size;

	/* CriManaVideoInfo.average_bitrate */
	/*JP< 平均ビットレート */
	CriSint32				average_bitrate;
} CriManaVideoInfo;

/* CriManaAlphaInfo */
/*JP 
 * \brief アルファパラメータ
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * ムービーファイルに含まれるアルファチャンネルデータの解析情報です。
 * 
 * \sa CriManaMovieInfo, criManaPlayer_GetMovieInfo
 */
typedef struct {
	/* CriManaAlphaInfo.width */
	/*JP< アルファフレームの最大幅 */
	CriUint32			width;

	/* CriManaAlphaInfo.height */
	/*JP< アルファフレームの最大高さ */
	CriUint32			height;

	/* CriManaAlphaInfo.disp_width */
	/*JP< アルファフレームの実有効幅 */
	CriUint32			disp_width;

	/* CriManaAlphaInfo.disp_height */
	/*JP< アルファフレームの実有効高さ */
	CriUint32			disp_height;

	/* CriManaAlphaInfo.framerate */
	/*JP< アルファのフレームレート[x1000] */
	CriUint32			framerate;

	/* CriManaAlphaInfo.framerate_n */
	/*JP< 有理数形式フレームレート(分子) framerate [x1000] = framerate_n / framerate_d */
	CriUint32			framerate_n;

	/* CriManaAlphaInfo.framerate_d */
	/*JP< 有理数形式フレームレート(分母) framerate [x1000] = framerate_n / framerate_d */
	CriUint32			framerate_d;

	/* CriManaAlphaInfo.total_frames */
	/*JP< 総フレーム数 */
	CriUint32			total_frames;

	/* CriManaAlphaInfo.alpha_type */
	/*JP< アルファムービーの合成モード */
	CriManaAlphaType	alpha_type;

	/* CriManaAlphaInfo.codec_type */
	/*JP< コーデック種別 */
	CriManaVideoCodecType	codec_type;

	/* CriManaAlphaInfo.color_conv */
	/*JP< 色変換種別 */
	CriManaColorSpaceConversionType color_conv;

	/* CriManaAlphaInfo.max_picture_size */
	/*JP< 最大ピクチャサイズ */
	CriSint32			max_picture_size;

	/* CriManaAlphaInfo.average_bitrate */
	/*JP< 平均ビットレート */
	CriSint32			average_bitrate;
} CriManaAlphaInfo;

/* CriManaMovieInfo */
/*JP
 * \brief ムービー情報構造体
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * ヘッダ解析後に取得可能なムービーファイルの情報です。
 *
 * \sa criManaPlayer_GetMovieInfo
 */
typedef struct {
	/* Stream */
	/* CriManaMovieInfo.is_playable */
	/*JP< 再生可能フラグ（1: 再生可能、0: 再生不可） */
	CriUint32			is_playable;

	/* CriManaMovieInfo.average_bitrate */
	/*JP< 平均ビットレート(絵と音の合計) */
	CriUint32			average_bitrate;

	/* CriManaMovieInfo.max_chunk_size */
	/*JP< 最大USFチャンクサイズ */
	CriUint32			max_chunk_size;

	/* CriManaMovieInfo.min_buffer_size */
	/*JP< 最低限必要な読み込みバッファサイズ。<br> オーディオとビデオの合計 */
	CriUint32			min_buffer_size;

	/* Video */
	/* CriManaMovieInfo.num_video_streams */
	/*JP< ビデオストリームの数。現在は1固定。 */
	CriUint32			num_video_streams;

	/* CriManaMovieInfo.video_prm */
	/*JP< ビデオパラメータ */
	CriManaVideoInfo	video_prm[CRIMANA_MAX_VIDEO_NUM];

	/* Audio */
	/* CriManaMovieInfo.num_audio_streams */
	/*JP< オーディオストリームの数 */
	CriUint32			num_audio_streams;

	/* CriManaMovieInfo.audio_prm */
	/*JP< オーディオパラメータ */
	CriManaAudioInfo	audio_prm[CRIMANA_MAX_AUDIO_NUM];

	/* Subtitle */
	/* CriManaMovieInfo.num_subtitle_channels */
	/*JP< 字幕チャンネル数 */
	CriUint32			num_subtitle_channels;

	/* CriManaMovieInfo.max_subtitle_size */
	/*JP< 字幕データの最大サイズ*/
	CriUint32			max_subtitle_size;

	/* Composite mode */
	/* CriManaMovieInfo.num_alpha_streams */
	/*JP< アルファストリームの数。現在は1固定。 */
	CriUint32			num_alpha_streams;

	/* CriManaMovieInfo.alpha_prm */
	/*JP< アルファパラメータ */
	CriManaAlphaInfo	alpha_prm[CRIMANA_MAX_ALPHA_NUM];

	/* Seek */
	/* CriManaMovieInfo.seekinfo_flag */
	/*JP< シーク情報フラグ */
	CriBool				seekinfo_flag;

	/* Others */
	/* CriManaMovieInfo.format_ver */
	/*JP< フォーマットバージョン */
	CriUint32			format_ver;
} CriManaMovieInfo;

/*==========================================================================
 *      ビデオフレーム情報構造体
 *=========================================================================*/
/* CriManaImageBufferInfo */
/*JP
 * \brief イメージ情報
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * フレーム取得関連APIで参照したビデオフレームのイメージ情報（プレーン情報）です。<br>
 * CriManaFrameInfo構造体のメンバ配列としてY、U、V、Aの組み合わせで構成されます。<br>
 * これらのイメージ情報はcriManaPlayer_CopyFrameToBufferARGB32などでCPUによる<br>
 * フレーム変換（色変換）で利用されます。また、機種によってはこれらのイメージ情報を<br>
 * 直接扱い、ハードウェアやピクセルシェーダなどを使用して効率的にフレーム変換を<br>
 * 行います。
 *
 * \sa CriManaFrameInfo, criManaPlayer_CopyFrameToBufferARGB32
 */
typedef struct {
	/* CriManaImageBufferInfo.imageptr */
	/*JP< イメージ情報のバッファポインタ */
	CriUint8	*imageptr;

	/* CriManaImageBufferInfo.bufsize */
	/*JP< イメージ情報のバッファサイズ [Byte] */
	CriUint32	bufsize;

	/* CriManaImageBufferInfo.line_pitch */
	/*JP< イメージ情報のラインピッチ [Byte] */
	CriUint32	line_pitch;

	/* CriManaImageBufferInfo.line_size */
	/*JP< イメージ情報のラインサイズ [Byte] */
	CriUint32	line_size;

	/* CriManaImageBufferInfo.num_lines */
	/*JP< イメージ情報のライン数 */
	CriUint32	num_lines;
} CriManaImageBufferInfo;

/* CriManaFrameInfo */
/*JP
 * \brief ビデオフレーム情報
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * フレーム取得関連APIで参照したビデオフレームの情報です。
 * フレーム情報の取得に失敗した場合は、frame_noに-1が入ります。
 *
 * \sa criManaPlayer_IsFrameOnTime, criManaPlayer_ReferFrame, criManaPlayer_DiscardFrame, 
 * criManaPlayer_CopyFrameToBufferARGB32, criManaPlayer_CopyFrameToBuffersYUV
 */
typedef struct {
	/* CriManaFrameInfo.frame_no */
	/*JP< フレーム識別番号(0からの通し番号) */
	CriSint32						frame_no;

	/* CriManaFrameInfo.frame_no_per_file */
	/*JP< フレーム識別番号(ムービーファイル固有の識別番号) */
	CriSint32						frame_no_per_file;

	/* CriManaFrameInfo.width */
	/*JP< ムービーの横幅[pixel] (８の倍数) */
	CriUint32						width;

	/* CriManaFrameInfo.height */
	/*JP< ムービーの高さ[pixel] (８の倍数) */
	CriUint32						height;

	/* CriManaFrameInfo.disp_width */
	/*JP< 表示したい映像の横ピクセル数（左端から） */
	CriUint32						disp_width;

	/* CriManaFrameInfo.disp_height */
	/*JP< 表示したい映像の縦ピクセル数（上端から） */
	CriUint32						disp_height;

	/* CriManaFrameInfo.framerate */
	/*JP< フレームレートの1000倍の値 */
	CriUint32						framerate;

	/* CriManaFrameInfo.framerate_n */
	/*JP< 有理数形式フレームレート(分子) framerate [x1000] = framerate_n / framerate_d */
	CriUint32						framerate_n;

	/* CriManaFrameInfo.framerate_d */
	/*JP< 有理数形式フレームレート(分母) framerate [x1000] = framerate_n / framerate_d */
	CriUint32						framerate_d;

	/* CriManaFrameInfo.total_frames_per_file */
	/*JP< ファイル単位の総フレーム数 */
	CriUint32						total_frames_per_file;

	/* CriManaFrameInfo.time */
	/*JP< 時刻。time / tunit で秒を表す。ループ再生や連結再生時には継続加算。 */
	CriUint64						time;

	/* CriManaFrameInfo.time_per_file */
	/*JP< [未サポート] ファイル内の再生時刻 */
	CriUint64						time_per_file;

	/* CriManaFrameInfo.tunit */
	/*JP< 時刻単位 */
	CriUint64						tunit;

	/* CriManaFrameInfo.cnt_concatenated_movie */
	/*JP< ムービーの連結回数 */
	CriUint32						cnt_concatenated_movie;

	/* CriManaFrameInfo.num_images */
	/*JP< デコード済フレームバッファの画像枚数 */
	CriSint32						num_images;

	/* CriManaFrameInfo.image_info */
	/*JP< フレーム取得時にY、U、V、Aの各イメージ情報（プレーン情報）を配列として格納します */
	CriManaImageBufferInfo			image_info[4];

	/* CriManaFrameInfo.csc_flag */
	/*JP< テスト中の変数です。アクセスしないでください。 */
	CriBool							csc_flag;

	/* CriManaFrameInfo.alpha_type */
	/*JP< アルファの合成モード*/
	CriManaAlphaType				alpha_type;

	/* CriManaFrameInfo.ref_result */
	/*JP< フレーム参照結果  */
	CriManaReferFrameResult			ref_result;

	/* CriManaFrameInfo.details_ptr */
	/*JP< 詳細情報へのポインタ（特定用途向け） */
	void							*details_ptr[CRIMANA_FRAME_DETAILS_NUM];
	CriManaColorSpaceConversionType	color_conv;

	/* CriManaFrameInfo.cnt_skipped_frames */
	/*JP< 内部でデコードスキップされたフレーム数 */
	CriUint32						cnt_skipped_frames;

	/* CriManaFrameInfo.reserved */
	/*JP< 予約領域 */
	CriUint32						reserved[1];
} CriManaFrameInfo;

/* CriManaTextureBuffer */
/*JP
 * \brief テクスチャバッファ構造体
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * Manaプレーヤーがアプリケーションに渡す、デコード済みビデオフレームの出力バッファー構造体。
 * この構造体には、アプリケーションが描画に必要なバッファアドレスやサイズなどの情報が含まれます。
 *
 * \sa criManaPlayer_CopyFrameToBufferARGB32
 */
typedef struct {
	/* CriManaTextureBuffer.imagebuf */
	/*JP< テクスチャのバッファポインタ */
	CriUint8	*imagebuf;

	/* CriManaTextureBuffer.bufsize */
	/*JP< テクスチャのバッファサイズ[byte] */
	CriUint32	bufsize;

	/* CriManaTextureBuffer.pitch */
	/*JP< テクスチャのピッチ[byte] */
	CriUint32	pitch;
} CriManaTextureBuffer;

/* CriManaTextureBuffersYUV */
/*JP
 * \brief YUVテクスチャバッファ構造体
 * \ingroup MDL_MANALIB_PLAYER
 * \par 説明:
 * criManaPlayer_CopyFrameToBuffersYUV 関数の出力バッファー構造体です。
 * デコードした Y, U, V, Alphaプレーンを、それぞれ個別のバッファー情報として含みます。
 * アプリケーションはこの個別のバッファーを、シェーダーを利用してRGBに色変換をして描画してください。
 * 
 * \sa criManaPlayer_CopyFrameToBuffersYUV
 */
typedef struct {
	/* CriManaTextureBuffersYUV.y_plane */
	/*JP< Yプレーンのテクスチャバッファ構造体 */
	CriManaTextureBuffer y_plane;

	/* CriManaTextureBuffersYUV.u_plane */
	/*JP< Uプレーンのテクスチャバッファ構造体 */
	CriManaTextureBuffer u_plane;

	/* CriManaTextureBuffersYUV.v_plane */
	/*JP< Vプレーンのテクスチャバッファ構造体 */
	CriManaTextureBuffer v_plane;

	/* CriManaTextureBuffersYUV.a_plane */
	/*JP< Alphaプレーンのテクスチャバッファ構造体 */
	CriManaTextureBuffer a_plane;
} CriManaTextureBuffersYUV;

/*==========================================================================
 *      CRI Mana Decrypter API
 *=========================================================================*/
/* CriManaDecrypterConfigTag */
/*JP
 * \brief デクリプタ作成用コンフィグ構造体
 * \ingroup MDL_MANALIB_DECRYPTER
 * \par 説明:
 * デクリプタ作成に必要なパラメータを格納した構造体です。<br>
 * ::criManaDecrypter_Create 関数の引数として指定します。<br>
 * \attention
 * 将来的にメンバが増える可能性があるため、使用前に必ず構造体をゼロクリアしてください。<br>
 * （構造体のメンバに不定値が入らないようご注意ください。）
 * \sa criManaDecrypter_Create
 */
typedef struct CriManaDecrypterConfigTag {
	/* CriManaDecrypterConfig.key */
	/*JP
		\brief 復号キー
		\par 説明:
		復号に使用するキーの番号を指定します。<br>
		データを暗号化する際に使用したキーと同一の値を指定する必要があります。<br>
		\attention
		復号キーに 0 を指定することはできません。<br>
		（ 0 を指定した場合、デクリプタの作成に失敗します。）<br>
		
	*/
	CriUint64 key;
} CriManaDecrypterConfig;

/* CriManaDecrypterHn */
/*JP
 * \brief デクリプタハンドル
 * \ingroup MDL_MANALIB_DECRYPTER
 * \par 説明:
 * デクリプタを操作するためのハンドルです。<br>
 * ::criManaDecrypter_Create 関数の戻り値として返されます。<br>
 * \sa criManaDecrypter_Create
 */
typedef void *CriManaDecrypterHn;

/***************************************************************************
 *      コールバック宣言
 *      Prototype Callback
 ***************************************************************************/
/* CriManaMallocFunc */
/*JP
 * \brief メモリ確保関数
 * \ingroup MDL_MANALIB_GLOBAL
 * \param[in]	obj		ユーザー指定オブジェクト
 * \param[in]	size	要求メモリサイズ（バイト単位）
 * \return		void*	確保したメモリのアドレス（失敗時はNULL）
 * \par 説明:
 * メモリ確保関数登録用のインターフェイスです。<br>
 * CRI Manaライブラリがライブラリ内で行なうメモリ確保処理を、
 * ユーザー独自のメモリ確保処理に置き換えたい場合に使用します。<br>
 * \par 備考:
 * コールバック関数が実行される際には、sizeに必要とされるメモリのサイズがセット
 * されています。<br>
 * コールバック関数内でsize分のメモリを確保し、確保したメモリのアドレスを
 * 戻り値として返してください。<br>
 * 尚、引数の obj には、 ::criMana_SetUserAllocator 関数で登録したユーザー指定
 * オブジェクトが渡されます。<br>
 * メモリ確保時にメモリマネージャ等を参照する必要がある場合には、
 * 当該オブジェクトを ::criMana_SetUserAllocator 関数の引数にセットしておき、
 * 本コールバック関数の引数を経由して参照してください。<br>
 * \attention
 * メモリの確保に失敗した場合、エラーコールバックが返されたり、呼び出し元の関数が
 * 失敗する可能性がありますのでご注意ください。
 * \sa CriManaFreeFunc, criMana_SetUserAllocator
 */
typedef void *(CRIAPI *CriManaMallocFunc)(void *obj, CriUint32 size);

/* CriManaFreeFunc */
/*JP
 * \brief メモリ解放関数
 * \ingroup MDL_MANALIB_GLOBAL
 * \param[in]	obj		ユーザー指定オブジェクト
 * \param[in]	mem		解放するメモリアドレス
 * \return				なし
 * \par 説明:
 * メモリ解放関数登録用のインターフェイスです。<br>
 * CRI Manaライブラリ内がライブラリ内で行なうメモリ解放処理を、
 * ユーザー独自のメモリ解放処理に置き換えたい場合に使用します。<br>
 * \par 備考:
 * コールバック関数が実行される際には、memに解放すべきメモリのアドレスがセット
 * されています。<br>
 * コールバック関数内でmemの領域のメモリを解放してください。
 * 尚、引数の obj には、::criMana_SetUserAllocator 関数で登録したユーザー指定
 * オブジェクトが渡されます。<br>
 * メモリ確保時にメモリマネージャ等を参照する必要がある場合には、
 * 当該オブジェクトを ::criMana_SetUserAllocator 関数の引数にセットしておき、
 * 本コールバック関数の引数を経由して参照してください。<br>
 * \sa CriManaMallocFunc, criMana_SetUserAllocator
 */
typedef void (CRIAPI *CriManaFreeFunc)(void *obj, void *mem);

/* CriManaMetaMallocFunc */
/*JP
 * \brief メタデータワーク用メモリ確保関数
 * \ingroup MDL_MANALIB_GLOBAL
 * \par 説明:
 * メタデータワーク領域を確保するためのインターフェイスです。
 * 
 * \sa criManaPlayer_SetMetaDataWorkAllocator
 */
#define CriManaMetaMallocFunc CriManaMallocFunc

/* CriManaMetaFreeFunc */
/*JP
 * \brief メタデータワーク用メモリ解放関数
 * \ingroup MDL_MANALIB_GLOBAL
 * \par 説明:
 * メタデータワーク領域を解放するためのインターフェイスです。
 * 
 * \sa criManaPlayer_SetMetaDataWorkAllocator
 */
#define CriManaMetaFreeFunc CriManaFreeFunc

/* CriManaPlayerDataRequestCbFunc */
/*JP
 * \brief データ要求コールバック関数
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	obj		ユーザー指定オブジェクト
 * \param[in]	player	Manaプレーヤハンドル
 * \par 説明:
 * 次に再生するムービーを指定するためのコールバック関数です。
 * 複数のムービーファイルをシームレスに連結して再生する場合に使用します。
 * <br>
 * コールバック関数の登録には ::criManaPlayer_SetDataRequestCallbackを使用します。
 *
 */
typedef void (CRIAPI *CriManaPlayerDataRequestCbFunc)(void* obj, CriManaPlayerHn player);

/* CriManaPlayerCuePointCbFunc */
/*JP
 * \brief キューポイントコールバック関数
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	obj			ユーザー指定オブジェクト
 * \param[in]	player		Manaプレーヤハンドル
 * \param[in]	eventinfo	キューポイントイベント情報
 * \par 説明:
 * ムービーに埋め込まれたキューポイント（タイミング情報）が発生した時に実行される
 * コールバック関数です。
 * <br>
 * コールバック関数の登録には ::criManaPlayer_SetCuePointCallback 関数を使用します。
 */
typedef void (CRIAPI *CriManaPlayerCuePointCbFunc)(void* obj, CriManaPlayerHn player, CriManaEventPoint *eventinfo);


/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*==========================================================================
 *      CRI Mana API
 *=========================================================================*/
/* criMana_GetVersionNumber */
/*JP
 * \brief Manaライブラリバージョン番号の取得
 * \ingroup MDL_MANALIB_GLOBAL
 * \return		CriUint32	バージョン番号
 * \par 呼び出し条件:
 * スレッドセーフです。
 * \par 説明:
 * Manaライブラリのバージョン番号を取得します。<br>
 * 本関数で取得した値とヘッダファイル（ cri_mana.h ）に記載された
 * CRIMANA_VERSION の値が異なる場合、インクルードしているヘッダと、
 * リンクしているライブラリのバージョンが一致していない可能性があります。<br>
 * （ビルド環境に設定されたインクルードパスとライブラリパスを確認してください。）<br>
 * \par 備考:
 * 以下のコードで、デバッグ時にライブラリとヘッダバージョンの不一致を検出可能です。<br>
 * \code
 * 		：
 * 	// ライブラリとヘッダの整合性をチェック
 * 	assert(CRIMANA_VERSION == criMana_GetVersionNumber());
 * 		：
 * \endcode
 */
CriUint32 CRIAPI criMana_GetVersionNumber(void);

/* criMana_GetLibraryVersionString */
/*JP
 * \brief Manaライブラリのバージョン番号やビルド情報を返します。
 * \ingroup MDL_MANALIB_GLOBAL
 * \return	const CriChar8 * ライブラリ情報文字列
 * \par 呼び出し条件:
 * スレッドセーフです。
 * \par 説明:
 * ライブラリのバージョン、ビルドした日時、プラットホームの情報が表示されます。
 */
const CriChar8* CRIAPI criMana_GetLibraryVersionString(void);

/* criMana_SetUserAllocator */
/*JP
 * \brief ユーザアロケータの登録
 * \ingroup MDL_MANALIB_GLOBAL
 * \param[in]	malloc_func		メモリ確保関数
 * \param[in]	free_func		メモリ解放関数
 * \param[in]	obj				ユーザオブジェクト（任意）
 * \par 呼び出し条件:
 * ::criMana_Initialize 関数の前に呼び出してください。<br>
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * CRI Manaライブラリにメモリアロケータ（メモリの確保／解放関数）を登録します。<br>
 * CRI Manaライブラリがライブラリ内で行なうメモリ解放処理を、
 * ユーザー独自のメモリ解放処理に置き換えたい場合に使用します。
 * \par 備考:
 * 登録したメモリアロケータは複数スレッドから呼び出される場合があります。
 * マルチスレッドセーフなアロケーターを登録してください。<br>
 * 
 * \sa CriManaMallocFunc, CriManaFreeFunc
 */
void CRIAPI criMana_SetUserAllocator(CriManaMallocFunc malloc_func, CriManaFreeFunc free_func, void *obj);

/* criMana_CalculateLibWorkSize */
/*JP
 * \brief ライブラリ初期化用ワーク領域サイズの計算
 * \ingroup MDL_MANALIB_GLOBAL
 * \param[in]	config	初期化用コンフィグ構造体
 * \return		CriSint32 ライブラリワーク領域サイズ
 * \par 呼び出し条件:
 * スレッドセーフです。
 * \par 説明:
 * Manaライブラリを使用するために必要なワーク領域のサイズを取得します。<br>
 * ライブラリが必要とするワーク領域のサイズは、ライブラリの初期化コンフィグ構造体の
 * 内容によって変化します。<br>
 * 引数 config の情報は、関数内でのみ参照されます。関数実行後に config の領域を解放
 * しても問題ありません。
 * \par 備考:
 * 各機種固有の関数が存在する場合があります。詳細は機種別のリファレンスを参照してください。
 */
CriSint32 CRIAPI criMana_CalculateLibWorkSize(const CriManaLibConfig *config);

/* criMana_Initialize */
/*JP
 * \brief ライブラリの初期化
 * \ingroup MDL_MANALIB_GLOBAL
 * \param[in]	config	初期化用コンフィグ構造体
 * \param[in]	work	アプリ側で確保済みのワーク領域のアドレス (内部でアライン調整する)
 * \param[in]	work_size	ワーク領域のサイズ (=criMana_CalculateLibWorkSizeの戻り値)
 * \par 呼び出し条件:
 * AtomExライブラリの初期化後、かつ 全てのCRI Mana APIの前に呼んでください。<br>
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * Manaライブラリの初期化をします。<br>
 * ライブラリの機能を利用するには、必ずこの関数を実行する必要があります。<br>
 * <br>
 * ライブラリを初期化する際には、ライブラリが内部で利用するためのメモリ領域（ワーク領域）
 * を確保する必要があります。<br>
 * <br>
 * 本関数で、最大同時に使用するハンドル資源数やスレッドモデルを指定します。
 * 途中でハンドル資源数を調整したい場合は、一度再生を全て停止して及び ::criMana_Finalize 関数で
 * 終了してから、再度本関数を呼び出してください。
 * <br>
 * workにNULL、work_sizeに0を指定した場合は、::criMana_SetUserAllocator 関数で登録された
 * メモリ確保関数を使用して内部でメモリ確保を行います。
 * <br>
 * \par 備考:
 * ライブラリが必要とするワーク領域のサイズは、初期化用コンフィグ構造体の内容に応じて
 * 変化します。<br>
 * また、必要なワーク領域のサイズは、プラットフォームによっても異なります。
 * <br>
 * 引数 config の情報は、関数内でのみ参照されます。<br>
 * 関数を抜けた後は参照されませんので、関数実行後に config の領域を解放しても
 * 問題ありません。
 * \par 備考:
 * 各機種固有の関数が存在する場合があります。詳細は機種別のリファレンスを参照してください。
 *
 * \sa criMana_Finalize
 */
void CRIAPI criMana_Initialize(const CriManaLibConfig* config, void* work, CriSint32 work_size);

/* criMana_InitializeFrameARGB32 */
/*JP
 * \brief フレーム変換処理の初期化 (32bit ARGB8888形式)
 * \ingroup MDL_MANALIB_GLOBAL
 * \par 呼び出し条件:
 * CRI Manaライブラリの初期化後に本関数を実行してください。
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * 32bit ARGB8888 フォーマットで、フレームを取得するために必要な初期化関数です。<br>
 * 上記フォーマットでフレームを取得する場合は、取得関数の前に必ず本関数を呼び出してください。<br>
 * <br>
 * \par 備考
 * 本関数を呼び出すと、内部のフレーム色変換に必要な静的メモリ領域(数百KB)がリンクされます。
 * そのため、上記フォーマットを使用せず他のフォーマットでフレームを取得する
 * アプリケーションは本関数を使用しないでください。
 *
 * \sa criManaPlayer_CopyFrameToBufferARGB32
 */
void CRIAPI criMana_InitializeFrameARGB32(void);

/* criMana_Finalize */
/*JP
 * \brief ライブラリの終了処理
 * \ingroup MDL_MANALIB_GLOBAL
 * \par 呼び出し条件:
 * 全てのハンドルが CRIMANAPLAYER_STATUS_STOP or CRIMANAPLAYER_STATUS_PLAYEND 状態であること。 <br>
 * 本関数の後にAtomExライブラリ終了関数を呼んでください。<br>
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * Manaライブラリの終了処理を行います。<br>
 * \par 備考:
 * 各機種固有の関数が存在する場合があります。詳細は機種別のリファレンスを参照してください。
 *
 * \sa criMana_Initialize
 */
void CRIAPI criMana_Finalize(void);

/* criMana_IsInitialized */
/*JP
* \brief ライブラリ初期化状態の取得
* \ingroup MDL_MANALIB_GLOBAL
* \return	CriBool		初期化中かどうか
* \retval	CRI_FALSE	未初期化状態
* \retval	CRI_TRUE	初期化済み
* \par 説明:
* CRI Manaライブラリが既に初期化されているかどうかをチェックします。<br>
* \sa criMana_Initialize, criMana_Finalize
*/
CriBool CRIAPI criMana_IsInitialized(void);

/* criMana_ExecuteMain */
/*JP
 * \brief サーバー処理の実行
 * \ingroup MDL_MANALIB_GLOBAL
 * \par 呼び出し条件:
 * ライブラリが初期化済みであること。<br>
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * Manaライブラリの内部状態を更新します。<br>
 * アプリケーションは、この関数を定期的に実行する必要があります。<br>
 * <br>
 * ハンドルごとに内部状態の更新を管理したい場合は、
 * ::criManaPlayer_ExecuteMain 関数を使用してください。<br>
 * <br>
 * \attention
 * 本関数はCRI Atomライブラリの criAtomEx_ExecuteMain とは違い、CRIMANA_THREAD_MODEL_MULTI
 * を設定しても Manaライブラリ内部で定期的に自動実行を行いません。したがって本関数の呼び出し
 * が滞るとムービー再生ががたつく場合があります。
 */
void CRIAPI criMana_ExecuteMain(void);

/* criMana_SyncMasterTimer */
/*JP
 * \brief マスタータイマーへの同期
 * \ingroup MDL_MANALIB_GLOBAL
 * \par 呼び出し条件:
 * ライブラリが初期化済みであること。<br>
 * <br>
 * スレッドセーフではありません。別スレッドで実行するとGetTimeが返す時刻が不正になる場合があります。
 * \par 説明:
 * 全てのManaプレーヤハンドルの再生時刻を、マスタータイマーに同期させます。<br>
 * ハンドルごとにマスタータイマーの時刻同期を管理したい場合は ::criManaPlayer_SyncMasterTimer 関数を
 * 使用してください。
 * <br>
 * \attention
 * 本関数は、最低でもビデオのフレームレートよりも細かい周期で呼び出す必要があります。
 */
void CRIAPI criMana_SyncMasterTimer(void);

/* criMana_AdvanceManualTimer */
/*JP
 * \brief マニュアルタイマーの時刻を進める
 * \ingroup MDL_MANALIB_GLOBAL
 * \par 呼び出し条件:
 * ライブラリが初期化済みであること。<br>
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * 全てのManaプレーヤハンドルのマニュアルタイマーの時刻を進めます。
 * 機能説明は個別ハンドル用のcriManaPlayer_AdvanceManualTimer関数を
 * ご参照下さい。
 * 
 * \sa criManaPlayer_AdvanceManualTimer
 */
void CRIAPI criMana_AdvanceManualTimer(void);

/* criMana_ExecuteVideoProcess */
/*JP
 * \brief ビデオのデコード（全ハンドル一括）
 * \ingroup MDL_MANALIB_GLOBAL
 * \par 呼び出し条件:
 * ライブラリが初期化済みであること。<br>
 * <br>
 * スレッドセーフです。
 * \par 説明:
 * 作成済みの全てのManaプレーヤハンドルに対し、::criManaPlayer_ExecuteVideoProcess 関数を
 * 一度ずつ実行します。
 * <br>
 * CRIMANA_THREAD_MODEL_MULTIを ::criMana_Initialize 関数で設定した場合、本関数は
 * Manaライブラリ内部で作られる別スレッド上で自動的に実行されます。そのため、
 * アプリケーションから呼び出す必要はありません。なお、優先度はメインスレッドより
 * 低くなるように設定されています。
 */
void CRIAPI criMana_ExecuteVideoProcess(void);

/* criMana_SetDecodeSkipFlag */
/*JP
 * \brief デコードスキップフラグの設定（全ハンドル一括）
 * \ingroup MDL_MANALIB_GLOBAL
 * \par 呼び出し条件:
 * スレッドセーフです。
 * \par 説明:
 * デコードスキップ機能を有効にします。<br>
 * 引数 flag がCRI_TRUE(1)なら、スキップ有効。引数 flag がCRI_FALSE(0)ならスキップ無効です。<br>
 * <br>
 * デコードスキップを有効にすると、再生負荷による処理落ちが発生した場合に内部で自動的に
 * 必要な枚数分フレームをスキップします。見た目はガタつきますが、音飛びの発生を防ぎやすくなります。<br>
 * <br>
 * デフォルトでスキップが有効かどうかは機種に依存します。デバイス性能が一意に決まる機種
 * ではデフォルト無効、一意に定まらない機種はデフォルト有効になります。
 * \par 注意:
 * ・Sofdec.Primeのみ有効。H.264/VP9はデコードスキップには未対応です。<br>
 */
void CRIAPI criMana_SetDecodeSkipFlag(CriBool flag);

/* criMana_SetAllPauseFlag */
/*JP
 * \brief 全体ポーズの設定（全ハンドル一括）
 * \ingroup MDL_MANALIB_GLOBAL
 * \param[in]	flag	全体ポーズフラグ（CRI_TRUE：全体ポーズ有効、CRI_FALSE：全体ポーズ無効）
 * \par 呼び出し条件:
 * ライブラリが初期化済みであること。<br>
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * 全体ポーズ機能を有効にします。<br>
 * 引数 flag がCRI_TRUE(1)なら、全体ポーズ有効。引数 flag がCRI_FALSE(0)なら全体ポーズ無効です。<br>
 * <br>
 * 全体ポーズを無効から有効にすると、全てのハンドルについてポーズを行います。<br>
 * （アプリ側から見ると全てのムービーが一時停止になっている状態になります）
 * ただし、ユーザーがcriManaPlayer_Pause関数でハンドル個別に指定する
 * ポーズスイッチ（ユーザポーズフラグ）の有効/無効の情報は内部で保持しています。
 * （全体ポーズ有効中でもcriManaPlayer_Pause関数に
 * よるユーザポーズフラグの更新は行います）<br>
 * また、criManaPlayer_IsPaused関数はユーザポーズフラグの情報を返しますので、
 * 全体ポーズの有効/無効の情報とは別物になります。
 * <br>
 * 全体ポーズを有効から無効にした場合、ユーザポーズフラグが有効なハンドルについては
 * ポーズが継続したままになり、ユーザポーズフラグが無効なハンドルについては
 * ムービーが再生し始めます。
 *
 * \sa criMana_GetAllPauseFlag, criManaPlayer_Pause, criManaPlayer_IsPaused
 */
void criMana_SetAllPauseFlag(CriBool flag);

/* criMana_GetAllPauseFlag */
/*JP
 * \brief 全体ポーズフラグの取得
 * \ingroup MDL_MANALIB_GLOBAL
 * \return	CriBool 全体ポーズフラグ
 * \par 呼び出し条件:
 * スレッドセーフではありません。
 * \par 説明:
 * 全体ポーズフラグを取得します。<br>
 * <br>
 * criManaPlayer_IsPaused関数で取得できるユーザポーズフラグとは別物なのでご注意下さい。
 *
 * \sa criMana_SetAllPauseFlag
 */
CriBool criMana_GetAllPauseFlag(void);

/* criMana_Lock */
/*JP
 * \brief デコード処理の割り込みを防止
 * \ingroup MDL_MANALIB_GLOBAL
 * \par 説明:
 * デコード処理の割り込み抑止します。<br>
 * 本関数実行後、::criMana_Unlock関数実行までの間、デコード処理の動作が抑止されます。<br>
 * \sa criMana_Unlock
 */
void criMana_Lock(void);

/* criMana_Unlock */
/*JP
 * \brief デコード処理の割り込み防止を解除
 * \ingroup MDL_MANALIB_GLOBAL
 * \par 説明:
 * ::criMana_Lock関数による、デコード処理の割り込み防止を解除します。
 * \sa criMana_Lock
 */
void criMana_Unlock(void);

/* criMana_UseStreamerManager */
/*JP
 * \brief ストリーミング管理モジュールを使用するかどうかの制御フラグをセットする
 * \ingroup MDL_MANALIB_GLOBAL
 * \param[in]	flag	制御フラグ （CRI_TRUE：使用する、CRI_FALSE：使用しない）
 * \par 呼び出し条件:
 * ライブラリの初期化後も呼び出し可能。ただし、動画再生していない状態で呼び出してください。
 * \par 説明:
 * ADX2と共通のストリーミング管理モジュールを使用するかどうかのフラグを指定します。<br>
 * デフォルトは機種により異なります。iOSやAndroidなどのシーク負荷が無視できる機種についてはデフォルトとして使用しません。
 * \attention
 * 本関数追加前のライブラリについては全機種使用します。従来挙動に戻したい場合は本関数でCRI_TRUEを指定してください。
 * \sa criMana_IsStreamerManagerUsed
 */
void CRIAPI criMana_UseStreamerManager(CriBool flag);

/* criMana_IsStreamerManagerUsed */
/*JP
 * \brief ストリーミング管理モジュールを使用するかどうかの制御フラグの取得
 * \ingroup MDL_MANALIB_GLOBAL
 * \return	制御フラグ （CRI_TRUE：使用する、CRI_FALSE：使用しない）
 * \par 呼び出し条件:
 * スレッドセーフです。 
 * \par 説明:
 * ストリーミング管理モジュールを使用するかどうかの制御フラグの取得します。
 * \sa criMana_UseStreamerManager
 */
CriBool CRIAPI criMana_IsStreamerManagerUsed(void);

/*==========================================================================
 *      CRI Mana Player API
 *=========================================================================*/
/* criManaPlayer_CalculateHandleWorkSize */
/*JP
 * \brief ManaPlayer用ワーク領域サイズの計算（コンフィグ指定なし）
 * \ingroup MDL_MANALIB_PLAYER
 * \return	CriSint32 ハンドルワークサイズ
 * \par 呼び出し条件:
 * スレッドセーフです。
 * \par 説明:
 * Manaプレーヤーを作成するために必要な、ワーク領域のサイズを取得します。<br>
 * Manaプレーヤーの作成に必要なワークメモリサイズは機種ごとに異なります。
 *
 * \sa criManaPlayer_Create
 */
CriSint32 CRIAPI criManaPlayer_CalculateHandleWorkSize(void);

/* criManaPlayer_CalculateHandleWorkSizeWithConfig */
/*JP
 * \brief ManaPlayer用ワーク領域サイズの計算（コンフィグ指定つき）
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	config	Manaプレーヤハンドル作成用コンフィグ構造体へのポインタ
 * \return	CriSint32 ハンドルワークサイズ
 * \par 呼び出し条件:
 * スレッドセーフです。
 * \par 説明:
 * Manaプレーヤーを作成するために必要なワーク領域のサイズを取得します。<br>
 * ::criManaPlayer_CalculateHandleWorkSize とは異なり、ハンドル作成用のパラメータに
 * 基づいてワークサイズを計算します。<br>
 * 本関数で計算したワークサイズ以上のメモリを確保し、criManaPlayer_CreateWithConfigに
 * 指定してください。<br>
 * \par 備考:
 * 同一のパラメタの場合でも、Manaプレーヤーの作成に必要なワークメモリサイズは機種ごとに異なります。
 *
 * \sa criManaPlayer_CreateWithConfig, criManaPlayer_SetDefaultHandleConfig
 */
CriSint32 CRIAPI criManaPlayer_CalculateHandleWorkSizeWithConfig(const CriManaPlayerConfig *config);

/* criManaPlayer_Create */
/*JP
 * \brief Manaプレーヤーの作成（コンフィグ指定なし）
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	work	ワーク領域のアドレス （内部でアライン調整）
 * \param[in]	work_size	ワーク領域のサイズ
 * \return	CriManaPlayerHn Manaプレーヤハンドル
 * \par 呼び出し条件:
 * ライブラリが初期化済みであること。<br>
 * <br>
 * スレッドセーフではありません。<br>
 * \par 説明:
 * Manaプレーヤーを作成します。<br>
 * <br>
 * プレーヤーを作成する際には、ライブラリが内部で利用するためのメモリ領域（ワーク領域）
 * を確保する必要があります。<br>
 * アプリケーション側で明示的にメモリを確保する場合は、::criManaPlayer_CalculateHandleWorkSize
 * 関数で必要なワークサイズを確認し、確保したワークメモリを引数に渡してください。<br>
 * workにNULL、work_sizeに0を指定した場合は、 ::criMana_SetUserAllocator 関数で登録された
 * メモリ確保関数を使用して内部でメモリ確保を行います。
 * \par 注意:
 * ムービーデコーダーのハンドルリソースとは別に
 * ストリーミングを行うためのデータ読み込み用のローダーハンドル(CriFsLoader)、
 * ファイルオープン用のリソースを消費します。<br>
 * 動画・音声のストリーミング再生本数が多い場合（デフォルト：16本を超える場合）、
 * AtomEx/Manaライブラリとは別途、先にFile Systemの初期化コンフィグのパラメタ
 *（ローダーハンドル数、ファイルオープン数の上限値）を設定し、初期化する必要があります。
 * 
 * \sa criManaPlayer_CalculateHandleWorkSize, criManaPlayer_Destroy
 */
CriManaPlayerHn CRIAPI criManaPlayer_Create(void *work, CriSint32 work_size);

/* criManaPlayer_CreateWithConfig */
/*JP
 * \brief Manaプレーヤーの作成（コンフィグ指定つき）
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	config	Manaプレーヤハンドル作成用コンフィグ構造体へのポインタ
 * \param[in]	work	ワーク領域のアドレス （内部でアライン調整）
 * \param[in]	work_size	ワーク領域のサイズ
 * \return	CriManaPlayerHn Manaプレーヤハンドル
 * \par 呼び出し条件:
 * ライブラリが初期化済みであること。<br>
 * <br>
 * スレッドセーフではありません。<br>
 * \par 説明:
 * ハンドル作成パラメタを指定して、Manaプレーヤーを作成します。<br>
 * 作成されたプレーヤーは、パラメタに応じて内部リソースを必要なだけ確保します。<br>
 * <br>
 * プレーヤーを作成する際には、ライブラリが内部で利用するためのメモリ領域（ワーク領域）
 * を確保する必要があります。<br>
 * アプリケーション側で明示的にメモリを確保する場合は、::criManaPlayer_CalculateHandleWorkSizeWithConfig
 * 関数で必要なワークサイズを確認し、確保したワークメモリを引数に渡してください。<br>
 * workにNULL、work_sizeに0を指定した場合は、 ::criMana_SetUserAllocator 関数で登録された
 * メモリ確保関数を使用して内部でメモリ確保を行います。<br>
 * <br>
 * \attention
 * ハンドル作成パラメタを調整してManaプレーヤハンドルを作成した場合、そのハンドルを使用して
 * 再生できるムービーは、ハンドル作成パラメタの条件を満たすものに限定されます。<br>
 * その代り、再生開始遅延を少なく再生できるなどのメリットを享受できます。<br>
 * 
 * \sa criManaPlayer_CalculateHandleWorkSizeWithConfig, criManaPlayer_Destroy
 */
CriManaPlayerHn CRIAPI criManaPlayer_CreateWithConfig(const CriManaPlayerConfig *config, void *work, CriSint32 work_size);

/* criManaPlayer_Destroy */
/*JP
 * \brief Manaプレーヤーの破棄
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \par 呼び出し条件:
 * プレーヤステータスが、CRIMANAPLAYER_STSTUS_STOP もしくは CRIMANAPLAYER_STSTUS_PLAYENDの時。<br>
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * Manaプレーヤーを破棄します。<br>
 * 本関数を実行した時点で、AtomExプレーヤー作成時に確保されたリソースが全て解放されます。<br>
 * また、引数に指定したManaプレーヤハンドルも無効になります。<br>
 * ::criManaPlayer_Create 関数で設定したワークメモリは、本関数の実行後に解放してください。
 * アプリケーション側でワークメモリを渡さず、ライブラリ側でメモリ確保を行った場合は、本関数で
 * 内部でメモリの解放を行ないます。
 * 
 * \sa criManaPlayer_Create
 */
void CRIAPI criManaPlayer_Destroy(CriManaPlayerHn player);

/* criManaPlayer_SetMetaDataWorkAllocator */
/*JP
 * \brief メタデータ用アロケーターの登録
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	allocfunc	メタデータワーク確保コールバック関数
 * \param[in]	freefunc	メタデータワーク解放コールバック関数
 * \param[in]	obj			ユーザオブジェクト（任意）
 * \param[in]	meta_flag	使用メタフラグ（未対応）
 * \par 呼び出し条件:
 * ヘッダの解析処理前に本関数を呼び出してください。<br>
 * <br>
 * スレッドセーフではありません。<br>
 * \par 説明:
 * シークやキューポイントの機能に必要なメタデータを確保するためのメモリアロケータ
 * （メモリの確保／解放関数）の設定を行います。
 * <br>
 * 本関数でメモリアロケータを登録した場合、ヘッダ解析時にライブラリ内部で
 * 登録されたメタデータワーク用のアロケーターを使用してメタデータワークを確保します。<br>
 * <br>
 * 逆に、本関数を実行しない場合は自動的にメタデータは破棄されます。この場合はシークや
 * キューポイントの機能が使用できないので注意してください。
 * <br>
 * \par 備考:
 * 現在、meta_flagによるメタデータの種別ごとに設定は利用できません。CRIMANA_META_FLAG_ALLを使用して下さい。<br>
 * <br>
 * また、ハンドルを再利用してムービーを再生する場合には、アロケーターの再登録は必要ありません。
 * \par 注意:
 * 登録したメモリアロケータは複数スレッドから呼び出される場合があります。
 * マルチスレッドセーフなアロケーターを登録してください。<br>
 *
 * \sa CriManaMetaMallocFunc, CriManaMetaFreeFunc
 */
void CRIAPI criManaPlayer_SetMetaDataWorkAllocator(
	CriManaPlayerHn player, CriManaMetaMallocFunc allocfunc, CriManaMetaFreeFunc freefunc, void *obj, CriManaMetaFlag meta_flag);

/* criManaPlayer_GetPlaybackWorkParam */
/*JP
 * \brief 再生ワークパラメタ構造体の取得
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[out]	config_basic	基本再生ワーク用パラメタのポインタ（必須）
 * \param[out]	config_ex	拡張再生ワーク用パラメタのポインタ（追加）NULL指定可
 * \return	CriBool ワークパラメタが取得できたかどうか<br>
 * ヘッダ解析前の状態だとCRI_FALSEを返します。
 * \par 呼び出し条件:
 * ヘッダ解析が完了し、CRIMANAPLAYER_STATUS_WAIT_PREPの時に呼び出してください。<br>
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * ムービーファイルのヘッダ情報を元にした、ムービー再生用のデフォルト再生パラメタを取得します。<br>
 * <br>
 * ヘッダ解析が完了していない状態で、本関数を呼び出した場合は、config_basic や config_ex の内容をクリアします。<br>
 * <br>
 * サブオーディオ再生やアルファムービー再生を行わない場合は、config_ex をNULLで省略することが出来ます。
 * \par 注意:
 * サブオーディオ再生を行う場合、本APIの呼び出しよりも前にオーディオトラックの設定を行ってください。<br>
 * 本APIの呼び出しよりも後にオーディオトラックの設定を行うと、オーディオの再生ワークサイズが正しく計算できず、
 * 再生ワーク領域の設定に失敗する場合があります。
 */
CriBool CRIAPI criManaPlayer_GetPlaybackWorkParam(
	CriManaPlayerHn player, CriManaPlaybackBasicWorkConfig *config_basic, CriManaPlaybackExWorkConfig *config_ex);

/* criManaPlayer_CalculatePlaybackWorkSize */
/*JP
 * \brief 再生用ワーク領域のサイズ計算
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	config_basic	基本再生ワーク用パラメタのポインタ（必須）
 * \param[in]	config_ex		拡張再生ワーク用パラメタのポインタ（追加）NULL指定可
 * \return	CriSint32 再生ワークサイズ
 * \par 呼び出し条件:
 * スレッドセーフです。
 * \par 説明:
 * 指定されたムービーファイルの再生に必要なワークメモリのサイズを計算します。<br>
 * config_ex が NULLで渡された場合は、拡張再生用のワークメモリを除外したサイズを返します。
 */
CriSint32 CRIAPI criManaPlayer_CalculatePlaybackWorkSize(
	CriManaPlayerHn player, const CriManaPlaybackBasicWorkConfig *config_basic, const CriManaPlaybackExWorkConfig *config_ex);

/* criManaPlayer_SetPlaybackWork */
/*JP
 * \brief 再生ワーク領域の設定
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	config_basic	基本再生ワーク用パラメタのポインタ（必須）
 * \param[in]	config_ex		拡張再生ワーク用パラメタのポインタ（追加）NULL指定可
 * \param[in]	work			アプリが確保したワーク領域（内部自動アライン調整）
 * \param[in]	work_size		ワークサイズ
 * \par 呼び出し条件:
 * CRIMANAPLAYER_STATUS_WAIT_PREP か、それよりも前の状態で呼び出してください。<br>
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * 指定されたムービーファイルの再生に必要なワークメモリ設定します。<br>
 * <br>
 * アプリケーション側で明示的にメモリを確保する場合は、::criManaPlayer_CalculatePlaybackWorkSize
 * 関数で必要なワークサイズを確認し、確保したワークメモリを引数に渡してください。<br>
 * workにNULL、work_sizeに0を指定した場合は、::criMana_SetUserAllocator 関数で登録された
 * メモリ確保関数を使用して内部でメモリ確保を行います。
 * <br>
 * 本関数を実行せずに再生を開始した場合、内部で自動的にデフォルトの再生パラメタを使用して再生を
 * 行います。この場合、::criMana_SetUserAllocator 関数でメモリアロケータが登録されていることが条件になります。
 * <br>
 * 設定したワーク領域の解放はプレーヤハンドル破棄後に行ってください。再生終了後もしくは再生停止後
 * にワーク領域を解放する場合、 ::criManaPlayer_FreePlaybackWork 関数を呼び出してからメモリの解放を
 * 行ってください。<br>
 * 
 * \sa criManaPlayer_CalculatePlaybackWorkSize
 */
void CRIAPI criManaPlayer_SetPlaybackWork(
	CriManaPlayerHn player, const CriManaPlaybackBasicWorkConfig *config_basic, const CriManaPlaybackExWorkConfig *config_ex, 
	void *work, CriSint32 work_size);

/* criManaPlayer_FreePlaybackWork */
/*JP
 * \brief 再生用ワークの解放
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \par 呼び出し条件:
 * MANAPLAYER_STATUS_STOP や CRIMANAPLAYER_STATUS_PLAYEND状態で本関数を実行してください。<br>
 * 再生中に本関数を呼び出すとエラーになります。
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * プレーヤハンドル内部で確保/保持している再生用ワークメモリおよび内部リソースを解放します。<br>
 * <br>
 * ユーザアロケータを登録して再生ワークを確保した場合、本関数内部でアロケーター経由で
 * 再生ワークメモリを解放します。<br>
 * <br>
 * 本関数を実行するとプレーヤハンドルからの再生ワークメモリへのアクセスを行わなくなります。
 * アプリケーションから再生ワークメモリを渡している場合、本関数を呼び出し後にメモリの解放が
 * 可能です。
 */
void CRIAPI criManaPlayer_FreePlaybackWork(CriManaPlayerHn player);

/* criManaPlayer_GetStatus */
/*JP
 * \brief Manaプレーヤーの状態取得
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \return	CriManaPlayerStatus Manaプレーヤーのハンドル状態
 * \par 呼び出し条件:
 * スレッドセーフではありません。
 * \par 説明:
 * Manaプレーヤーのステータスを取得します。
 * 
 * \sa CriManaPlayerStatus
 */
CriManaPlayerStatus CRIAPI criManaPlayer_GetStatus(CriManaPlayerHn player);

/* criManaPlayer_ExecuteMain */
/*JP
 * \brief Manaプレーヤーのサーバー処理の実行
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \par 呼び出し条件:
 * スレッドセーフではありません。
 * \par 説明:
 * Manaプレーヤハンドルの内部状態を更新します。<br>
 * <br>
 * アプリケーションで ::criMana_ExecuteMain 関数を呼び出している場合は、内部でこの関数が自動的に呼び出されるので、
 * 本関数を実行する必要はありません。
 */
void CRIAPI criManaPlayer_ExecuteMain(CriManaPlayerHn player);

/* criManaPlayer_SyncMasterTimer */
/*JP
 * \brief マスタータイマーへの時刻同期
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \par 呼び出し条件:
 * スレッドセーフではありません。
 * \par 説明:
 * Manaプレーヤハンドルの時刻を、マスタータイマーに同期させます。<br>
 * この関数は、最低でもビデオのフレームレートよりも細かい周期で呼び出す必要が
 * あります。
 * <br>
 * アプリケーションで ::criMana_SyncMasterTimer 関数を呼び出している場合は、内部でこの関数が自動的に呼び出されるので、
 * 本関数を実行する必要はありません。
 */
void CRIAPI criManaPlayer_SyncMasterTimer(CriManaPlayerHn player);

/* criManaPlayer_SetMasterTimerType */
/*JP
 * \brief マスタータイマー種別の設定
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	type    マスタータイマー種別
 * \par 呼び出し条件:
 * スレッドセーフです。
 * \par 説明:
 * ビデオフレームの時刻管理に使用するタイマ種別を指定します。<br>
 * デフォルトはハンドル作成時に指定するシステムタイマです。<br>
 * ビデオフレームの表示タイミングをオーディオの時刻と同期させたいときはオーディオタイマを指定してください。<br>
 * オーディオタイマを指定した場合でも、再生するムービーにオーディオが含まれていない場合はシステムタイマ同期となります。<br>
 * ビデオフレームの表示タイミングをムービーのオーディオトラック以外の任意のタイマと同期させたい場合は、ユーザータイマーを指定してください。<br>
 * ユーザータイマーを指定した場合はムービーのオーディオが含まれているか否かに関わらず、ユーザータイマーを基準に同期します。
 * <br>
 * \par 備考: 
 * オーディオタイマまたはユーザータイマーを指定した場合、マスタータイマーはオーディオタイマまたはユーザータイマーを基準に内部で時刻計算を行い、時刻を更新します。<br>
 * すなわちオーディオタイマまたはユーザータイマーが時刻を進めていない区間でも、ムービーがポーズ中である場合を除き、
 * ::criManaPlayer_GetTimeでは呼び出す度に更新された時刻を返します。
 * 
 * \sa criMana_SyncMasterTimer, criManaPlayer_SyncMasterTimer
 */
void  CRIAPI criManaPlayer_SetMasterTimerType(CriManaPlayerHn player, CriManaTimerType type);

/* criManaPlayer_GetMasterTimerType */
/*JP
 * \brief マスタータイマーの種別取得
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \return	CriManaTimerType カレントマスタタイマ種別
 * \par 呼び出し条件:
 * スレッドセーフです。
 * \par 説明:
 * 現在設定されているマスタータイマーの種別を取得します。
 */
CriManaTimerType CRIAPI criManaPlayer_GetMasterTimerType(CriManaPlayerHn player);

/* criManaPlayer_SetUserTime */
/*JP
 * \brief ユーザータイマーの時刻をマスタータイマーの時刻に設定
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \param[in]	user_count	ユーザータイマーのカウント
 * \param[in]	user_unit	ユーザータイマー1秒あたりのカウント数。 user_count / user_unit で秒単位の時刻になります。
 * \par 呼び出し条件:
 * スレッドセーフではありません。
 * \par 説明:
 * ユーザータイマーの時刻をマスタータイマーの時刻に設定します。<br>
 * ユーザータイマーでムービー再生を進めるためには定期的に本関数を呼び出し、
 * その後 ::criMana_SyncMasterTimer や ::criManaPlayer_SyncMasterTimer を呼び出してください。<br>
 * ムービーをポーズする際は同時にユーザータイマーを一時停止するか、本関数を呼ばないようにしてください。
 * <br>
 * \par 備考:
 * ユーザータイマーを早送り又はスローで進めるとマスタータイマーの精度が悪くなり、正しい時刻でフレームを取得しにくくなるため
 * 通常は等倍速再生で本関数をご使用ください。
 * 
 * \sa criMana_SyncMasterTimer, criManaPlayer_SyncMasterTimer, criManaPlayer_SetMasterTimerType
 */
void CRIAPI criManaPlayer_SetUserTime(CriManaPlayerHn player, CriUint64 user_count, CriUint64 user_unit);

/* criManaPlayer_SetManualTimerUnit */
/*JP
 * \brief マニュアルタイマーの時刻が進む単位の設定（有理数形式）
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	timer_manual_unit_n	時刻が進む単位の分子。
 * \param[in]	timer_manual_unit_d	時刻が進む単位の分母。
 * \par 呼び出し条件:
 * スレッドセーフではありません。
 * \par 説明:
 * マニュアルタイマーの時刻が進む単位を有理数形式で設定します。
 * 「分子(timer_manual_unit_n) / 分母(timer_manual_unit_d) = 時刻が進む単位（秒）」になります。
 * 以下の設定例を参考にして下さい。
  <br>
  \table "設定例" align=center width=90% border=1 cellspacing=0 cellpadding=4
  {メインループ（アプリ更新頻度）     |分子（timer_manual_unit_n） |分母（timer_manual_unit_d） |時刻が進む単位               }
  [60    fps                          |1                           |60                          |1    / 60    （≒ 16.6 msec）]
  [59.94 fps                          |1001                        |60000                       |1001 / 60000 （≒ 16.7 msec）]
  [30    fps                          |1                           |30                          |1    / 30    （≒ 33.3 msec）]
  [29.97 fps                          |1001                        |30000                       |1001 / 30000 （≒ 33.4 msec）]
  \endtable
 * 
 * 本関数で設定した値とcriMana_AdvanceManualTimer関数、または
 * criManaPlayer_AdvanceManualTimer関数で進めたカウント値から
 * Manaプレーヤー内部の時刻を計算します。
 * 
 * \sa criMana_AdvanceManualTimer, criManaPlayer_AdvanceManualTimer
 */
void CRIAPI criManaPlayer_SetManualTimerUnit(CriManaPlayerHn player, CriUint64 timer_manual_unit_n, CriUint64 timer_manual_unit_d);

/* criManaPlayer_AdvanceManualTimer */
/*JP
 * \brief マニュアルタイマーの時刻（カウント値）を進める
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \par 呼び出し条件:
 * スレッドセーフではありません。
 * \par 説明:
 * マニュアルタイマーで個別のハンドルについて時刻（カウント値）を進めます。
 * マニュアルタイマーでムービー再生を進めるためにはアプリケーションで定期的に本関数を呼び出す必要があります。
 * ・プレーヤステータスがPLAYING（再生中）の時に時刻が進みます。
 * ・ポーズ中は本関数を呼び出しても時刻は進みません。
 * ・再生開始、再生停止時に時刻（カウント値）は0にリセットされます。
 * 
 * \sa criManaPlayer_SetManualTimerUnit
 */
void CRIAPI criManaPlayer_AdvanceManualTimer(CriManaPlayerHn player);

/* criManaPlayer_ExecuteVideoProcess */
/*JP
 * \brief ビデオのデコード処理
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \return	CriBool ムービー再生中はCRI_TRUE、再生終了または停止後はCRI_FALSE
 * \par 呼び出し条件:
 * スレッドセーフです。
 * \par 説明:
 * Manaプレーヤハンドルに対して、一ピクチャ分のデコードを行います。<br>
 * ピクチャデコードは負荷の高い処理なので、アプリケーションのメインスレッドから呼び出すと処理落ちが発生する可能性があります。<br>
 * その場合は、マルチスレッドフレームワークに設定するか、あるいは本関数を優先度の低い別スレッドから呼び出してください。<br>
 * <br>
 * ::criMana_ExecuteVideoProcess 関数を呼び出している場合は、本関数は内部で自動的に呼び出されるため、
 * 本関数を使用する必要はありません。また、マルチスレッドフレームワーク動作時の場合、本関数を呼び出しても何も
 * 処理を行ないません。
 * <br>
 * 本関数の返り値は、ムービー再生の実行中かどうかを表しています。<br>
 * アプリケーション側で別スレッドでこの関数を呼び出す場合は、必ずCRI_FALSEになるまでこの関数呼びつづけて下さい。<br>
 * 返り値がCRI_TRUEの間にデコードスレッドを終了してしまうと、ハンドルの状態が CRIMANAPLAYER_STATUS_STOP や
 * CRIMANAPLAYER_STATUS_PLAYEND に遷移できず、ハンドル破棄が出来なくなります。<br> 
 */
CriBool CRIAPI criManaPlayer_ExecuteVideoProcess(CriManaPlayerHn player); 

/* criManaPlayer_SetFile */
/*JP
 * \brief ムービーファイルの設定
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	bndrhn	CPKファイルをバインド済みのバインダハンドル
 * \param[in]	path	CPKファイル内のコンテンツパス
 * \par 呼び出し条件:
 * CRIMANAPLAYER_STATUS_STOP や CRIMANAPLAYER_STATUS_PLAYEND状態で本関数を実行してください。<br>
 * または、データ要求コールバックの内部で呼び出すことができます。<br> 
 * スレッドセーフではありません。<br>
 * \par 説明:
 * ストリーミング再生用のファイルパスを設定します。<br>
 * 第二引数の bndhn にCPKファイルをバインドしたバインダハンドルを指定することにより、CPKファイルからのムービー再生が行なえます。
 * CPKからではなく直接ファイルからストリーミングする場合は、bndhn にNULLを指定ください。<br>
 * <br>
 * 本関数を繰り返し呼び出した場合は、ファイルパス情報は上書きされます。<br>
 * ::criManaPlayer_SetData 関数を呼び出した場合は、本関数で指定したメモリ情報はハンドル内から消去されます。<br>
 * <br>
 * 同じハンドルで同じムービーデータを繰り返し再生する場合は、本関数の呼び出しは省略することができます。<br>
 */
void CRIAPI criManaPlayer_SetFile(
	CriManaPlayerHn player, CriFsBinderHn bndrhn, const CriChar8 *path);

/* criManaPlayer_SetData */
/*JP
 * \brief メモリ再生用のデータ指定
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \param[in]	dataptr		全ムービーデータをロード済みのバッファアドレス
 * \param[in]	datasize	バッファサイズ
 * \par 呼び出し条件:
 * CRIMANAPLAYER_STATUS_STOP や CRIMANAPLAYER_STATUS_PLAYEND状態で本関数を実行してください。<br>
 * または、データ要求コールバックの内部で呼び出すことができます。<br> 
 * <br>
 * スレッドセーフではありません。<br>
 * \par 説明:
 * メモリ再生用のデータアドレスを設定します。<br>
 * <br>
 * 本関数を繰り返し呼び出した場合は、メモリ情報は上書きされます。<br>
 * ::criManaPlayer_SetFile 関数、 ::criManaPlayer_SetFileRange、もしくは ::criManaPlayer_SetContentId 
 * 関数を呼び出した場合は、本関数で指定したメモリ情報はハンドル内から消去されます。<br>
 * <br>
 * 同じハンドルで同じムービーデータを繰り返し再生する場合は、本関数の呼び出しは省略することができます。<br>
 * <br>
 * 指定されたメモリ領域に実際にアクセスするのは、 ::criManaPlayer_DecodeHeader 関数, ::criManaPlayer_Prepare 関数, 
 * ::criManaPlayer_Start 関数のいずれかが呼び出された時以降です。<br>
 *
 * \par 備考:
 * 指定バッファーがムービーデータサイズよりも大きい場合、バッファー末尾の非ムービーデータ部分は読み飛ばします。
 * バッファアドレスについては必ずムービーデータ先頭を指定してください。
 */
void CRIAPI criManaPlayer_SetData(
	CriManaPlayerHn player, const void *dataptr, CriSint64 datasize);

/* criManaPlayer_SetContentId */
/*JP
 * \brief 再生ムービーファイルの設定 (CPKのコンテンツID指定)
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	bndrhn	CPKをバインド済みのバインダハンドル
 * \param[in]	id		CPKファイル内のコンテンツID
 * \par 呼び出し条件:
 * CRIMANAPLAYER_STATUS_STOP や CRIMANAPLAYER_STATUS_PLAYEND状態で本関数を実行してください。<br>
 * または、データ要求コールバックの内部で呼び出すことができます。<br> 
 * スレッドセーフではありません。<br>
 * \par 説明:
 * ムービーファイルのコンテンツIDを設定します。<br>
 * 第二引数の bndhn にCPKファイルをバインドしたバインダハンドルを指定することにより、CPKファイルからストリーミング再生が行なえます。
 * CPKからではなく単体のムービーファイルからストリーミング再生する場合は、bndhn にNULLを指定ください。<br>
 * <br>
 * 本関数を繰り返し呼び出した場合は、ファイルパス情報は上書きされます。<br>
 * ::criManaPlayer_SetData 関数を呼び出した場合は、本関数で指定したコンテンツID情報はハンドル内から消去されます。<br>
 * <br>
 * 同じハンドルで同じムービーデータを繰り返し再生する場合は、本関数の呼び出しは省略することができます。<br>
 */
void CRIAPI criManaPlayer_SetContentId(
	CriManaPlayerHn player, CriFsBinderHn bndrhn, CriSint32 id);

/* criManaPlayer_SetFileRange */
/*JP
 * \brief パックファイル内のムービーファイルの指定 (ファイル範囲指定)
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	fname	ムービーファイルを含むパックファイルへのパス
 * \param[in]	offset	パックファイル先頭からのムービーへのオフセット
 * \param[in]	range	ムービーデータの範囲 (単位：バイト)
 * \par 呼び出し条件:
 * CRIMANAPLAYER_STATUS_STOP や CRIMANAPLAYER_STATUS_PLAYEND状態で本関数を実行してください。<br>
 * または、データ要求コールバックの内部で呼び出すことができます。<br> 
 * <br>
 * スレッドセーフではありません。<br>
 * \par 説明:
 * ストリーミング再生を行いたいムービーファイルをパッキングしたファイルを指定します。引数にオフセット位置とデータ範囲を指定
 * することで、パックファイル内の任意のムービーデータを読み込み元にすることが出来ます。
 * ファイルパスとして指定できる文字列の最大長は CRIMANA_MAX_FILE_NAME バイトです。<br>
 * <br>
 * 本関数を繰り返し呼び出した場合は、ファイルパス情報は上書きされます。<br>
 * ::criManaPlayer_SetData 関数を呼び出した場合は、本関数で指定したファイル情報はハンドル内から消去されます。<br>
 * <br>
 * 同じハンドルで同じムービーデータを繰り返し再生する場合は、本関数の呼び出しは省略することができます。
 * 
 * \par 備考:
 * この関数の内部ではファイルのオープン要求はしません。ファイルのオープン処理は ::criManaPlayer_DecodeHeader 関数, ::criManaPlayer_Prepare 関数, 
 * ::criManaPlayer_Start 関数 のいずれかの呼出し後、::criManaPlayer_ExecuteMain 関数の中で行われます。
 */
void CRIAPI criManaPlayer_SetFileRange(CriManaPlayerHn player, const CriChar8 *fname, CriUint64 offset, CriSint64 range);

/* criManaPlayer_SetPreviousDataAgain */
/*JP
 * \brief 同一ムービーデータの再セット
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \par 説明:
 * 前回再生したデータを、再度再生するようManaプレーヤーに指示します。<br>
 * \par 例:
 * 以下の処理により、同一音声データを無限にループ再生可能です。<br>
 * \code
 * // データ要求コールバック関数
 * void on_data_request(void *obj, CriManaPlayerHn player)
 * {
 * 	// 前回再生したデータを再セット
 * 	criManaPlayer_SetPreviousDataAgain(player);
 * }
 * 
 * main()
 * {
 * 		:
 * 	// データ要求コールバック関数の登録
 * 	criManaPlayer_SetDataRequestCallback(player, on_data_request, NULL);
 * 	
 * 	// まず最初のムービーデータをセット
 * 	criManaPlayer_SetData(player, buffer, buffer_size);
 * 	// [MEMO] ムービーソースは以下も同様に使用可能
 * 	// criManaPlayer_SetFile(player, NULL, movie_path);
 *  // criManaPlayer_SetFileRange(player, movie_path, offset, range);
 *
 * 	// セットされた音声データを再生
 * 	criManaPlayer_Start(player);
 * 		:
 * }
 * \endcode
 * \attention
 * 本関数はデータ要求コールバック関数内でのみ使用します。<br>
 * （データ要求コールバック関数外でも実行可能ですが、その場合何の効果もありません。）<br>
 *
 * \par 備考:
 * 単純にループ再生をする場合は、::criManaPlayer_Loop関数でも実現可能です。
 *
 * \sa criManaPlayer_SetDataRequestCallback
 */
void CRIAPI criManaPlayer_SetPreviousDataAgain(CriManaPlayerHn player);

/* criManaPlayer_GetMovieInfo */
/*JP
 * \brief ムービーヘッダ情報の取得
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[out]	mvinf	ムービー情報構造体へのアドレス
 * \return	CriBool ムービー情報が取得できたかどうか
 * \par 呼び出し条件:
 * Manaプレーヤーの状態が CRIMANAPLAYER_STATUS_WAIT_PREP 以降の呼び出しで取得できます。<br>
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * ムービー情報 CriManaMovieInfo を取得します。<br>
 * ムービー情報からは主にビットレートや解像度、オーディオ数などがわかります。<br>
 * 再生開始前にムービー情報を知りたい場合は、 ::criManaPlayer_DecodeHeader 関数を呼び出してヘッダ解析を行ってください。<br>
 * <br>
 * 連結再生を行った場合、最後に取得したフレームを含むムービーファイルについての情報を返します。
 * 
 * \sa criManaPlayer_DecodeHeader
 */
CriBool CRIAPI criManaPlayer_GetMovieInfo(CriManaPlayerHn player, CriManaMovieInfo *mvinf);

/* criManaPlayer_DecodeHeader */
/*JP
 * \brief ヘッダのデコード要求
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \par 呼び出し条件:
 * Manaプレーヤーの状態がCRIMANAPLAYER_STATUS_STOP や CRIMANAPLAYER_STATUS_PLAYEND状態で本関数を実行してください。<br>
 * また、本関数の実行の前に再生ファイルや再生データの指定を行ってください。<br>
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * ムービーの再生は開始せず、ヘッダ解析のみ行って待機するための関数です。<br>
 * この関数を使用してヘッダ解析を事前に済ませることにより、再生開始前にムービーの解像度やオーディオの情報を
 * 得ることができます。<br>
 * 本関数を呼び出すと、Manaプレーヤーのハンドル状態はCRIMANAPLAYER_STATUS_STOP → CRIMANAPLAYER_STATUS_DECHDR と遷移していき、
 * ヘッダ解析が完了するとCRIMANAPLAYER_STATUS_WAIT_PREPとなります。<br>
 * ムービー情報を取得するには、ハンドル状態がCRIMANAPLAYER_STATUS_WAIT_PREPになったあとに ::criManaPlayer_GetMovieInfo 関数
 * を実行してください。<br>
 * <br>
 * ハンドル状態がCRIMANAPLAYER_STATUS_WAIT_PREPの時に、 ::criManaPlayer_Prepare関数 か ::criManaPlayer_Start関数 を
 * 呼ぶことで再生処理を続けることができます。<br>
 * 
 * \par 備考:
 * データ要求コールバック関数を登録している場合は事前のムービーデータ設定は省略することだきます。
 * 
 * \sa criManaPlayer_GetStatus, criManaPlayer_Prepare, criManaPlayer_Start, criManaPlayer_SetDataRequestCallback
 */
void CRIAPI criManaPlayer_DecodeHeader(CriManaPlayerHn player);

/* criManaPlayer_Prepare */
/*JP
 * \brief 再生の準備要求
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \par 呼び出し条件:
 * 本関数の実行の前に再生ファイルや再生データの指定を行ってください。<br>
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * ムービーの再生は開始せず、ヘッダ解析と再生準備のみを行って待機するための関数です。<br>
 * この関数を使用して再生準備を事前に済ませることにより、ムービー再生開始のタイミングを細かく制御することができます。<br>
 * （再生準備無しで再生開始関数を呼び出した場合は、実際に再生が始まるまでにタイムラグが発生します。）<br>
 * 本関数を呼び出すと、Manaプレーヤーのハンドル状態は CRIMANAPLAYER_STATUS_STOP → CRIMANAPLAYER_STATUS_DECHDR → CRIMANAPLAYER_STATUS_PREP と遷移していき、
 * 再生準備が完了するとCRIMANAPLAYER_STATUS_READYとなります。<br>
 * <br>
 * ハンドル状態がCRIMANAPLAYER_STATUS_READYの時に、 ::criManaPlayer_Start 関数 を呼ぶことで再生を開始することができます。<br>
 * <br>
 * ::criManaPlayer_DecodeHeader 関数の呼び出し無しでこの関数を呼び出す場合は、Manaプレーヤーのハンドル状態が
 * CRIMANAPLAYER_STATUS_STOP か CRIMANAPLAYER_STATUS_PLAYEND でなければいけません。
 * 
 * \par 備考:
 * データ要求コールバック関数を登録している場合は事前のムービーデータ設定は省略することだきます。
 * 
 * \sa criManaPlayer_GetStatus, criManaPlayer_DecodeHeader, criManaPlayer_Start,
 *     criManaPlayer_SetBufferingTime
 */
void CRIAPI criManaPlayer_Prepare(CriManaPlayerHn player);

/* criManaPlayer_Start */
/*JP
 * \brief 再生開始
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \par 呼び出し条件:
 * 本関数の実行の前に再生ファイルや再生データの指定を行ってください。<br>
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * ムービーの再生を開始します。<br>
 * ::criManaPlayer_Prepare 関数を呼ばずに、本関数を呼び出した場合は、ムービーの解析と再生の準備を行うため、
 * 実際にムービーの再生が始まるまでにタイムラグが発生します。<br>
 * ::criManaPlayer_Prepare 関数を先に呼び出して、ハンドル状態がCRIMANAPLAYER_STATUS_READYになっていれば、
 * この関数を呼び出してすぐに再生が始まります。<br>
 * <br>
 * ::criManaPlayer_DecodeHeader 関数または ::criManaPlayer_Prepare 関数の呼び出し無しでこの関数を呼び出す場合は、
 * Manaプレーヤーのハンドル状態が CRIMANAPLAYER_STATUS_STOPかCRIMANAPLAYER_STATUS_PLAYEND でなければいけません。
 * 
 * \par 備考:
 * データ要求コールバック関数を登録している場合は事前のムービーデータ設定は省略することだきます。
 * 
 * \sa criManaPlayer_GetStatus, criManaPlayer_DecodeHeader, criManaPlayer_Prepare
 */
void CRIAPI criManaPlayer_Start(CriManaPlayerHn player);

/* criManaPlayer_Stop */
/*JP
 * \brief 再生の停止要求
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \par 呼び出し条件:
 * スレッドセーフではありません。
 * \par 説明:
 * ムービー再生停止の要求を出します。本関数は即時復帰関数です。本関数内で全ての停止処理が実行されるわけではありません。<br>
 * 本関数呼出し後、再生状態が CRIMANAPLAYER_STATUS_STOP なるまでは通常のメインループ処理を動かしてください。<br>
 * 具体的には ::criMana_ExecuteMain 関数, ::criMana_ExecuteVideoProcess 関数等が通常通り呼び出される必要があります。<br>
 * <br>
 * 再生状態が CRIMANAPLAYER_STATUS_ERROR になった場合は、本関数を呼び出して CRIMANAPLAYER_STATUS_STOP を待ってください。<br>
 * 
 * \par 備考:
 * Ver.1.10よりcriManaPlayer_RequestStopからAPI名を変更しました。
 *
 * \sa criManaPlayer_GetStatus
 */
void CRIAPI criManaPlayer_Stop(CriManaPlayerHn player);

/* criManaPlayer_StopAndWaitCompletion */
/*JP
 * \brief 再生の停止 (完了復帰API)
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \par 呼び出し条件:
 * スレッドセーフではありません。
 * \par 説明:
 * ムービー再生を停止します。本関数は完了復帰関数です。本関数から処理が戻るとプレーヤーの状態は停止状態になります。<br>
 * 本関数内部では、ストリーミング処理の停止待ちなど行うため、数十ミリ秒ほど処理をブロックする場合があります。<br>
 * シーンの切替タイミングなど、ゲーム処理が滞っても構わない場合でのみ本関数を実行してください。<br>
 * <br>
 * メイン処理がブロックされると困る場合は、 ::criManaPlayer_Stop 関数を使用してください。<br>
 *
 * \sa criManaPlayer_GetStatus
 */
void CRIAPI criManaPlayer_StopAndWaitCompletion(CriManaPlayerHn player);

/* criManaPlayer_Pause */
/*JP
 * \brief 再生のポーズ切り替え
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	sw	ポーズスイッチ（CRI_ON：ポーズ、CRI_OFF：ポーズ解除）
 * \par 呼び出し条件:
 * スレッドセーフです。
 * \par 説明:
 * 本関数の動作は引数に依存します。<br>
 * 引数 sw がCRI_ON(1)なら、一時停止。引数 sw がCRI_OFF(0)なら再生再開です。<br>
 * <br>
 * 本関数を呼び出しても、ファイル読み込みやデコードの停止はすぐには行われません。<br>
 * ::criManaPlayer_Stop を呼び出すと、ポーズ状態は解除されます。
 */
void CRIAPI criManaPlayer_Pause(CriManaPlayerHn player, CriBool sw);

/* criManaPlayer_IsPaused */
/*JP
 * \brief 再生のポーズ状態の取得
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \return CriBool ポーズ状態
 * \par 呼び出し条件:
 * スレッドセーフです。
 * \par 説明:
 * Manaプレーヤーの現在のポーズ状態を取得します。
 * 返り値が CRI_ON(1)なら、ポーズ状態。引数 sw がCRI_OFF(0)ならポーズ解除状態です。<br>
 * <br>
 */
CriBool CRIAPI criManaPlayer_IsPaused(CriManaPlayerHn player);

/* criManaPlayer_Loop */
/*JP
 * \brief 再生のループ切り替え
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	sw		ループスイッチ（CRI_FALSE：ループなし、CRI_TRUE：ループあり）
 * \par 呼び出し条件:
 * スレッドセーフです。
 * \par 説明:
 * ループ再生の有無を設定します。デフォルトはループOFFです。<br>
 * ループ再生ONにした場合は、ムービーの終端まで再生してもハンドル状態はCRIMANAPLAYER_STATUS_PLAYENDにならず、
 * ムービーの先頭から再生を繰り返します。<br>
 * <br>
 * ループ再生OFFに設定した場合は、そのとき読み込んでいたムービーの終端まで再生すると、
 * ハンドル状態がCRIMANAPLAYER_STATUS_PLAYENDに遷移します。<br>
 * 再生中にループOFFにした場合、タイミングによっては、再生中のムービー終端で終わらず、次の繰り返し
 * 再生まで実行されます。
 */
void CRIAPI criManaPlayer_Loop(CriManaPlayerHn player, CriBool sw);

/* criManaPlayer_GetTime */
/*JP
 * \brief 再生時刻の取得
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[out]	count	タイマカウント
 * \param[out]	unit	1秒あたりのタイマカウント数。 count / unit で秒単位の時刻になります。
 * \par 呼び出し条件:
 * スレッドセーフではありません。
 * \par 説明:
 * タイマ時刻を取得します。時刻はcountとunitの二つの変数で表現します。<br>
 * count ÷ unit で秒単位の時刻となるような値を返します。<br>
 * 再生開始前および再生停止後は、時刻０（タイマカウントが０）を返します。<br>
 * 本関数はマスタータイマーで指定されたタイマの時刻を返すだけで、ビデオフレームの時刻を返すものではありません。<br>
 * 取得したビデオフレームの本来の表示時刻は、ビデオフレーム取得時の CriManaFrameInfo 構造体を参照してください。
 */
void CRIAPI criManaPlayer_GetTime(CriManaPlayerHn player, CriUint64 *count, CriUint64 *unit);

/* criManaPlayer_GetInputBufferSize */
/*JP
 * \brief 入力バッファサイズの取得
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \return		CriSint32	入力バッファサイズ [byte]
 * \par 呼び出し条件:
 * スレッドセーフではありません。
 * \par 説明:
 * Manaプレーヤーの入力バッファサイズを取得します。<br>
 * 入力バッファサイズは CRIMANAPLAYER_STATUS_PREP 以降で有効な値が取得できます。<br>
 * \par 備考:
 * 本関数は情報取得用途のみ利用可能なデバッグ関数です。<br>
 *
 * \sa criManaPlayer_SetBufferingTime, criManaPlayer_GetInputBufferRemainSize
 */
CriSint32 CRIAPI criManaPlayer_GetInputBufferSize(CriManaPlayerHn player);

/* criManaPlayer_GetInputBufferRemainSize */
/*JP
 * \brief 入力バッファー内データ残量の取得
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \return		CriSint32	入力バッファー内のデータ残量 [byte]
 * \par 呼び出し条件:
 * スレッドセーフではありません。
 * \par 説明:
 * Manaプレーヤーの入力バッファー内のデータ残量を取得します。<br>
 * CRIMANAPLAYER_STATUS_PREP 以降で有効な値が取得できます。<br>
 * \par 備考:
 * 本関数は情報取得用途のみ利用可能なデバッグ関数です。<br>
 * <br>
 * 動画のコマ落ちやコマ飛ばしが発生した場合、本関数を使用して再生中のプレーヤーの
 * 入力バッファーにデータが残っているかどうかを確認できます。<br>
 * 再生中にもかかわらずデータ残量が長時間0の場合、何らかの異常によりデータ供給
 * が滞っている可能性があります。<br>
 *
 * \sa criManaPlayer_SetBufferingTime, criManaPlayer_GetInputBufferRemainSize
 */
CriSint32 CRIAPI criManaPlayer_GetInputBufferRemainSize(CriManaPlayerHn player);

/* criManaPlayer_SetNumberOfFramePools */
/*JP
 * \brief 内部ビデオバッファ（フレームプール）数の指定
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	npools	内部ビデオバッファ数（最低でも１）
 * \par 呼び出し条件:
 * Manaプレーヤーのハンドル状態が、CRIMANAPLAYER_STATUS_WAIT_PREP以前に呼び出してください。<br>
 * また、この関数は再生ワークパラメタに影響するため、::criManaPlayer_GetPlaybackWorkParam 関数よりも前に呼び出してください。<br>
 * <br>
 * スレッドセーフではありません。<br>
 * \par 説明:
 * Manaプレーヤー内部のビデオバッファ数を指定します。<br>
 * この内部ビデオバッファはデコード結果を蓄えておくためのもので、フレームプールと呼びます。<br>
 * フレームプールが多いほど先行してビデオデコードを進めることができるため、デコードの
 * 負荷変動が大きかったり、デコードに使用できるCPU時間の変動が大きい場合にもスムーズな再生を
 * 行いやすくなります。<br>
 * <br>
 * デフォルトのフレームプール数は１です。<br>
 */
void CRIAPI criManaPlayer_SetNumberOfFramePools(CriManaPlayerHn player, CriUint32 npools);

/* criManaPlayer_SetBufferingTime */
/*JP
 * \brief 入力データのバッファリング時間の指定
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	sec		バッファリング時間。単位は秒。
 * \par 呼び出し条件:
 * Manaプレーヤーのハンドル状態が、CRIMANAPLAYER_STATUS_WAIT_PREP以前に呼び出してください。<br>
 * また、この関数は再生ワークパラメタに影響するため、::criManaPlayer_GetPlaybackWorkParam 関数よりも前に呼び出してください。<br>
 * <br>
 * スレッドセーフではありません。<br>
 * \par 説明:
 * ストリーミング再生でバッファリングする入力データの量を秒単位の時間で指定します。<br>
 * Manaプレーヤーは、バッファリング時間とムービーのビットレート等から読み込みバッファーのサイズを決定します。<br>
 * <br>
 * デフォルトのバッファリング時間は、1秒分です。<br>
 * <br>
 * バッファリング時間に 0.0f を指定した場合、バッファリング時間はライブラリのデフォルト値となります。<br>
 */
void CRIAPI criManaPlayer_SetBufferingTime(CriManaPlayerHn player, CriFloat32 sec);

/* criManaPlayer_SetMinBufferSize */
/*JP
 * \brief 最小バッファサイズの指定
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	min_buffer_size		最小バッファサイズ [byte]
 * \par 呼び出し条件:
 * Manaプレーヤーのハンドル状態が、CRIMANAPLAYER_STATUS_WAIT_PREP以前に呼び出してください。<br>
 * また、この関数は再生ワークパラメタに影響するため、::criManaPlayer_GetPlaybackWorkParam 関数よりも前に呼び出してください。<br>
 * <br>
 * スレッドセーフではありません。<br>
 * \par 説明:
 * ムービーデータの最小バッファサイズを指定します。<br>
 * 最小バッファサイズを指定した場合、指定サイズ分以上の入力バッファーがManaプレーヤー内部で確保することが保証されます。<br>
 * <br>
 * 単純再生においては本関数を使用する必要はありません。極端にビットレートが異なるようなムービーを連結再生する場合に使用します。<br>
 * <br>
 * 最小バッファサイズに 0を指定した場合、最小バッファサイズはムービーデータの持つ値となります。(デフォルト)<br>
 * \par 備考:
 * 本関数で設定した最小バッファサイズは ::criManaPlayer_GetMovieInfo 関数で取得するムービー情報には反映されません。<br>
 * ::criManaPlayer_GetMovieInfo 関数できるのはムービーデータ本来の情報です。<br>
 *
 * \sa criManaPlayer_GetInputBufferSize
 */
void CRIAPI criManaPlayer_SetMinBufferSize(CriManaPlayerHn player, CriUint32 min_buffer_size);

/* criManaPlayer_SetFileStreamingFlag */
/*JP
 * \brief ストリーミングフラグの設定
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	streaming_flag	ストリーミングフラグ
 * \par 呼び出し条件:
 * Manaプレーヤーのハンドル状態が、再生停止または再生終了状態の時のみ呼び出し可能です。<br>
 * <br>
 * スレッドセーフです。<br>
 * \par 説明:
 * ストリーミング再生を行うかどうかの指定をManaプレーヤーに行います。<br>
 * デフォルトはCRI_TRUEです。CRI_FALSEを指定すると、Manaプレーヤーはオンメモリ再生専用となります。 <br>
 * <br>
 * \par 備考:
 * ストリーミングフラグを無効にした状態で、ストリーミング再生を開始すると再生エラーになります。
 */
void CRIAPI criManaPlayer_SetFileStreamingFlag(CriManaPlayerHn player, CriBool streaming_flag);

/* criManaPlayer_SetMaxMovieSize */
/*JP
 * \brief 最大解像度の設定
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	max_width	ムービーの最大幅
 * \param[in]	max_height	ムービーの最大高さ
 * \par 呼び出し条件:
 * Manaプレーヤーのハンドル状態が、再生停止または再生終了状態の時のみ呼び出し可能です。<br>
 * <br>
 * スレッドセーフです。<br>
 * \par 説明:
 * プレーヤーで再生するムービーの最大解像度を指定します。<br>
 */
void CRIAPI criManaPlayer_SetMaxMovieSize(CriManaPlayerHn player, CriSint32 max_width, CriSint32 max_height);

/* criManaPlayer_SetMaxPictureDataSize */
/*JP
 * \brief 最大ピクチャデータサイズの指定
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	max_data_size	最大ピクチャデータサイズ[byte]
 * \par 呼び出し条件:
 * 再生開始 (::criManaPlayer_Prepare 関数または ::criManaPlayer_Start 関数) 前に本関数を呼び出してください。<br>
 * <br>
 * スレッドセーフです。<br>
 * \par 説明:
 * プレーヤーで再生するムービーデータの最大ピクチャデータサイズを指定します。<br>
 * 本APIは特定コーデックの連結再生を行う場合にのみ使用します。(Sofdec.Primeコーデックには不要)<br>
 * <br>
 * 最大ピクチャデータサイズに 0 を指定した場合、最大ピクチャデータサイズはムービーデータの持つ値となります。<br>
 * <br>
 * \sa criManaPlayer_SetDataRequestCallback
 */
void CRIAPI criManaPlayer_SetMaxPictureDataSize(CriManaPlayerHn player, CriUint32 max_data_size);

/* criManaPlayer_ExecuteAutoSkip */
/*JP
 * \brief デコードフレームの自動スキップ判定の実行
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \param[in]	frame_info	フレーム情報構造体
 * \par 呼び出し条件:
 * Manaプレーヤーのハンドル状態が CRIMANAPLAYER_STATUS_PLAYING の時に本関数を呼び出してください。
 * <br>
 * スレッドセーフではありません。<br>
 * \par 説明:
 * デコードフレームのスキップ判定をアプリ側で指定したManaプレーヤハンドル単位で行います。
 * 関数内で再生負荷による処理落ちが発生すると判定した場合、
 * 内部で自動的にフレームをスキップします。
 * 見た目はガタつきますが、音飛びの発生を防ぎやすくなります。
 *
 * デコード済みのフレーム情報の参照、フレームの時刻判定がCRI_TRUEを返したタイミングで本関数を呼び出してください。
 * \par 例:
 * \code
 * 	CriManaPlayerHn player;
 * 	CriManaFrameInfo frame_info;
 * 	
 * 	if (criManaPlayer_ReferFrame(player, &frame_info) == CRI_TRUE) {
 * 		if (criManaPlayer_IsFrameOnTime(player, &frame_info) == CRI_TRUE) {
 * 			criManaPlayer_ExecuteAutoSkip(player, &frame_info);
 * 		}
 * 	}
 * \endcode
 * \par 注意:
 * ・criMana_SetDecodeSkipFlag関数とは同時に使用しないで下さい。
 *   （criMana_SetDecodeSkipFlag関数は呼び出さない形かCRI_FALSEで設定して下さい）<br>
 * ・同じフレーム情報構造体に対して複数回呼び出すと、スキップ要求を
 *   複数回実行してしまうので、各フレーム情報について呼び出しは１度だけに
 *   するよう注意してください。<br>
 * ・Sofdec.Primeのみ有効。H.264/VP9はデコードスキップには未対応です。<br>
 */
void criManaPlayer_ExecuteAutoSkip(CriManaPlayerHn player, CriManaFrameInfo *frame_info);

/***********************************/
/*         オーディオ関連          */
/***********************************/
/* criManaPlayer_SetAudioTrack */
/*JP
 * \brief メインオーディオトラック番号の設定
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	track	トラック番号
 * \par 呼び出し条件:
 * 再生開始 (::criManaPlayer_Prepare 関数または ::criManaPlayer_Start 関数) 前に本関数を呼び出してください。<br>
 * <br>
 * この関数はスレッドセーフではありません。
 * \par 説明:
 * ムービーが複数のオーディオトラックを持っている場合に、再生するオーディオを指定します。<br>
 * 本関数を実行しなかった場合は、もっとも若い番号のオーディオトラックを再生します。<br>
 * データが存在しないトラック番号を指定した場合は、オーディオは再生されません。<br>
 * <br>
 * トラック番号としてCRIMANA_AUDIO_TRACK_OFFを指定すると、例えムービーにオーディオが
 * 含まれていたとしてもオーディオは再生しません。<br>
 * <br>
 * また、デフォルト設定（もっとも若いチャンネルのオーディオを再生する）にしたい場合は、
 * チャンネルとしてCRIMANA_AUDIO_TRACK_AUTOを指定してください。<br>
 * <br>
 * オーディオのトラック情報は、再生開始前にヘッダ解析を行うことでランタイム時に取得できます。
 *
 * \par 備考:
 * 再生中のトラック変更は未対応です。変更前のフレーム番号を記録しておいてシーク再生
 * してください。
 *
 * \sa criManaPlayer_GetMovieInfo, criManaPlayer_DecodeHeader
 */
void  CRIAPI criManaPlayer_SetAudioTrack(CriManaPlayerHn player, CriSint32 track);

/* criManaPlayer_SetSubAudioTrack */
/*JP
 * \brief サブオーディオトラック番号の設定
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	track	サブオーティオのトラック番号
 * \par 呼び出し条件:
 * 再生開始 (::criManaPlayer_Prepare 関数または ::criManaPlayer_Start 関数) 前に本関数を呼び出してください。<br>
 * <br>
 * この関数はスレッドセーフではありません。
 * \par 説明:
 * サブオーティオのトラック番号を設定します。
 * <br>
 * サブオーディオを再生するには、本関数でトラック番号を指定した後、::criManaPlayer_SetPlaybackWork 関数で指定する 
 * CriManaPlaybackExWorkConfig 構造体の sub_audio_params パラメタを有効にする必要があります。<br>
 * <br>
 * 指定したトラック番号にオーディオが入っていない場合はOFFになります。
 */
void CRIAPI criManaPlayer_SetSubAudioTrack(CriManaPlayerHn player, CriSint32 track);

/* criManaPlayer_SetExtraAudioTrack */
/*JP
 * \brief エクストラオーディオトラック番号の設定
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	track	エクストラオーティオのトラック番号
 * \par 呼び出し条件:
 * 再生開始 (::criManaPlayer_Prepare 関数または ::criManaPlayer_Start 関数) 前に本関数を呼び出してください。<br>
 * <br>
 * この関数はスレッドセーフではありません。
 * \par 説明:
 * エクストラオーティオのトラック番号を設定します。
 * <br>
 * エクストラオーディオを再生するには、本関数でトラック番号を指定した後、::criManaPlayer_SetPlaybackWork 関数で指定する 
 * CriManaPlaybackExWorkConfig 構造体の extra_audio_params パラメタを有効にする必要があります。<br>
 * <br>
 * 指定したトラック番号にオーディオが入っていない場合はOFFになります。
 */
void CRIAPI criManaPlayer_SetExtraAudioTrack(CriManaPlayerHn player, CriSint32 track);

/* criManaPlayer_SetVolume */
/*JP
 * \brief オーディオボリュームの設定 (メインオーディオトラック)
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	volume	ボリューム
 * \par 呼び出し条件:
 * 本関数はスレッドセーフではありません。
 * \par 説明:
 * ムービーのメインオーディオトラックの出力音声ボリュームを指定します。<br>
 * <br>
 * ボリューム値には、0.0f以上1.0f以下の範囲で実数値を指定します。<br>
 * ボリューム値は音声データの振幅に対する倍率です（単位はデシベルではありません）。<br>
 * 例えば、1.0fを指定した場合、原音はそのままのボリュームで出力されます。<br>
 * 0.0fを指定した場合、音声はミュートされます（無音になります）。<br>
 * ボリュームのデフォルト値は1.0fです。<br>
 * <br>
 * なお、0.0f以上1.0f以下の範囲外を指定した場合は、それぞれの最小・最大値にクリップされます。
 *
 * \sa criManaPlayer_GetVolume
 */
void CRIAPI criManaPlayer_SetVolume(CriManaPlayerHn player, CriFloat32 volume);

/* criManaPlayer_GetVolume */
/*JP
 * \brief オーディオボリュームの取得 (メインオーディオトラック)
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \return CriFloat32 ボリューム
 * \par 説明:
 * ムービーのメインオーディオトラックの出力音声ボリューム値を取得します。
 * ボリュームの値域は、0.0f以上1.0f以下です。
 * 
 * \sa criManaPlayer_SetVolume
 */
CriFloat32 CRIAPI criManaPlayer_GetVolume(CriManaPlayerHn player);

/* criManaPlayer_GetAtomExPlayerByTrackId */
/*JP
 * \brief AtomExPlayerHn の取得
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	track_id	オーディオトラック番号 (0: メイン, 1: サブ, 2: エクストラ)
 * \return CriAtomExPlayerHn CriAtomExPlayer ハンドル
 * \par 説明:
 * CriManaPlayer が音声再生に使用している ::criAtomExPlayerHn を取得します。<br>
 * 各オーディオトラックトラックに異なる::criAtomExPlayerHnを取得することが可能です。<br>
 * 引数 track_id の値でオーディオトラックを指定します。<br>
 * track_id が 0 ならメイン、1 ならサブ、2 ならエクストラオーディオトラックを指定することになります。<br>
 * CriManaPlayerConfig 構造体の use_atomex_player メンバに CRI_TRUE を指定し、
 * ::criManaPlayer_CreateWithConfig 関数で作成した CriManaPlayerHn 以外では
 * 本関数は NULL を返します。
 * \attention
 * 本関数で取得した ::CriAtomExPlayerHn を破棄しないでください。
 */
CriAtomExPlayerHn CRIAPI criManaPlayer_GetAtomExPlayerByTrackId(CriManaPlayerHn player, CriUint32 track_id);

/* criManaPlayer_SetSubAudioVolume */
/*JP
 * \brief オーディオボリュームの設定 (サブオーディオトラック)
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	volume	ボリューム
 * \par 呼び出し条件:
 * 本関数はスレッドセーフではありません。
 * \par 説明:
 * ムービーのサブオーディオトラックの出力音声ボリュームを指定します。<br>
 * <br>
 * ボリューム値には、0.0f以上1.0f以下の範囲で実数値を指定します。<br>
 * ボリューム値は音声データの振幅に対する倍率です（単位はデシベルではありません）。<br>
 * 例えば、1.0fを指定した場合、原音はそのままのボリュームで出力されます。<br>
 * 0.0fを指定した場合、音声はミュートされます（無音になります）。<br>
 * ボリュームのデフォルト値は1.0fです。<br>
 * <br>
 * なお、0.0f以上1.0f以下の範囲外を指定した場合は、それぞれの最小・最大値にクリップされます。
 *
 * \sa criManaPlayer_GetSubAudioVolume
 */
void CRIAPI criManaPlayer_SetSubAudioVolume(CriManaPlayerHn player, CriFloat32 volume);

/* criManaPlayer_GetSubAudioVolume */
/*JP
 * \brief オーディオボリュームの取得 (サブオーディオトラック)
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \return CriFloat32 ボリューム
 * \par 説明:
 * ムービーのサブオーディオトラックの出力音声ボリューム値を取得します。
 * ボリュームの値域は、0.0f以上1.0f以下です。
 * 
 * \sa criManaPlayer_SetSubAudioVolume
 */
CriFloat32 CRIAPI criManaPlayer_GetSubAudioVolume(CriManaPlayerHn player);

/* criManaPlayer_SetExtraAudioVolume */
/*JP
 * \brief オーディオボリュームの設定 (エクストラオーディオトラック)
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	volume	ボリューム
 * \par 呼び出し条件:
 * 本関数はスレッドセーフではありません。
 * \par 説明:
 * ムービーのエクストラオーディオトラックの出力音声ボリュームを指定します。<br>
 * <br>
 * ボリューム値には、0.0f以上1.0f以下の範囲で実数値を指定します。<br>
 * ボリューム値は音声データの振幅に対する倍率です（単位はデシベルではありません）。<br>
 * 例えば、1.0fを指定した場合、原音はそのままのボリュームで出力されます。<br>
 * 0.0fを指定した場合、音声はミュートされます（無音になります）。<br>
 * ボリュームのデフォルト値は1.0fです。<br>
 * <br>
 * なお、0.0f以上1.0f以下の範囲外を指定した場合は、それぞれの最小・最大値にクリップされます。
 *
 * \sa criManaPlayer_GetExtraAudioVolume
 */
void CRIAPI criManaPlayer_SetExtraAudioVolume(CriManaPlayerHn player, CriFloat32 volume);

/* criManaPlayer_GetExtraAudioVolume */
/*JP
 * \brief オーディオボリュームの取得 (エクストラオーディオトラック)
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \return CriFloat32 ボリューム
 * \par 説明:
 * ムービーのエクストラオーディオトラックの出力音声ボリューム値を取得します。
 * ボリュームの値域は、0.0f以上1.0f以下です。
 * 
 * \sa criManaPlayer_SetExtraAudioVolume
 */
CriFloat32 CRIAPI criManaPlayer_GetExtraAudioVolume(CriManaPlayerHn player);

/* criManaPlayer_SetPan */
/*JP
 * \brief 2Dパンの設定（メインオーディオトラック）
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	ch			チャンネル番号
 * \param[in]	pan			パン設定値（-1.0f以上1.0f以下）
 * \par 呼び出し条件:
 * 本関数はスレッドセーフではありません。
 * \par 説明:
 * メインオーディオトラックの2Dパン（音源の左右定位位置）を指定します。<br>
 * 本関数を実行することで、モノラル音声やステレオ音声に対し、音源の定位位置を自由
 * にコントロールすることが可能です。<br>
 * <br>
 * 第2引数のチャンネル番号は"音声データのチャンネル番号"を指定します。<br>
 * 【モノラル音声を再生する場合】<br>
 * <ul><li>チャンネル番号には0を指定します。</li></ul>
 * 【ステレオ音声を再生する場合】<br>
 * <ul>
 *  <li>チャンネル番号0:左音声
 *  <li>チャンネル番号1:右音声
 * </ul><br>
 * 第3引数のパン設定値には、指定したチャンネル番号のデータの定位をどの位置にする
 * かを指定します。<br>
 * パン設定値は、-1.0f以上1.0f以下の範囲で実数値を指定します。<br>
 * 音源は、負の値を指定すると中央より左側（値が小さいほど左寄り）、0.0fを指定すると
 * 中央、正の値を指定すると中央より右側（値が大きいほど右寄り）に定位します。<br>
 * （キリのいい値では、-1.0fが左端、0.0fが中央、1.0fが右端になります。）<br>
 * \par 備考:
 * 実際に適用される値が1.0fを超える値になった場合、値は1.0fにクリップされます。<br>
 * 同様に、実際に適用される値が-1.0f未満の値になった場合も、値は-1.0fにクリップされます。<br>
 * <br>
 * 再生中の音声データに存在しないチャンネル番号を指定した場合は設定が無視されます。<br>
 * また、チャンネル2以降を指定した場合も同様に無効です。
 *
 * \sa criManaPlayer_GetPan
 */
void CRIAPI criManaPlayer_SetPan(CriManaPlayerHn player, CriSint32 ch, CriFloat32 pan);

/* criManaPlayer_GetPan */
/*JP
 * \brief 2Dパンの取得（メインオーディオトラック）
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	ch			チャンネル番号
 * \return CriFloat32 パン設定値
 * \par 説明:
 * メインオーディオトラックの指定したチャンネル番号のパン設定値を取得します。<br>
 * パン設定値の値域は、-1.0f以上1.0f以下です。<br>
 * 
 * \sa criManaPlayer_SetPan
 */
CriFloat32 CRIAPI criManaPlayer_GetPan(CriManaPlayerHn player, CriSint32 ch);

/* criManaPlayer_ResetSendLevel */
/*JP
 * \brief センドレベルのリセット（メインオーディオトラック）
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \par 説明:
 * メインオーディオトラックのセンドレベルの設定をリセットします。<br>
 * 本関数を実行することで、過去にセットされたセンドレベル設定が全てクリアされます。<br>
 * <br>
 * 本関数をムービー再生中に実行した場合、設定したセンドレベルがクリアされて全てのスピーカから<br>
 * 音声出力がされなくなります。
 * \par 備考:
 * センドレベルの設定には「自動設定」「手動設定」の2通りが存在します。<br>
 * Manaプレーヤーを作成した直後や、本関数でセンドレベルをリセットした場合、
 * センドレベルの設定は「自動設定」となります。<br>
 * （自動設定時のルーティングについては、 ::criManaPlayer_SetSendLevel
 * 関数の説明を参照してください。）<br>
 * \sa criManaPlayer_SetSendLevel
 */
void CRIAPI criManaPlayer_ResetSendLevel(CriManaPlayerHn player);

/* criManaPlayer_SetSendLevel */
/*JP
 * \brief センドレベルの設定（メインオーディオトラック）
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \param[in]	ch			チャンネル番号
 * \param[in]	spk			スピーカーID
 * \param[in]	level		ボリューム値（0.0f以上1.0f以下）
 * \par 説明:
 * メインオーディオトラックのセンドレベルを指定します。<br>
 * センドレベルは、音声データの各チャンネルの音声を、どのスピーカーから
 * どの程度の音量で出力するかを指定するための仕組みです。<br>
 * <br>
 * 第2引数のチャンネル番号は"音声データのチャンネル番号"を指定します。<br>
 * 第3引数のスピーカーIDには、指定したチャンネル番号のデータをどのスピーカーから
 * 出力するかを指定し、第4引数の送信時のボリュームを指定します。<br>
 * ボリューム値は、0.0f以上1.0f以下の範囲で実数値を指定します。<br>
 * ボリューム値は音声データの振幅に対する倍率です（単位はデシベルではありません）。<br>
 * 例えば、1.0fを指定した場合、原音はそのままのボリュームで出力されます。<br>
 * 0.5fを指定した場合、原音波形の振幅を半分にしたデータと同じ音量（-6dB）で
 * 音声が出力されます。<br>
 * 0.0fを指定した場合、音声はミュートされます（無音になります）。
 * <br>
 * 例えば、音声データのチャンネル0番のデータをライトスピーカーから
 * フルボリューム（1.0f）で出力したい場合、指定は以下のようになります。
 * \code
 * criManaPlayer_SetSendLevel(player, 0, CRIATOM_SPEAKER_FRONT_RIGHT, 1.0f);
 * \endcode
 * <br>
 * 本関数では1つのチャンネルのデータを複数のスピーカーから異なるボリュームで
 * 出力することが可能です。<br>
 * 例えば、チャンネル1番の音声をレフトスピーカーから0.3f、ライトスピーカー
 * から0.5f、センタースピーカーから0.7fで出力したい場合、指定は以下のようになります。
 * \code
 * criManaPlayer_SetSendLevel(player, 1, CRIATOM_SPEAKER_FRONT_LEFT, 0.3f);
 * criManaPlayer_SetSendLevel(player, 1, CRIATOM_SPEAKER_FRONT_RIGHT, 0.5f);
 * criManaPlayer_SetSendLevel(player, 1, CRIATOM_SPEAKER_FRONT_CENTER, 0.7f);
 * \endcode
 * <br>
 * 尚、セットされたセンドレベルの値は ::criManaPlayer_ResetSendLevel 関数で
 * リセットすることが可能です。<br>
 * \par 備考:
 * センドレベルの設定には「自動設定」「手動設定」の2通りが存在します。<br>
 * Manaプレーヤーを作成した直後や、 ::criManaPlayer_ResetSendLevel 関数で
 * センドレベルをクリアした場合、センドレベルの設定は「自動設定」となります。<br>
 * 本関数を実行した場合、センドレベルの設定は「手動設定」となります。<br>
 * <br>
 * 「自動設定」の場合、Manaプレーヤーは以下のように音声をルーティングします。<br>
 * <br>
 * - 【モノラル音声を再生する場合】<br>
 * チャンネル0の音声を左右のスピーカーから約0.7f（-3dB）のボリュームで出力します。<br>
 * <br>
 * - 【ステレオ音声を再生する場合】<br>
 * チャンネル0の音声をレフトスピーカーから、
 * チャンネル1の音声をライトスピーカーから出力します。<br>
 * <br>
 * - 【4ch音声を再生する場合】<br>
 * チャンネル0の音声をレフトスピーカーから、チャンネル1の音声をライトスピーカーから、
 * チャンネル2の音声をレフトサラウンドスピーカーから、
 * チャンネル3の音声をライトサラウンドスピーカーからでそれぞれ出力します。<br>
 * <br>
 * - 【5.1ch音声を再生する場合】<br>
 * チャンネル0の音声をレフトスピーカーから、チャンネル1の音声をライトスピーカーから、
 * チャンネル2の音声をセンタースピーカーから、チャンネル3の音声をLFEから、
 * チャンネル4の音声をレフトサラウンドスピーカーから、
 * チャンネル5の音声をライトサラウンドスピーカーからそれぞれ出力します。<br>
 * \par
 * これに対し、本関数を用いて「手動設定」を行った場合、音声データのチャンネル数に
 * 関係なく、指定されたルーティングで音声が出力されます。<br>
 * （センドレベルを設定していないチャンネルの音声は出力されません。）<br>
 * センドレベルの設定をクリアし、ルーティングを「自動設定」の状態に戻したい場合は、
 * ::criManaPlayer_ResetSendLevel 関数を実行してください。<br>
 * <br>
 * 本関数の設定値と、以下の関数のボリューム設定値は独立して制御されます。<br>
 * 	- ::criManaPlayer_SetVolume
 * 例えば、本関数に0.5fを、 ::criManaPlayer_SetVolume 関数にも0.5fを設定した場合、
 * 出力音声のボリュームは原音を0.25f倍したボリュームで出力されます。<br>
 * （0.5f×0.5f＝0.25fの演算が行われます。）
 * \attention
 * 再生する音声データがマルチチャンネルのデータであっても、センドレベルが一部の
 * チャンネルのみにしか設定されていない場合、センドレベルの設定されていない
 * チャンネルの音声は出力されません。<br>
 * <br>
 * 本関数と ::criManaPlayer_SetPan 関数を併用しないでください。<br>
 * ::criManaPlayer_SetPan 関数が、内部的に本関数を呼び出すため、
 * 両者を併用した場合、後から実行した関数により設定が上書きされる可能性があります。<br>
 * 音源の定位をコントロールする際には、本関数かまたは ::criManaPlayer_SetPan 関数
 * のいずれか一方のみをご利用ください。<br>
 * （3Dパンを利用する場合は本関数を、2Dパンのみを行う場合は ::criManaPlayer_SetPan
 * 関数をご利用ください。）
 * <br>
 * 本関数は一部の機種で対応していない場合があります。<br>
 * （プラットフォームのサウンドライブラリの仕様によっては実装が困難な場合が
 * あるため、全ての環境で利用できるワケではありません。）<br>
 * 本関数が利用可能かどうかは、別途マニュアルの機種依存情報のページをご参照ください。
 * \sa criManaPlayer_SetVolume, criManaPlayer_SetPan, criManaPlayer_ResetSendLevel
 */
void CRIAPI criManaPlayer_SetSendLevel(
	CriManaPlayerHn player, CriSint32 ch, CriAtomSpeakerId spk, CriFloat32 level);

/* criManaPlayer_ResetSubAudioSendLevel */
/*JP
 * \brief センドレベルのリセット（サブオーディオトラック）
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \par 説明:
 * サブオーディオトラックのセンドレベルの設定をリセットします。<br>
 * 本関数を実行することで、過去にセットされたセンドレベル設定が全てクリアされます。<br>
 * \par 備考:
 * 本関数の動作仕様は ::criManaPlayer_ResetSendLevel と同じです。<br>
 * 本関数の設定対象はサブオーディオトラックのみとなります。
 * \sa criManaPlayer_ResetSendLevel
 */
void CRIAPI criManaPlayer_ResetSubAudioSendLevel(CriManaPlayerHn player);

/* criManaPlayer_SetSubAudioSendLevel */
/*JP
 * \brief センドレベルの設定（サブオーディオトラック）
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \param[in]	ch			チャンネル番号
 * \param[in]	spk			スピーカーID
 * \param[in]	level		ボリューム値（0.0f以上1.0f以下）
 * \par 説明:
 * サブオーディオトラックのセンドレベルを指定します。<br>
 * センドレベルは、音声データの各チャンネルの音声を、どのスピーカーから
 * どの程度の音量で出力するかを指定するための仕組みです。<br>
 * <br>
 * \par 備考:
 * 本関数の動作仕様は ::criManaPlayer_SetSendLevel と同じです。<br>
 * 本関数の設定対象はサブオーディオトラックのみとなります。
 * \sa criManaPlayer_SetSubAudioSendLevel
 */
void CRIAPI criManaPlayer_SetSubAudioSendLevel(
	CriManaPlayerHn player, CriSint32 ch, CriAtomSpeakerId spk, CriFloat32 level);

/* criManaPlayer_ResetExtraAudioSendLevel */
/*JP
 * \brief センドレベルのリセット（エクストラオーディオトラック）
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \par 説明:
 * エクストラオーディオトラックのセンドレベルの設定をリセットします。<br>
 * 本関数を実行することで、過去にセットされたセンドレベル設定が全てクリアされます。<br>
 * \par 備考:
 * 本関数の動作仕様は ::criManaPlayer_ResetSendLevel と同じです。<br>
 * 本関数の設定対象はエクストラオーディオトラックのみとなります。
 * \sa criManaPlayer_ResetSendLevel
 */
void CRIAPI criManaPlayer_ResetExtraAudioSendLevel(CriManaPlayerHn player);

/* criManaPlayer_SetExtraAudioSendLevel */
/*JP
 * \brief センドレベルの設定（エクストラオーディオトラック）
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \param[in]	ch			チャンネル番号
 * \param[in]	spk			スピーカーID
 * \param[in]	level		ボリューム値（0.0f以上1.0f以下）
 * \par 説明:
 * エクストラオーディオトラックのセンドレベルを指定します。<br>
 * センドレベルは、音声データの各チャンネルの音声を、どのスピーカーから
 * どの程度の音量で出力するかを指定するための仕組みです。<br>
 * <br>
 * \par 備考:
 * 本関数の動作仕様は ::criManaPlayer_SetSendLevel と同じです。<br>
 * 本関数の設定対象はエクストラオーディオトラックのみとなります。
 * \sa criManaPlayer_SetSendLevel
 */
void CRIAPI criManaPlayer_SetExtraAudioSendLevel(
	CriManaPlayerHn player, CriSint32 ch, CriAtomSpeakerId spk, CriFloat32 level);

/* criManaPlayer_SetBusSendLevelByName */
/*JP
 * \brief バスセンドレベルの設定（メインオーディオトラック）
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \param[in]	bus_name	バス名
 * \param[in]	level		センドレベル値（0.0f以上1.0f以下）
 * \par 説明:
 * メインオーディオトラックのバスセンドレベルを指定します。<br>
 * バスセンドレベルは、音声をどのバスにどれだけ流すかを指定するための仕組みです。<br>
 * 再生開始前にバスセンドレベルを設定した場合、設定値に応じて音声再生を開始します。<br>
 * 再生中にバスセンドレベルを設定した場合は即座にその設定値が反映されます。<br>
 * <br>
 * 第2引数のバス名はDSPバス設定内のバス名を指定します。<br>
 * 第3引数では送信時のレベル（ボリューム）を指定します。<br>
 * <br>
 * センドレベル値の範囲や扱いは、ボリュームと同等です。::criManaPlayer_SetVolume 関数を参照してください。
 * \par 備考:
 * 本関数を複数回呼び出すことで、複数のバスに流すこともできます。<br>
 * \sa criManaPlayer_SetVolume
 */
void CRIAPI criManaPlayer_SetBusSendLevelByName(
	CriManaPlayerHn player, const CriChar8* bus_name, CriFloat32 level);

/* criManaPlayer_SetSubAudioBusSendLevelByName */
/*JP
 * \brief バスセンドレベルの設定（サブオーディオトラック）
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \param[in]	bus_name	バス名
 * \param[in]	level		センドレベル値（0.0f以上1.0f以下）
 * \par 説明:
 * サブオーディオトラックのバスセンドレベルを指定します。<br>
 * バスセンドレベルは、音声をどのバスにどれだけ流すかを指定するための仕組みです。<br>
 * 再生開始前にバスセンドレベルを設定した場合、設定値に応じて音声再生を開始します。<br>
 * 再生中にバスセンドレベルを設定した場合は即座にその設定値が反映されます。<br>
 * <br>
 * \par 備考:
 * 本関数の動作仕様は ::criManaPlayer_SetBusSendLevelByName と同じです。<br>
 * 本関数の設定対象はサブオーディオトラックのみとなります。
 * \sa criManaPlayer_SetBusSendLevelByName
 */
void CRIAPI criManaPlayer_SetSubAudioBusSendLevelByName(
	CriManaPlayerHn player, const CriChar8* bus_name, CriFloat32 level);

/* criManaPlayer_SetExtraAudioBusSendLevelByName */
/*JP
 * \brief バスセンドレベルの設定（エクストラオーディオトラック）
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \param[in]	bus_name	バス名
 * \param[in]	level		センドレベル値（0.0f以上1.0f以下）
 * \par 説明:
 * エクストラオーディオトラックのバスセンドレベルを指定します。<br>
 * バスセンドレベルは、音声をどのバスにどれだけ流すかを指定するための仕組みです。<br>
 * 再生開始前にバスセンドレベルを設定した場合、設定値に応じて音声再生を開始します。<br>
 * 再生中にバスセンドレベルを設定した場合は即座にその設定値が反映されます。<br>
 * <br>
 * \par 備考:
 * 本関数の動作仕様は ::criManaPlayer_SetBusSendLevelByName と同じです。<br>
 * 本関数の設定対象はエクストラオーディオトラックのみとなります。
 * \sa criManaPlayer_SetExtraAudioBusSendLevelByName
 */
void CRIAPI criManaPlayer_SetExtraAudioBusSendLevelByName(
	CriManaPlayerHn player, const CriChar8* bus_name, CriFloat32 level);

/* criManaPlayer_ResetBusSendLevelAll */
/*JP
 * \brief バスセンドレベルのリセット（全トラック）
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \par 説明:
 * メイン、サブオーディオ、エクストラオーディオの全トラックのバスセンドレベルをリセットします。<br>
 * 再生中にバスセンドレベルをリセットした場合は即座にその設定値が反映されます。<br>
 * <br>
 * \par 注意:
 * 本関数は、CriManaPlayerConfig.use_atomex_player に CRI_TRUE をセットして作成したプレーヤーには効果がありません。
*/
void CRIAPI criManaPlayer_ResetBusSendLevelAll(
	CriManaPlayerHn player);

/* criManaPlayer_SetSoundRendererType */
/*JP
 * \brief サウンドレンダラタイプの設定
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	sound_renderer_type	サウンドレンダラタイプ
 * \par 呼び出し条件:
 * スレッドセーフではありません。
 * 
 * \par 説明:
 * Manaプレーヤーで再生する音声の出力サウンドレンダラを指定します。<br>
 * 出力サウンドレンダラタイプの設定は各オーディオトラック（メイン、サブ、エクストラ）に反映されます。<br>
 * <br>
 * 再生中のサウンドレンダラタイプの変更はできません。再生中にサウンドレンダラタイプを指定した場合、
 * 次のムービー再生から設定が反映されます。<br>
 * <br>
 * \attention
 * CRI Mana Ver.1.68.02より前のバージョンの場合は以下の二つのみ指定できます。
 * <ul>
 *   <li>CRIATOM_SOUND_RENDERER_NATIVE
 *   <li>CRIATOM_SOUND_RENDERER_ASR
 * </ul><br>
 */
void criManaPlayer_SetSoundRendererType(CriManaPlayerHn player, CriAtomSoundRendererType sound_renderer_type);

/* criManaPlayer_SetAsrRackId */
/*JP
 * \brief ASRラックIDの設定
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \param[in]	asr_rack_id	ASRラックID
 * \par 呼び出し条件:
 * スレッドセーフではありません。
 * 
 * \par 説明:
 * Manaプレーヤーで再生する音声の出力ASRラックIDを指定します。<br>
 * 出力ASRラックIDの設定は各オーディオトラック（メイン、サブ、エクストラ）に反映されます。<br>
 * <br>
 * 出力サウンドレンダラタイプがCRIATOM_SOUND_RENDERER_ASRの場合のみ有効です。<br>
 * 再生中のASRラックIDの変更はできません。再生中にASRラックIDを指定した場合、
 * 次のムービー再生から設定が反映されます。<br>
 */
void criManaPlayer_SetAsrRackId(CriManaPlayerHn player, CriSint32 asr_rack_id);

/***********************************/
/*         フレーム取得関連        */
/***********************************/
/* criManaPlayer_ReferFrame */
/*JP
 * \brief デコード済みのフレーム情報の参照
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \param[out]	frame_info	フレーム情報構造体
 * \return CriBool フレーム情報の参照結果 (CRI_TRUE:参照成功, CRI_FALSE:参照失敗)
 * \par 呼び出し条件:
 * Manaプレーヤーのハンドル状態が CRIMANAPLAYER_STATUS_PLAYING の時に本関数を呼び出してください。
 * それ以外の状態では常にCRI_FALSEを返します。<br>
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * デコード済みのビデオフレームのフレーム情報構造体を参照します。
 * Manaプレーヤーのフレームプールにデコード済みのフレームがあれば、次のフレームの情報を取得できます。<br>
 * <br>
 * ビデオフレームの取得手順として、まずこの関数でデコード済みのフレームがあるかどうかを判断し、
 * 次に ::criManaPlayer_IsFrameOnTime 関数でフレームが再生時刻に達しているかチェックしてください。<br>
 * <br>
 * なお、::criManaPlayer_DiscardFrame を呼ばない限り、本関数は同じフレーム情報を取得します。
 * \par 備考:
 * Manaプレーヤーがポーズ中でも内部でビデオデコード処理は進み、本関数によるデコード済みのフレーム情報は
 * 参照可能です。
 *
 * \sa criManaPlayer_IsFrameOnTime
 */
CriBool CRIAPI criManaPlayer_ReferFrame(CriManaPlayerHn player, CriManaFrameInfo *frame_info);

/* criManaPlayer_IsFrameOnTime */
/*JP
 * \brief フレームの時刻判定
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \param[in]	frame_info	フレーム情報構造体
 * \return CriBool 入力したフレーム情報がすでに表示時刻になっている場合はCRI_TRUE(1)を返します。<br>
 * \par 呼び出し条件:
 * Manaプレーヤーのハンドル状態が CRIMANAPLAYER_STATUS_PLAYING の時に本関数を呼び出してください。
 * それ以外の状態では常にCRI_FALSEを返します。<br>
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * ::criManaPlayer_ReferFrame 関数で参照しているフレームの表示時刻判定を行ないます。
 * この表示時刻には、マスタータイマーの時刻を基準に判定を行ないます。
 * 
 * \sa criManaPlayer_ReferFrame
 */
CriBool CRIAPI criManaPlayer_IsFrameOnTime(CriManaPlayerHn player, const CriManaFrameInfo *frame_info);

/* criManaPlayer_KeepFrame */
/*JP
 * \brief フレームの保持
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \param[out]	frame_info	フレーム情報構造体
 * \return CriBool フレームが保持できたかどうか
 * \par 呼び出し条件:
 * スレッドセーフではありません。
 * \par 説明:
 * デコード済みのビデオフレームが存在する場合、アプリケーションがデコード結果のフレームを保持
 * できます。本関数を呼び出すと、フレームを破棄せずにフレームプール内の次のフレームが参照可能
 * になります。これにより複数枚のフレームをアプリケーションが管理できます。<br>
 * <br>
 * 保持されたフレームは、 ::criManaPlayer_Discard 関数や::criManaPlayer_DiscardOneKeepFrame 関数で
 * フレームを破棄しない限り、Manaプレーヤーはそのデコード結果を維持しますので、保持中は描画
 * 対象として使うことが可能です。<br>
 * <br>
 * 本関数は、フレームを複数取得して描画に使用する場合や、描画レイテンシがあるシステムにおいて
 * フレームを一定期間保持したい場合に使用してください。
 *
 * \sa criManaPlayer_ReferFrame, criManaPlayer_GetNumberOfKeepFrames, criManaPlayer_DiscardOneKeepFrame
 */
CriBool CRIAPI criManaPlayer_KeepFrame(CriManaPlayerHn player, CriManaFrameInfo *frame_info);

/* criManaPlayer_GetNumberOfKeepFrames */
/*JP
 * \brief 保持フレーム数の取得
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \return CriSint32 保持フレーム数
 * \par 呼び出し条件:
 * スレッドセーフではありません。
 * \par 説明:
 * ::criManaPlayer_KeepFrame関数で、現在アプリケーションが保持しているフレーム数を取得します。
 *
 * \sa criManaPlayer_KeepFrame
 */
CriSint32 CRIAPI criManaPlayer_GetNumberOfKeepFrames(CriManaPlayerHn player);

/* criManaPlayer_CopyFrameToBufferARGB32 */
/*JP
 * \brief デコード結果の取得 (32bit ARGB形式)
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \param[out]	frame_info	フレーム情報構造体
 * \param[out]	frame_buf	コピー先のフレームバッファ（描画対象）
 * \par 呼び出し条件:
 * 事前に::criMana_InitializeFrameARGB32 関数の呼び出しを必ず行ってください。
 * <br>
 * Manaプレーヤーのハンドル状態が CRIMANAPLAYER_STATUS_PLAYING の時に本関数を呼び出してください。
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * 32bit ARGBフォーマットで、表示時刻になっているビデオフレームを取得(コピー)します。<br>
 * この関数を呼び出す場合は、ARGBバッファーの実体を確保したうえで呼び出す必要があります。<br>
 * <br>
 * 32bit ARGB の実際のピクセルデータの並びについては、そのプラットフォームで最も標準的な
 * フォーマットになります。 
 *
 * \attention
 * PS3, Xbox360 でも本関数は使用できますが、とてもCPU負荷の高い関数となります。<br>
 * 解像度が 1280x720 のムービーを本関数をフレーム取得すると1vsyncに近い時間がかかります。<br>
 * PS3, Xbox360 では ::criManaPlayer_CopyFrameToBuffersYUV 関数と ピクセルシェーダ
 * の組み合わせによるフレーム変換をおすすめします。<br>
 *
 * \attention
 * 本関数は、Wii U/XboxOne/VITAでH.264再生時には対応していません。
 *
 * \sa criMana_InitializeFrameARGB32
 */
void CRIAPI criManaPlayer_CopyFrameToBufferARGB32(
	CriManaPlayerHn player, const CriManaFrameInfo *frame_info, CriManaTextureBuffer *frame_buf);

/* criManaPlayer_CopyFrameToBuffersYUV */
/*JP
 * \brief デコード結果の取得 (YUV個別バッファー方式)
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[out]	frame_info	フレーム情報構造体
 * \param[in]	frame_buf	コピー先のフレームバッファ（描画対象）
 * \par 呼び出し条件:
 * Manaプレーヤーのハンドル状態が CRIMANAPLAYER_STATUS_PLAYING の時に本関数を呼び出してください。
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * YUV個別バッファー形式で表示時刻になっているビデオフレームを取得します。<br>
 * YUV個別バッファー形式はピクセルシェーダーでフレームを描画するための出力フォーマットです。<br>
 * この関数を呼び出す場合は、YUV個別バッファーの実体を確保したうえで呼び出す必要があります。<br>
 * ビデオフレームは引数frame_bufで指定したYUV個別バッファーに書き出されます。<br>
 * <br>
 * アルファムービー再生を行わない場合は、引数 frame_buf の a_plane パラメータは使用しません。<br>
 */
void CRIAPI criManaPlayer_CopyFrameToBuffersYUV(
	CriManaPlayerHn player, const CriManaFrameInfo *frame_info, CriManaTextureBuffersYUV *frame_buf);

/* criManaPlayer_DiscardFrame */
/*JP
 * \brief フレームの解放
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	frame_info	フレーム情報構造体
 * \par 呼び出し条件:
 * スレッドセーフではありません。
 * \par 説明:
 * ::criManaPlayer_ReferFrame 関数または::criManaPlayer_KeepFrame 関数で参照しているフレームを解放します。<br>
 * 参照したフレームは本関数または ::criManaPlayer_DiscardOneKeepFrame 関数を呼び出して解放してください。<br>
 * <br>
 * フレームを解放すると、Manaプレーヤーは解放されたフレームのバッファーを空きとみなしてデコードに使用します。<br>
 */
void CRIAPI criManaPlayer_DiscardFrame(CriManaPlayerHn player, const CriManaFrameInfo *frame_info);

/* criManaPlayer_DiscardOneKeepFrame */
/*JP
 * \brief フレームの解放
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \par 呼び出し条件:
 * スレッドセーフではありません。
 * \par 説明:
 * ::criManaPlayer_KeepFrame 関数でアプリケーションが保持している一番古いフレームを破棄します。
 * <br>
 * どのフレームを破棄するかをアプリケーションが明示的に指定したい場合は ::criManaPlayer_DiscardFrame 関数を
 * 代わりに使用してください。
 *
 * \sa criManaPlayer_KeepFrame, criManaPlayer_GetNumberOfKeepFrames, criManaPlayer_DiscardFrame
 */
void CRIAPI criManaPlayer_DiscardOneKeepFrame(CriManaPlayerHn player);

/***********************************/
/*         特殊再生関連            */
/***********************************/
/* criManaPlayer_SetDataRequestCallback */
/*JP
 * \brief データ要求コールバックの登録
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	func	データ要求コールバック
 * \param[in]	obj		データ要求コールバックの引数として渡るユーザデータ
 * \par 呼び出し条件:
 * スレッドセーフではありません。
 * \par 説明:
 * ムービーの連結再生を行うために、ムービーファイルを要求するコールバック関数を登録します。
 * このコールバック関数は以下のタイミングで発生します。<br>
 * ・ムービーファイルを読み込み終了した時。<br>
 * ・ファイルの指定無しで再生を開始した時。<br>
 * <br>
 * データ要求コールバック関数内で ::criManaPlayer_SetFile 関数、::criManaPlayer_SetFileRange 関数、
 * または ::criManaPlayer_SetData 関数を呼び出すことで、連続して次のムービーファイルを指定することができます。<br>
 * いずれも呼び出さなかった場合は、読み込み済みのムービーが終わると再生終了になります。<br>
 * <br>
 * また、前回と指定したムービーソースをもう一度再生したい場合は、コールバック関数内で 
 * ::criManaPlayer_SetPreviousDataAgain 関数を呼んでください。<br>
 * <br>
 * データ要求コールバック発生時、コールバック関数の第二引数usrobjには、登録時に指定
 * したユーザオブジェクトが渡されます。登録ファイルリストなどの管理に利用してください。<br>
 * <br>
 * 連結再生できるムービーファイルには以下の条件があります。<br>
 * - ビデオ解像度が同じ
 * - ビデオのフレームレートが同じ（マスタータイマーが無同期以外の場合）
 * - ビデオのコーデックが同じ
 * - オーディオおよび字幕のトラック構成が同じ
 *
 * \par 備考:
 * Ver.1.10より、::criManaPlayer_SetFileRequestCallback 関数からAPI名を変更しました。
 *
 * \sa criManaPlayer_SetFile, criManaPlayer_SetData, criManaPlayer_SetFileRange, criManaPlayer_SetPreviousDataAgain, criManaPlayer_DeferCallback
 */
void CRIAPI criManaPlayer_SetDataRequestCallback(CriManaPlayerHn player, 
												 CriManaPlayerDataRequestCbFunc func, void* obj);

/* criManaPlayer_DeferCallback */
/*JP
 * \brief データ要求コールバックの再実行要求
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \par 呼び出し条件:
 * 本関数はデータ要求コールバック関数内でのみ使用可能です。<br>
 * ただし、ファイル未登録状態で再生開始した最初のデータ要求コールバックについては呼び出しできません。<br>
 * \par 説明:
 * ::criManaPlayer_SetDataRequestCallback 関数で登録したデータ要求コールバックが発生した際に、
 * コールバック関数内で呼び出すと、次回のサーバー処理でデータ要求コールバックを発生させます。<br>
 * 本関数を使用することで、データ要求コールバックが発生したタイミングで登録すべきムービーファイルが
 * 決定できない場合に、ムービーファイルの設定を先延ばしにすることが出来ます。<br>
 * <br>
 * \par 備考:
 * データ要求コールバック関数内で何もしなかった場合、Manaプレーヤーのステータスは
 * CRIMANAPLAYER_STATUS_PLAYEND に遷移します。<br>
 * 本関数を実行した場合、次のデータ要求コールバックが発行されるまではManaプレーヤーのステータスを
 * CRIMANAPLAYER_STATUS_PLAYING で維持することが出来ます。
 * \attention
 * ファイル未登録時に最初に呼び出されたデータ要求コールバックについては本関数を使用することは出来ません。<br>
 * 必ず２個目以降のムービーの連結対象の指定を先送りする場合に使用してください。<br>
 * <br>
 * 次のムービーの登録は遅くとも登録済みのムービーの最終フレームが表示される100ミリ秒前には行うように
 * してください。それ以上先送りをした場合、次のムービーの読み込みが間に合わずにシームレスに連結されない場合があります。<br>
 *
 * \sa criManaPlayer_SetFile, criManaPlayer_SetData, criManaPlayer_SetFileRange, criManaPlayer_SetPreviousDataAgain, criManaPlayer_SetDataRequestCallback
 */
void CRIAPI criManaPlayer_DeferCallback(CriManaPlayerHn player);

/* criManaPlayer_SetSeekPosition */
/*JP
 * \brief シーク位置の設定
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	seek_frame_no	シーク先のフレーム番号　（seek_frame_noに変更）
 * \par 呼び出し条件:
 * 再生開始前( ::criManaPlayer_Prepare 関数または ::criManaPlayer_Start 関数呼び出し前)に本関数を実行してください。<br>
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * シーク再生を開始するフレーム番号を指定します。<br>
 * <br>
 * また、この関数はムービーの再生中に呼び出すことは出来ません。再生中にシークをする場合は、一度再生を停止してから
 * 本関数を呼び出してください。<br>
 * <br>
 * 本関数を実行しなかった場合、またはフレーム番号０を指定した場合はムービーの先頭から再生を開始します。
 * 指定したフレーム番号が、ムービーデータの総フレーム数より大きかったり負の値だった場合もムービーの先頭から再生します。<br>
 * <br>
 * また、本関数呼び出し後にプレーヤーの再生停止、再生開始を行った場合でもシーク位置の設定情報は残ったままになります。
 * <br>
 * \par 注意:
 * シーク再生を行うには、あらかじめ ::criManaPlayer_SetMetaDataWorkAllocator 関数を使用してメタデータ用メモリアロケータ
 * を登録しておく必要があります。
 *
 * \sa criManaPlayer_SetMetaDataWorkAllocator
 */
void CRIAPI criManaPlayer_SetSeekPosition(CriManaPlayerHn player, CriSint32 seek_frame_no); 

/* criManaPlayer_SetMovieEventSyncMode */
/*JP
 * \brief ムービーイベント同期モードの指定
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	mode	ムービーイベントの同期モード
 * \par 呼び出し条件:
 * 再生開始前( ::criManaPlayer_Prepare 関数または ::criManaPlayer_Start 関数呼び出し前)に本関数を実行してください。<br>
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * ムービーイベント（キューポイント、字幕）の同期モードの設定をします。<br>
 * デフォルトはハンドル作成時にムービー再生時刻：CRIMANAPLAYER_EVENT_SYNC_PLAYBACK_TIMEに同期するよう設定します。
 * ビデオフレームの取得に同期させたいときはCRIMANAPLAYER_EVENT_SYNC_FRAME_TIMEを指定してください。<br>
 * \sa CriManaPlayerMovieEventSyncMode
 */
void CRIAPI criManaPlayer_SetMovieEventSyncMode(CriManaPlayerHn player, CriManaPlayerMovieEventSyncMode mode);

/* criManaPlayer_SetSubtitleChannel */
/*JP
 * \brief 字幕チャンネルの取得
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \param[in]	channel		字幕チャンネル番号
 * \par 呼び出し条件:
 * スレッドセーフではありません。<br>
 * \par 説明:
 * 取得する字幕チャンネルを設定します。デフォルトは字幕取得無しです。
 * <br>
 * ::criManaPlayer_DecodeHeader 関数と ::criManaPlayer_GetMovieInfo 関数を使うことで、再生するムービーが
 * いくつの字幕を含んでいるかを再生開始前に知ることができます。<br>
 * <br>
 * データが存在しないチャンネル番号を指定した場合は、字幕は取得できません。<br>
 * デフォルト設定（字幕取得無し）にしたい場合は、チャンネルとしてCRIMANA_SUBTITLE_CHANNEL_OFFを指定してください。<br>
 * <br>
 * この関数で字幕チャンネルを指定した場合は、メインループから定期的に ::criManaPlayer_GetSubtitleOnTime 関数を
 * 実行してください。字幕取得を定期的に行わない場合は、ムービー再生が途中で止まります。<br>
 * <br>
 * ムービー再生中に字幕チャンネルを切り替えることも可能です。ただし、実際のチャンネルが切り替わるのは
 * 設定した直後の次の字幕からとなります。
 *
 * \sa criManaPlayer_GetSubtitleOnTime
 */
void CRIAPI criManaPlayer_SetSubtitleChannel(CriManaPlayerHn player, CriSint32 channel);

/* criManaPlayer_GetSubtitleOnTime */
/*JP
 * \brief 字幕の取得
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \param[out]	bufptr		書き込み先の字幕バッファアドレス
 * \param[in]	bufsize		書き込み字幕バッファーのサイズ
 * \return CriUint32 書き込んだ字幕データのサイズ（バイト）
 * \par 呼び出し条件:
 * Manaプレーヤーのハンドル状態が CRIMANAPLAYER_STATUS_PLAYING の時に本関数を呼び出してください。
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * 表示時刻になっている字幕データを取得します。
 * この関数を呼び出す場合は、字幕用バッファーの実体を確保したうえで呼び出してください。<br>
 * 字幕データは引数 bufptr で指定したバッファーに書き出されます。<br>
 * もし字幕データが bufsize よりも大きい場合は、bufsize に収まる量だけ書き出し、残りは破棄されます。<br>
 * <br>
 * もしも表示時刻の字幕が無い場合は、バッファーの中身はクリアされます。<br>
 * <br>
 * ::criManaPlayer_SetSubtitleChannel 関数で存在する字幕チャンネルを指定している場合は、
 * メインループから定期的に本関数を実行してください。<br>
 * 実行しない場合は、ムービー再生が途中で止まります。
 * 
 * \sa criManaPlayer_GetSubtitleOnTime
 */
CriUint32 CRIAPI criManaPlayer_GetSubtitleOnTime(CriManaPlayerHn player, CriUint8 *bufptr, CriUint32 bufsize);

/* criManaPlayer_GetCuePointInfo */
/*JP
 * \brief キューポイント情報の一覧取得
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \return CriManaCuePointInfo*		キューポイント情報
 * \par 呼び出し条件:
 * Manaプレーヤーのハンドル状態が CRIMANAPLAYER_STATUS_WAIT_PREP以降に本関数を呼び出してください。
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * キューポイント情報（イベントポイント一覧）を取得します。
 * <br>
 * この関数で取得するキューポイント情報は、再生ハンドルのワークバッファを直接参照しています。<br>
 * 再生停止状態での参照は可能ですが、次の再生を開始した後は参照を禁止します。<br>
 * このキューポイント情報を別のメモリにコピーした場合もこの条件は変わりません。
 * \attention
 * キューポイント情報を取得するには、あらかじめ ::criManaPlayer_SetMetaDataWorkAllocator 関数を
 * 使用してメタデータ用メモリアロケータを登録しておく必要があります。
 */
CriManaCuePointInfo* CRIAPI criManaPlayer_GetCuePointInfo(CriManaPlayerHn player);

/* criManaPlayer_SetCuePointCallback */
/*JP
 * \brief キューポイントコールバックの設定
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	func	キューポイント取得コールバック関数
 * \param[in]	obj		ユーザオブジェクト（任意）
 * \par 呼び出し条件:
 * ムービー再生開始前に本関数を呼び出してください。<br>
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * キューポイントのコールバック関数を登録します。
 * このコールバック関数は、ムービーの再生時刻が各イベントポイントで指定された時刻を経過した時に発生します。
 * コールバック関数の呼び出し判定は ::criManaPlayer_ExecuteMain 関数から行われます。<br>
 * <br>
 * キューポイントコールバック発生時、コールバック関数の第２引数 eventinfo にはエベントポイント情報が、
 * 第３引数usrobjには、登録時に指定したユーザオブジェクトが渡されます。<br>
 * <br>
 * キューポイントコールバック関数内では、ムービー再生をコントロールする関数（例えば ::criManaPlayer_Stop 関数）
 * を呼び出してはいけません。
 * \attention
 * キューポイント情報を取得するには、あらかじめ ::criManaPlayer_SetMetaDataWorkAllocator 関数を
 * 使用してメタデータ用メモリアロケータを登録しておく必要があります。
 */
void CRIAPI criManaPlayer_SetCuePointCallback(CriManaPlayerHn player, CriManaPlayerCuePointCbFunc func, void *obj);

/* criManaPlayer_CalcFrameNoFromTime */
/*JP
 * \brief 再生時刻からフレーム番号の計算
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \param[in]	count		タイマカウント
 * \param[in]	unit		１秒あたりのタイマカウント値。count ÷ unit で秒単位の時刻となります。
 * \return		CriSint32	frame ID
 * \par 呼び出し条件:
 * この関数は、Manaプレーヤハンドルの状態が CRIMANAPLAYER_STATUS_WAIT_PREP 以降になったあとに使用できます。
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * 再生時刻からフレーム番号を計算します。
 * <br>
 * シーク再生開始位置を、時刻から計算したいときに使用してください。
 * （例えばキューポイント情報からシーク位置を決定する場合など。）
 * 
 * \sa criManaPlayer_CalcTimeFromFrameNo
 */
CriSint32 CRIAPI criManaPlayer_CalcFrameNoFromTime(CriManaPlayerHn player, 
													CriUint64 count, CriUint64 unit);

/* criManaPlayer_CalcTimeFromFrameNo */
/*JP
 * \brief フレーム番号から再生時刻の計算
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \param[in]	frame_no	フレーム番号
 * \param[in]	unit		１秒あたりのタイマカウント値。count ÷ unit で秒単位の時刻となります。
 * \return		タイマカウント
 * \par 呼び出し条件:
 * この関数は、Manaプレーヤハンドルの状態が CRIMANAPLAYER_STATUS_WAIT_PREP 以降になったあとに使用できます。
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * フレーム番号から再生時刻を計算します。
 * <br>
 * 実際にフレーム取得した場合は、計算の必要はありません。フレーム情報構造体の時刻を参照してください。
 * 
 * \sa criManaPlayer_CalcFrameNoFromTime
 */
CriUint64 CRIAPI criManaPlayer_CalcTimeFromFrameNo(CriManaPlayerHn player, CriSint32 frame_no, CriUint64 unit);

/* criManaPlayer_SearchNextEventPointByTime */
/*JP
 * \brief 指定時刻直後のイベントポイントの検索
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \param[in]	count		タイマカウント
 * \param[in]	unit		１秒あたりのタイマカウント値。count ÷ unit で秒単位の時刻となります。
 * \param[in]	type		検索対象とするイベントポイントのtype値
 * \param[in]	eventinfo	発見したイベントポイントの情報
 * \return		CriSint32	フレーム番号
 * \par 呼び出し条件:
 * この関数は、Manaプレーヤハンドルの状態が CRIMANAPLAYER_STATUS_WAIT_PREP 以降になったあとに使用できます。
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * 指定時刻の次にあるイベントポイントを検索し、イベントポイント情報とフレーム番号を取得します。
 * <br>
 * 検索の対象となるのは type で指定した値が一致するイベントポイントです。
 * type に -1を指定した場合は、全てのイベントポイントが検索対象となります。
 * <br>
 * 検索対象となるイベントポイントが発見できなかった場合は、フレーム番号は-1を返します。
 * 
 * \sa  criManaPlayer_SearchPrevEventPointByTime
 */
CriSint32 CRIAPI criManaPlayer_SearchNextEventPointByTime(CriManaPlayerHn player, 
														CriUint64 count, CriUint64 unit, CriSint32 type, CriManaEventPoint *eventinfo);

/* criManaPlayer_SearchPrevEventPointByTime */
/*JP
 * \brief 指定時刻直前のイベントポイントの検索
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \param[in]	count		タイマカウント
 * \param[in]	unit		１秒あたりのタイマカウント値。count ÷ unit で秒単位の時刻となります。
 * \param[in]	type		検索対象とするイベントポイントのtype値
 * \param[in]	eventinfo	発見したイベントポイントの情報
 * \return		CriSint32	フレーム番号
 * \par 呼び出し条件:
 * この関数は、Manaプレーヤハンドルの状態が CRIMANAPLAYER_STATUS_WAIT_PREP 以降になったあとに使用できます。
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * 指定時刻の手前にあるイベントポイントを検索し、イベントポイント情報とフレーム番号を取得します。<br>
 * <br>
 * 検索の対象となるのは type で指定した値が一致するイベントポイントです。
 * type に -1を指定した場合は、全てのイベントポイントが検索対象となります。<br>
 * <br>
 * 検索対象となるイベントポイントが発見できなかった場合は、フレーム番号は-1を返します。
 *
 * \sa  criManaPlayer_SearchNextEventPointByTime
 */
CriSint32 CRIAPI criManaPlayer_SearchPrevEventPointByTime(CriManaPlayerHn player, 
														CriUint64 count, CriUint64 unit, CriSint32 type, CriManaEventPoint *eventinfo);

/***********************************/
/*         特殊設定関連            */
/*    Special playback related     */
/***********************************/
/* criManaPlayer_SetSpeed */
/*JP
  \brief 再生速度の設定
  \ingroup MDL_MANALIB_PLAYER
  \param[in]	player	Manaプレーヤハンドル
  \param[in]	speed_numerator	再生速度指定の分子。
  \param[in]	speed_denominator	再生速度指定の分母。
  \par 呼び出し条件:
  Manaプレーヤーのハンドル状態が、CRIMANAPLAYER_STATUS_WAIT_PREP以前に呼び出してください。<br>
  ただし、音声なしのムービーを再生する場合に限り、再生中のムービーに対しても再生速度を変更することが可能です。<br>
  <br>
  スレッドセーフではありません。<br>
  \par 説明:
  ムービーの再生速度を有理数で指定します。早回し（再生速度を上げる）がどこまで可能かは、デコード
  負荷やデータの読み込みレートに依存します。<br>
  再生性能を超える速度を設定した場合、動画のがたつきや音途切れが発生する可能性があります。<br>
  <br>
  \table "使用例" align=center width=90% border=1 cellspacing=0 cellpadding=4
  {分子(numerator) | 分母(denominator)  | 意　味                      }
  [1               | 1	                | 等倍速再生(デフォルト)      ]
  [2               | 1                  | ２倍速再生                  ]
  [1               | 2                  | ２分の1スロー再生           ]
  [1000            | 1001               | 30fpsの動画を29.97fpsで再生、60fpsの動画を59.94fpsで再生 ]
  \endtable
  <br>
  \par 注意:
  音声つきムービーの再生中の速度設定はできません。再生途中からの再生速度変更を実現したい場合、一度再生停止
  してから、再生速度を変更し、目的のフレーム番号からシーク再生をしてください。<br>
  <br>
  \par 備考:
  プラットフォームによっては、オーディオ付きムービーには再生速度設定ができません。<br>
  <br>
  無同期モード(CRIMANAPLAYER_TIMER_NONE)を指定している場合、速度は変化しません。
 */
void CRIAPI criManaPlayer_SetSpeed(CriManaPlayerHn player, 
	CriSint32 speed_numerator, CriSint32 speed_denominator);

/* criManaPlayer_SetDecodeMode */
/*JP
  \brief デコード動作モードの設定
  \ingroup MDL_MANALIB_PLAYER
  \param[in]	player	Manaプレーヤハンドル
  \param[in]	mode	デコード動作モード
  \par 呼び出し条件:
  Manaプレーヤーのハンドル状態が、CRIMANAPLAYER_STATUS_WAIT_PREP以前に呼び出してください。<br>
  <br>
  スレッドセーフです。<br>
  \par 説明:
  Manaプレーヤーのデコード動作モードを指定します。
  <br>
  \par 注意:
  再生中のデコード動作モードの変更はできません。再生中に動作モードを設定した場合、次の
  再生から設定が反映されます。

  \par 備考:
  プラットフォームやコーデックによって指定可能なモードに制限があります。<br>
 */
void CRIAPI criManaPlayer_SetDecodeMode(CriManaPlayerHn player, 
	 CriManaDecodeMode mode);

/*==========================================================================
 *      CRI Mana Decrypter API
 *=========================================================================*/
/* criManaDecrypter_CalculateWorkSize */
/*JP
 * \brief デクリプタ作成用ワーク領域サイズの計算
 * \ingroup MDL_MANALIB_DECRYPTER
 * \param[in]	config				デクリプタ作成用コンフィグ構造体
 * \return		CriSint32			ワーク領域サイズ
 * \par 説明:
 * デクリプタの作成に必要なワーク領域のサイズを計算します。<br>
 * ::criMana_SetUserAllocator マクロによるアロケーター登録を行わずに
 * ::criManaDecrypter_Create 関数でデクリプタを作成する場合、
 * 本関数で計算したサイズ分のメモリをワーク領域として渡す必要があります。<br>
 * <br>
 * ワーク領域サイズの計算に失敗すると、本関数は -1 を返します。<br>
 * ワーク領域サイズの計算に失敗した理由については、エラーコールバックのメッセージで確認可能です。<br>
 * \par 備考:
 * 引数 config の情報は、関数内でのみ参照されます。<br>
 * 関数を抜けた後は参照されませんので、関数実行後に config の領域を解放しても
 * 問題ありません。
 * \sa CriManaDecrypterConfig, criManaDecrypter_Create, criMana_SetUserAllocator
 */
CriSint32 CRIAPI criManaDecrypter_CalculateWorkSize(
	const CriManaDecrypterConfig *config);

/* criManaDecrypter_Create */
/*JP
 * \brief デクリプタの作成
 * \ingroup MDL_MANALIB_DECRYPTER
 * \param[in]	config				デクリプタ作成用コンフィグ構造体
 * \param[in]	work				ワーク領域
 * \param[in]	work_size			ワーク領域サイズ
 * \return		CriManaDecrypterHn	デクリプタハンドル
 * \par 説明:
 * 暗号化された音声データを解読するための復号器（デクリプタ）を作成します。<br>
 * <br>
 * ワーク領域サイズの計算に失敗すると、本関数は -1 を返します。<br>
 * ワーク領域サイズの計算に失敗した理由については、エラーコールバックのメッセージで確認可能です。<br>
 * \par 備考:
 * 現状、デクリプタは作成と同時にアクティブ化され、グローバルに適用されます。<br>
 * そのため、本関数を実行すると以降は " 暗号化されたデータのみが再生可能 " となります。<br>
 * （暗号化されていないムービーデータや、暗号キーの異なるムービーデータは全て再生エラーとなります。）<br>
 * <br>
 * 引数 config の情報は、関数内でのみ参照されます。<br>
 * 関数を抜けた後は参照されませんので、関数実行後に config の領域を解放しても
 * 問題ありません。
 * \par 例:
 * \code
 * 	CriManaDecrypterConfig decrypter_config;
 * 	CriManaDecrypterHn decrypter;
 * 	CriManaPlayerHn player;
 * 	
 * 	// デクリプタ作成用コンフィグ構造体をゼロクリア
 * 	memset(&decrypter_config, 0, sizeof(decrypter_config));
 * 	
 * 	// 暗号化キーの設定
 * 	decrypter_config.key = 123456789;
 * 	
 * 	// デクリプタの作成
 * 	decrypter = criManaDecrypter_Create(&decrypter_config, NULL, 0);
 * 	
 * 	// プレーヤーの作成
 * 	player = criManaPlayer_Create(NULL, NULL, 0);
 * \endcode
 * \attention
 * 本関数を実行する前に、ライブラリを初期化しておく必要があります。<br>
 * <br>
 * 現状、アプリケーションで同時に使用できるデクリプタの数は1つだけです。<br>
 * （複数のデクリプタを同時に作成することはできません。）<br>
 * <br>
 * デクリプタ作成後にプレーヤーを作成した場合は、
 * プレーヤーを破棄するまでデクリプタを破棄しないでください。<br>
 * プレーヤーを残した状態でデクリプタを破棄した場合、
 * ムービーデータが正しく再生されなくなったり、アクセス違反が発生する恐れがあります。<br>
 * <br>
 * 本関数を実行後、必ず対になる ::criManaDecrypter_Destroy 関数を実行してください。<br>
 * また、 ::criManaDecrypter_Destroy 関数を実行するまでは、
 * 本関数にセットしたワーク領域を解放したり、本関数を再度実行したりしないでください。<br>
 * \sa CriManaDecrypterConfig, criManaDecrypter_CalculateWorkSize, criManaDecrypter_Destroy, criMana_SetUserAllocator
 */
CriManaDecrypterHn CRIAPI criManaDecrypter_Create(
	const CriManaDecrypterConfig *config, void *work, CriSint32 work_size);

/* criManaDecrypter_Destroy */
/*JP
 * \brief デクリプタの破棄
 * \ingroup MDL_MANALIB_DECRYPTER
 * \par 説明:
 * デクリプタを破棄します。<br>
 * 本関数を実行することで、暗号化データの復号処理が停止されます。<br>
 * ::criMana_SetUserAllocator マクロを使用してアロケーターを登録済みの場合、
 * デクリプタ作成時に確保されたメモリ領域が解放されます。<br>
 * （デクリプタ作成時にワーク領域を渡した場合、本関数実行後であれば
 * ワーク領域を解放可能です。）<br>
 * \attention
 * 本関数を実行すると、以降の復号化処理は行われなくなります。<br>
 * 暗号化されたムービーの再生途中では本関数を呼び出さないでください。<br>
 * \sa criManaDecrypter_Create, criMana_SetUserAllocator
 */
void CRIAPI criManaDecrypter_Destroy(CriManaDecrypterHn decrypter);


/* OUTER_FRAMEPOOL_WORK */
/*****************************************************/
/*     外部フレームプール(一部の機種でのみ使用)      */
/* External frame pool (used only on some platforms) */
/****************************************************/
/* criManaPlayer_CalculateExtDecoderWorkSize */
/*JP
 * \brief 拡張デコーダワーク領域のサイズ計算
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	config_basic	基本再生ワーク用パラメタのポインタ（必須）
 * \param[in]	config_ex		拡張再生ワーク用パラメタのポインタ（追加）NULL指定可
 * \return	CriSint32 拡張デコーダワークサイズ
 * \par 呼び出し条件:
 * スレッドセーフです。
 * \par 説明:
 * 指定されたムービーファイルの再生に必要な拡張デコーダワーク領域のサイズを計算します。<br>
 * 拡張デコーダワークが不要な場合は0を返します。
 */
CriSint32 CRIAPI criManaPlayer_CalculateExtDecoderWorkSize(
	CriManaPlayerHn player, const CriManaPlaybackBasicWorkConfig *config_basic, const CriManaPlaybackExWorkConfig *config_ex);

/* criManaPlayer_SetExtDecoderWork */
/*JP
 * \brief 拡張デコーダワーク領域の設定
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	config_basic	基本再生ワーク用パラメタのポインタ（必須）
 * \param[in]	config_ex		拡張再生ワーク用パラメタのポインタ（追加）NULL指定可
 * \param[in]	work			アプリが確保したワーク領域（内部自動アライン調整）
 * \param[in]	work_size		ワークサイズ
 * \par 呼び出し条件:
 * CRIMANAPLAYER_STATUS_WAIT_PREP か、それよりも前の状態で呼び出してください。<br>
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * 指定されたムービーファイルの再生に必要な拡張デコーダワークメモリを設定します。<br>
 * <br>
 * アプリケーション側で明示的にメモリを確保する場合は、::criManaPlayer_CalculateExtDecoderWorkSize
 * 関数で必要なワークサイズを確認し、確保したワークメモリを引数に渡してください。<br>
 * workにNULL、work_sizeに0を指定した場合は、::criManaPlayer_SetExtDecoderWorkAllocator 関数で登録された
 * メモリ確保関数を使用して内部でメモリ確保を行います。
 * <br>
 * 本関数を実行せずに再生を開始した場合、内部で自動的にデフォルトの再生パラメタを使用して再生を
 * 行います。この場合、::criManaPlayer_SetExtDecoderWorkAllocator 関数でメモリアロケータが登録されていることが条件になります。
 *
 */
void CRIAPI criManaPlayer_SetExtDecoderWork(
	CriManaPlayerHn player, const CriManaPlaybackBasicWorkConfig *config_basic, const CriManaPlaybackExWorkConfig *config_ex, 
	void *work, CriSint32 work_size);

/* 拡張デコーダワークの確保／解放コールバック関数型 */
/* Extended decoder work area allocate/release callback function type */
#define CriManaExtDecoderWorkMallocFunc CriManaMallocFunc
#define CriManaExtDecoderWorkFreeFunc   CriManaFreeFunc

/* criManaPlayer_SetExtDecoderWorkAllocator */
/*JP
 * \brief 拡張デコーダワーク確保用のコールバックの登録
 * \ingroup MDL_MANALIB_GLOBAL
 * \param[in]	player			Manaプレーヤハンドル
 * \param[in]	allocfunc		メモリ確保関数
 * \param[in]	freefunc		メモリ解放関数
 * \param[in]	obj				ユーザオブジェクト（任意）
 * \par 呼び出し条件:
 * ::criMana_Initialize 関数の前に呼び出してください。<br>
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * CRI Manaライブラリに拡張デコーダワーク用のメモリアロケータ（メモリの確保／解放関数）を登録します。<br>
 * CRI Manaライブラリがライブラリ内で行なうメモリ解放処理を、
 * ユーザー独自のメモリ解放処理に置き換えたい場合に使用します。
 */
void CRIAPI criManaPlayer_SetExtDecoderWorkAllocator(
	CriManaPlayerHn player, CriManaExtDecoderWorkMallocFunc allocfunc, CriManaExtDecoderWorkFreeFunc freefunc, void *obj);

/*JP
 * \brief オーディオトラックのカテゴリの設定
 * \ingroup MDL_MANALIB_GLOBAL
 * \param[in]	player			Manaプレーヤハンドル
 * \param[in]	track_id		オーディオトラック番号 (0: メイン, 1: サブ, 2: エクストラ)
 * \param[in]	category_name	カテゴリ名
 * \par 説明:
 * ::criManaPlayer_Start 関数の前に本関数を呼ぶことで、その後再生するムービーのオーディオトラックにカテゴリを設定することができます。<br>
 * 各オーディオトラックトラックに異なるカテゴリ設定を適用することが可能です。引数 track_id の値でオーディオトラックを指定します。<br>
 * track_id が 0 ならメイン、1 ならサブ、2 ならエクストラオーディオトラックを指定することになります。<br>
 * 設定したカテゴリ情報を削除するには、 ::criManaPlayer_SetSoundCategoryByName 関数を使用します。<br>
 * カテゴリという機能の詳細については CRI ADX2マニュアルを参照してください。
 * 
 * \par 呼び出し条件:
 * CriManaPlayerConfig.use_atomex_player の値を CRI_TRUE として作成した CriManaPlayerHn を使用してください。<br>
 * デフォルト設定で作成した CriManaPlayerHn では、本関数は無効です。<br>
 * <br>
 * カテゴリ設定は再生開始前に行ってください。
 * 再生中の音声に対してカテゴリ設定の更新を行うと、カテゴリの再生数カウントが異常になる危険があります。<br>
 * <br>
 * また、事前に ACF のレジストを行う必要があります。使用可能なカテゴリ情報は ACF 内に記述されているためです。<br>
 * ACF については別途 CRI ADX2マニュアルをご確認ください。
 * <br>
 * 本関数はスレッドセーフではありません。
 * \sa criManaPlayer_CreateWithConfig, criManaPlayer_UnsetSoundCategory
 */
void CRIAPI criManaPlayer_SetSoundCategoryByName(CriManaPlayerHn player, CriUint32 track_id, const CriChar8* category_name);

/*JP
 * \brief オーディオトラックのカテゴリの削除
 * \ingroup MDL_MANALIB_GLOBAL
 * \param[in]	player			Manaプレーヤハンドル
 * \param[in]	track_id		オーディオトラック番号 (0: メイン, 1: サブ, 2: エクストラ)
 * \par 説明:
 * オーディオトラックに設定されているカテゴリ情報を削除します。<br>
 * 引数 track_id の値が 0 ならメイン、1 ならサブ、2 ならエクストラオーディオトラックを指定することになります。<br>
 * カテゴリの設定を行うには ::criManaPlayer_SetSoundCategoryByName 関数を使用してください。
 * \par 呼び出し条件:
 * CriManaPlayerConfig.use_atomex_player の値を CRI_TRUE として作成した CriManaPlayerHn を使用してください。<br>
 * デフォルト設定で作成した CriManaPlayerHn では、本関数は無効です。<br>
 * <br>
 * 本関数はスレッドセーフではありません。
 */
void CRIAPI criManaPlayer_UnsetSoundCategory(CriManaPlayerHn player, CriUint32 track_id);

/**********************/
/* 特殊設定           */
/* Special settings   */
/**********************/
/* criManaPlayer_SetNumberOfFramesForPrep */
/*JP
 * \brief 再生準備時の貯蓄フレーム数の設定
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player	Manaプレーヤハンドル
 * \param[in]	num_frames	再生準備完了と判断するフレーム数（フレームプール数以下であること）
 * \par 説明:
 * 再生準備時に内部に貯めておくデコードフレームの数を設定します。<br>
 * これはフレームプール数とは異なり、CRIMANAPLAYER_STATUS_PREP時に何枚フレームをデコードしたら
 * CRIMANAPLAYER_STATUS_READY状態にして良いかを決定するための設定値です。
 *
 * 何も設定しない場合はフレームプール数が適用されます。フレームプール数は大きくしたいが
 * 再生レイテンシは極力少なくしたい場合に、この関数を使用します。
 *
 */
void CRIAPI criManaPlayer_SetNumberOfFramesForPrep(CriManaPlayerHn player, CriUint32 num_frames);


/**********************/
/* Experimental       */
/**********************/
void CRIAPI criManaPlayer_SetVideoFramerate(CriManaPlayerHn player, CriUint32 framerate_n, CriUint32 framerate_d);

/********************************************************************************/
/* デバッグ用：ユーザー使用禁止。仕様も保証しません                               */
/* For debugging: User use prohibited functions. Specifications not guaranteed. */
/********************************************************************************/
/* オーディオ準備判定の無視フラグ */
/* Audio preparation judgment ignore flag */
void CRIAPI criMana_SetIgnoreAudioPrep(CriBool flag);

/*   シークテーブル情報   */
/* Seek table information */
typedef struct {
	CriSint32   top_frame_id;
} CriManaSeekBlockInfo;

/* シークブロック数の取得 */
/* 取得失敗時は０を返す。PLAYINGになっても０なら情報なし。 */
/* Obtains the number of seek blocks. */
/* Returns 0 on failure. No information if 0 is returned while PLAYING. */
CriSint32 CRIAPI criManaPlayer_GetNumberOfSeekBlocks(CriManaPlayerHn player);

/* シークブロック情報の取得 */
/* <入力>
 * - num_seekblock : 出力配列の要素数
 * <出力>
 * - blockinfo : シークブロック情報配列へのポインタ（num_seekblock分の領域を確保して渡すこと）
 */
/* Obtains seek block information. */
/* <Input>
 * - num_seekblock : Number of output array elements
 * <Output>
 * - blockinfo : Pointer to seek block information array (allocate the size required for num_seekblock before passing)
 */
void CRIAPI criManaPlayer_GetSeekBlockInfo(CriManaPlayerHn player, CriSint32 num_seekblock, CriManaSeekBlockInfo *blockinfo);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/***************************************************************************
 *      旧バージョンとの互換用
 *      For compatibility with old version
 ***************************************************************************/
 /* 互換性維持用置換マクロ */
 /* Replacement macro for retaining compatibility */
#define criManaPlayer_CalculateHanldeWorkSize criManaPlayer_CalculateHandleWorkSize

#define criManaPlayer_GetAtomExPlayer(player)     criManaPlayer_GetAtomExPlayerByTrackId(player, 0)

#ifdef __cplusplus
extern "C" {
#endif

void CRIAPI criManaPlayer_SetBusSendLevel(
	CriManaPlayerHn player, CriSint32 bus_id, CriFloat32 level);

void CRIAPI criManaPlayer_SetSubAudioBusSendLevel(
	CriManaPlayerHn player, CriSint32 bus_id, CriFloat32 level);
#ifdef __cplusplus
}
#endif

#endif //CRI_INCL_CRI_MANA_H

/* end of file */
