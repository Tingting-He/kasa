/*
 * ambhw/idc.h
 *
 * History:
 *	2006/12/27 - [Charles Chiou] created file
 *
 * Copyright (C) 2004-2014, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef __AMBHW__IDC_H__
#define __AMBHW__IDC_H__

/* ==========================================================================*/
#if (CHIP_REV == A5S) || (CHIP_REV == A7L)
#define IDC_INSTANCES			2
#define IDC_SUPPORT_INTERNAL_MUX	1
#define IDC3_BUS_MUX			GPIO(36)
#else
#define IDC_INSTANCES			3
#define IDC_SUPPORT_INTERNAL_MUX	0
#endif

/* ==========================================================================*/
#define IDC_OFFSET			0x3000
#define IDC_BASE			(APB_BASE + IDC_OFFSET)
#define IDC_REG(x)			(IDC_BASE + (x))

#if (CHIP_REV == S2L) || (CHIP_REV == S3) || (CHIP_REV == S3L)
#define IDC2_OFFSET			0x1000
#else
#define IDC2_OFFSET			0x7000
#endif
#define IDC2_BASE			(APB_BASE + IDC2_OFFSET)
#define IDC2_REG(x)			(IDC2_BASE + (x))

#if (CHIP_REV == S2) || (CHIP_REV == S2E)
#define IDC3_OFFSET			0x13000
#elif (CHIP_REV == A8)
#define IDC3_OFFSET			0xE000
#elif (CHIP_REV == S2L) || (CHIP_REV == S3) || (CHIP_REV == S3L)
#define IDC3_OFFSET			0x7000
#endif
#define IDC3_BASE			(APB_BASE + IDC3_OFFSET)
#define IDC3_REG(x)			(IDC3_BASE + (x))

/* ==========================================================================*/
#define IDC_ENR_OFFSET			0x00
#define IDC_CTRL_OFFSET			0x04
#define IDC_DATA_OFFSET			0x08
#define IDC_STS_OFFSET			0x0c
#define IDC_PSLL_OFFSET			0x10
#define IDC_PSLH_OFFSET			0x14
#define IDC_FMCTRL_OFFSET		0x18
#define IDC_FMDATA_OFFSET		0x1c
#define IDC_DUTYCYCLE_OFFSET	0x24
#define IDC_ENR_REG_ENABLE		(0x01)
#define IDC_ENR_REG_DISABLE		(0x00)

#define IDC_CTRL_STOP			(0x08)
#define IDC_CTRL_START			(0x04)
#define IDC_CTRL_IF			(0x02)
#define IDC_CTRL_ACK			(0x01)
#define IDC_CTRL_CLS			(0x00)

#define IDC_FIFO_BUF_SIZE		(63)

#define IDC_FMCTRL_STOP			(0x08)
#define IDC_FMCTRL_START		(0x04)
#define IDC_FMCTRL_IF			(0x02)

/* ==========================================================================*/
#define IDC_INT_MAX_WAIT_LOOP		24000

#define IDC_MASTER1			0
#define IDC_MASTER2			1
#define IDC_MASTER3			2

/* ==========================================================================*/
#ifndef __ASM__
/* ==========================================================================*/

extern void idc_bld_init(u8 idc_id, u32 freq_hz);

extern u8 idc_bld_isl12022m_read(u8 idc_id, u8 sub_adds);
extern void idc_bld_isl12022m_write(u8 idc_id, u8 sub_adds, u8 reg_val);

extern u8 idc_bld_pca953x_read(u8 idc_id, u8 adds, u8 sub_adds);
extern void idc_bld_pca953x_write(u8 idc_id, u8 adds, u8 sub_adds, u8 reg_val);

extern u16 idc_bld_read_16_16(u8 idc_id, u8 adds, u16 sub_adds);
extern void idc_bld_write_16_16(u8 idc_id, u8 adds, u16 sub_adds, u16 reg_val);
extern u8 idc_bld_read_16_8(u8 idc_id, u8 adds, u16 sub_adds);
extern void idc_bld_write_16_8(u8 idc_id, u8 adds, u16 sub_adds, u8 reg_val);
extern u8 idc_bld_read_8_8(u8 idc_id, u8 adds, u8 sub_adds);
extern void idc_bld_write_8_8(u8 idc_id, u8 adds, u8 sub_adds, u8 reg_val);

extern int idc_bld_send_buf_without_ack(unsigned char idc_id, unsigned char adds, unsigned char* buf, int count);
extern int idc_bld_send_buf(unsigned char idc_id, unsigned char adds, unsigned char* send_buf, int send_len);
extern int idc_bld_recv_buf(unsigned char idc_id, unsigned char adds, unsigned char* recv_buf, int recv_len);
extern int idc_bld_read_buf(unsigned char idc_id, unsigned char adds, unsigned char* read_buf, int read_len);

/* ==========================================================================*/
#endif
/* ==========================================================================*/

#endif

