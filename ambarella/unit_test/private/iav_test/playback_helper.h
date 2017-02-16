/*
 * playback_helper.h
 *
 * History:
 *	2015/02/10 - [Zhi He] create file
 *
 * Copyright (C) 2015 -2020, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 *
 */

enum {
	AVC_NAL_TYPE_NON_IDR_BEGIN = 0x01,

	AVC_NAL_TYPE_IDR = 0x05,
	AVC_NAL_TYPE_SEI = 0x06,
	AVC_NAL_TYPE_SPS = 0x07,
	AVC_NAL_TYPE_PPS = 0x08,
	AVC_NAL_TYPE_DELIMITER = 0x09,
	AVC_NAL_TYPE_EOS = 0x0a,
} AVC_NAL_TYPE;

#define DMAX_GOP_LENGTH 264
typedef unsigned long _file_size_t;

typedef struct
{
	_file_size_t frame_offset;
} fast_navi_frame;

typedef struct __s_fast_navi_gop
{
	_file_size_t gop_offset;
	unsigned int gop_index;
	unsigned int start_frame_index;

	unsigned int frame_count;
	fast_navi_frame frames[DMAX_GOP_LENGTH];

	struct __s_fast_navi_gop *p_next;
	struct __s_fast_navi_gop *p_pre;
} fast_navi_gop;

typedef struct
{
	//gop, frame, tree
	fast_navi_gop gop_list_header;
	unsigned int tot_gop_count;
	unsigned int tot_frame_count;

	//file related
	FILE* file_fd;
	_file_size_t file_size;
	unsigned char *p_buf_base;
	_file_size_t buf_size;
	_file_size_t cur_file_offset;

} fast_navi_file;

typedef struct {
	FILE* fd;

	unsigned long read_buffer_size;
	unsigned long file_total_size;
	unsigned long file_remainning_size;
	unsigned long data_remainning_size_in_buffer;

	unsigned char* p_read_buffer_base;
	unsigned char* p_read_buffer_end;

	unsigned char* p_read_buffer_cur_start;
	unsigned char* p_read_buffer_cur_end;

	unsigned char b_alloc_read_buffer;
	unsigned char b_opened_file;
	unsigned char reserved1;
	unsigned char reserved2;
} _t_file_reader;

int file_reader_init(_t_file_reader* p_reader, FILE* file_fd, unsigned long read_buffer_size, void* prealloc_file_buffer);
void file_reader_deinit(_t_file_reader* p_reader);
void file_reader_reset(_t_file_reader* p_reader);
int file_reader_read_trunk(_t_file_reader* p_reader);

unsigned int get_next_frame(unsigned char* pstart, unsigned char* pend, unsigned char* p_nal_type, unsigned char* p_slice_type, unsigned char* need_more_data);

int build_fast_navi_tree(FILE* file_fd, fast_navi_file* navi_tree);
void delete_fast_navi_tree(fast_navi_file* navi_tree);

