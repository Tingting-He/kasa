/**
 * codec_interface.cpp
 *
 * History:
 *  2013/11/18 - [Zhi He] create file
 *
 * Copyright (C) 2014 - 2024, the Ambarella Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of the Ambarella Inc.
 */

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "lwmd_if.h"
#include "lwmd_log.h"

#include "internal.h"

#include "codec_interface.h"

extern SCodecVideoH264 *gfGetVideoCodecH264Parser(TU8 *p_data, TMemSize data_size, EECode &ret);

SCodecVideoCommon *gfGetVideoCodecParser(TU8 *p_data, TMemSize data_size, StreamFormat format, EECode &ret)
{
    DASSERT(p_data);
    DASSERT(data_size);

    if (DLikely(p_data && data_size)) {
        switch (format) {
            case StreamFormat_H264:
                return (SCodecVideoCommon *)gfGetVideoCodecH264Parser(p_data, data_size, ret);
            default:
                LOG_ERROR("gfGetVideoCodecParser, unsupported format=%d\n", format);
                return NULL;
        }
    } else {
        LOG_ERROR("invalid params, p_data=%p data_size=%lu\n", p_data, data_size);
        return NULL;
    }
}

void gfReleaseVideoCodecParser(SCodecVideoCommon *parser)
{
    if (DLikely(parser)) {
        free(parser);
    } else {
        LOG_FATAL("NULL parser\n");
    }
}

TU32 gfGetADTSFrameLength(TU8 *p)
{
    return (((TU32)(p[3] & 0x3)) << 11) | (((TU32)p[4]) << 3) | (((TU32)p[5] >> 5) & 0x7);
}

EECode gfParseADTSHeader(TU8 *p, SADTSHeader *header)
{
    if (DUnlikely((!p) || (!header))) {
        LOG_ERROR("NULL parameters in gfParseADTSHeader\n");
        return EECode_BadParam;
    }

    if (DUnlikely((0xff != p[0]) || (0xf0 != (p[1] & 0xf0)))) {
        LOG_ERROR("not find sync byte in gfParseADTSHeader\n");
        return EECode_DataCorruption;
    }

    header->ID = (p[1] >> 3) & 0x01;
    header->layer = (p[1] >> 1) & 0x03;
    header->protection_absent = p[1] & 0x01;

    header->profile = (p[2] >> 6) & 0x03;
    header->sampling_frequency_index = (p[2] >> 2) & 0x0f;
    header->private_bit = (p[2] >> 1) & 0x01;
    header->channel_configuration = ((p[2] & 0x01) << 2) | ((p[3] >> 6) & 0x3);
    header->original_copy = (p[3] >> 5) & 0x01;
    header->home = (p[3] >> 4) & 0x01;

    header->copyright_identification_bit = (p[3] >> 3) & 0x01;
    header->copyright_identification_start = (p[3] >> 2) & 0x01;

    header->aac_frame_length = (((TU16)p[3] & 0x03) << 11) | ((TU16)p[4] << 3) | (((TU16)p[5] >> 5) & 0x7);

    header->adts_buffer_fullness = (((TU16)p[5] & 0x1f) << 6) | (((TU16)p[6] >> 2) & 0x3f);
    header->number_of_raw_data_blocks_in_frame = p[6] & 0x03;

    return EECode_OK;
}

TU32 gfGetADTSSamplingFrequency(TU8 sampling_frequency_index)
{
    switch (sampling_frequency_index) {

        case EADTSSamplingFrequency_96000:
            return 96000;
            break;

        case EADTSSamplingFrequency_88200:
            return 88200;
            break;

        case EADTSSamplingFrequency_64000:
            return 64000;
            break;

        case EADTSSamplingFrequency_48000:
            return 48000;
            break;

        case EADTSSamplingFrequency_44100:
            return 44100;
            break;

        case EADTSSamplingFrequency_32000:
            return 32000;
            break;

        case EADTSSamplingFrequency_24000:
            return 24000;
            break;

        case EADTSSamplingFrequency_22050:
            return 22050;
            break;

        case EADTSSamplingFrequency_16000:
            return 16000;
            break;

        case EADTSSamplingFrequency_12000:
            return 12000;
            break;

        case EADTSSamplingFrequency_11025:
            return 11025;
            break;

        case EADTSSamplingFrequency_8000:
            return 8000;
            break;

        case EADTSSamplingFrequency_7350:
            return 7350;
            break;

        default:
            break;
    }

    return 0;
}


