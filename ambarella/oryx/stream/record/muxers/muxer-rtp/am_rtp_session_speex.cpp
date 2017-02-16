/*******************************************************************************
 * am_rtp_session_speex.cpp
 *
 * History:
 *   Jul 25, 2015 - [ypchang] created file
 *
 * Copyright (C) 2008-2015, Ambarella Co, Ltd.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella.
 *
 ******************************************************************************/

#include "am_base_include.h"
#include "am_define.h"
#include "am_log.h"

#include "am_rtp_session_speex.h"
#include "am_rtp_data.h"
#include "am_muxer_rtp_config.h"

#include "am_amf_types.h"
#include "am_amf_packet.h"
#include "am_audio_define.h"

#include "am_mutex.h"
#include "rtp.h"

AMIRtpSession* create_speex_session(const char *name,
                                    uint32_t queue_size,
                                    void *ainfo)
{
  return AMRtpSessionSpeex::create(name, queue_size, (AM_AUDIO_INFO*)ainfo);
}

AMIRtpSession* AMRtpSessionSpeex::create(const std::string &name,
                                         uint32_t queue_size,
                                         AM_AUDIO_INFO *ainfo)
{
  AMRtpSessionSpeex *result = new AMRtpSessionSpeex(name, queue_size);
  if (AM_UNLIKELY(result && !result->init(ainfo))) {
    delete result;
    result = nullptr;
  }

  return result;
}

uint32_t AMRtpSessionSpeex::get_session_clock()
{
  return m_audio_info->sample_rate;
}

AM_SESSION_STATE AMRtpSessionSpeex::process_data(
    AMPacket &packet, uint32_t max_tcp_payload_size)
{
  AM_SESSION_STATE state = AM_SESSION_OK;

  int64_t curr_pts = packet.get_pts();
  AMRtpData *rtp_data = alloc_data(curr_pts);

  if (AM_LIKELY(rtp_data)) {
    uint32_t frame_count = packet.get_frame_count();
    uint32_t total_data_size = packet.get_data_size() +
        frame_count * (sizeof(AMRtpHeader) + sizeof(AMRtpTcpHeader));

    if (AM_LIKELY(rtp_data->create(this, total_data_size,
                                   packet.get_data_size(),
                                   frame_count))) {
      int64_t pts_diff = ((m_config_rtp &&
                           m_config_rtp->dynamic_audio_pts_diff) &&
                          (m_prev_pts > 0)) ?
                   (curr_pts - m_prev_pts) : m_audio_info->pkt_pts_increment;
      int32_t pkt_pts_incr = (int32_t)
          (((2 * pts_diff * get_session_clock() + 90000LL) / (2 * 90000LL)) /
              frame_count);
      m_ptime = pkt_pts_incr * 1000 / get_session_clock();

      DEBUG("PTS diff in 90K is %lld, PTS diff in %uK is %d",
            pts_diff, get_session_clock() / 1000, pkt_pts_incr);

      assemble_packet(packet.get_data_ptr(),
                      packet.get_data_size(),
                      frame_count,
                      rtp_data,
                      pkt_pts_incr);
      add_rtp_data_to_client(rtp_data);
    }
    rtp_data->release();
  } else {
    state = AM_SESSION_IO_SLOW;
    ERROR("Network too slow? Not enough data buffer? Drop data!");
  }
  m_prev_pts = curr_pts;
  packet.release();

  return state;
}

AMRtpSessionSpeex::AMRtpSessionSpeex(const std::string &name,
                                     uint32_t queue_size) :
  inherited(name, queue_size, AM_SESSION_SPEEX),
  m_audio_info(nullptr),
  m_ptime(0)
{}

AMRtpSessionSpeex::~AMRtpSessionSpeex()
{
  delete m_audio_info;
}

bool AMRtpSessionSpeex::init(AM_AUDIO_INFO *ainfo)
{
  bool ret = false;
  do {
    if (AM_UNLIKELY(!inherited::init())) {
      ERROR("Failed to ini AMRtpSession!");
      break;
    }

    m_audio_info = new AM_AUDIO_INFO();
    if (AM_UNLIKELY(!m_audio_info)) {
      ERROR("Failed to allocate memory for audio info!");
      break;
    }
    memcpy(m_audio_info, ainfo, sizeof(*m_audio_info));
    ret = true;
  }while(0);

  return ret;
}

