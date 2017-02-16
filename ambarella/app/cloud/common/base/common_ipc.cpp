/**
 * common_ipc.cpp
 *
 * History:
 *  2014/04/22 - [Zhi He] create file
 *
 * Copyright (C) 2014 - 2024, the Ambarella Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of the Ambarella Inc.
 */


#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "common_config.h"

#include "common_types.h"
#include "common_osal.h"
#include "common_utils.h"

#include "common_log.h"

#include "common_base.h"
#include "common_network_utils.h"
#include "common_ipc.h"

DCONFIG_COMPILE_OPTION_CPPFILE_IMPLEMENT_BEGIN
DCODE_DELIMITER;

IIPCAgent *gfCreateUnixDomainSocketIPCAgent()
{
    return CIPCUnixDomainSocketAgent::Create();
}

IIPCAgent *CIPCUnixDomainSocketAgent::Create()
{
    CIPCUnixDomainSocketAgent *result = new CIPCUnixDomainSocketAgent();
    if (result && result->Construct() != EECode_OK) {
        delete result;
        result = NULL;
    }
    return result;
}

CObject *CIPCUnixDomainSocketAgent::GetObject() const
{
    return (CObject *) this;
}

CIPCUnixDomainSocketAgent::CIPCUnixDomainSocketAgent()
    : CObject("CIPCUnixDomainSocketAgent", 0)
    , mpWorkQueue(NULL)
    , mPort(0)
    , mRemotePort(0)
    , mbRunning(0)
    , msState(EIPCAgentState_idle)
    , mIsServerMode(0)
    , mbConnecting(0)
    , mSocket(-1)
    , mTransferSocket(-1)
    , mMaxFd(-1)
    , mMaxWriteFd(-1)
    , mpReadBuffer(NULL)
    , mReadBufferSize(0)
    , mReadDataSize(0)
{
    memset(mTag, 0x0, DPathMax);
    memset(&mAddr, 0x0, sizeof(mAddr));
    mReadCallBack = NULL;
}

CIPCUnixDomainSocketAgent::~CIPCUnixDomainSocketAgent()
{
    if (mpReadBuffer) {
        DDBG_FREE(mpReadBuffer, "CIPC");
        mpReadBuffer = NULL;
    }
}

EECode CIPCUnixDomainSocketAgent::Construct()
{
    TInt ret = 0;

    //mConfigLogLevel = ELogLevel_Verbose;
    //mConfigLogOption = 0xffff;

    mpWorkQueue = CIWorkQueue::Create((IActiveObject *)this);
    if (DUnlikely(!mpWorkQueue)) {
        LOGM_ERROR("Create CIWorkQueue fail.\n");
        return EECode_NoMemory;
    }

    ret = pipe(mPipeFd);
    if (DUnlikely(0 > ret)) {
        LOGM_ERROR("pipe fail.\n");
        return EECode_OSError;
    }

    LOGM_INFO("before CIPCUnixDomainSocketAgent::mpWorkQueue->Run().\n");
    DASSERT(mpWorkQueue);
    mpWorkQueue->Run();
    LOGM_INFO("after CIPCUnixDomainSocketAgent::mpWorkQueue->Run().\n");

    return EECode_OK;
}

EECode CIPCUnixDomainSocketAgent::CreateContext(TChar *tag, TUint is_server, void *p_context, TIPCAgentReadCallBack callback, TU16 native_port, TU16 remote_port)
{
    if (DUnlikely(!tag)) {
        LOG_ERROR("NULL tag\n");
        return EECode_BadParam;
    }

    if (DIsSocketHandlerValid(mSocket)) {
        gfNetCloseSocket(mSocket);
        mSocket = DInvalidSocketHandler;
    }

    mSocket = socket(PF_UNIX, SOCK_SEQPACKET, 0);
    if (DUnlikely(0 > mSocket)) {
        LOG_ERROR("Failed to create a socket!\n");
        return EECode_OSError;
    }

    memset(&mAddr, 0, sizeof(struct sockaddr_un));

    mAddr.sun_family = AF_UNIX;
    snprintf(mAddr.sun_path, sizeof(mAddr.sun_path), "%s", tag);

    snprintf(mTag, sizeof(mAddr.sun_path), "%s", tag);

    if (is_server) {
        mIsServerMode = 1;
    } else {
        mIsServerMode = 0;

        mReadBufferSize = 16 * 1024;
        mpReadBuffer = (TU8 *) DDBG_MALLOC(mReadBufferSize, "CIPC");
        DASSERT(mpReadBuffer);
    }

    FD_ZERO(&mAllSet);
    FD_ZERO(&mAllWriteSet);
    FD_ZERO(&mReadSet);

    FD_SET(mPipeFd[0], &mAllSet);
    FD_SET(mSocket, &mAllSet);

    if (mPipeFd[0] > mSocket) {
        mMaxFd = mPipeFd[0];
    } else {
        mMaxFd = mSocket;
    }

    mpReadCallBackContext = p_context;
    mReadCallBack = callback;

    return EECode_OK;
}

