/*
 * video_capturer_active.cpp
 *
 * History:
 *    2014/11/03 - [Zhi He] create file
 *
 * Copyright (C) 2014 - 2024, the Ambarella Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of the Ambarella Inc.
 */

#include "common_config.h"

//#ifdef DCONFIG_COMPILE_NOT_FINISHED_MODULE

#include "common_types.h"
#include "common_osal.h"
#include "common_utils.h"
#include "common_log.h"

#include "common_base.h"

#include "media_mw_if.h"
#include "mw_internal.h"
#include "media_mw_utils.h"

#include "framework_interface.h"
#include "filters_interface.h"
#include "dsp_platform_interface.h"
#include "streaming_if.h"
#include "modules_interface.h"

#include "video_capturer_active.h"

DCONFIG_COMPILE_OPTION_CPPFILE_IMPLEMENT_BEGIN

IFilter *gfCreateVideoCapturerActiveFilter(const TChar *pName, const volatile SPersistMediaConfig *pPersistMediaConfig, IMsgSink *pEngineMsgSink, TU32 index)
{
    return CVideoCapturerActive::Create(pName, pPersistMediaConfig, pEngineMsgSink, index);
}

static EVideoInputModuleID _guess_video_capturer_type_from_string(TChar *string)
{
    if (!string) {
        LOG_NOTICE("NULL input in _guess_video_capturer_type_from_string, choose default\n");
        return EVideoInputModuleID_WinDeskDup;
    }

    if (!strncmp(string, DNameTag_AMBA, strlen(DNameTag_AMBA))) {
        return EVideoInputModuleID_AMBADSP;
    } else if (!strncmp(string, DNameTag_WDup, strlen(DNameTag_WDup))) {
        return EVideoInputModuleID_WinDeskDup;
    }

    LOG_WARN("unknown string tag(%s) in _guess_video_capturer_type_from_string, choose default\n", string);
    return EVideoInputModuleID_WinDeskDup;
}

//-----------------------------------------------------------------------
//
// CVideoCapturerActive
//
//-----------------------------------------------------------------------

IFilter *CVideoCapturerActive::Create(const TChar *pName, const volatile SPersistMediaConfig *pPersistMediaConfig, IMsgSink *pEngineMsgSink, TUint index)
{
    CVideoCapturerActive *result = new CVideoCapturerActive(pName, pPersistMediaConfig, pEngineMsgSink, index);
    if ((result) && (EECode_OK != result->Construct())) {
        result->Delete();
        result = NULL;
    }
    return result;
}

CVideoCapturerActive::CVideoCapturerActive(const TChar *pName, const volatile SPersistMediaConfig *pPersistMediaConfig, IMsgSink *pEngineMsgSink, TU32 index)
    : inherited(pName, index, pPersistMediaConfig, pEngineMsgSink)
    , mpCapturer(NULL)
    , mbContentSetup(0)
    , mbCaptureFirstFrame(0)
    , mbRunning(0)
    , mbStarted(0)
    , mnCurrentOutputPinNumber(0)
    , mpOutputPin(NULL)
    , mpBufferPool(NULL)
    , mpBuffer(NULL)
    , mCapFramerateNum(DDefaultVideoFramerateNum)
    , mCapFramerateDen(DDefaultVideoFramerateDen)
    , mCaptureDuration(50000)
    , mSkipedFrameCount(0)
    , mCapturedFrameCount(0)
    , mFirstFrameTime(0)
    , mCurTime(0)
    , mEstimatedNextFrameTime(0)
    , mSkipThreashold(0)
    , mWaitThreashold(0)
    , mCurModuleID(EVideoInputModuleID_Auto)
{

}

EECode CVideoCapturerActive::Construct()
{
    DSET_MODULE_LOG_CONFIG(ELogModuleVideoCapturerFilter);
    mPersistEOSBuffer.SetBufferType(EBufferType_FlowControl_EOS);

    memset(&mModuleParams, 0, sizeof(SVideoCaptureParams));

    if (mpPersistMediaConfig->video_capture_config.framerate_num && mpPersistMediaConfig->video_capture_config.framerate_den) {
        mCapFramerateNum = mpPersistMediaConfig->video_capture_config.framerate_num;
        mCapFramerateDen = mpPersistMediaConfig->video_capture_config.framerate_den;
    }

    return inherited::Construct();
}

CVideoCapturerActive::~CVideoCapturerActive()
{

}

