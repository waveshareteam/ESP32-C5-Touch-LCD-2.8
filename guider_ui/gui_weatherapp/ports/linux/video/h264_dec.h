/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __H264_DEC_H__
#define __H264_DEC_H__

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include "lv_conf.h"
#if LV_USE_VIDEO

#include "wels/codec_api.h"
#include "wels/codec_def.h"
#include <stdint.h>
#include <stddef.h>

int OpenH264_Init(void);
int OpenH264_Decode(const unsigned char* pSrc, const int iSrcLen, unsigned char** ppDst, SBufferInfo* pDstInfo);
void OpenH264_GetOption(void* pOption);
void OpenH264_FlashFrame(unsigned char** ppDst, SBufferInfo* pDstInfo);
void OpenH264_Uninit(void);

#endif /* LV_USE_VIDEO */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* __H264_DEC_H__ */