void CIPCUnixDomainSocketAgent::DestroyContext()
{
    if (DIsSocketHandlerValid(mSocket)) {
        gfNetCloseSocket(mSocket);
        mSocket = DInvalidSocketHandler;
    }

    if (mTag) {
        unlink(mTag);
    }
}

TInt CIPCUnixDomainSocketAgent::GetHandle() const
{
    return mSocket;
}

EECode CIPCUnixDomainSocketAgent::Start()
{
    EECode err = EECode_OK;
    DASSERT(mpWorkQueue);

    LOGM_INFO("CIPCUnixDomainSocketAgent::Start start.\n");

    TChar wake_char = 'a';
    size_t ret = 0;

    ret = write(mPipeFd[1], &wake_char, 1);
    DASSERT(1 == ret);

    err = mpWorkQueue->PostMsg(ECMDType_Start, NULL);
    LOGM_INFO("CIPCUnixDomainSocketAgent::Start end, ret %d.\n", err);

    return EECode_OK;
}

EECode CIPCUnixDomainSocketAgent::Stop()
{
    EECode err = EECode_OK;
    DASSERT(mpWorkQueue);

    LOGM_INFO("CIPCUnixDomainSocketAgent::Stop start.\n");

    TChar wake_char = 'b';
    size_t ret = 0;

    ret = write(mPipeFd[1], &wake_char, 1);
    DASSERT(1 == ret);

    err = mpWorkQueue->SendCmd(ECMDType_Stop, NULL);
    LOGM_INFO("CIPCUnixDomainSocketAgent::Stop end, ret %d.\n", err);
    return err;
}

EECode CIPCUnixDomainSocketAgent::Write(TU8 *data, TInt size)
{
    if (DLikely(0 <= mTransferSocket)) {
        SCMD cmd;
        TChar wake_char = 'w';
        size_t ret = 0;

        ret = write(mPipeFd[1], &wake_char, 1);
        DASSERT(1 == ret);

        SDataPiece piece;
        cmd.code = ECMDType_SendData;
        cmd.needFreePExtra = 0;
        piece.p_data = data;
        piece.data_size = size;
        cmd.pExtra = &piece;

        EECode err = mpWorkQueue->SendCmd(cmd);
        return err;
    }

    return EECode_OK;
}

void CIPCUnixDomainSocketAgent::Destroy()
{
    delete this;
}

void CIPCUnixDomainSocketAgent::Delete()
{
    CObject::Delete();
}