void CVideoCapturerActive::Delete()
{
    if (mpCapturer) {
        mpCapturer->Destroy();
        mpCapturer = NULL;
    }

    if (mpOutputPin) {
        mpOutputPin->Delete();
        mpOutputPin = NULL;
    }

    if (mpBufferPool) {
        mpBufferPool->Delete();
        mpBufferPool = NULL;
    }

    inherited::Delete();
}

EECode CVideoCapturerActive::Initialize(TChar *p_param)
{
    EVideoInputModuleID id;
    EECode err = EECode_OK;
    id = _guess_video_capturer_type_from_string(p_param);

    LOGM_INFO("Initialize() start, id %d\n", id);

    if (mbContentSetup) {

        LOGM_INFO("Initialize() start, there's a decoder already\n");

        DASSERT(mpCapturer);
        if (!mpCapturer) {
            LOGM_FATAL("[Internal bug], why the mpCapturer is NULL here?\n");
            return EECode_InternalLogicalBug;
        }

        LOGM_INFO("before mpCapturer->DestroyContext()\n");
        mpCapturer->DestroyContext();
        mbContentSetup = 0;

        if (id != mCurModuleID) {
            LOGM_INFO("before mpCapturer->Delete(), cur id %d request id %d\n", (TU32)mCurModuleID, (TU32)id);
            mpCapturer->Destroy();
            mpCapturer = NULL;
        }
    }

    if (!mpCapturer) {
        LOGM_INFO("Initialize() start, before gfModuleFactoryVideoInput(%d)\n", (TU32)id);
        mpCapturer = gfModuleFactoryVideoInput(id, mpPersistMediaConfig, mpEngineMsgSink);
        if (mpCapturer) {
            mCurModuleID = id;
        } else {
            mCurModuleID = EVideoInputModuleID_Auto;
            LOGM_FATAL("[Internal bug], request gfModuleFactoryVideoInput(%d) fail\n", (TU32)id);
            return EECode_InternalLogicalBug;
        }
    }

    LOGM_INFO("before mpCapturer->SetupContext()\n");
    err = mpCapturer->SetupContext(&mModuleParams);
    if (DUnlikely(EECode_OK != err)) {
        LOGM_ERROR("mpCapturer->SetupContext() failed.\n");
        return err;
    }
    mbContentSetup = 1;

    LOGM_INFO("Initialize() done\n");
    return EECode_OK;
}

EECode CVideoCapturerActive::Finalize()
{
    if (mpCapturer) {

        LOGM_INFO("before mpCapturer->DestroyContext()\n");
        mpCapturer->DestroyContext();
        mbContentSetup = 0;

        LOGM_INFO("before mpCapturer->Delete(), cur id %d\n", mCurModuleID);
        mpCapturer->Destroy();
        mpCapturer = NULL;
    }

    return EECode_OK;
}

EECode CVideoCapturerActive::SwitchInput(TComponentIndex focus_input_index)
{
    LOGM_FATAL("TO DO\n");
    return EECode_NoImplement;
}

EECode CVideoCapturerActive::FlowControl(EBufferType flowcontrol_type)
{
    LOGM_FATAL("TO DO\n");

    return EECode_NoImplement;
}

EECode CVideoCapturerActive::AddInputPin(TUint &index, TUint type)
{
    LOGM_FATAL("Not support yet\n");

    return EECode_InternalLogicalBug;
}

EECode CVideoCapturerActive::AddOutputPin(TUint &index, TUint &sub_index, TUint type)
{
    DASSERT(StreamType_Video == type);

    if (StreamType_Video == type) {
        if (mpOutputPin == NULL) {
            mpOutputPin = COutputPin::Create("[Video output pin for CVideoCapturerActive filter]", (IFilter *) this);

            if (!mpOutputPin)  {
                LOGM_FATAL("COutputPin::Create() fail?\n");
                return EECode_NoMemory;
            }

            mpBufferPool = CBufferPool::Create("[Buffer pool for video output pin in CVideoCapturerActive filter]", 8);
            if (!mpBufferPool)  {
                LOGM_FATAL("CBufferPool::Create() fail?\n");
                return EECode_NoMemory;
            }

            mpOutputPin->SetBufferPool(mpBufferPool);
        }

        index = 0;
        if (mpOutputPin->AddSubPin(sub_index) != EECode_OK) {
            LOGM_FATAL("COutputPin::AddSubPin() fail?\n");
            return EECode_Error;
        }

    } else {
        LOGM_ERROR("BAD output pin type %d\n", type);
        return EECode_InternalLogicalBug;
    }

    return EECode_OK;
}

