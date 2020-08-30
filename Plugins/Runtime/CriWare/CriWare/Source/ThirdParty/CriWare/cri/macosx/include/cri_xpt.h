/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 1998-2011 CRI Middleware Co., Ltd.
 *
 * Library  : CRI Middleware Library
 * Module   : CRI Common Header for MacOSX
 * File     : cri_xpt.h
 * Date     : 2014-11-06
 * Version  : 1.06
 *
 ****************************************************************************/

#ifndef CRI_INCL_CRI_XPT_H
#define CRI_INCL_CRI_XPT_H

/* 
   コンパイラオプションの指定（-arch 386 or -arch ppc）により定義される
   シンボルでターゲットを決める
*/
#if defined( __i386__ ) || defined( __x86_64__ )
	#define XPT_TGT_MACINTEL
	#define XPT_CCS_LEND

    #if defined(__x86_64__)
        #define CRI_TARGET_STR "Mac_X86_64"
    #elif defined(__i386__)
        #define CRI_TARGET_STR "Mac_X86"
    #endif

#elif defined(__ppc__)
	#define XPT_TGT_MACPPC
    #define XPT_CCS_BEND
	#define CRI_TARGET_STR "Mac_PPC"

#else
    /* 定義が見つからない場合はビルドエラー */
    #error cri_xpt.h : CRI_TARGET_STR is not defined by reason that the compiler is not assumed.
#endif
#define XPT_TGT_MACOSX

#define TGT_STR CRI_TARGET_STR

#define XPT_SUPPORT_MULTICHANNEL

#include "cri_xpts_posix.h"
#include "cri_xpt_post.h"

#endif  /* CRI_INCL_CRI_XPT_H */
/* End Of File */