void CIPCUnixDomainSocketAgent::OnRun()
{
    SCMD cmd;
    TInt nready = 0;
    EECode err = EECode_OK;

    mbRunning = 1;
    mpWorkQueue->CmdAck(EECode_OK);

    while (mbRunning) {

        LOGM_STATE("CIPCUnixDomainSocketAgent::OnRun start switch state %d, mbRun %d.\n", msState, mbRunning);

        switch (msState) {

            case EIPCAgentState_idle:
                mpWorkQueue->WaitMsg(&cmd, sizeof(cmd));
                processCmd(cmd);
                break;

            case EIPCAgentState_server_listening:
                DASSERT(mIsServerMode);

                mReadSet = mAllSet;
                //LOGM_INFO("[CIPCUnixDomainSocketAgent]: before select.\n");
                nready = select(mMaxFd + 1, &mReadSet, NULL, NULL, NULL);
                if (DUnlikely(0 > nready)) {
                    LOGM_FATAL("select fail\n");
                    msState = EIPCAgentState_error;
                    break;
                } else if (nready == 0) {
                    break;
                }

                //LOGM_INFO("[CIPCUnixDomainSocketAgent]: after select, nready %d.\n", nready);
                if (FD_ISSET(mPipeFd[0], &mReadSet)) {
                    mpWorkQueue->WaitMsg(&cmd, sizeof(cmd));
                    processCmd(cmd);
                    nready --;
                    if (EIPCAgentState_server_listening != msState) {
                        LOGM_INFO(" transit from EIPCAgentState_server_listening to state %d.\n", msState);
                        break;
                    }

                    if (nready <= 0) {
                        //read done
                        LOGM_INFO(" read done.\n");
                        break;
                    }
                }

                if (DLikely(FD_ISSET(mSocket, &mReadSet))) {
                    socklen_t addr_len = sizeof(struct sockaddr);
                    LOGM_INFO("accept from client\n");
                    if (DIsSocketHandlerValid(mTransferSocket)) {
                        LOGM_WARN("clear previous socket %d\n", mTransferSocket);
                        gfNetCloseSocket(mTransferSocket);
                        mTransferSocket = DInvalidSocketHandler;
                    }

                    mTransferSocket = accept(mSocket, (struct sockaddr *)&mAddr, &addr_len);
                    if (DUnlikely(0 > mTransferSocket)) {
                        LOGM_ERROR("accept fail, return %d.\n", mTransferSocket);
                        msState = EIPCAgentState_error;
                    } else {
                        //struct timeval timeout = {2, 0};
                        //setsockopt(mTransferSocket, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(struct timeval));
                        //setsockopt(mTransferSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));
                        msState = EIPCAgentState_server_running;

                        FD_SET(mTransferSocket, &mAllWriteSet);
                        if (mTransferSocket > mMaxWriteFd) {
                            mMaxWriteFd = mTransferSocket;
                        }

                        FD_SET(mTransferSocket, &mAllSet);
                        if (mTransferSocket > mMaxFd) {
                            mMaxFd = mTransferSocket;
                        }

                        FD_CLR(mSocket, &mAllSet);
                    }

                } else {
                    LOGM_ERROR("not expected here.\n");
                }
                break;

            case EIPCAgentState_server_running:
                DASSERT(mIsServerMode);

                mReadSet = mAllSet;
                //LOGM_INFO("[CIPCUnixDomainSocketAgent]: before select.\n");
                nready = select(mMaxFd + 1, &mReadSet, NULL, NULL, NULL);
                if (DUnlikely(0 > nready)) {
                    LOGM_FATAL("select fail\n");
                    msState = EIPCAgentState_error;
                    break;
                } else if (nready == 0) {
                    LOGM_WARN("select return 0\n");
                    break;
                }

                //LOGM_INFO("[CIPCUnixDomainSocketAgent]: after select, nready %d.\n", nready);
                if (FD_ISSET(mPipeFd[0], &mReadSet)) {
                    mpWorkQueue->WaitMsg(&cmd, sizeof(cmd));
                    processCmd(cmd);
                    nready --;
                    if (EIPCAgentState_server_running != msState) {
                        LOGM_INFO(" transit from EIPCAgentState_server_running to state %d.\n", msState);
                        break;
                    }

                    if (nready <= 0) {
                        //read done
                        //LOGM_DEBUG(" read done.\n");
                        break;
                    }
                }

                if (DLikely(FD_ISSET(mTransferSocket, &mReadSet))) {
                    if (DLikely(mReadCallBack)) {

                        if (DLikely(mpReadBuffer && mReadBufferSize)) {
                            TU32 remaining = 0;
                            mReadDataSize = recv(mTransferSocket, (TChar *)mpReadBuffer, mReadBufferSize, 0);
                            if (DUnlikely(mReadDataSize < 0)) {
                                LOGM_ERROR("recv error, ret %d\n", mReadDataSize);
                                msState = EIPCAgentState_error;
                                break;
                            } else if (!mReadDataSize) {
                                LOGM_NOTICE("peer close\n");
                                if (DLikely(mIsServerMode)) {
                                    msState = EIPCAgentState_server_listening;
                                    FD_CLR(mTransferSocket, &mAllSet);
                                    FD_CLR(mTransferSocket, &mAllWriteSet);
                                    FD_SET(mSocket, &mAllSet);
                                } else {
                                    LOGM_ERROR("why here mIsServerMode %d\n", mIsServerMode);
                                    msState = EIPCAgentState_idle;
                                    FD_CLR(mTransferSocket, &mAllSet);
                                    FD_CLR(mTransferSocket, &mAllWriteSet);
                                }
                                break;
                            } else if (mReadDataSize == mReadBufferSize) {
                                remaining = 1;
                            }

                            //LOG_NOTICE("read size %d, buffer size %d, remaining %d\n", mReadDataSize, mReadBufferSize, remaining);
                            err = mReadCallBack(mpReadCallBackContext, mpReadBuffer, mReadDataSize, remaining);
                            DASSERT_OK(err);
                            memset(mpReadBuffer, 0x0, mReadDataSize);
                        } else {
                            LOGM_WARN("NULL %p, %d\n", mpReadBuffer, mReadBufferSize);
                        }
                    } else {
                        LOGM_NOTICE("NULL %p, %p\n", mpReadCallBackContext, mReadCallBack);
                        mReadDataSize = recv(mTransferSocket, (TChar *)mpReadBuffer, mReadBufferSize, 0);
                        if (!mReadDataSize) {
                            LOGM_NOTICE("peer close\n");
                            msState = EIPCAgentState_server_listening;
                            FD_CLR(mTransferSocket, &mAllSet);
                            FD_CLR(mTransferSocket, &mAllWriteSet);
                            FD_SET(mSocket, &mAllSet);
                            break;
                        }
                    }
                } else {
                    LOGM_WARN("nready %d\n", nready);
                }
                break;

            case EIPCAgentState_client_running:
                DASSERT(!mIsServerMode);

                mReadSet = mAllSet;
                //LOGM_INFO("[CIPCUnixDomainSocketAgent]: before select.\n");
                nready = select(mMaxFd + 1, &mReadSet, NULL, NULL, NULL);
                if (DUnlikely(0 > nready)) {
                    LOGM_FATAL("select fail\n");
                    msState = EIPCAgentState_error;
                    break;
                } else if (DUnlikely(0 == nready)) {
                    LOGM_WARN("select return 0\n");
                    break;
                }

                //LOGM_INFO("[CIPCUnixDomainSocketAgent]: after select, nready %d.\n", nready);
                if (FD_ISSET(mPipeFd[0], &mReadSet)) {
                    mpWorkQueue->WaitMsg(&cmd, sizeof(cmd));
                    processCmd(cmd);
                    nready --;
                    if (EIPCAgentState_client_running != msState) {
                        LOGM_INFO(" transit from EIPCAgentState_client_running to state %d.\n", msState);
                        break;
                    }

                    if (nready <= 0) {
                        //read done
                        //LOGM_DEBUG(" read done.\n");
                        break;
                    }
                }

                if (DLikely(FD_ISSET(mSocket, &mReadSet))) {
                    if (DLikely(mReadCallBack)) {

                        if (DLikely(mpReadBuffer && mReadBufferSize)) {
                            TU32 remaining = 0;
                            mReadDataSize = recv(mSocket, (TChar *)mpReadBuffer, mReadBufferSize, 0);
                            if (DUnlikely(mReadDataSize < 0)) {
                                LOGM_ERROR("recv error, ret %d\n", mReadDataSize);
                                msState = EIPCAgentState_error;
                                break;
                            } else if (!mReadDataSize) {
                                LOGM_NOTICE("peer close\n");
                                if (DUnlikely(mIsServerMode)) {
                                    LOGM_ERROR("why here mIsServerMode %d\n", mIsServerMode);
                                    msState = EIPCAgentState_server_listening;
                                    FD_CLR(mSocket, &mAllSet);
                                    FD_CLR(mSocket, &mAllWriteSet);
                                    FD_SET(mSocket, &mAllSet);
                                } else {
                                    msState = EIPCAgentState_idle;
                                    FD_CLR(mSocket, &mAllSet);
                                    FD_CLR(mSocket, &mAllWriteSet);
                                }
                                break;
                            } else if (mReadDataSize == mReadBufferSize) {
                                remaining = 1;
                            }

                            err = mReadCallBack(mpReadCallBackContext, mpReadBuffer, mReadDataSize, remaining);
                            DASSERT_OK(err);
                            memset(mpReadBuffer, 0x0, mReadDataSize);
                        } else {
                            LOGM_WARN("NULL %p, %d\n", mpReadBuffer, mReadBufferSize);
                        }
                    } else {
                        LOGM_WARN("NULL %p, %p\n", mpReadCallBackContext, mReadCallBack);
                    }
                } else if (DLikely(FD_ISSET(mTransferSocket, &mReadSet))) {
                    if (DLikely(mReadCallBack)) {

                        if (DLikely(mpReadBuffer && mReadBufferSize)) {
                            TU32 remaining = 0;
                            mReadDataSize = recv(mTransferSocket, (TChar *)mpReadBuffer, mReadBufferSize, 0);
                            if (DUnlikely(mReadDataSize < 0)) {
                                LOGM_ERROR("recv error, ret %d\n", mReadDataSize);
                                msState = EIPCAgentState_error;
                                break;
                            } else if (!mReadDataSize) {
                                LOGM_NOTICE("peer close\n");
                                if (DUnlikely(mIsServerMode)) {
                                    LOGM_ERROR("why here mIsServerMode %d\n", mIsServerMode);
                                    msState = EIPCAgentState_server_listening;
                                    FD_CLR(mTransferSocket, &mAllSet);
                                    FD_CLR(mTransferSocket, &mAllWriteSet);
                                    FD_SET(mSocket, &mAllSet);
                                } else {
                                    msState = EIPCAgentState_idle;
                                    FD_CLR(mTransferSocket, &mAllSet);
                                    FD_CLR(mTransferSocket, &mAllWriteSet);
                                }
                                break;
                            } else if (mReadDataSize == mReadBufferSize) {
                                remaining = 1;
                            }

                            err = mReadCallBack(mpReadCallBackContext, mpReadBuffer, mReadDataSize, remaining);
                            DASSERT_OK(err);
                            memset(mpReadBuffer, 0x0, mReadDataSize);
                        } else {
                            LOGM_WARN("NULL %p, %d\n", mpReadBuffer, mReadBufferSize);
                        }
                    }
                } else {
                    LOGM_WARN("nready %d\n", nready);
                }
                break;

            case EIPCAgentState_error:
                //todo
                LOGM_ERROR("EIPCAgentState_error here.\n");
                mpWorkQueue->WaitMsg(&cmd, sizeof(cmd));
                processCmd(cmd);
                break;

            default:
                mbRunning = 0;
                LOGM_FATAL("bad state %d.\n", msState);
                break;
        }

        mDebugHeartBeat ++;
    }

    LOGM_NOTICE("exit running\n");

}

