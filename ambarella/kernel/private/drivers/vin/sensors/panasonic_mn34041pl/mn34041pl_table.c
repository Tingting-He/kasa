/*
 * Filename : mn34041pl_reg_tbl.c
 *
 * History:
 *2011/01/12 - [Haowei Lo] Create
 *
 * Copyright (C) 2004-2011, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 *
 */

static struct vin_reg_16_16 mn34041pl_pll_regs[][6] = {
	{
		{0x0001, 0x0036}, {0x0002, 0x0002}, {0x0004, 0x01c2},
		{0x0003, 0x0209}, {0x0006, 0x442a}, {0x0005, 0x0000}
	},
};

static struct vin_video_pll mn34041pl_plls[] = {
	{0, 27000000, 162000000},
};

static struct vin_video_format mn34041pl_formats[] = {
	{
		.video_mode	= AMBA_VIDEO_MODE_1080P,
		.def_start_x	= 64,
		.def_start_y	= 8,
		.def_width	= 1920,
		.def_height	= 1080,
		/* sensor mode */
		.device_mode	= 0,
		.pll_idx	= 0,
		.width		= 2016,
		.height		= 1108,
		.format		= AMBA_VIDEO_FORMAT_PROGRESSIVE,
		.type		= AMBA_VIDEO_TYPE_RGB_RAW,
		.bits		= AMBA_VIDEO_BITS_12,
		.ratio		= AMBA_VIDEO_RATIO_16_9,
		.max_fps	= AMBA_VIDEO_FPS_60,
		.default_fps	= AMBA_VIDEO_FPS_29_97,
		.default_agc	= 0,
		.default_shutter_time	= AMBA_VIDEO_FPS_120,
		.default_bayer_pattern	= VINDEV_BAYER_PATTERN_BG,
	},
};

static struct vin_reg_16_16 mn34041pl_share_regs[] = {
	/* <add share regs setting here> */
	{ 0x0000, 0x0013 },
	{ 0x0036, 0x0021 },
	{ 0x0037, 0x0300 },

	{ 0x0020, 0x0080 },
	{ 0x0021, 0x0080 },
	{ 0x0022, 0x0000 },
	{ 0x0023, 0x0000 },
	{ 0x0024, 0x0010 },
	{ 0x0025, 0x0011 },
	{ 0x0026, 0x0012 },
	{ 0x0027, 0x0013 },
	{ 0x0028, 0x0000 },
	{ 0x0030, 0x0424 },
	{ 0x0031, 0x110A },
	{ 0x0032, 0x7450 },
	{ 0x0033, 0x0000 },
	{ 0x0034, 0x0000 },
	{ 0x0035, 0x0281 },
	{ 0x0038, 0x0001 },
	{ 0x0039, 0x0210 },
	{ 0x003a, 0x0333 },
	{ 0x003b, 0x0111 },
	{ 0x003f, 0x0000 },
	{ 0x0040, 0x0004 },
	{ 0x0041, 0x0200 },
	{ 0x0042, 0x0000 },
	{ 0x0043, 0x0070 },
	{ 0x0044, 0x0000 },
	{ 0x0045, 0x0000 },
	{ 0x0046, 0x0000 },
	{ 0x0047, 0x0010 },
	{ 0x0048, 0x0000 },
	{ 0x0049, 0x0002 },
	{ 0x004a, 0x0ffe },
	{ 0x004b, 0x0004 },
	{ 0x0056, 0x0000 },
	{ 0x0057, 0x1fff },
	{ 0x0058, 0x2000 },
	{ 0x0059, 0x0000 },
	{ 0x005a, 0x0011 },
	{ 0x0070, 0x2084 },
	{ 0x0071, 0xffff },
	{ 0x0072, 0x0000 },
	{ 0x0073, 0x0000 },
	{ 0x0074, 0x0000 },
	{ 0x0075, 0x0004 },
	{ 0x0076, 0x0230 },
	{ 0x0077, 0x0541 },
	{ 0x0078, 0x0101 },
	{ 0x0079, 0x0011 },
	{ 0x0080, 0x0AB0 },	//SOF ch1 port0
	{ 0x0081, 0x0B60 },	//EOF ch1 port0
	{ 0x0082, 0x0800 },	//SOL ch1 port0
	{ 0x0083, 0x09D0 },	//EOL ch1 port0
	{ 0x0084, 0x0AB0 },	//SOF ch2 port0
	{ 0x0085, 0x0B60 },	//EOF ch2 port0
	{ 0x0086, 0x0800 },	//SOL ch2 port0
	{ 0x0087, 0x09D0 },	//EOL ch2 port0
	{ 0x0088, 0x000a },
	{ 0x0089, 0x000b },
	{ 0x008a, 0x0008 },
	{ 0x008b, 0x0009 },
	{ 0x008c, 0x0AB0 },	//SOF ch1 port1
	{ 0x008d, 0x0B60 },	//EOF ch1 port1
	{ 0x008e, 0x0800 },	//SOL ch1 port1
	{ 0x008f, 0x09D0 },	//EOL ch1 port1
	{ 0x0090, 0x0AB0 },	//SOF ch2 port1
	{ 0x0091, 0x0B60 },	//EOF ch2 port1
	{ 0x0092, 0x0800 },	//SOL ch2 port1
	{ 0x0093, 0x09D0 },	//EOL ch2 port1
	{ 0x0094, 0x001a },
	{ 0x0095, 0x001b },
	{ 0x0096, 0x0018 },
	{ 0x0097, 0x0019 },
	{ 0x00a0, 0x3000 },
	{ 0x00a1, 0x0000 },
	{ 0x00a2, 0x0002 },
	{ 0x00a3, 0x0000 },
	{ 0x00a4, 0x0000 },
	{ 0x00a5, 0x0000 },
	{ 0x00a6, 0x0000 },
	{ 0x00a7, 0x0000 },
	{ 0x00a8, 0x000f },
	{ 0x00a9, 0x0022 },
	{ 0x00c0, 0x5540 },
	{ 0x00c1, 0x5fd5 },
	{ 0x00c2, 0xf757 },
	{ 0x00c3, 0xdf5f },
	{ 0x00c4, 0x208a },
	{ 0x00c5, 0x0071 },
	{ 0x00c6, 0x0557 },
	{ 0x00c7, 0x0000 },
	{ 0x00ca, 0x0080 },
	{ 0x00cb, 0x0000 },
	{ 0x00cc, 0x0000 },
	{ 0x00cd, 0x0000 },
	{ 0x00ce, 0x0000 },

