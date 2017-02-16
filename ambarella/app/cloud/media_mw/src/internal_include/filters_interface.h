/**
 * filters_interface.h
 *
 * History:
 *  2012/12/12 - [Zhi He] create file
 *
 * Copyright (C) 2014 - 2024, the Ambarella Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of the Ambarella Inc.
 */

#ifndef __FILTERS_INTERFACES_H__
#define __FILTERS_INTERFACES_H__

DCONFIG_COMPILE_OPTION_HEADERFILE_BEGIN
DCODE_DELIMITER;

typedef enum {
    EFilterType_Invalid = 0,

    EFilterType_Demuxer = 0x01,
    EFilterType_VideoDecoder = 0x02,
    EFilterType_ScheduledVideoDecoder = 0x03,
    EFilterType_AudioDecoder = 0x04,
    EFilterType_VideoEncoder = 0x05,
    EFilterType_AudioEncoder = 0x06,
    EFilterType_Muxer = 0x07,
    EFilterType_ScheduledMuxer = 0x08,
    EFilterType_VideoPreProcessor = 0x09,
    EFilterType_AudioPreProcessor = 0x0a,
    EFilterType_VideoPostProcessor = 0x0b,
    EFilterType_AudioPostProcessor = 0x0c,
    EFilterType_VideoCapture = 0x0d,
    EFilterType_AudioCapture = 0x0e,
    EFilterType_VideoRenderer = 0x0f,
    EFilterType_AudioRenderer = 0x10,
    EFilterType_RTSPTransmiter = 0x11,
    EFilterType_ConnecterPinmuxer = 0x12,
    EFilterType_CloudConnecterServerAgent = 0x13,
    EFilterType_CloudConnecterClientAgent = 0x14,
    EFilterType_CloudConnecterCmdAgent = 0x15,
    EFilterType_FlowController = 0x16,
    EFilterType_VideoOutput = 0x17,
    EFilterType_AudioOutput = 0x18,
    EFilterType_ExtVideoSource = 0x19,
    EFilterType_ExtAudioSource = 0x1a,

} EFilterType;

extern IFilter *gfFilterFactory(EFilterType type, const volatile SPersistMediaConfig *pPersistMediaConfig, IMsgSink *pMsgSink, TU32 index = 0, TU32 version = 2);

DCONFIG_COMPILE_OPTION_HEADERFILE_END

#endif