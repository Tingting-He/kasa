/**
 * common_file_io.cpp
 *
 * History:
 *  2013/09/09 - [Zhi He] create file
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
#include "common_file_io.h"

DCONFIG_COMPILE_OPTION_CPPFILE_IMPLEMENT_BEGIN
DCODE_DELIMITER;

IIO *CFileIO::Create()
{
    CFileIO *result = new CFileIO();
    if ((result) && (EECode_OK != result->Construct())) {
        result->Delete();
        result = NULL;
    }
    return result;
}

void CFileIO::Delete()
{
    if (mpFile) {
        fclose(mpFile);
        mpFile = NULL;
    }
}

CFileIO::CFileIO()
    : mpFile(NULL)
    , mpFileName(NULL)
    , mnFileNameSize(0)
    , mInitFlag(0x0)
    , mReadTotalFileSize(0)
    , mWirteCurrentFileSize(0)
{
}

CFileIO::~CFileIO()
{
    if (mpFile) {
        fclose(mpFile);
        mpFile = NULL;
    }
}

EECode CFileIO::Construct()
{
    return EECode_OK;
}

EECode CFileIO::Open(TChar *name, TU32 flags)
{
    const TChar *pmode = NULL;
    DASSERT(name);
    if (!name) {
        LOG_FATAL("NULL param\n");
        return EECode_BadParam;
    }

    DASSERT(!mpFile);
    if (mpFile) {
        fclose(mpFile);
        mpFile = NULL;
    }

    if (flags & EIOFlagBit_Read) {
        if (flags & EIOFlagBit_Binary) {
            pmode = "rb";
            mInitFlag = EIOFlagBit_Read | EIOFlagBit_Binary;
        } else if (flags & EIOFlagBit_Text) {
            pmode = "rt";
            mInitFlag = EIOFlagBit_Read | EIOFlagBit_Text;
        } else {
            LOG_ERROR("not specified read file's format, txt or binary\n");
            return EECode_BadParam;
        }
    } else if (flags & EIOFlagBit_Write) {
        if (flags & EIOFlagBit_Append) {
            if (flags & EIOFlagBit_Binary) {
                pmode = "wb+";
                mInitFlag = EIOFlagBit_Write | EIOFlagBit_Append | EIOFlagBit_Binary;
            } else if (flags & EIOFlagBit_Text) {
                pmode = "wt+";
                mInitFlag = EIOFlagBit_Write | EIOFlagBit_Append | EIOFlagBit_Text;
            } else {
                LOG_ERROR("not specified write append file's format, txt or binary\n");
                return EECode_BadParam;
            }
        } else {
            if (flags & EIOFlagBit_Binary) {
                pmode = "wb";
                mInitFlag = EIOFlagBit_Write | EIOFlagBit_Binary;
            } else if (flags & EIOFlagBit_Text) {
                pmode = "wt";
                mInitFlag = EIOFlagBit_Write | EIOFlagBit_Write;
            } else {
                LOG_ERROR("not specified write file's format, txt or binary\n");
                return EECode_BadParam;
            }
        }
    } else {
        LOG_ERROR("not specified file's operation, read or write\n");
        return EECode_BadParam;
    }

    mpFile = fopen((const TChar *)name, pmode);
    if (!mpFile) {
        perror("fopen");
        //LOG_NOTICE("open file fail, %s, pmode %s\n", name, pmode);
        return EECode_Error;
    }

    fseek(mpFile, 0, SEEK_END);
    mTotalFileSize = (TFileSize)ftell(mpFile);
    fseek(mpFile, 0, SEEK_SET);

    return EECode_OK;
}

EECode CFileIO::Close()
{
    if (mpFile) {
        fclose(mpFile);
        mpFile = NULL;
        mInitFlag = 0;
    }

    return EECode_OK;
}

EECode CFileIO::SetProperty(TIOSize write_block_size, TIOSize read_block_size)
{
    LOG_FATAL("TO DO\n");
    return EECode_OK;
}

EECode CFileIO::GetProperty(TIOSize &write_block_size, TIOSize &read_block_size) const
{
    LOG_FATAL("TO DO\n");
    return EECode_OK;
}

EECode CFileIO::Write(TU8 *pdata, TU32 size, TIOSize count, TU8 sync)
{
    DASSERT(count > 0);
    DASSERT(size);
    DASSERT(pdata);
    if (DUnlikely((!pdata) || (!size) || (0 >= count))) {
        LOG_FATAL("bad parameters pdata %p, size %d, count %lld\n", pdata, size, count);
        return EECode_BadParam;
    }

    DASSERT(mpFile);
    if (DUnlikely((!mpFile) || !(mInitFlag & EIOFlagBit_Write))) {
        LOG_FATAL("NULL mpFile %p, or mInitFlag (%08x) no write flag bit\n", mpFile, mInitFlag);
        return EECode_BadState;
    }

    //DASSERT(1 == sync);
    if (DUnlikely(0 == fwrite((void *)pdata, size, count, mpFile))) {
        LOG_ERROR("write data fail, size %d, count %lld\n", size, count);
        return EECode_IOError;
    }

    return EECode_OK;
}

EECode CFileIO::Read(TU8 *pdata, TU32 size, TIOSize &count)
{
    DASSERT(count > 0);
    DASSERT(size);
    DASSERT(pdata);
    if (DUnlikely((!pdata) || (!size) || (0 >= count))) {
        LOG_FATAL("bad parameters pdata %p, size %d, count %lld\n", pdata, size, count);
        return EECode_BadParam;
    }

    DASSERT(mpFile);
    if (DUnlikely((!mpFile) || !(mInitFlag & EIOFlagBit_Read))) {
        LOG_FATAL("NULL mpFile %p, or mInitFlag (%08x) no read flag bit\n", mpFile, mInitFlag);
        return EECode_BadState;
    }

    TIOSize ret = 0;
    ret = fread((void *)pdata, size, count, mpFile);
    if (DUnlikely(ret == 0)) {
        LOG_ERROR("read data fail, size %d, count %lld\n", size, count);
        return EECode_IOError;
    }
    count = ret;

    return EECode_OK;
}

EECode CFileIO::Seek(TIOSize offset, EIOPosition posision)
{
    DASSERT(mpFile);
    if (mpFile) {
        if (EIOPosition_Begin == posision) {
            fseek(mpFile, offset, SEEK_SET);
        } else if (EIOPosition_Current == posision) {
            fseek(mpFile, offset, SEEK_CUR);
        } else if (EIOPosition_End == posision) {
            fseek(mpFile, offset, SEEK_END);
        } else {
            LOG_FATAL("not specified posision %d\n", posision);
            return EECode_BadParam;
        }
    }

    return EECode_OK;
}

void CFileIO::Sync()
{
    DASSERT(mpFile);
    if (mpFile) {
        fflush(mpFile);
    }
}

EECode CFileIO::Query(TIOSize &total_size, TIOSize &current_posotion) const
{
    total_size = mTotalFileSize;
    current_posotion = (TIOSize)ftell(mpFile);
    return EECode_OK;
}

DCONFIG_COMPILE_OPTION_CPPFILE_IMPLEMENT_END