	//	TG setting
	{ 0x0100, 0x03a8 },
	{ 0x0101, 0x03a8 },
	{ 0x0102, 0x02c0 },
	{ 0x0103, 0x037a },
	{ 0x0104, 0x002b },
	{ 0x0105, 0x00de },
	{ 0x0106, 0x00fa },
	{ 0x0170, 0x0002 },
	{ 0x0171, 0x000d },
	{ 0x0172, 0x0007 },
	{ 0x0173, 0x0004 },
	{ 0x0174, 0x002a },
	{ 0x0175, 0x0062 },
	{ 0x0176, 0x0079 },
	{ 0x0177, 0x0326 },
	{ 0x0178, 0x0003 },
	{ 0x0179, 0x0033 },
	{ 0x017a, 0x0360 },
	{ 0x017b, 0x0002 },
	{ 0x017c, 0x000d },
	{ 0x0190, 0x0000 },
	{ 0x0191, 0x0000 },
	{ 0x0192, 0x0000 },
	{ 0x0193, 0x0000 },
	{ 0x0194, 0x0000 },
	{ 0x0195, 0x0000 },
	{ 0x0196, 0x0000 },
	{ 0x0197, 0x01ba },
	{ 0x0198, 0xb060 },
	{ 0x0199, 0x7c0a },
	{ 0x019a, 0x0000 },
	{ 0x019b, 0x0313 },
	{ 0x019c, 0x0b08 },
	{ 0x019d, 0x3906 },
	{ 0x019e, 0x0000 },
	{ 0x01a0, 0x0464 },
	{ 0x01a1, 0x0000 },
	{ 0x01a2, 0x0000 },
	{ 0x01a3, 0x0464 },
	{ 0x01a4, 0x0000 },
	{ 0x01a5, 0x0453 },
	{ 0x01a6, 0x0000 },
	{ 0x01a7, 0x0464 },
	{ 0x01a8, 0x0000 },
	{ 0x01a9, 0x0454 },
	{ 0x01aa, 0x0000 },
	{ 0x01ab, 0x0000 },
	{ 0x01ac, 0x0000 },
	{ 0x01ad, 0x0454 },
	{ 0x01ae, 0x0000 },
	{ 0x01af, 0x0000 },
	{ 0x01b0, 0x0000 },
	{ 0x01b1, 0x0454 },
	{ 0x01b2, 0x0000 },
	{ 0x01b3, 0x0000 },
	{ 0x01b4, 0x0000 },
	{ 0x01b5, 0x0454 },
	{ 0x01b6, 0x0000 },
	{ 0x01b7, 0x0000 },
	{ 0x01b8, 0x0000 },
	{ 0x01b9, 0x0453 },
	{ 0x01ba, 0x0000 },
	{ 0x01bb, 0x0000 },
	{ 0x01bc, 0x0000 },
	{ 0x01bd, 0x0453 },
	{ 0x01be, 0x0000 },
	{ 0x01c4, 0x0000 },
	{ 0x01c5, 0x0000 },
	{ 0x01c6, 0x0011 },
	{ 0x0200, 0x03a8 },
	{ 0x0201, 0x03a8 },
	{ 0x0202, 0x02c0 },
	{ 0x0203, 0x037a },
	{ 0x0204, 0x002b },
	{ 0x0205, 0x00de },
	{ 0x0206, 0x00fa },
	{ 0x0270, 0x0002 },
	{ 0x0271, 0x000d },
	{ 0x0272, 0x0007 },
	{ 0x0273, 0x0004 },
	{ 0x0274, 0x002a },
	{ 0x0275, 0x0062 },
	{ 0x0276, 0x0079 },
	{ 0x0277, 0x0326 },
	{ 0x0278, 0x0003 },
	{ 0x0279, 0x0033 },
	{ 0x027a, 0x0360 },
	{ 0x027b, 0x0002 },
	{ 0x027c, 0x000d },
	{ 0x0290, 0x0000 },
	{ 0x0291, 0x0000 },
	{ 0x0292, 0x0000 },
	{ 0x0293, 0x0000 },
	{ 0x0294, 0x0000 },
	{ 0x0295, 0x0000 },
	{ 0x0296, 0x0000 },
	{ 0x0297, 0x01ba },
	{ 0x0298, 0xb060 },
	{ 0x0299, 0x7c0a },
	{ 0x029a, 0x0000 },
	{ 0x029b, 0x0313 },
	{ 0x029c, 0x0b08 },
	{ 0x029d, 0x3906 },
	{ 0x029e, 0x0000 },
	{ 0x02a0, 0x0464 },
	{ 0x02a1, 0x0000 },
	{ 0x02a2, 0x0000 },
	{ 0x02a3, 0x0464 },
	{ 0x02a4, 0x0000 },
	{ 0x02a5, 0x0453 },
	{ 0x02a6, 0x0000 },
	{ 0x02a7, 0x0464 },
	{ 0x02a8, 0x0000 },
	{ 0x02a9, 0x0454 },
	{ 0x02aa, 0x0000 },
	{ 0x02ab, 0x0000 },
	{ 0x02ac, 0x0000 },
	{ 0x02ad, 0x0454 },
	{ 0x02ae, 0x0000 },
	{ 0x02af, 0x0000 },
	{ 0x02b0, 0x0000 },
	{ 0x02b1, 0x0454 },
	{ 0x02b2, 0x0000 },
	{ 0x02b3, 0x0000 },
	{ 0x02b4, 0x0000 },
	{ 0x02b5, 0x0454 },
	{ 0x02b6, 0x0000 },
	{ 0x02b7, 0x0000 },
	{ 0x02b8, 0x0000 },
	{ 0x02b9, 0x0453 },
	{ 0x02ba, 0x0000 },
	{ 0x02bb, 0x0000 },
	{ 0x02bc, 0x0000 },
	{ 0x02bd, 0x0453 },
	{ 0x02be, 0x0000 },
	{ 0x02c4, 0x0000 },
	{ 0x02c5, 0x0000 },
	{ 0x02c6, 0x0011 },
	{ 0x0108, 0x0000 },
	{ 0x0109, 0x000f },
	{ 0x010a, 0x0009 },
	{ 0x010b, 0x0000 },
	{ 0x010c, 0x0016 },
	{ 0x010d, 0x0000 },
	{ 0x010e, 0x000f },
	{ 0x010f, 0x0000 },
	{ 0x0110, 0x0009 },
	{ 0x0111, 0x0000 },
	{ 0x0112, 0x0016 },
	{ 0x0113, 0x0003 },
	{ 0x0114, 0x000a },
	{ 0x0115, 0x0000 },
	{ 0x0116, 0x0009 },
	{ 0x0117, 0x0000 },
	{ 0x0118, 0x0016 },
	{ 0x0119, 0x0018 },
	{ 0x011a, 0x0017 },
	{ 0x011b, 0x0000 },
	{ 0x011c, 0x0002 },
	{ 0x011d, 0x0009 },
	{ 0x011e, 0x0012 },
	{ 0x011f, 0x0001 },
	{ 0x0120, 0x003a },
	{ 0x0121, 0x0000 },
	{ 0x0122, 0x0000 },
	{ 0x0123, 0x0000 },
	{ 0x0124, 0x0011 },
	{ 0x0125, 0x0000 },
	{ 0x0126, 0x0003 },
	{ 0x0127, 0x0003 },
	{ 0x0128, 0x0000 },
	{ 0x0129, 0x0010 },
	{ 0x012a, 0x0000 },
	{ 0x012b, 0x0003 },
	{ 0x012c, 0x0000 },
	{ 0x012d, 0x0011 },
	{ 0x012e, 0x0000 },
	{ 0x012f, 0x0009 },
	{ 0x0130, 0x0009 },
	{ 0x0131, 0x0012 },
	{ 0x0132, 0x0000 },
	{ 0x0133, 0x0000 },
	{ 0x0134, 0x0009 },
	{ 0x0135, 0x0009 },
	{ 0x0136, 0x0012 },
	{ 0x0137, 0x0006 },
	{ 0x0138, 0x0000 },
	{ 0x0139, 0x0010 },
	{ 0x013a, 0x0000 },
	{ 0x0140, 0x0020 },
	{ 0x0141, 0x0036 },
	{ 0x0142, 0x0000 },
	{ 0x0143, 0x0001 },
	{ 0x0144, 0x0003 },
	{ 0x0145, 0x0000 },
	{ 0x0146, 0x0000 },
	{ 0x0150, 0x0011 },
	{ 0x0151, 0x0001 },
	{ 0x0152, 0x0001 },
	{ 0x0153, 0x0001 },
	{ 0x0154, 0x0010 },
	{ 0x0155, 0x0000 },
	{ 0x0156, 0x0003 },
	{ 0x0157, 0x0000 },
	{ 0x0158, 0x0013 },
	{ 0x0159, 0x0000 },
	{ 0x015a, 0x0000 },
	{ 0x015b, 0x0008 },
	{ 0x015c, 0x0000 },
	{ 0x015d, 0x0004 },
	{ 0x015e, 0x0005 },
	{ 0x015f, 0x000a },
	{ 0x0160, 0x0006 },
	{ 0x0161, 0x000f },
	{ 0x0162, 0x000d },
	{ 0x0163, 0x0004 },
	{ 0x0164, 0x0002 },
	{ 0x0165, 0x0002 },
	{ 0x0166, 0x0000 },
	{ 0x0167, 0x0010 },
	{ 0x0168, 0x0009 },
	{ 0x0169, 0x0003 },
	{ 0x016a, 0x0000 },
	{ 0x016b, 0x0000 },
	{ 0x0180, 0x0017 },
	{ 0x0181, 0x00c5 },
	{ 0x0182, 0x0000 },
	{ 0x0183, 0x0000 },
	{ 0x0184, 0x00fa },
	{ 0x0185, 0x00a5 },
	{ 0x0186, 0x01ef },
	{ 0x0187, 0x03d9 },
	{ 0x0188, 0x01fb },
	{ 0x0189, 0x035f },
	{ 0x018a, 0x0000 },
	{ 0x018b, 0x0000 },
	{ 0x018c, 0x0000 },
	{ 0x018d, 0x0000 },
	{ 0x01d0, 0x0000 },
	{ 0x01d1, 0x0000 },
	{ 0x01d2, 0x0000 },
	{ 0x01d3, 0x0000 },
	{ 0x01d4, 0x0000 },
	{ 0x01d5, 0x0000 },
	{ 0x01d6, 0x0000 },
	{ 0x01d7, 0x0000 },
	{ 0x01d8, 0x0006 },
	{ 0x01d9, 0x0005 },
	{ 0x01da, 0x0001 },
	{ 0x01db, 0x0006 },
	{ 0x01dc, 0x0001 },
	{ 0x01dd, 0x0007 },
	{ 0x01de, 0x0001 },
	{ 0x01df, 0x0002 },
	{ 0x01e0, 0x0001 },
	{ 0x01e1, 0x0001 },
	{ 0x01e2, 0x00c9 },
	{ 0x01e3, 0x8000 },
	{ 0x01e4, 0x003e },
	{ 0x01e5, 0x0015 },
	{ 0x01e6, 0x003e },
	{ 0x01e7, 0x00c8 },
	{ 0x01e8, 0x0043 },
	{ 0x01e9, 0x00a9 },
	{ 0x01ea, 0x00a9 },
	{ 0x01eb, 0x00a9 },
	{ 0x01ec, 0x00fb },
	{ 0x01ed, 0x00b0 },
	{ 0x01ee, 0x00b9 },
	{ 0x01ef, 0x01bb },
	{ 0x01f0, 0x02ec },
	{ 0x01f1, 0x02ec },
	{ 0x01f2, 0x02ec },
	{ 0x01f3, 0x01bd },
	{ 0x01f4, 0x034a },
	{ 0x01f5, 0x03d8 },
	{ 0x01f6, 0x03fc },
	{ 0x0000, 0x0113 }
};