bool CIPCUnixDomainSocketAgent::processCmd(SCMD &cmd)
{
    LOGM_NOTICE("cmd.code %x.\n", cmd.code);
    DASSERT(mpWorkQueue);
    TChar char_buffer;
    EECode err = EECode_OK;

    //bind with pipe fd
    gfOSReadPipe(mPipeFd[0], char_buffer);
    LOGM_DEBUG("****CIPCUnixDomainSocketAgent::ProcessCmd, cmd.code %d, TChar %c.\n", cmd.code, char_buffer);

    switch (cmd.code) {

        case ECMDType_Start:
            err = EECode_OK;
            if (DLikely(EIPCAgentState_idle == msState)) {
                if (mIsServerMode) {
                    err = beginListen();
                    if (DUnlikely(EECode_OK != err)) {
                        LOGM_ERROR("listen err %d %s\n", err, gfGetErrorCodeString(err));
                        msState = EIPCAgentState_error;
                    } else {
                        msState = EIPCAgentState_server_listening;
                    }
                } else {
                    err = connectToServer();
                    if (DUnlikely(EECode_OK != err)) {
                        LOGM_ERROR("connect err %d %s\n", err, gfGetErrorCodeString(err));
                        msState = EIPCAgentState_error;
                    } else {
                        msState = EIPCAgentState_client_running;
                    }
                }
            } else {
                LOGM_ERROR("not expected state %d\n", msState);
            }
            //mpWorkQueue->CmdAck(err);
            break;

        case ECMDType_Stop:
            mbRunning = 0;
            mpWorkQueue->CmdAck(EECode_OK);
            break;

        case ECMDType_SendData: {
                if (DUnlikely(0 > mTransferSocket)) {
                    LOG_WARN("bad mTransferSocket %d\n", mTransferSocket);
                    mpWorkQueue->CmdAck(EECode_Closed);
                    break;
                }

                SDataPiece *piece = (SDataPiece *)cmd.pExtra;
                TInt sentsize = send(mTransferSocket, (void *)piece->p_data, piece->data_size, 0);
                if (DUnlikely((TInt)piece->data_size != sentsize)) {
                    LOG_ERROR("size %ld, send size %d\n", piece->data_size, sentsize);
                    mpWorkQueue->CmdAck(EECode_Closed);
                    FD_CLR(mTransferSocket, &mAllSet);
                    FD_CLR(mTransferSocket, &mAllWriteSet);
                    if (DIsSocketHandlerValid(mTransferSocket)) {
                        gfNetCloseSocket(mTransferSocket);
                        mTransferSocket = DInvalidSocketHandler;
                    }
                    if (mIsServerMode) {
                        msState = EIPCAgentState_server_listening;
                    } else {
                        msState = EIPCAgentState_error;
                    }
                } else {
                    mpWorkQueue->CmdAck(EECode_OK);
                }
            }
            break;

        default:
            LOGM_ERROR("wrong cmd.code: %d", cmd.code);
            break;
    }

    LOGM_FLOW("****CIPCUnixDomainSocketAgent::ProcessCmd, cmd.code %d end.\n", cmd.code);
    return false;
}

