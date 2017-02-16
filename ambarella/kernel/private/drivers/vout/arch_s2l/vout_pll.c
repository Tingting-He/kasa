/*
 * kernel/private/drivers/ambarella/vout/vout_pll.c
 *
 * Author: Anthony Ginger <hfjiang@ambarella.com>
 *
 * Copyright (C) 2004-2013, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 *
 */

#include <linux/io.h>
#include <linux/i2c.h>
#include <linux/clk.h>
#include <plat/clk.h>

#include "vout_pri.h"

/* ==========================================================================*/
static struct clk gclk_vo = {
	.parent		= NULL,
	.name		= "gclk_vo",
	.rate		= 0,
	.frac_mode	= 1,
	.ctrl_reg	= PLL_HDMI_CTRL_REG,
	.pres_reg	= SCALER_HDMI_PRE_REG,
	.pres_val	= 4,
	.post_reg	= -1,
	.frac_reg	= PLL_HDMI_FRAC_REG,
	.ctrl2_reg	= PLL_HDMI_CTRL2_REG,
	.ctrl3_reg	= PLL_HDMI_CTRL3_REG,
	.ctrl3_val	= 0x00068306,
	.lock_reg	= PLL_LOCK_REG,
	.lock_bit	= 8,
	.divider	= 10,
	.max_divider	= 0,
	.extra_scaler	= 1,
	.table		= ambarella_pll_vout_table,
	.table_size	= ARRAY_SIZE(ambarella_pll_vout_table),
	.ops		= &ambarella_rct_pll_ops,
};

static struct clk *rct_register_clk_vout(void)
{
	struct clk *pgclk_vo = NULL;

	pgclk_vo = clk_get(NULL, "gclk_vo");
	if (IS_ERR(pgclk_vo)) {
		ambarella_clk_add(&gclk_vo);
		pgclk_vo = &gclk_vo;
		pr_info("SYSCLK:VOUT[%lu]\n", clk_get_rate(pgclk_vo));
	}

	return pgclk_vo;
}

static struct clk gclk_vo2 = {
	.parent		= NULL,
	.name		= "gclk_vo2",
	.rate		= 0,
	.frac_mode	= 1,
	.ctrl_reg	= PLL_VIDEO2_CTRL_REG,
	.pres_reg	= SCALER_VIDEO2_PRE_REG,
	.pres_val	= 4,
	.post_reg	= SCALER_VIDEO2_POST_REG,
	.post_val	= 10,
	.frac_reg	= PLL_VIDEO2_FRAC_REG,
	.ctrl2_reg	= PLL_VIDEO2_CTRL2_REG,
	.ctrl3_reg	= PLL_VIDEO2_CTRL3_REG,
	.ctrl3_val	= 0x00068306,
	.lock_reg	= PLL_LOCK_REG,
	.lock_bit	= 0,
	.divider	= 0,
	.max_divider	= (1 << 4) - 1,
	.extra_scaler	= 1,
	.table		= ambarella_pll_vout_table,
	.table_size	= ARRAY_SIZE(ambarella_pll_vout_table),
	.ops		= &ambarella_rct_pll_ops,
};

static struct clk *rct_register_clk_vout2(void)
{
	struct clk *pgclk_vo2 = NULL;

	pgclk_vo2 = clk_get(NULL, "gclk_vo2");
	if (IS_ERR(pgclk_vo2)) {
		ambarella_clk_add(&gclk_vo2);
		pgclk_vo2 = &gclk_vo2;
		pr_info("SYSCLK:VOUT[%lu]\n", clk_get_rate(pgclk_vo2));
	}

	return pgclk_vo2;
}

/* ========================================================================== */
void rct_set_vout_clk_src(u32 mode)
{
}

void rct_set_vout_freq_hz(u32 freq_hz)
{
	clk_set_rate(rct_register_clk_vout(), freq_hz);
}

u32 rct_get_vout_freq_hz(void)
{
	return clk_get_rate(rct_register_clk_vout());
}

void rct_set_vout2_clk_src(u32 mode)
{
}

void rct_set_vout2_freq_hz(u32 freq_hz)
{
	clk_set_rate(rct_register_clk_vout2(), freq_hz);
}

u32 rct_get_vout2_freq_hz(void)
{
	return clk_get_rate(rct_register_clk_vout2());
}

