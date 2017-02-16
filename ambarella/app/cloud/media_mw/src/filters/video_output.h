/*
 * video_output.h
 *
 * History:
 *    2014/10/14 - [Zhi He] create file
 *
 * Copyright (C) 2014 - 2024, the Ambarella Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of the Ambarella Inc.
 */

#ifndef __VIDEO_OUTPUT_H__
#define __VIDEO_OUTPUT_H__

DCONFIG_COMPILE_OPTION_HEADERFILE_BEGIN
DCODE_DELIMITER;

//-----------------------------------------------------------------------
//
// CVideoOutput
//
//-----------------------------------------------------------------------
class CVideoOutput: public CActiveFilter
{
    typedef CActiveFilter inherited;

public:
    static IFilter *Create(const TChar *pName, const volatile SPersistMediaConfig *pPersistMediaConfig, IMsgSink *pEngineMsgSink, TU32 index);

protected:
    CVideoOutput(const TChar *pName, const volatile SPersistMediaConfig *pPersistMediaConfig, IMsgSink *pEngineMsgSink, TU32 index);

    EECode Construct();
    virtual ~CVideoOutput();

public:
    virtual void Delete();
    virtual void Pause();
    virtual void Resume();
    virtual void Flush();
    virtual void ResumeFlush();

    virtual EECode Suspend(TUint release_context = 0);
    virtual EECode ResumeSuspend(TComponentIndex input_index = 0);

    virtual EECode SwitchInput(TComponentIndex focus_input_index = 0);

    virtual void PrintState();

    virtual EECode Run();

    virtual EECode Start();
    virtual EECode Stop();

    virtual EECode FlowControl(EBufferType type);

    virtual IInputPin *GetInputPin(TUint index);
    virtual IOutputPin *GetOutputPin(TUint index, TUint sub_index);
    virtual EECode AddInputPin(TUint &index, TUint type);
    virtual EECode AddOutputPin(TUint &index, TUint &sub_index, TUint type);

    virtual void EventNotify(EEventType type, TU64 param1, TPointer param2);
    virtual EECode FilterProperty(EFilterPropertyType property_type, TUint set_or_get, void *p_param);

protected:
    virtual void OnRun();

public:
    virtual EECode Initialize(TChar *p_param = NULL);
    virtual EECode Finalize();
    virtual void PrintStatus();
    void GetInfo(INFO &info);

private:
    EECode flushInputData();
    EECode processCmd(SCMD &cmd);
    void renderBuffer();
    void initSync();
    TTime getEstimatedNextFrameTime();
    void sendFPSStatistics();

private:
    IVisualDirectRendering *mpRenderer;
    CQueueInputPin *mpInputPins[EConstVideoRendererMaxInputPinNumber];
    CQueueInputPin *mpCurrentInputPin;
    TU32 mnInputPinsNumber;

private:
    CIBuffer *mpBuffer;
    CIBuffer *mpRenderingBuffer;
    SClockListener *mpTimer;

private:
    TU8 mbSendFPSStatistics;
    TU8 mbPaused;
    TU8 mbEOS;
    TU8 mbStepMode;

private:
    TU8 mSyncStrategy;
    TU8 mbSyncStarted;
    TU8 mSyncOverflowThreshold;
    TU8 mSyncInitialCachedFrameCount;

    TTime mSyncPlaybackCurTime;
    TTime mSyncBaseTime;
    TTime mSyncDriftTime;
    TTime mSyncEstimatedNextFrameTime;
    TTime mSyncFrameDurationNum;
    TTime mSyncFrameDurationDen;

    TU64 mCurrentFramCount;

    TU64 mStasticsFramCount;
    TTime mStasticsBeginTime;
    TTime mStasticsEndTime;
};

DCONFIG_COMPILE_OPTION_HEADERFILE_END

#endif