EECode CIPCUnixDomainSocketAgent::beginListen()
{
    TInt ret = 0;

    if (DUnlikely(0 > mSocket)) {
        LOG_ERROR("mSocket(%d) not create yet\n", mSocket);
        return EECode_BadState;
    }

    if (DUnlikely(mbConnecting)) {
        LOGM_ERROR("already in connecting state\n");
        return EECode_BadState;
    }

    LOGM_DEBUG("beginListen: before access\n");

    ret = access(mTag, F_OK);

    LOGM_DEBUG("beginListen: after access\n");

    if (mIsServerMode) {
        if (DUnlikely(0 == ret)) {
            LOG_WARN("File(%s) exists, previous exit without clean\n", mTag);
            unlink(mTag);
        }
    }

    LOGM_DEBUG("beginListen: before bind\n");
    ret = bind(mSocket, (struct sockaddr *)&mAddr, sizeof(struct sockaddr_un));

    if (DUnlikely(0 > ret)) {
        perror("bind");
        LOG_ERROR("Failed to bind %d\n", mSocket);
        gfNetCloseSocket(mSocket);
        mSocket = DInvalidSocketHandler;
        return EECode_Error;
    }

    LOGM_DEBUG("beginListen: before listen\n");
    ret = listen(mSocket, 5);

    if (DUnlikely(0 > ret)) {
        LOG_ERROR("Failed to listen %d\n", mSocket);
        gfNetCloseSocket(mSocket);
        mSocket = DInvalidSocketHandler;
        return EECode_Error;
    }
    LOGM_DEBUG("beginListen: after listen\n");

    return EECode_OK;
}

