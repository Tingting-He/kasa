/**
 * fileparser_aac.cpp
 *
 * History:
 *  2014/12/10 - [Zhi He] create file
 *
 * Copyright (C) 2014 - 2024, the Ambarella Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of the Ambarella Inc.
 */

#include "common_config.h"
#include "common_types.h"
#include "common_osal.h"
#include "common_utils.h"

#include "common_log.h"
#include "common_base.h"
#include "common_io.h"

#include "media_mw_if.h"
#include "codec_interface.h"

#include "fileparser_aac.h"

DCONFIG_COMPILE_OPTION_CPPFILE_IMPLEMENT_BEGIN
DCODE_DELIMITER;

IMediaFileParser *gfCreateBasicAACParser()
{
    return CAACFileParser::Create();
}

CAACFileParser *CAACFileParser::Create()
{
    CAACFileParser *result = new CAACFileParser();
    if ((result) && (EECode_OK != result->Construct())) {
        result->Delete();
        result = NULL;
    }
    return result;
}

void CAACFileParser::Delete()
{
    if (mpIO) {
        mpIO->Delete();
        mpIO = NULL;
    }

    if (mpReadBuffer) {
        DDBG_FREE(mpReadBuffer, "FPAA");
        mpReadBuffer = NULL;
    }
}

CAACFileParser::CAACFileParser()
    : mpIO(NULL)
    , mpReadBuffer(NULL)
    , mReadBufferSize(0)
    , mReadBlockSize(0)
    , mReadNextThreashold(0)
    , mpCurrentPtr(NULL)
    , mBufRemaingSize(0)
    , mFileTotalSize(0)
    , mFileCurrentOffset(0)
    , mbFileOpend(0)
    , mbFileEOF(0)
    , mbParseMediaInfo(0)
    , mbStartReadFile(0)
    , mAudioChannelNumber(0)
    , mAudioSampleSize(16)
    , mAudioSampleRate(0)
    , mAudioFrameSize(DDefaultAudioFrameSize)
{
}

CAACFileParser::~CAACFileParser()
{

}

EECode CAACFileParser::Construct()
{
    mReadBlockSize = 2 * 1024 * 1024;
    mReadNextThreashold = 256 * 1024;

    mReadBufferSize = mReadBlockSize + mReadNextThreashold;
    mpReadBuffer = (TU8 *) DDBG_MALLOC(mReadBufferSize, "FPAA");
    if (DUnlikely(!mpReadBuffer)) {
        LOG_FATAL("CAACFileParser::Construct(), no memory, request size %lld\n", mReadBufferSize);
        mReadBufferSize = 0;
        return EECode_NoMemory;
    }

    mpIO = gfCreateIO(EIOType_File);

    if (DUnlikely(!mpIO)) {
        LOG_FATAL("CAACFileParser::Construct(), gfCreateIO() fail\n");
        return EECode_NoMemory;
    }

    return EECode_OK;
}

EECode CAACFileParser::OpenFile(TChar *name)
{
    if (DUnlikely(mbFileOpend)) {
        LOG_WARN("close previous file first\n");
        mpIO->Close();
        mbFileOpend = 0;
    }

    EECode err = mpIO->Open(name, EIOFlagBit_Read | EIOFlagBit_Binary);
    if (DUnlikely(EECode_OK != err)) {
        LOG_ERROR("CAACFileParser::OpenFile(%s) fail, ret %d, %s\n", name, err, gfGetErrorCodeString(err));
        return err;
    }

    mpIO->Query(mFileTotalSize, mFileCurrentOffset);

    mbFileOpend = 1;

    return EECode_OK;
}

EECode CAACFileParser::CloseFile()
{
    if (DUnlikely(!mbFileOpend)) {
        LOG_WARN("file not opened, or already closed\n");
        return EECode_OK;
    }

    mpIO->Close();
    mbFileOpend = 0;

    return EECode_OK;
}

EECode CAACFileParser::SeekTo(TIOSize offset)
{
    if (DUnlikely(!mbFileOpend)) {
        LOG_ERROR("file not opened, or already closed\n");
        return EECode_BadState;
    }

    if (DUnlikely(offset >= mFileTotalSize)) {
        LOG_ERROR("CAACFileParser::SeekTo, offset >= total size\n");
        return EECode_BadParam;
    }

    mpIO->Seek(offset, EIOPosition_Begin);
    mFileCurrentOffset = offset;
    return EECode_OK;
}

EECode CAACFileParser::ReadPacket(SMediaPacket *packet)
{
    if ((!mbFileEOF) && (mBufRemaingSize < mReadNextThreashold)) {
        readDataFromFile();
    }

    if (mBufRemaingSize < 20) {
        DASSERT(mBufRemaingSize);
        return EECode_OK_EOF;
    }

    packet->sub_packet_number = 0;

    packet->data_len = gfGetADTSFrameLength(mpCurrentPtr);
    packet->p_data = mpCurrentPtr;
    packet->have_pts = 0;

    mpCurrentPtr += packet->data_len;
    mBufRemaingSize -= packet->data_len;

    return EECode_OK;
}

EECode CAACFileParser::GetMediaInfo(SMediaInfo *info)
{
    if (!mbStartReadFile) {
        readDataFromFile();
    }

    if (!mbParseMediaInfo) {
        LOG_ERROR("can not find extradata, this file maybe not aac file, or file is corrupted\n");
        return EECode_DataCorruption;
    }

    info->audio_channel_number = mAudioChannelNumber;
    info->audio_sample_size = mAudioSampleSize;

    info->audio_sample_rate = mAudioSampleRate;
    info->audio_frame_size = mAudioFrameSize;

    return EECode_OK;
}

EECode CAACFileParser::getMediaInfo()
{
    EECode err = gfParseADTSHeader(mpCurrentPtr, &mADTSHeader);
    if (DUnlikely(EECode_OK != err)) {
        LOG_ERROR("not aac file? gfParseADTSHeader fail ret %d, %s\n", err, gfGetErrorCodeString(err));
        return err;
    }

    mAudioChannelNumber = mADTSHeader.channel_configuration;
    mAudioProfile = mADTSHeader.profile;
    mAudioSampleRate = gfGetADTSSamplingFrequency(mADTSHeader.sampling_frequency_index);

    return EECode_OK;
}

EECode CAACFileParser::readDataFromFile()
{
    TIOSize read_buf_size = mReadBlockSize;
    memcpy(mpReadBuffer, mpCurrentPtr, mBufRemaingSize);
    mpCurrentPtr = mpReadBuffer;

    EECode err = mpIO->Read(mpReadBuffer + mBufRemaingSize, 1, read_buf_size);
    if (DUnlikely(EECode_OK != err)) {
        LOG_ERROR("CAACFileParser::ReadDataFromFile(), mpIO->Read() ret %d, %s\n", err, gfGetErrorCodeString(err));
        mbFileEOF = 1;
        return err;
    }

    mbStartReadFile = 1;
    mBufRemaingSize += read_buf_size;

    if (DUnlikely(!mbParseMediaInfo)) {
        mbParseMediaInfo = 1;
        err = getMediaInfo();
    }

    return EECode_OK;
}

DCONFIG_COMPILE_OPTION_CPPFILE_IMPLEMENT_END