/** MN34041PL global gain table row size */
#define MN34041PL_GAIN_COLS		(3)
#if 0
#define MN34041PL_GAIN_ROWS		(321)
#define MN34041PL_GAIN_0DB		(320)
#else
#define MN34041PL_GAIN_ROWS		(321 + 16*8)		/* add 12db gain to gain table */
#define MN34041PL_GAIN_0DB		(320 + 16*8)		/* add 12db gain to gain table */
#endif

#define MN34041PL_CGAIN_COL		(0)
#define MN34041PL_AGAIN_COL		(1)
#define MN34041PL_DGAIN_COL		(2)

/* This is 64-step gain table, MN34041PL_GAIN_ROWS = 359, MN34041PL_GAIN_COLS = 3 */
const s16 mn34041pl_gains[MN34041PL_GAIN_ROWS][MN34041PL_GAIN_COLS] =
{
	/* Column Gain,Analog Gain,Digital Gain */
#if 0
	{0xC000, 0x00FF, 0x0140},		/* 42 db =12db column amplifer gain, ~12 db analog gain, 18 db digital gain */

	{0xC000, 0x00FF, 0x013F},
	{0xC000, 0x00FF, 0x013E},
	{0xC000, 0x00FF, 0x013D},
	{0xC000, 0x00FF, 0x013C},
	{0xC000, 0x00FF, 0x013B},
	{0xC000, 0x00FF, 0x013A},
	{0xC000, 0x00FF, 0x0139},
	{0xC000, 0x00FF, 0x0138},
	{0xC000, 0x00FF, 0x0137},
	{0xC000, 0x00FF, 0x0136},
	{0xC000, 0x00FF, 0x0135},
	{0xC000, 0x00FF, 0x0134},
	{0xC000, 0x00FF, 0x0133},
	{0xC000, 0x00FF, 0x0132},
	{0xC000, 0x00FF, 0x0131},
	{0xC000, 0x00FF, 0x0130},

	{0xC000, 0x00FF, 0x012F},
	{0xC000, 0x00FF, 0x012E},
	{0xC000, 0x00FF, 0x012D},
	{0xC000, 0x00FF, 0x012C},
	{0xC000, 0x00FF, 0x012B},
	{0xC000, 0x00FF, 0x012A},
	{0xC000, 0x00FF, 0x0129},
	{0xC000, 0x00FF, 0x0128},
	{0xC000, 0x00FF, 0x0127},
	{0xC000, 0x00FF, 0x0126},
	{0xC000, 0x00FF, 0x0125},
	{0xC000, 0x00FF, 0x0124},
	{0xC000, 0x00FF, 0x0123},
	{0xC000, 0x00FF, 0x0122},
	{0xC000, 0x00FF, 0x0121},
	{0xC000, 0x00FF, 0x0120},		/* 39 db */


	{0xC000, 0x00FF, 0x011F},
	{0xC000, 0x00FF, 0x011E},
	{0xC000, 0x00FF, 0x011D},
	{0xC000, 0x00FF, 0x011C},
	{0xC000, 0x00FF, 0x011B},
	{0xC000, 0x00FF, 0x011A},
	{0xC000, 0x00FF, 0x0119},
	{0xC000, 0x00FF, 0x0118},
	{0xC000, 0x00FF, 0x0117},
	{0xC000, 0x00FF, 0x0116},
	{0xC000, 0x00FF, 0x0115},
	{0xC000, 0x00FF, 0x0114},
	{0xC000, 0x00FF, 0x0113},
	{0xC000, 0x00FF, 0x0112},
	{0xC000, 0x00FF, 0x0111},
	{0xC000, 0x00FF, 0x0110},

	{0xC000, 0x00FF, 0x010F},
	{0xC000, 0x00FF, 0x010E},
	{0xC000, 0x00FF, 0x010D},
	{0xC000, 0x00FF, 0x010C},
	{0xC000, 0x00FF, 0x010B},
	{0xC000, 0x00FF, 0x010A},
	{0xC000, 0x00FF, 0x0109},
	{0xC000, 0x00FF, 0x0108},
	{0xC000, 0x00FF, 0x0107},
	{0xC000, 0x00FF, 0x0106},
	{0xC000, 0x00FF, 0x0105},
	{0xC000, 0x00FF, 0x0104},
	{0xC000, 0x00FF, 0x0103},
	{0xC000, 0x00FF, 0x0102},
	{0xC000, 0x00FF, 0x0101},
	{0xC000, 0x00FF, 0x0100},		/* 36 db = 12db column amplifer gain, ~12 db analog gain, 12 db digital gain */
#else
	{0xC000, 0x00C0, 0x0180},		/* 42 db =12db column amplifer gain, ~6 db analog gain, 24 db digital gain */

	{0xC000, 0x00C0, 0x017F},
	{0xC000, 0x00C0, 0x017E},
	{0xC000, 0x00C0, 0x017D},
	{0xC000, 0x00C0, 0x017C},
	{0xC000, 0x00C0, 0x017B},
	{0xC000, 0x00C0, 0x017A},
	{0xC000, 0x00C0, 0x0179},
	{0xC000, 0x00C0, 0x0178},
	{0xC000, 0x00C0, 0x0177},
	{0xC000, 0x00C0, 0x0176},
	{0xC000, 0x00C0, 0x0175},
	{0xC000, 0x00C0, 0x0174},
	{0xC000, 0x00C0, 0x0173},
	{0xC000, 0x00C0, 0x0172},
	{0xC000, 0x00C0, 0x0171},
	{0xC000, 0x00C0, 0x0170},

	{0xC000, 0x00C0, 0x016F},
	{0xC000, 0x00C0, 0x016E},
	{0xC000, 0x00C0, 0x016D},
	{0xC000, 0x00C0, 0x016C},
	{0xC000, 0x00C0, 0x016B},
	{0xC000, 0x00C0, 0x016A},
	{0xC000, 0x00C0, 0x0169},
	{0xC000, 0x00C0, 0x0168},
	{0xC000, 0x00C0, 0x0167},
	{0xC000, 0x00C0, 0x0166},
	{0xC000, 0x00C0, 0x0165},
	{0xC000, 0x00C0, 0x0164},
	{0xC000, 0x00C0, 0x0163},
	{0xC000, 0x00C0, 0x0162},
	{0xC000, 0x00C0, 0x0161},
	{0xC000, 0x00C0, 0x0160},		/* 39 db */


	{0xC000, 0x00C0, 0x015F},
	{0xC000, 0x00C0, 0x015E},
	{0xC000, 0x00C0, 0x015D},
	{0xC000, 0x00C0, 0x015C},
	{0xC000, 0x00C0, 0x015B},
	{0xC000, 0x00C0, 0x015A},
	{0xC000, 0x00C0, 0x0159},
	{0xC000, 0x00C0, 0x0158},
	{0xC000, 0x00C0, 0x0157},
	{0xC000, 0x00C0, 0x0156},
	{0xC000, 0x00C0, 0x0155},
	{0xC000, 0x00C0, 0x0154},
	{0xC000, 0x00C0, 0x0153},
	{0xC000, 0x00C0, 0x0152},
	{0xC000, 0x00C0, 0x0151},
	{0xC000, 0x00C0, 0x0150},

	{0xC000, 0x00C0, 0x014F},
	{0xC000, 0x00C0, 0x014E},
	{0xC000, 0x00C0, 0x014D},
	{0xC000, 0x00C0, 0x014C},
	{0xC000, 0x00C0, 0x014B},
	{0xC000, 0x00C0, 0x014A},
	{0xC000, 0x00C0, 0x0149},
	{0xC000, 0x00C0, 0x0148},
	{0xC000, 0x00C0, 0x0147},
	{0xC000, 0x00C0, 0x0146},
	{0xC000, 0x00C0, 0x0145},
	{0xC000, 0x00C0, 0x0144},
	{0xC000, 0x00C0, 0x0143},
	{0xC000, 0x00C0, 0x0142},
	{0xC000, 0x00C0, 0x0141},
	{0xC000, 0x00C0, 0x0140},		/* 36 db = 12db column amplifer gain, ~6 db analog gain, 18 db digital gain */
#endif

	{0xC000, 0x00C0, 0x013F},
	{0xC000, 0x00C0, 0x013E},
	{0xC000, 0x00C0, 0x013D},
	{0xC000, 0x00C0, 0x013C},
	{0xC000, 0x00C0, 0x013B},
	{0xC000, 0x00C0, 0x013A},
	{0xC000, 0x00C0, 0x0139},
	{0xC000, 0x00C0, 0x0138},
	{0xC000, 0x00C0, 0x0137},
	{0xC000, 0x00C0, 0x0136},
	{0xC000, 0x00C0, 0x0135},
	{0xC000, 0x00C0, 0x0134},
	{0xC000, 0x00C0, 0x0133},
	{0xC000, 0x00C0, 0x0132},
	{0xC000, 0x00C0, 0x0131},
	{0xC000, 0x00C0, 0x0130},

	{0xC000, 0x00C0, 0x012F},
	{0xC000, 0x00C0, 0x012E},
	{0xC000, 0x00C0, 0x012D},
	{0xC000, 0x00C0, 0x012C},
	{0xC000, 0x00C0, 0x012B},
	{0xC000, 0x00C0, 0x012A},
	{0xC000, 0x00C0, 0x0129},
	{0xC000, 0x00C0, 0x0128},
	{0xC000, 0x00C0, 0x0127},
	{0xC000, 0x00C0, 0x0126},
	{0xC000, 0x00C0, 0x0125},
	{0xC000, 0x00C0, 0x0124},
	{0xC000, 0x00C0, 0x0123},
	{0xC000, 0x00C0, 0x0122},
	{0xC000, 0x00C0, 0x0121},
	{0xC000, 0x00C0, 0x0120},		 /* 33 db*/

	{0xC000, 0x00C0, 0x011F},
	{0xC000, 0x00C0, 0x011E},
	{0xC000, 0x00C0, 0x011D},
	{0xC000, 0x00C0, 0x011C},
	{0xC000, 0x00C0, 0x011B},
	{0xC000, 0x00C0, 0x011A},
	{0xC000, 0x00C0, 0x0119},
	{0xC000, 0x00C0, 0x0118},
	{0xC000, 0x00C0, 0x0117},
	{0xC000, 0x00C0, 0x0116},
	{0xC000, 0x00C0, 0x0115},
	{0xC000, 0x00C0, 0x0114},
	{0xC000, 0x00C0, 0x0113},
	{0xC000, 0x00C0, 0x0112},
	{0xC000, 0x00C0, 0x0111},
	{0xC000, 0x00C0, 0x0110},

	{0xC000, 0x00C0, 0x010F},
	{0xC000, 0x00C0, 0x010E},
	{0xC000, 0x00C0, 0x010D},
	{0xC000, 0x00C0, 0x010C},
	{0xC000, 0x00C0, 0x010B},
	{0xC000, 0x00C0, 0x010A},
	{0xC000, 0x00C0, 0x0109},
	{0xC000, 0x00C0, 0x0108},
	{0xC000, 0x00C0, 0x0107},
	{0xC000, 0x00C0, 0x0106},
	{0xC000, 0x00C0, 0x0105},
	{0xC000, 0x00C0, 0x0104},
	{0xC000, 0x00C0, 0x0103},
	{0xC000, 0x00C0, 0x0102},
	{0xC000, 0x00C0, 0x0101},
	{0xC000, 0x00C0, 0x0100},		/* 30 db*/

	{0xC000, 0x00C0, 0x00FF},
	{0xC000, 0x00C0, 0x00FE},
	{0xC000, 0x00C0, 0x00FD},
	{0xC000, 0x00C0, 0x00FC},
	{0xC000, 0x00C0, 0x00FB},
	{0xC000, 0x00C0, 0x00FA},
	{0xC000, 0x00C0, 0x00F9},
	{0xC000, 0x00C0, 0x00F8},
	{0xC000, 0x00C0, 0x00F7},
	{0xC000, 0x00C0, 0x00F6},
	{0xC000, 0x00C0, 0x00F5},
	{0xC000, 0x00C0, 0x00F4},
	{0xC000, 0x00C0, 0x00F3},
	{0xC000, 0x00C0, 0x00F2},
	{0xC000, 0x00C0, 0x00F1},
	{0xC000, 0x00C0, 0x00F0},

	{0xC000, 0x00C0, 0x00EF},
	{0xC000, 0x00C0, 0x00EE},
	{0xC000, 0x00C0, 0x00ED},
	{0xC000, 0x00C0, 0x00EC},
	{0xC000, 0x00C0, 0x00EB},
	{0xC000, 0x00C0, 0x00EA},
	{0xC000, 0x00C0, 0x00E9},
	{0xC000, 0x00C0, 0x00E8},
	{0xC000, 0x00C0, 0x00E7},
	{0xC000, 0x00C0, 0x00E6},
	{0xC000, 0x00C0, 0x00E5},
	{0xC000, 0x00C0, 0x00E4},
	{0xC000, 0x00C0, 0x00E3},
	{0xC000, 0x00C0, 0x00E2},
	{0xC000, 0x00C0, 0x00E1},
	{0xC000, 0x00C0, 0x00E0},		/*27 db */

	{0xC000, 0x00C0, 0x00DF},
	{0xC000, 0x00C0, 0x00DE},
	{0xC000, 0x00C0, 0x00DD},
	{0xC000, 0x00C0, 0x00DC},
	{0xC000, 0x00C0, 0x00DB},
	{0xC000, 0x00C0, 0x00DA},
	{0xC000, 0x00C0, 0x00D9},
	{0xC000, 0x00C0, 0x00D8},
	{0xC000, 0x00C0, 0x00D7},
	{0xC000, 0x00C0, 0x00D6},
	{0xC000, 0x00C0, 0x00D5},
	{0xC000, 0x00C0, 0x00D4},
	{0xC000, 0x00C0, 0x00D3},
	{0xC000, 0x00C0, 0x00D2},
	{0xC000, 0x00C0, 0x00D1},
	{0xC000, 0x00C0, 0x00D0},

	{0xC000, 0x00C0, 0x00CF},
	{0xC000, 0x00C0, 0x00CE},
	{0xC000, 0x00C0, 0x00CD},
	{0xC000, 0x00C0, 0x00CC},
	{0xC000, 0x00C0, 0x00CB},
	{0xC000, 0x00C0, 0x00CA},
	{0xC000, 0x00C0, 0x00C9},
	{0xC000, 0x00C0, 0x00C8},
	{0xC000, 0x00C0, 0x00C7},
	{0xC000, 0x00C0, 0x00C6},
	{0xC000, 0x00C0, 0x00C5},
	{0xC000, 0x00C0, 0x00C4},
	{0xC000, 0x00C0, 0x00C3},
	{0xC000, 0x00C0, 0x00C2},
	{0xC000, 0x00C0, 0x00C1},
	{0xC000, 0x00C0, 0x00C0},		/* 24 db,*/

	{0xC000, 0x00C0, 0x00BF},
	{0xC000, 0x00C0, 0x00BE},
	{0xC000, 0x00C0, 0x00BD},
	{0xC000, 0x00C0, 0x00BC},
	{0xC000, 0x00C0, 0x00BB},
	{0xC000, 0x00C0, 0x00BA},
	{0xC000, 0x00C0, 0x00B9},
	{0xC000, 0x00C0, 0x00B8},
	{0xC000, 0x00C0, 0x00B7},
	{0xC000, 0x00C0, 0x00B6},
	{0xC000, 0x00C0, 0x00B5},
	{0xC000, 0x00C0, 0x00B4},
	{0xC000, 0x00C0, 0x00B3},
	{0xC000, 0x00C0, 0x00B2},
	{0xC000, 0x00C0, 0x00B1},
	{0xC000, 0x00C0, 0x00B0},

	{0xC000, 0x00C0, 0x00AF},
	{0xC000, 0x00C0, 0x00AE},
	{0xC000, 0x00C0, 0x00AD},
	{0xC000, 0x00C0, 0x00AC},
	{0xC000, 0x00C0, 0x00AB},
	{0xC000, 0x00C0, 0x00AA},
	{0xC000, 0x00C0, 0x00A9},
	{0xC000, 0x00C0, 0x00A8},
	{0xC000, 0x00C0, 0x00A7},
	{0xC000, 0x00C0, 0x00A6},
	{0xC000, 0x00C0, 0x00A5},
	{0xC000, 0x00C0, 0x00A4},
	{0xC000, 0x00C0, 0x00A3},
	{0xC000, 0x00C0, 0x00A2},
	{0xC000, 0x00C0, 0x00A1},
	{0xC000, 0x00C0, 0x00A0},		/* 21 db */

	{0xC000, 0x00C0, 0x009F},
	{0xC000, 0x00C0, 0x009E},
	{0xC000, 0x00C0, 0x009D},
	{0xC000, 0x00C0, 0x009C},
	{0xC000, 0x00C0, 0x009B},
	{0xC000, 0x00C0, 0x009A},
	{0xC000, 0x00C0, 0x0099},
	{0xC000, 0x00C0, 0x0098},
	{0xC000, 0x00C0, 0x0097},
	{0xC000, 0x00C0, 0x0096},
	{0xC000, 0x00C0, 0x0095},
	{0xC000, 0x00C0, 0x0094},
	{0xC000, 0x00C0, 0x0093},
	{0xC000, 0x00C0, 0x0092},
	{0xC000, 0x00C0, 0x0091},
	{0xC000, 0x00C0, 0x0090},

	{0xC000, 0x00C0, 0x008F},
	{0xC000, 0x00C0, 0x008E},
	{0xC000, 0x00C0, 0x008D},
	{0xC000, 0x00C0, 0x008C},
	{0xC000, 0x00C0, 0x008B},
	{0xC000, 0x00C0, 0x008A},
	{0xC000, 0x00C0, 0x0089},
	{0xC000, 0x00C0, 0x0088},
	{0xC000, 0x00C0, 0x0087},
	{0xC000, 0x00C0, 0x0086},
	{0xC000, 0x00C0, 0x0085},
	{0xC000, 0x00C0, 0x0084},
	{0xC000, 0x00C0, 0x0083},
	{0xC000, 0x00C0, 0x0082},
	{0xC000, 0x00C0, 0x0081},
	{0xC000, 0x00C0, 0x0080},		/* 18 db */

	{0xC000, 0x0080, 0x00BF},
	{0xC000, 0x0080, 0x00BE},
	{0xC000, 0x0080, 0x00BD},
	{0xC000, 0x0080, 0x00BC},
	{0xC000, 0x0080, 0x00BB},
	{0xC000, 0x0080, 0x00BA},
	{0xC000, 0x0080, 0x00B9},
	{0xC000, 0x0080, 0x00B8},
	{0xC000, 0x0080, 0x00B7},
	{0xC000, 0x0080, 0x00B6},
	{0xC000, 0x0080, 0x00B5},
	{0xC000, 0x0080, 0x00B4},
	{0xC000, 0x0080, 0x00B3},
	{0xC000, 0x0080, 0x00B2},
	{0xC000, 0x0080, 0x00B1},
	{0xC000, 0x0080, 0x00B0},

	{0xC000, 0x0080, 0x00AF},
	{0xC000, 0x0080, 0x00AE},
	{0xC000, 0x0080, 0x00AD},
	{0xC000, 0x0080, 0x00AC},
	{0xC000, 0x0080, 0x00AB},
	{0xC000, 0x0080, 0x00AA},
	{0xC000, 0x0080, 0x00A9},
	{0xC000, 0x0080, 0x00A8},
	{0xC000, 0x0080, 0x00A7},
	{0xC000, 0x0080, 0x00A6},
	{0xC000, 0x0080, 0x00A5},
	{0xC000, 0x0080, 0x00A4},
	{0xC000, 0x0080, 0x00A3},
	{0xC000, 0x0080, 0x00A2},
	{0xC000, 0x0080, 0x00A1},
	{0xC000, 0x0080, 0x00A0},		/* 15 db */

	{0xC000, 0x0080, 0x009F},
	{0xC000, 0x0080, 0x009E},
	{0xC000, 0x0080, 0x009D},
	{0xC000, 0x0080, 0x009C},
	{0xC000, 0x0080, 0x009B},
	{0xC000, 0x0080, 0x009A},
	{0xC000, 0x0080, 0x0099},
	{0xC000, 0x0080, 0x0098},
	{0xC000, 0x0080, 0x0097},
	{0xC000, 0x0080, 0x0096},
	{0xC000, 0x0080, 0x0095},
	{0xC000, 0x0080, 0x0094},
	{0xC000, 0x0080, 0x0093},
	{0xC000, 0x0080, 0x0092},
	{0xC000, 0x0080, 0x0091},
	{0xC000, 0x0080, 0x0090},

	{0xC000, 0x0080, 0x008F},
	{0xC000, 0x0080, 0x008E},
	{0xC000, 0x0080, 0x008D},
	{0xC000, 0x0080, 0x008C},
	{0xC000, 0x0080, 0x008B},
	{0xC000, 0x0080, 0x008A},
	{0xC000, 0x0080, 0x0089},
	{0xC000, 0x0080, 0x0088},
	{0xC000, 0x0080, 0x0087},
	{0xC000, 0x0080, 0x0086},
	{0xC000, 0x0080, 0x0085},
	{0xC000, 0x0080, 0x0084},
	{0xC000, 0x0080, 0x0083},
	{0xC000, 0x0080, 0x0082},
	{0xC000, 0x0080, 0x0081},
	{0xC000, 0x0080, 0x0080},		/* 12 db*/

	{0x8000, 0x0080, 0x00BF},
	{0x8000, 0x0080, 0x00BE},
	{0x8000, 0x0080, 0x00BD},
	{0x8000, 0x0080, 0x00BC},
	{0x8000, 0x0080, 0x00BB},
	{0x8000, 0x0080, 0x00BA},
	{0x8000, 0x0080, 0x00B9},
	{0x8000, 0x0080, 0x00B8},
	{0x8000, 0x0080, 0x00B7},
	{0x8000, 0x0080, 0x00B6},
	{0x8000, 0x0080, 0x00B5},
	{0x8000, 0x0080, 0x00B4},
	{0x8000, 0x0080, 0x00B3},
	{0x8000, 0x0080, 0x00B2},
	{0x8000, 0x0080, 0x00B1},
	{0x8000, 0x0080, 0x00B0},

	{0x8000, 0x0080, 0x00AF},
	{0x8000, 0x0080, 0x00AE},
	{0x8000, 0x0080, 0x00AD},
	{0x8000, 0x0080, 0x00AC},
	{0x8000, 0x0080, 0x00AB},
	{0x8000, 0x0080, 0x00AA},
	{0x8000, 0x0080, 0x00A9},
	{0x8000, 0x0080, 0x00A8},
	{0x8000, 0x0080, 0x00A7},
	{0x8000, 0x0080, 0x00A6},
	{0x8000, 0x0080, 0x00A5},
	{0x8000, 0x0080, 0x00A4},
	{0x8000, 0x0080, 0x00A3},
	{0x8000, 0x0080, 0x00A2},
	{0x8000, 0x0080, 0x00A1},
	{0x8000, 0x0080, 0x00A0},		/* 9 db */

	{0x8000, 0x0080, 0x009F},
	{0x8000, 0x0080, 0x009E},
	{0x8000, 0x0080, 0x009D},
	{0x8000, 0x0080, 0x009C},
	{0x8000, 0x0080, 0x009B},
	{0x8000, 0x0080, 0x009A},
	{0x8000, 0x0080, 0x0099},
	{0x8000, 0x0080, 0x0098},
	{0x8000, 0x0080, 0x0097},
	{0x8000, 0x0080, 0x0096},
	{0x8000, 0x0080, 0x0095},
	{0x8000, 0x0080, 0x0094},
	{0x8000, 0x0080, 0x0093},
	{0x8000, 0x0080, 0x0092},
	{0x8000, 0x0080, 0x0091},
	{0x8000, 0x0080, 0x0090},

	{0x8000, 0x0080, 0x008F},
	{0x8000, 0x0080, 0x008E},
	{0x8000, 0x0080, 0x008D},
	{0x8000, 0x0080, 0x008C},
	{0x8000, 0x0080, 0x008B},
	{0x8000, 0x0080, 0x008A},
	{0x8000, 0x0080, 0x0089},
	{0x8000, 0x0080, 0x0088},
	{0x8000, 0x0080, 0x0087},
	{0x8000, 0x0080, 0x0086},
	{0x8000, 0x0080, 0x0085},
	{0x8000, 0x0080, 0x0084},
	{0x8000, 0x0080, 0x0083},
	{0x8000, 0x0080, 0x0082},
	{0x8000, 0x0080, 0x0081},
	{0x8000, 0x0080, 0x0080},		/* 6 db */

	{0x0000, 0x0080, 0x00BF},
	{0x0000, 0x0080, 0x00BE},
	{0x0000, 0x0080, 0x00BD},
	{0x0000, 0x0080, 0x00BC},
	{0x0000, 0x0080, 0x00BB},
	{0x0000, 0x0080, 0x00BA},
	{0x0000, 0x0080, 0x00B9},
	{0x0000, 0x0080, 0x00B8},
	{0x0000, 0x0080, 0x00B7},
	{0x0000, 0x0080, 0x00B6},
	{0x0000, 0x0080, 0x00B5},
	{0x0000, 0x0080, 0x00B4},
	{0x0000, 0x0080, 0x00B3},
	{0x0000, 0x0080, 0x00B2},
	{0x0000, 0x0080, 0x00B1},
	{0x0000, 0x0080, 0x00B0},

	{0x0000, 0x0080, 0x00AF},
	{0x0000, 0x0080, 0x00AE},
	{0x0000, 0x0080, 0x00AD},
	{0x0000, 0x0080, 0x00AC},
	{0x0000, 0x0080, 0x00AB},
	{0x0000, 0x0080, 0x00AA},
	{0x0000, 0x0080, 0x00A9},
	{0x0000, 0x0080, 0x00A8},
	{0x0000, 0x0080, 0x00A7},
	{0x0000, 0x0080, 0x00A6},
	{0x0000, 0x0080, 0x00A5},
	{0x0000, 0x0080, 0x00A4},
	{0x0000, 0x0080, 0x00A3},
	{0x0000, 0x0080, 0x00A2},
	{0x0000, 0x0080, 0x00A1},
	{0x0000, 0x0080, 0x00A0},		/* 3 db */

	{0x0000, 0x0080, 0x009F},
	{0x0000, 0x0080, 0x009E},
	{0x0000, 0x0080, 0x009D},
	{0x0000, 0x0080, 0x009C},
	{0x0000, 0x0080, 0x009B},
	{0x0000, 0x0080, 0x009A},
	{0x0000, 0x0080, 0x0099},
	{0x0000, 0x0080, 0x0098},
	{0x0000, 0x0080, 0x0097},
	{0x0000, 0x0080, 0x0096},
	{0x0000, 0x0080, 0x0095},
	{0x0000, 0x0080, 0x0094},
	{0x0000, 0x0080, 0x0093},
	{0x0000, 0x0080, 0x0092},
	{0x0000, 0x0080, 0x0091},
	{0x0000, 0x0080, 0x0090},

	{0x0000, 0x0080, 0x008F},
	{0x0000, 0x0080, 0x008E},
	{0x0000, 0x0080, 0x008D},
	{0x0000, 0x0080, 0x008C},
	{0x0000, 0x0080, 0x008B},
	{0x0000, 0x0080, 0x008A},
	{0x0000, 0x0080, 0x0089},
	{0x0000, 0x0080, 0x0088},
	{0x0000, 0x0080, 0x0087},
	{0x0000, 0x0080, 0x0086},
	{0x0000, 0x0080, 0x0085},
	{0x0000, 0x0080, 0x0084},
	{0x0000, 0x0080, 0x0083},
	{0x0000, 0x0080, 0x0082},
	{0x0000, 0x0080, 0x0081},
	{0x0000, 0x0080, 0x0080},		/* 0 db */
};