EECode CIPCUnixDomainSocketAgent::connectToServer()
{
    TInt ret = 0;

    if (DUnlikely(0 > mSocket)) {
        LOG_ERROR("mSocket(%d) not create yet\n", mSocket);
        return EECode_BadState;
    }

    if (DUnlikely(mbConnecting)) {
        LOGM_ERROR("already in connecting state\n");
        return EECode_BadState;
    }

    LOGM_DEBUG("connectToServer: before access\n");
    ret = access(mTag, F_OK);

#if 0
    LOGM_DEBUG("connectToServer: before bind\n");
    ret = bind(mSocket, (struct sockaddr *)&mAddr, sizeof(struct sockaddr_un));

    if (DUnlikely(0 > ret)) {
        perror("bind");
        LOG_ERROR("Failed to bind %d\n", mSocket);
        close(mSocket);
        mSocket = -1;
        return EECode_Error;
    }
#endif

    LOGM_DEBUG("connectToServer: before connect\n");
    ret = connect(mSocket, (struct sockaddr *)&mAddr, sizeof(struct sockaddr_un));

    if (DUnlikely(0 > ret)) {
        LOG_ERROR("Failed to connect %d, ret %d\n", mSocket, ret);
        perror("connect");
        gfNetCloseSocket(mSocket);
        mSocket = DInvalidSocketHandler;
        return EECode_Error;
    }

    LOGM_DEBUG("connectToServer: after connect\n");
    return EECode_OK;
}

DCONFIG_COMPILE_OPTION_CPPFILE_IMPLEMENT_END

