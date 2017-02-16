/*
 * amba_video_post_processor.h
 *
 * History:
 *    2013/04/05 - [Zhi He] create file
 *
 * Copyright (C) 2014 - 2024, the Ambarella Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of the Ambarella Inc.
 */

#ifndef __AMBA_VIDEO_POST_PROCESSOR_H__
#define __AMBA_VIDEO_POST_PROCESSOR_H__

DCONFIG_COMPILE_OPTION_HEADERFILE_BEGIN
DCODE_DELIMITER;

#define DMaxDisplayPatternNumber 8

class CAmbaVideoPostProcessor
    : public CObject
    , virtual public IVideoPostProcessor
{
    typedef CObject inherited;

protected:
    CAmbaVideoPostProcessor(const TChar *pname, const volatile SPersistMediaConfig *pPersistMediaConfig, IMsgSink *pMsgSink);
    virtual ~CAmbaVideoPostProcessor();

public:
    static IVideoPostProcessor *Create(const TChar *pname, const volatile SPersistMediaConfig *pPersistMediaConfig, IMsgSink *pMsgSink);
    virtual void Destroy();
    virtual void Destroy();

public:
    virtual EECode SetupContext();
    virtual EECode DestroyContext();

    virtual EECode ConfigurePostP(SVideoPostPConfiguration *p_config, SVideoPostPDisplayLayout *p_layout_config, SVideoPostPGlobalSetting *p_global_setting, TUint b_set_initial_value = 0, TUint b_p_config_direct_to_dsp = 0);
    virtual EECode ConfigureWindow(SDSPMUdecWindow *p_window, TUint start_index, TUint number);
    virtual EECode ConfigureRender(SDSPMUdecRender *p_render, TUint start_index, TUint number);

    virtual EECode UpdateToPreSetDisplayLayout(SVideoPostPDisplayLayout *config);
    virtual EECode UpdateHighlightenWindowSize(SVideoPostPDisplayHighLightenWindowSize *size);

    virtual EECode UpdateDisplayMask(TU8 new_mask);
    virtual EECode UpdateToLayer(TUint layer, TUint seamless = 0);
    virtual EECode SwitchWindowToHD(SVideoPostPDisplayHD *phd);

    virtual EECode SwapContent(SVideoPostPSwap *swap);
    virtual EECode CircularShiftContent(SVideoPostPShift *shift);
    virtual EECode StreamSwitch(SVideoPostPStreamSwitch *stream_switch);

    virtual EECode PlaybackCapture(SPlaybackCapture *p_capture);

    virtual EECode PlaybackZoom(SPlaybackZoom *p_zoom);

public:
    virtual EECode QueryCurrentWindow(TUint window_id, TUint &dec_id, TUint &render_id) const;
    virtual EECode QueryCurrentRender(TUint render_id, TUint &dec_id, TUint &window_id, TUint &window_id_2rd) const;
    virtual EECode QueryCurrentUDEC(TUint dec_id, TUint &window_id, TUint &render_id, TUint &window_id_2rd) const;
    virtual EECode QueryVideoPostPInfo(const SVideoPostPConfiguration *&postp_info) const;

private:
    EECode Construct();
    EECode findDisplayLaout(TU8 &index, SVideoPostPDisplayLayout *p_layout_config);
    EECode setupDisplayLayout(TU8 index, SVideoPostPDisplayLayout *config);
    EECode updateDisplay();
    EECode updateToLayer(SVideoPostPConfiguration *p_cur_config, TU8 layer);
    EECode configureRender(SVideoPostPConfiguration *p_cur_config, TU8 render_id, TU8 win_id, TU8 win_id_2rd, TU8 dec_id);
private:
    const volatile SPersistMediaConfig *mpPersistMediaConfig;
    IMsgSink *mpMsgSink;
    IMutex *mpMutex;

private:
    IDSPAPI *mpDSP;
    SVideoPostPGlobalSetting mGlobalSetting;
    SVideoPostPDisplayLayout mDisplayLayoutConfiguration[DMaxDisplayPatternNumber];
    SVideoPostPConfiguration mVideoPostPConfiguration[DMaxDisplayPatternNumber];
    TU8 mbVideoPostPConfigurationSetup[DMaxDisplayPatternNumber];

    SVideoPostPConfiguration mCustomizedVideoPostPConfiguration;

    TU8 mConsistantConfigVoutMask;
    TU8 mConsistantConfigVoutPrimaryIndex;
    TU8 mVoutMask;
    TU8 mbWaitSwitchMsg;

    TU8 mbResetLayoutWhenChanged;
    TU8 mCurrentConfiguration;
    TU8 mbUseCustomizedConfiguration;
    TU8 mbCustomizedConfigurationSetup;
};
DCONFIG_COMPILE_OPTION_HEADERFILE_END
#endif

