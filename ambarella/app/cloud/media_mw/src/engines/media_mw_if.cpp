/**
 * media_mw_if.cpp
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

#include "common_config.h"
#include "common_types.h"
#include "common_osal.h"
#include "common_utils.h"
#include "common_log.h"

#include "common_base.h"

#include "media_mw_if.h"

DCONFIG_COMPILE_OPTION_CPPFILE_IMPLEMENT_BEGIN
DCODE_DELIMITER;

#ifdef DCONFIG_COMPILE_OLD_VERSION_FILE
extern IGenericEngineControl *CreateGenericEngine();
extern IGenericEngineControlV2 *CreateGenericEngineV2();
#endif

extern IGenericEngineControlV3 *CreateGenericEngineV3(IStorageManagement *p_storage, SSharedComponent *p_shared_component = NULL);
extern IGenericEngineControlV4 *CreateGenericEngineV4(IStorageManagement *p_storage, SSharedComponent *p_shared_component = NULL);

IGenericEngineControl *gfMediaEngineFactory(TUint type)
{
    IGenericEngineControl *thiz = NULL;

    switch (type) {

        case EFactory_MediaEngineType_Generic:
#ifdef DCONFIG_COMPILE_OLD_VERSION_FILE
            thiz = CreateGenericEngine();
#endif
            DASSERT(thiz);
            break;

        default:
            LOG_FATAL("unknown MediaEngineType %d\n", type);
            break;
    }

    return thiz;
}

IGenericEngineControlV2 *gfMediaEngineFactoryV2(TUint type)
{
    IGenericEngineControlV2 *thiz = NULL;

    switch (type) {

        case EFactory_MediaEngineType_Generic:
#ifdef DCONFIG_COMPILE_OLD_VERSION_FILE
            thiz = CreateGenericEngineV2();
#endif
            DASSERT(thiz);
            break;

        default:
            LOG_FATAL("unknown MediaEngineType %d\n", type);
            break;
    }

    return thiz;
}

IGenericEngineControlV3 *gfMediaEngineFactoryV3(TUint type, IStorageManagement *p_storage, SSharedComponent *p_shared_component)
{
    IGenericEngineControlV3 *thiz = NULL;

    switch (type) {

        case EFactory_MediaEngineType_Generic:
            //#ifdef DCONFIG_COMPILE_OLD_VERSION_FILE
            thiz = CreateGenericEngineV3(p_storage, p_shared_component);
            //#endif
            DASSERT(thiz);
            break;

        default:
            LOG_FATAL("unknown MediaEngineType %d\n", type);
            break;
    }

    return thiz;
}

IGenericEngineControlV4 *gfMediaEngineFactoryV4(TUint type, IStorageManagement *p_storage, SSharedComponent *p_shared_component)
{
    IGenericEngineControlV4 *thiz = NULL;

    switch (type) {

        case EFactory_MediaEngineType_Generic:
            thiz = CreateGenericEngineV4(p_storage, p_shared_component);
            DASSERT(thiz);
            break;

        default:
            LOG_FATAL("unknown MediaEngineType %d\n", type);
            break;
    }

    return thiz;
}

extern IMediaFileParser *gfCreateBasicAVCParser();
extern IMediaFileParser *gfCreateBasicAACParser();
extern IMediaFileParser *gfCreateBasicHEVCParser();

IMediaFileParser *gfCreateMediaFileParser(EMediaFileParser type)
{
    IMediaFileParser *thiz = NULL;
    switch (type) {

        case EMediaFileParser_BasicAVC:
            thiz = gfCreateBasicAVCParser();
            break;

        case EMediaFileParser_BasicAAC:
            thiz = gfCreateBasicAACParser();
            break;

        case EMediaFileParser_BasicHEVC:
            thiz = gfCreateBasicHEVCParser();
            break;

        default:
            LOG_FATAL("NOT supported type %d\n", type);
            break;
    }

    return thiz;
}

DCONFIG_COMPILE_OPTION_CPPFILE_IMPLEMENT_END

