/*
 * kernel/private/drivers/ambarella/vin/sensors/sony_imx224/imx224_pri.h
 *
 * History:
 *    2014/08/05 - [Long Zhao] Create
 *
 * Copyright (C) 2004-2014, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 *
 */

#ifndef __IMX224_PRI_H__
#define __IMX224_PRI_H__

#define USE_960P_2X_30FPS

#define IMX224_STANDBY			0x3000
#define IMX224_REGHOLD			0x3001
#define IMX224_XMSTA			0x3002
#define IMX224_SWRESET			0x3003

#define IMX224_ADBIT			0x3005
#define IMX224_MODE			0x3006
#define IMX224_WINMODE		0x3007

#define IMX224_FRSEL			0x3009
#define IMX224_BLKLEVEL_LSB	0x300A
#define IMX224_BLKLEVEL_MSB	0x300B
#define IMX224_WDMODE			0x300C

#define IMX224_GAIN_LSB		0x3014
#define IMX224_GAIN_MSB		0x3015

#define IMX224_VMAX_LSB		0x3018
#define IMX224_VMAX_MSB		0x3019
#define IMX224_VMAX_HSB		0x301A
#define IMX224_HMAX_LSB		0x301B
#define IMX224_HMAX_MSB		0x301C

#define IMX224_SHS1_LSB		0x3020
#define IMX224_SHS1_MSB		0x3021
#define IMX224_SHS1_HSB		0x3022
#define IMX224_SHS2_LSB		0x3023
#define IMX224_SHS2_MSB		0x3024
#define IMX224_SHS2_HSB		0x3025
#define IMX224_SHS3_LSB		0x3026
#define IMX224_SHS3_MSB		0x3027
#define IMX224_SHS3_HSB		0x3028

#define IMX224_RHS1_LSB		0x302C
#define IMX224_RHS1_MSB		0x302D
#define IMX224_RHS1_HSB		0x302E
#define IMX224_RHS2_LSB		0x302F
#define IMX224_RHS2_MSB		0x3030
#define IMX224_RHS2_HSB		0x3031

#define IMX224_DOL_PAT1		0x3043

#define IMX224_ODBIT			0x3044

#define IMX224_INCKSEL1			0x305C
#define IMX224_INCKSEL2			0x305D
#define IMX224_INCKSEL3			0x305E
#define IMX224_INCKSEL4			0x305F

#define IMX224_DOL_PAT2		0x310A

#define IMX224_NULL0_SIZE		0x3354
#define IMX224_PIC_SIZE_LSB		0x3357
#define IMX224_PIC_SIZE_MSB	0x3358

#define IMX224_V_FLIP	(1<<0)
#define IMX224_H_MIRROR	(1<<1)

#define IMX224_HI_GAIN_MODE	(1<<4)

#define IMX224_QVGA_BRL		(993)
#define IMX224_720P_BRL		(735)
#define IMX224_H_PIXEL			(1312)
#define IMX224_HBLANK			(488)
#define IMX224_H_PERIOD		(1800)

#ifdef USE_960P_2X_30FPS
#define IMX224_960P_2X_RHS1		(0x1B3)
#else
#define IMX224_960P_2X_RHS1		(0xC1)
#endif

#define IMX224_960P_3X_RHS1		(0x208)
#define IMX224_960P_3X_RHS2		(0x25A)

#endif /* __IMX224_PRI_H__ */

