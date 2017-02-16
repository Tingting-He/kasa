/*******************************************************************************
 * am_audio_codec_g711.h
 *
 * History:
 *   2015-1-28 - [ypchang] created file
 *
 * Copyright (C) 2008-2015, Ambarella Co, Ltd.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella.
 *
 ******************************************************************************/
#ifndef ORYX_STREAM_COMMON_CODECS_CODEC_G711_AM_AUDIO_CODEC_G711_H_
#define ORYX_STREAM_COMMON_CODECS_CODEC_G711_AM_AUDIO_CODEC_G711_H_

class AMAudioCodecG711Config;
struct AudioCodecG711Config;
struct g711_state_s;

class AMAudioCodecG711: public AMAudioCodec
{
    typedef AMAudioCodec inherited;

  public:
    static AMIAudioCodec* create(const char *config);

  public:
    virtual void destroy();
    virtual bool initialize(AM_AUDIO_INFO *srcAudioInfo,
                            AM_AUDIO_CODEC_MODE mode);
    virtual bool finalize();
    virtual AM_AUDIO_INFO* get_codec_audio_info();
    virtual uint32_t get_codec_output_size();
    virtual bool get_encode_required_src_parameter(AM_AUDIO_INFO &info);
    /* encode: return generated bytes,
     * generated data size is also stored in out_data_size
     */
    virtual uint32_t encode(uint8_t *input,  uint32_t in_data_size,
                            uint8_t *output, uint32_t *out_data_size);
    /* decode: return consumed bytes,
     * generated data size is stored in out_data_size
     */
    virtual uint32_t decode(uint8_t *input,  uint32_t in_data_size,
                            uint8_t *output, uint32_t *out_data_size);

  protected:
    AMAudioCodecG711();
    virtual ~AMAudioCodecG711();
    bool init(const char *config);

  private:
    AudioCodecG711Config   *m_g711_config; /* No need to delete */
    AMAudioCodecG711Config *m_config;
    g711_state_s           *m_g711_state;
};

#endif /* ORYX_STREAM_COMMON_CODECS_CODEC_G711_AM_AUDIO_CODEC_G711_H_ */
