/*******************************************************************************
 * am_video_adv_iso_mode.h
 *
 * History:
 *   2014-8-6 - [lysun] created file
 *   2015-3-31 - [binwang] modified file
 *
 * Copyright (C) 2008-2014, Ambarella Co,Ltd.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella
 *
 ******************************************************************************/
#ifndef AM_VIDEO_ADV_ISO_MODE_H_
#define AM_VIDEO_ADV_ISO_MODE_H_

#include "am_encode_device.h"

class AMVideoAdvISOMode: public AMEncodeDevice
{
  public:
    virtual ~AMVideoAdvISOMode();
    static AMVideoAdvISOMode *create();
    virtual AM_RESULT construct();
    //each mode may use its own config file, however, if not override the
    //load_config method in AMEncodeDevice, default config file will be used.
    virtual AM_RESULT load_config();
    virtual AM_RESULT save_config();
    virtual AM_RESULT update();

  protected:
    AMVideoAdvISOMode();
    virtual AM_RESULT set_system_resource_limit();
};
#endif /* AM_VIDEO_ADV_ISO_MODE_H_ */