IOutputPin *CVideoCapturerActive::GetOutputPin(TUint index, TUint sub_index)
{
    if (DLikely(!index)) {
        if (DLikely(mpOutputPin)) {
            if (DLikely(sub_index < mpOutputPin->NumberOfPins())) {
                return mpOutputPin;
            } else {
                LOGM_FATAL("BAD sub_index %d\n", sub_index);
            }
        } else {
            LOGM_FATAL("NULL mpOutputPin\n");
        }
    } else {
        LOGM_FATAL("BAD index %d\n", index);
    }

    return NULL;
}

IInputPin *CVideoCapturerActive::GetInputPin(TUint index)
{
    return NULL;
}

EECode CVideoCapturerActive::FilterProperty(EFilterPropertyType property_type, TUint set_or_get, void *p_param)
{
    EECode ret = EECode_OK;

    switch (property_type) {

        default:
            LOGM_FATAL("BAD property 0x%08x\n", property_type);
            ret = EECode_InternalLogicalBug;
            break;
    }

    return ret;
}

void CVideoCapturerActive::GetInfo(INFO &info)
{
    info.mPriority = 0;
    info.mFlags = 0;
    info.nInput = 0;
    info.nOutput = 1;

    info.pName = mpModuleName;
}

void CVideoCapturerActive::PrintStatus()
{
    LOGM_PRINTF("skiped %lld, captured %lld, heart beat %d\n", mSkipedFrameCount, mCapturedFrameCount, mDebugHeartBeat);

    if (mpOutputPin) {
        LOGM_PRINTF("       mpOutputPin free buffer count %d\n", mpOutputPin->GetBufferPool()->GetFreeBufferCnt());
    }

    mDebugHeartBeat = 0;
}

void CVideoCapturerActive::OnRun()
{
    SCMD cmd;
    EECode err = EECode_OK;

    mpWorkQ->CmdAck(EECode_OK);
    //LOGM_INFO("OnRun loop, start\n");

    mbRun = 1;
    msState = EModuleState_WaitCmd;

    while (mbRun) {
        //LOGM_STATE("OnRun: start switch, msState=%d, %s\n", msState, gfGetModuleStateString(msState));

        switch (msState) {

            case EModuleState_WaitCmd:
                mpWorkQ->WaitMsg(&cmd, sizeof(cmd));
                processCmd(cmd);
                break;

            case EModuleState_Halt:
                mpWorkQ->WaitMsg(&cmd, sizeof(cmd));
                processCmd(cmd);
                break;

            case EModuleState_WaitTiming:
                mpWorkQ->WaitMsg(&cmd, sizeof(cmd));
                processCmd(cmd);
                break;

            case EModuleState_Error:
            case EModuleState_Completed:
            case EModuleState_Pending:
                mpWorkQ->WaitMsg(&cmd, sizeof(cmd));
                processCmd(cmd);
                break;

            case EModuleState_Running:
                if (!mpBuffer) {
                    mpOutputPin->AllocBuffer(mpBuffer, sizeof(CIBuffer));
                }

                while (mpWorkQ->PeekCmd(cmd)) {
                    processCmd(cmd);
                }
                if (DUnlikely(EModuleState_Running != msState)) {
                    break;
                }

                mCurTime = mpClockReference->GetCurrentTime();
                if ((mCurTime + mWaitThreashold) < (mEstimatedNextFrameTime)) {
                    mpClockReference->AddTimer(this, EClockTimerType_once, mEstimatedNextFrameTime, mCaptureDuration, 1);
                    msState = EModuleState_WaitTiming;
                    break;
                }

                if (mCurTime > (mEstimatedNextFrameTime + mSkipThreashold)) {
                    mSkipedFrameCount ++;
                    mEstimatedNextFrameTime += mCaptureDuration;
                }

                err = mpCapturer->Capture(mpBuffer);
                if (DUnlikely(EECode_OK != err)) {
                    mpBuffer->SetBufferFlags(CIBuffer::DUP_FRAME);
                    //LOGM_WARN("send duplicate frame\n");
                }

                if (DLikely(mCapturedFrameCount)) {
                    TTime time = mCurTime - mFirstFrameTime;
                    mpBuffer->SetBufferDTS(time);
                    mpBuffer->SetBufferPTS(time);
                    time = (time * 9) / 100;
                    mpBuffer->SetBufferNativeDTS(time);
                    mpBuffer->SetBufferNativePTS(time);
                    mpBuffer->SetBufferLinearDTS(time);
                    mpBuffer->SetBufferLinearPTS(time);
                    //LOGM_PRINTF("capture, set pts %lld\n", time);
                } else {
                    mFirstFrameTime = mCurTime;
                    mpBuffer->SetBufferDTS(0);
                    mpBuffer->SetBufferPTS(0);
                    mpBuffer->SetBufferNativeDTS(0);
                    mpBuffer->SetBufferNativePTS(0);
                    mpBuffer->SetBufferLinearDTS(0);
                    mpBuffer->SetBufferLinearPTS(0);
                }
                //LOGM_NOTICE("[debug flow]: capture done, send it\n");
                mpBuffer->SetBufferType(EBufferType_VideoFrameBuffer);
                mpOutputPin->SendBuffer(mpBuffer);
                mpBuffer = NULL;

                mCapturedFrameCount ++;
                mEstimatedNextFrameTime += mCaptureDuration;
                break;

            default:
                LOGM_ERROR("state invalid: 0x%x\n", (TUint)msState);
                msState = EModuleState_Error;
                break;
        }

        mDebugHeartBeat ++;
    }
}

