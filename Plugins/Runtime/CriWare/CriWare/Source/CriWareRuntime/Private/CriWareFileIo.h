﻿/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2015-2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : File I/O Interface
 * File     : CriWareFileIo.h
 *
 ****************************************************************************/

/* 多重定義防止 */
#pragma once

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
#define CRI_XPT_DISABLE_UNPREFIXED_TYPE
#if !defined(CRIWARE_UE4_LE)	/* <cri_delete_if_LE> */
#include <cri_xpt.h>
#include <cri_file_system.h>
#else	/* </cri_delete_if_LE> */
#include <cri_le_xpt.h>
#include <cri_le_file_system.h>
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
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/
namespace CriWareFileIo
{
	/* I/Oインターフェース選択関数 */
	CriError SelectIo(const CriChar8 *Filename,
		CriFsDeviceId *DeviceId, CriFsIoInterfacePtr *IoInterface);
};

/***************************************************************************
 *      クラス宣言
 *      Prototype Classes
 ***************************************************************************/

/* --- end of file --- */
