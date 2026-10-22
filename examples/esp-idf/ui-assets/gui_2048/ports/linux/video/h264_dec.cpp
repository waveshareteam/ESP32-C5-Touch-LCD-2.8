/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "h264_dec.h"
#include <cstdio>

#ifdef __cplusplus
extern "C" {
#endif

#if LV_USE_VIDEO != 0

static ISVCDecoder *s_pDecoder;


int OpenH264_Init(void)
{
    int result               = WelsCreateDecoder(&s_pDecoder);
    SDecodingParam sDecParam = {0};

    if (result == 0)
    {
        sDecParam.uiTargetDqLayer             = 255;
        sDecParam.eEcActiveIdc                = ERROR_CON_SLICE_COPY;
        sDecParam.sVideoProperty.eVideoBsType = VIDEO_BITSTREAM_DEFAULT;
        result                                = s_pDecoder->Initialize(&sDecParam);
    }

    int iLevelSetting = (int) WELS_LOG_WARNING;
    s_pDecoder->SetOption (DECODER_OPTION_TRACE_LEVEL, &iLevelSetting);
    return (result == 0) ? 0 : -1;
}

int OpenH264_Decode(const unsigned char* pSrc, const int iSrcLen, unsigned char** ppDst, SBufferInfo* pDstInfo) {
    int rst;
    rst = s_pDecoder->DecodeFrameNoDelay(pSrc, iSrcLen, ppDst, pDstInfo);
    //DecodeFrame2
    return rst;
}

void OpenH264_GetOption(void* pOption)
{
	s_pDecoder->GetOption(DECODER_OPTION_NUM_OF_FRAMES_REMAINING_IN_BUFFER, pOption);
}

void OpenH264_FlashFrame(unsigned char** ppDst, SBufferInfo* pDstInfo)
{
	s_pDecoder->FlushFrame(ppDst, pDstInfo);
}


void OpenH264_Uninit(void)
{
    s_pDecoder->Uninitialize();
    WelsDestroyDecoder(s_pDecoder);
}

#endif /* LV_USE_VIDEO */

#ifdef __cplusplus
}
#endif