void CVideoCapturerActive::EventNotify(EEventType type, TU64 param1, TPointer param2)
{
    switch (type) {

        case EEventType_Timer:
            mpWorkQ->PostMsg(EMSGType_TimeNotify, NULL);
            break;

        default:
            LOG_FATAL("to do\n");
            break;
    }
}

void CVideoCapturerActive::processCmd(SCMD &cmd)
{
    //LOGM_FLOW("processCmd, cmd.code %d, %s, state %d, %s.\n", cmd.code, gfGetCMDTypeString((ECMDType)cmd.code), msState, gfGetModuleStateString(msState));

    switch (cmd.code) {

        case ECMDType_ExitRunning:
            flushData();
            mbRun = 0;
            msState = EModuleState_Halt;
            mpWorkQ->CmdAck(EECode_OK);
            break;

        case ECMDType_Stop:
            flushData();
            msState = EModuleState_WaitCmd;
            mpWorkQ->CmdAck(EECode_OK);
            //LOGM_INFO("processCmd, ECMDType_Stop.\n");
            break;

        case ECMDType_Pause:
            mbPaused = 1;
            break;

        case ECMDType_Resume:
            mbPaused = 0;
            break;

        case ECMDType_Flush:
            flushData();
            msState = EModuleState_Pending;
            mpWorkQ->CmdAck(EECode_OK);
            break;

        case ECMDType_ResumeFlush:
            msState = EModuleState_Idle;
            mpWorkQ->CmdAck(EECode_OK);
            break;

        case ECMDType_NotifySynced:
            mpWorkQ->CmdAck(EECode_OK);
            break;

        case ECMDType_Start:
            if (EModuleState_WaitCmd == msState) {
                beginCapture(mCapFramerateNum, mCapFramerateDen);
                msState = EModuleState_Running;
            }
            mpWorkQ->CmdAck(EECode_OK);
            break;

        case EMSGType_TimeNotify:
            if (EModuleState_WaitTiming == msState) {
                msState = EModuleState_Running;
            }
            break;

        default:
            LOGM_ERROR("processCmd, wrong cmd %d, %s.\n", cmd.code, gfGetCMDTypeString((ECMDType)cmd.code));
            break;
    }
}

void CVideoCapturerActive::flushData()
{
    if (mpBuffer) {
        mpBuffer->Release();
        mpBuffer = NULL;
    }
}

void CVideoCapturerActive::beginCapture(TU32 fr_num, TU32 fr_den)
{
    if (DLikely(fr_num && fr_den)) {
        mCaptureDuration = (TTime)(((TTime)fr_den * (TTime)1000000) / (TTime)fr_num);
        LOGM_NOTICE("video cap fr %d/%d, duration %lld\n", fr_num, fr_den, mCaptureDuration);
    } else {
        mCaptureDuration = 50000;
        LOGM_WARN("not valid params for cap fr, use default duration %lld\n", mCaptureDuration);
    }

    mSkipThreashold = mCaptureDuration;
    mWaitThreashold = mCaptureDuration / 2;

    mSkipedFrameCount = 0;
    mCapturedFrameCount = 0;
}

DCONFIG_COMPILE_OPTION_CPPFILE_IMPLEMENT_END

//#endif