void AMRtpSessionSpeex::generate_sdp(std::string& host_ip_addr,
                                     uint16_t port,
                                     AM_IP_DOMAIN domain)
{
  m_sdp.clear();
  m_sdp =
      "m=audio " + std::to_string(port) + " RTP/AVP " +
      get_payload_type() + "\r\n" +
      "c=IN " + ((domain == AM_IPV4) ? "IP4 " : "IP6 ") +
      host_ip_addr + "\r\n" +
      get_param_sdp() +
      "a=control:" + "rtsp://" + host_ip_addr + "/audio=" + m_name + "\r\n";
}

std::string AMRtpSessionSpeex::get_param_sdp()
{
  return std::string("a=rtpmap:" + std::to_string(RTP_PAYLOAD_TYPE_SPEEX)) +
      " speex/" + std::to_string(get_session_clock()) + "/1\r\n" +
      "a=ptime:" + std::to_string(m_ptime) + "\r\n" +
      "a=maxptime:" + std::to_string(m_ptime) + "\r\n" +
      "a=sr:" + std::to_string(get_session_clock()) + "\r\n";
}

std::string AMRtpSessionSpeex::get_payload_type()
{
  return std::to_string(RTP_PAYLOAD_TYPE_SPEEX);
}

#define MAKE_FOURCC(a, b, c, d) (((a) << 24) | ((b) << 16) | ((c) << 8) | (d))

void AMRtpSessionSpeex::assemble_packet(uint8_t *data,
                                        uint32_t data_size,
                                        uint32_t frame_count,
                                        AMRtpData *rtp_data,
                                        int32_t pts_incr)
{
  uint8_t *raw = data;
  uint8_t *buf = rtp_data->buffer;
  uint32_t count = 0;

  while ((count < frame_count) && ((uint32_t)(raw - data) < (data_size - 8))) {
    uint8_t *tcp_header = buf;
    uint8_t *rtp_header = &tcp_header[sizeof(AMRtpTcpHeader)];
    uint8_t *payload    = &rtp_header[sizeof(AMRtpHeader)];

    if (AM_LIKELY(MAKE_FOURCC(raw[0], raw[1], raw[2], raw[3]) ==
                  MAKE_FOURCC(' ', 'S', 'P', 'X'))) {
      uint32_t raw_size = MAKE_FOURCC(raw[4], raw[5], raw[6], raw[7]);

      ++ m_sequence_num;
      AMRtpSession::m_lock_ts->lock();
      m_curr_time_stamp += pts_incr;
      AMRtpSession::m_lock_ts->unlock();
      /* Build RTP TCP Header */
      tcp_header[0]  = 0x24;
      /* tcp_header[1] should be set to rtp client channel num */
      tcp_header[2] = ((raw_size + sizeof(AMRtpHeader)) & 0xff00) >> 8;
      tcp_header[3] = ((raw_size + sizeof(AMRtpHeader)) & 0x00ff);

      /* Build RTP Header */
      rtp_header[0]  = 0x80;
      rtp_header[1]  = 0x80 | (RTP_PAYLOAD_TYPE_SPEEX & 0x7F);
      rtp_header[2]  = (m_sequence_num & 0xff00) >> 8;
      rtp_header[3]  = (m_sequence_num & 0x00ff);
      rtp_header[4]  = (m_curr_time_stamp & 0xff000000) >> 24;
      rtp_header[5]  = (m_curr_time_stamp & 0x00ff0000) >> 16;
      rtp_header[6]  = (m_curr_time_stamp & 0x0000ff00) >>  8;
      rtp_header[7]  = (m_curr_time_stamp & 0x000000ff);
      rtp_header[8]  = 0; // SSRC[3] filled by Client when sending this packet
      rtp_header[9]  = 0; // SSRC[2]
      rtp_header[10] = 0; // SSRC[1]
      rtp_header[11] = 0; // SSRC[0]
      memcpy(payload, raw + 8, raw_size);
      rtp_data->packet[count].tcp_data = tcp_header;
      rtp_data->packet[count].udp_data = rtp_header;
      rtp_data->packet[count].total_size = raw_size + sizeof(AMRtpHeader) +
          sizeof(AMRtpTcpHeader);
      buf += rtp_data->packet[count].total_size;
      raw += (raw_size + 8);
      ++ count;
    } else {
      raw += 8;
      ERROR("Corrupted SPEEX raw data!");
    }
  }
}

