/*
 * ambhw/rct.h
 *
 * History:
 *	2006/12/27 - [Charles Chiou] created file
 *
 * Copyright (C) 2006-2008, Ambarella, Inc.
 */

#ifndef __AMBHW_RCT_H__
#define __AMBHW_RCT_H__

/* ==========================================================================*/
#define RCT_OFFSET			0x170000
#if (CHIP_REV == A8) || (CHIP_REV == S2L) || (CHIP_REV == S3) || (CHIP_REV == S3L)
#define RCT_BASE			(DBGBUS_BASE + RCT_OFFSET)
#define RCT_PHYS_BASE			(DBGBUS_PHYS_BASE + RCT_OFFSET)
#else
#define RCT_BASE			(APB_BASE + RCT_OFFSET)
#define RCT_PHYS_BASE			(APB_PHYS_BASE + RCT_OFFSET)
#endif
#define RCT_REG(x)			(RCT_BASE + (x))

/* ==========================================================================*/
#define PLL_LOCK_OFFSET			0x2C
#define SOFT_OR_DLL_RESET_OFFSET	0x68

#define PLL_LOCK_REG			RCT_REG(PLL_LOCK_OFFSET)
#define SOFT_OR_DLL_RESET_REG		RCT_REG(SOFT_OR_DLL_RESET_OFFSET)

/* ==========================================================================*/
#define FIO_RESET_OFFSET		0x74
#define FIO_RESET_REG			RCT_REG(FIO_RESET_OFFSET)
#define FIO_RESET_FIO_RST		0x00000008
#define FIO_RESET_CF_RST		0x00000004
#define FIO_RESET_XD_RST		0x00000002
#define FIO_RESET_FLASH_RST		0x00000001

/* ==========================================================================*/
#if (CHIP_REV == A5S) || (CHIP_REV == S2) || (CHIP_REV == S2E)
#define USBP1_CTRL_OFFSET		0x88
#define UDC_SOFT_RESET_OFFSET		USBP1_CTRL_OFFSET
#define UDC_SOFT_RESET_MASK		0x20000000
#else
#define USBC_CTRL_OFFSET		0x2cc
#define UDC_SOFT_RESET_OFFSET		USBC_CTRL_OFFSET
#define UDC_SOFT_RESET_MASK		0x2
#endif
#define UDC_SOFT_RESET_REG		RCT_REG(UDC_SOFT_RESET_OFFSET)

#if (CHIP_REV == S2) || (CHIP_REV == S2E)
#define USB0_IS_HOST_MASK		0x00000020
#else
#define USBP0_SEL_OFFSET		0x2c0
#define USBP0_SEL_REG			RCT_REG(USBP0_SEL_OFFSET)
#define USB0_IS_HOST_MASK		0x00000002
#endif

/* ==========================================================================*/
#define PLL_AUDIO_CTRL_OFFSET		0x54
#define PLL_AUDIO_FRAC_OFFSET		0x58
#define SCALER_AUDIO_POST_OFFSET	0x5C
#define SCALER_AUDIO_PRE_OFFSET		0x60
#define PLL_AUDIO_CTRL2_OFFSET		0x124
#define PLL_AUDIO_CTRL3_OFFSET		0x12c

#define PLL_AUDIO_CTRL_REG		RCT_REG(PLL_AUDIO_CTRL_OFFSET)
#define PLL_AUDIO_FRAC_REG		RCT_REG(PLL_AUDIO_FRAC_OFFSET)
#define SCALER_AUDIO_POST_REG		RCT_REG(SCALER_AUDIO_POST_OFFSET)
#define SCALER_AUDIO_PRE_REG		RCT_REG(SCALER_AUDIO_PRE_OFFSET)
#define PLL_AUDIO_CTRL2_REG		RCT_REG(PLL_AUDIO_CTRL2_OFFSET)
#define PLL_AUDIO_CTRL3_REG		RCT_REG(PLL_AUDIO_CTRL3_OFFSET)

/* ==========================================================================*/
#define ANA_PWR_OFFSET			0x50
#define ANA_PWR_REG			RCT_REG(ANA_PWR_OFFSET)
#define ANA_PWR_POWER_DOWN		0x0020

#define SYS_CONFIG_OFFSET		0x34
#define SYS_CONFIG_REG			RCT_REG(SYS_CONFIG_OFFSET)

#define WDT_RST_L_OFFSET		0x78
#if (CHIP_REV == S2) || (CHIP_REV == S2E)
#define UNLOCK_WDT_RST_L_OFFSET		0x50
#define UNLOCK_WDT_RST_L_VAL		0x80
#else
#define UNLOCK_WDT_RST_L_OFFSET		0x260
#define UNLOCK_WDT_RST_L_VAL		0x01
#endif
#define WDT_RST_L_REG			RCT_REG(WDT_RST_L_OFFSET)
#define UNLOCK_WDT_RST_L_REG		RCT_REG(UNLOCK_WDT_RST_L_OFFSET)

/* ==========================================================================*/
#define CG_UART_OFFSET			0x38
#define CG_SSI_OFFSET			0x3C
#define CG_MOTOR_OFFSET			0x40
#define CG_IR_OFFSET			0x44
#define CG_HOST_OFFSET			0x48
#define CG_PWM_OFFSET			0x84
#define CG_SSI2_OFFSET			0xEC
#define CLK_REF_SSI_OFFSET		0x19c
#define CG_SSI3_OFFSET			0x518
#define CLK_REF_SSI3_OFFSET		0x51c

#define CG_UART_REG			RCT_REG(CG_UART_OFFSET)
#define CG_SSI_REG			RCT_REG(CG_SSI_OFFSET)
#define CG_MOTOR_REG			RCT_REG(CG_MOTOR_OFFSET)
#define CG_IR_REG			RCT_REG(CG_IR_OFFSET)
#define CG_HOST_REG			RCT_REG(CG_HOST_OFFSET)
#define CG_PWM_REG			RCT_REG(CG_PWM_OFFSET)
#define CG_SSI2_REG			RCT_REG(CG_SSI2_OFFSET)
#define CG_SSI3_REG			RCT_REG(CG_SSI3_OFFSET)
#define CLK_REF_SSI_REG			RCT_REG(CLK_REF_SSI_OFFSET)
#define CLK_REF_SSI3_REG		RCT_REG(CLK_REF_SSI3_OFFSET)

#define UART_CLK_SRC_CLK_REF		0x00
#define UART_CLK_SRC_CORE		0x01
#define UART_CLK_SRC_IDSP		0x03
#if (CHIP_REV == S2) || (CHIP_REV == S2E)
#define UART_CLK_SRC_SEL_OFFSET		0x320
#else
#define UART_CLK_SRC_SEL_OFFSET		0x1C8
#endif
#define UART_CLK_SRC_SEL_REG		RCT_REG(UART_CLK_SRC_SEL_OFFSET)

/* ==========================================================================*/
#define PLL_CORE_CTRL_OFFSET		0x00
#define PLL_CORE_FRAC_OFFSET		0x04
#define PLL_CORE_CTRL2_OFFSET		0x100
#define PLL_CORE_CTRL3_OFFSET		0x104
#define PLL_CORE_CTRL_REG		RCT_REG(PLL_CORE_CTRL_OFFSET)
#define PLL_CORE_FRAC_REG		RCT_REG(PLL_CORE_FRAC_OFFSET)
#define PLL_CORE_CTRL2_REG		RCT_REG(PLL_CORE_CTRL2_OFFSET)
#define PLL_CORE_CTRL3_REG		RCT_REG(PLL_CORE_CTRL3_OFFSET)

#if ((CHIP_REV == A5S) || (CHIP_REV == S2) || (CHIP_REV == S2E))
#define SCALER_CORE_POST_OFFSET		0x118
#define CORE_CLK_RATIO_1X_OFFSET	0x24C
#define SCALER_CORE_POST_REG		RCT_REG(SCALER_CORE_POST_OFFSET)
#define CORE_CLK_RATIO_1X_REG		RCT_REG(CORE_CLK_RATIO_1X_OFFSET)
#endif

/* ==========================================================================*/

#define PLL_HDMILC_CTRL_OFFSET		0x404
#define PLL_HDMILC_FRAC_OFFSET		0x408
#define PLL_HDMILC_CTRL2_OFFSET		0x40c
#define PLL_HDMILC_CTRL3_OFFSET		0x410
#define PLL_HDMILC_CTRL4_OFFSET		0x418
#define PLL_HDMILC_CTRL_REG		RCT_REG(PLL_HDMILC_CTRL_OFFSET)
#define PLL_HDMILC_FRAC_REG		RCT_REG(PLL_HDMILC_FRAC_OFFSET)
#define PLL_HDMILC_CTRL2_REG		RCT_REG(PLL_HDMILC_CTRL2_OFFSET)
#define PLL_HDMILC_CTRL3_REG		RCT_REG(PLL_HDMILC_CTRL3_OFFSET)
#define PLL_HDMILC_CTRL4_REG		RCT_REG(PLL_HDMILC_CTRL4_OFFSET)

/* ==========================================================================*/
#define PLL_IDSP_CTRL_OFFSET		0xE4
#define PLL_IDSP_FRAC_OFFSET		0xE8
#define PLL_IDSP_CTRL2_OFFSET		0x108
#define PLL_IDSP_CTRL3_OFFSET		0x10C
#define SCALER_IDSP_POST_OFFSET		0x1F4

#define PLL_IDSP_CTRL_REG		RCT_REG(PLL_IDSP_CTRL_OFFSET)
#define PLL_IDSP_FRAC_REG		RCT_REG(PLL_IDSP_FRAC_OFFSET)
#define PLL_IDSP_CTRL2_REG		RCT_REG(PLL_IDSP_CTRL2_OFFSET)
#define PLL_IDSP_CTRL3_REG		RCT_REG(PLL_IDSP_CTRL3_OFFSET)
#define SCALER_IDSP_POST_REG		RCT_REG(SCALER_IDSP_POST_OFFSET)

/* ==========================================================================*/
#define PLL_DDR_CTRL_OFFSET		0xDC
#define PLL_DDR_FRAC_OFFSET		0xE0
#define PLL_DDR_CTRL2_OFFSET		0x110
#define PLL_DDR_CTRL3_OFFSET		0x114

#define PLL_DDR_CTRL_REG		RCT_REG(PLL_DDR_CTRL_OFFSET)
#define PLL_DDR_FRAC_REG		RCT_REG(PLL_DDR_FRAC_OFFSET)
#define PLL_DDR_CTRL2_REG		RCT_REG(PLL_DDR_CTRL2_OFFSET)
#define PLL_DDR_CTRL3_REG		RCT_REG(PLL_DDR_CTRL3_OFFSET)

/* ==========================================================================*/
#define PLL_SENSOR_CTRL_OFFSET		0x24
#define PLL_SENSOR_FRAC_OFFSET		0x28
#define PLL_SENSOR_CTRL2_OFFSET		0x11C
#define PLL_SENSOR_CTRL3_OFFSET		0x120
#define SCALER_SENSOR_PRE_OFFSET	0x4C
#define SCALER_SENSOR_POST_OFFSET	0x30
#define CLK_SI_INPUT_MODE_OFFSET	0xBC

#define PLL_SENSOR_CTRL_REG		RCT_REG(PLL_SENSOR_CTRL_OFFSET)
#define PLL_SENSOR_FRAC_REG		RCT_REG(PLL_SENSOR_FRAC_OFFSET)
#define PLL_SENSOR_CTRL2_REG		RCT_REG(PLL_SENSOR_CTRL2_OFFSET)
#define PLL_SENSOR_CTRL3_REG		RCT_REG(PLL_SENSOR_CTRL3_OFFSET)
#define SCALER_SENSOR_PRE_REG		RCT_REG(SCALER_SENSOR_PRE_OFFSET)
#define SCALER_SENSOR_POST_REG		RCT_REG(SCALER_SENSOR_POST_OFFSET)
#define CLK_SI_INPUT_MODE_REG		RCT_REG(CLK_SI_INPUT_MODE_OFFSET)

/* ==========================================================================*/

#define HDMI_CLOCK_CTRL_OFFSET          0x008
#define PLL_HDMI_CTRL_OFFSET		0x164
#define PLL_HDMI_CTRL2_OFFSET		0x150
#define PLL_HDMI_CTRL3_OFFSET		0x154
#define PLL_HDMI_FRAC_OFFSET		0x168
#define SCALER_HDMI_PRE_OFFSET		0x170
#define SCALER_HDMI_POST_OFFSET		0x16C

#define HDMI_CLOCK_CTRL_REG		RCT_REG(HDMI_CLOCK_CTRL_OFFSET)
#define PLL_HDMI_CTRL_REG		RCT_REG(PLL_HDMI_CTRL_OFFSET)
#define PLL_HDMI_CTRL2_REG		RCT_REG(PLL_HDMI_CTRL2_OFFSET)
#define PLL_HDMI_CTRL3_REG		RCT_REG(PLL_HDMI_CTRL3_OFFSET)
#define PLL_HDMI_FRAC_REG		RCT_REG(PLL_HDMI_FRAC_OFFSET)
#define SCALER_HDMI_PRE_REG		RCT_REG(SCALER_HDMI_PRE_OFFSET)
#define SCALER_HDMI_POST_REG		RCT_REG(SCALER_HDMI_POST_OFFSET)

#define USE_CLK_SI_4_CLK_VO_OFFSET  	0xB8
#define CLK_REF_VIDEO_EXTERNAL_OFFSET	0xAC

#define USE_CLK_SI_4_CLK_VO_REG		RCT_REG(USE_CLK_SI_4_CLK_VO_OFFSET)
#define CLK_REF_VIDEO_EXTERNAL_REG	RCT_REG(CLK_REF_VIDEO_EXTERNAL_OFFSET)

/* ==========================================================================*/
#define PLL_VIDEO_CTRL_OFFSET		0x14
#define PLL_VIDEO_FRAC_OFFSET		0x18
#define SCALER_VIDEO_PRE_OFFSET		0x1C
#define SCALER_VIDEO_POST_OFFSET	0xA0
#define PLL_VIDEO_CTRL2_OFFSET		0x130
#define PLL_VIDEO_CTRL3_OFFSET		0x134
#define PLL_VIDEO_CTRL_REG		RCT_REG(PLL_VIDEO_CTRL_OFFSET)
#define PLL_VIDEO_FRAC_REG		RCT_REG(PLL_VIDEO_FRAC_OFFSET)
#define SCALER_VIDEO_PRE_REG		RCT_REG(SCALER_VIDEO_PRE_OFFSET)
#define SCALER_VIDEO_POST_REG		RCT_REG(SCALER_VIDEO_POST_OFFSET)
#define PLL_VIDEO_CTRL2_REG		RCT_REG(PLL_VIDEO_CTRL2_OFFSET)
#define PLL_VIDEO_CTRL3_REG		RCT_REG(PLL_VIDEO_CTRL3_OFFSET)

#define PLL_VIDEO2_CTRL_OFFSET		0xC0
#define PLL_VIDEO2_FRAC_OFFSET		0xC4
#define SCALER_VIDEO2_PRE_OFFSET	0xC8
#define SCALER_VIDEO2_POST_OFFSET	0xCC
#define PLL_VIDEO2_CTRL2_OFFSET		0x13C
#define PLL_VIDEO2_CTRL3_OFFSET		0x140
#define USE_CLK_SI_4_VO2_OFFSET		0xD0
#define USE_EXTERNAL_VD2_CLK_OFFSET	0xD4
#define CLK_REF_VIDEO2_EXTERNAL_OFFSET	0xD8

#define PLL_VIDEO2_CTRL_REG		RCT_REG(PLL_VIDEO2_CTRL_OFFSET)
#define PLL_VIDEO2_FRAC_REG		RCT_REG(PLL_VIDEO2_FRAC_OFFSET)
#define SCALER_VIDEO2_PRE_REG		RCT_REG(SCALER_VIDEO2_PRE_OFFSET)
#define SCALER_VIDEO2_POST_REG		RCT_REG(SCALER_VIDEO2_POST_OFFSET)
#define PLL_VIDEO2_CTRL2_REG		RCT_REG(PLL_VIDEO2_CTRL2_OFFSET)
#define PLL_VIDEO2_CTRL3_REG		RCT_REG(PLL_VIDEO2_CTRL3_OFFSET)
#define USE_CLK_SI_4_VO2_REG		RCT_REG(USE_CLK_SI_4_VO2_OFFSET)
#define USE_EXTERNAL_VD2_CLK_REG	RCT_REG(USE_EXTERNAL_VD2_CLK_OFFSET)
#define CLK_REF_VIDEO2_EXTERNAL_REG	RCT_REG(CLK_REF_VIDEO2_EXTERNAL_OFFSET)

/* ==========================================================================*/
#define SCALER_ARM_ASYNC_OFFSET		0x1F0

#define SCALER_ARM_ASYNC_REG		RCT_REG(SCALER_ARM_ASYNC_OFFSET)

/* ==========================================================================*/
#if (CHIP_REV == S2L) || (CHIP_REV == S3) || (CHIP_REV == S3L)
#define PLL_CORTEX_CTRL_OFFSET		0x264
#define PLL_CORTEX_FRAC_OFFSET		0x268
#define PLL_CORTEX_CTRL2_OFFSET		0x26C
#define PLL_CORTEX_CTRL3_OFFSET		0x270
#else
#define PLL_CORTEX_CTRL_OFFSET		0x2B0
#define PLL_CORTEX_FRAC_OFFSET		0x2B4
#define PLL_CORTEX_CTRL2_OFFSET		0x2B8
#define PLL_CORTEX_CTRL3_OFFSET		0x2BC
#endif

#define PLL_CORTEX_CTRL_REG		RCT_REG(PLL_CORTEX_CTRL_OFFSET)
#define PLL_CORTEX_FRAC_REG		RCT_REG(PLL_CORTEX_FRAC_OFFSET)
#define PLL_CORTEX_CTRL2_REG		RCT_REG(PLL_CORTEX_CTRL2_OFFSET)
#define PLL_CORTEX_CTRL3_REG		RCT_REG(PLL_CORTEX_CTRL3_OFFSET)

#if (CHIP_REV == S2) || (CHIP_REV == S2E)
#define SCALER_GTX_POST_OFFSET		0x2C8
#define ENET_GTXCLK_SRC_OFFSET		0x2CC
#endif
#define SCALER_GTX_POST_REG		RCT_REG(SCALER_GTX_POST_OFFSET)
#define ENET_GTXCLK_SRC_REG		RCT_REG(ENET_GTXCLK_SRC_OFFSET)

#if (CHIP_REV == S2L) || (CHIP_REV == S3) || (CHIP_REV == S3L)
#define PLL_ENET_CTRL_OFFSET		0x520
#define PLL_ENET_FRAC_OFFSET		0x524
#define PLL_ENET_CTRL2_OFFSET		0x528
#define PLL_ENET_CTRL3_OFFSET		0x52C
#define SCALER_ENET_POST_OFFSET		0x534
#endif
#define PLL_ENET_CTRL_REG		RCT_REG(PLL_ENET_CTRL_OFFSET)
#define PLL_ENET_FRAC_REG		RCT_REG(PLL_ENET_FRAC_OFFSET)
#define PLL_ENET_CTRL2_REG		RCT_REG(PLL_ENET_CTRL2_OFFSET)
#define PLL_ENET_CTRL3_REG		RCT_REG(PLL_ENET_CTRL3_OFFSET)
#define SCALER_ENET_POST_REG		RCT_REG(SCALER_ENET_POST_OFFSET)

/* ==========================================================================*/
#define SCALER_SD48_OFFSET		0x0C
#define SCALER_SD48_REG			RCT_REG(SCALER_SD48_OFFSET)

#if (CHIP_REV == S2) || (CHIP_REV == S2E)
#define SCALER_SDIO_OFFSET		0x10
#else
#define SCALER_SDIO_OFFSET		0x430
#endif
#define SCALER_SDIO_REG			RCT_REG(SCALER_SDIO_OFFSET)

#define SCALER_SDXC_OFFSET		0x434
#define SCALER_SDXC_REG			RCT_REG(SCALER_SDXC_OFFSET)

#if (CHIP_REV == S2L) || (CHIP_REV == S3) || (CHIP_REV == S3L)
#define PLL_SD_CTRL_OFFSET		0x4AC
#define PLL_SD_FRAC_OFFSET		0x4B0
#define PLL_SD_CTRL2_OFFSET		0x4B4
#define PLL_SD_CTRL3_OFFSET		0x4B8
#elif (CHIP_REV == S2E)
#define SDVCO_FOR_GTX_SOURCE            0x00000010
#define FUNC_MISC_CTRL_OFFSET           0x328
#define PLL_SD_CTRL_OFFSET		0x354
#define PLL_SD_FRAC_OFFSET		0x360
#define PLL_SD_CTRL2_OFFSET		0x358
#define PLL_SD_CTRL3_OFFSET		0x35C
#define FUNC_MISC_CTRL_REG		RCT_REG(FUNC_MISC_CTRL_OFFSET)
#endif
#define PLL_SD_CTRL_REG			RCT_REG(PLL_SD_CTRL_OFFSET)
#define PLL_SD_FRAC_REG			RCT_REG(PLL_SD_FRAC_OFFSET)
#define PLL_SD_CTRL2_REG		RCT_REG(PLL_SD_CTRL2_OFFSET)
#define PLL_SD_CTRL3_REG		RCT_REG(PLL_SD_CTRL3_OFFSET)

#if (CHIP_REV == A5S) || (CHIP_REV == S2)
#define SD_SOFT_PHY_SUPPORT		0
#else
#define SD_SOFT_PHY_SUPPORT		1
#endif

#if (SD_SOFT_PHY_SUPPORT == 1)
#define SD_PHY_CLKOUT_BYPASS		1 << 26
#define SD_PHY_RESET			1 << 25
#define SD_PHY_RX_CLK_POL		1 << 19
#define SD_PHY_DATA_CMD_BYPASS		1 << 18
#define SD_PHY_DLL_BYPASS		1 << 17

#if (CHIP_REV == S2E)
#define SD_PHY_CTRL_0_OFFSET		0x340
#define SD_PHY_CTRL_1_OFFSET		0x344
#define SD_PHY_OBSV_OFFSET		0x348
#else
#define SD_PHY_CTRL_0_OFFSET		0x4C0
#define SD_PHY_CTRL_1_OFFSET		0x4C4
#define SD_PHY_OBSV_OFFSET		0x4F0
#endif
#define SD_PHY_CTRL_0_REG		RCT_REG(SD_PHY_CTRL_0_OFFSET)
#define SD_PHY_CTRL_1_REG		RCT_REG(SD_PHY_CTRL_1_OFFSET)
#define SD_PHY_OBSV_REG			RCT_REG(SD_PHY_OBSV_OFFSET)

#if (CHIP_REV == S3L)
#define SDXC_PHY_CTRL_0_OFFSET		0x4C8
#define SDXC_PHY_CTRL_1_OFFSET		0x4CC
#define SDXC_PHY_OBSV_OFFSET		0x4F0
#define SDXC_PHY_CTRL_0_REG		RCT_REG(SDXC_PHY_CTRL_0_OFFSET)
#define SDXC_PHY_CTRL_1_REG		RCT_REG(SDXC_PHY_CTRL_1_OFFSET)
#define SDXC_PHY_OBSV_REG		RCT_REG(SDXC_PHY_OBSV_OFFSET)
#endif

#else
#define MS_DELAY_CTRL_OFFSET		0x1D0
#define MS_DELAY_CTRL_REG		RCT_REG(MS_DELAY_CTRL_OFFSET)
#endif


/* ==========================================================================*/
#if (CHIP_REV == A5S) || (CHIP_REV == A7L) || (CHIP_REV == S2) || (CHIP_REV == S2E)
#define ADC_SOFT_RESET			0x0
#else
#define ADC_SOFT_RESET			0x10000
#endif
#define SCALER_ADC_OFFSET		0x09C
#define SCALER_ADC_REG			RCT_REG(SCALER_ADC_OFFSET)

#define ADC16_CTRL_OFFSET		0x198
#define ADC16_CTRL_REG			RCT_REG(ADC16_CTRL_OFFSET)

/* ==========================================================================*/
#define AHB_MISC_EN_OFFSET		0x21C
#define AHB_MISC_EN_REG			RCT_REG(AHB_MISC_EN_OFFSET)

/* ==========================================================================*/
#define IOCTRL_GPIO_OFFSET		0x1F8
#define IOCTRL_GPIO_REG			RCT_REG(IOCTRL_GPIO_OFFSET)

#define IOCTRL_MISC1_OFFSET		0x1FC
#define IOCTRL_MISC1_REG		RCT_REG(IOCTRL_MISC1_OFFSET)

#define IOCTRL_MISC2_OFFSET		0x200
#define IOCTRL_MISC2_REG		RCT_REG(IOCTRL_MISC2_OFFSET)

#define IOCTRL_SMIOA_OFFSET		0x204
#define IOCTRL_SMIOA_REG		RCT_REG(IOCTRL_SMIOA_OFFSET)

#define IOCTRL_SMIOB_OFFSET		0x208
#define IOCTRL_SMIOB_REG		RCT_REG(IOCTRL_SMIOB_OFFSET)

#define IOCTRL_SMIOC_OFFSET		0x20C
#define IOCTRL_SMIOC_REG		RCT_REG(IOCTRL_SMIOC_OFFSET)

#define IOCTRL_SMIOD_OFFSET		0x210
#define IOCTRL_SMIOD_REG		RCT_REG(IOCTRL_SMIOD_OFFSET)

#define IOCTRL_VD1_OFFSET		0x214
#define IOCTRL_VD1_REG			RCT_REG(IOCTRL_VD1_OFFSET)

#define IOCTRL_SENSOR_OFFSET		0x218
#define IOCTRL_SENSOR_REG		RCT_REG(IOCTRL_SENSOR_OFFSET)

#define IOCTRL_STRIG_OFFSET		0x250
#define IOCTRL_STRIG_REG		RCT_REG(IOCTRL_STRIG_OFFSET)

#define IOCTRL_SDXC_OFFSET		0x2F4
#define IOCTRL_SDXC_REG			RCT_REG(IOCTRL_SDXC_OFFSET)

#define SDXC_PULL_CTRL_OFFSET		0x2F8
#define SDXC_PULL_CTRL_REG		RCT_REG(SDXC_PULL_CTRL_OFFSET)

#if (CHIP_REV == S2) || (CHIP_REV == S2E)
#define IOCTRL_DRIVE_STRENGTH_3MA	0x0
#define IOCTRL_DRIVE_STRENGTH_12MA	0x1
#define IOCTRL_DRIVE_STRENGTH_6MA	0x2
#define IOCTRL_DRIVE_STRENGTH_18MA	0x3
#else
#define IOCTRL_DRIVE_STRENGTH_2MA	0x0
#define IOCTRL_DRIVE_STRENGTH_8MA	0x1
#define IOCTRL_DRIVE_STRENGTH_4MA	0x2
#define IOCTRL_DRIVE_STRENGTH_12MA	0x3
#endif

#if (CHIP_REV == A5S)
#define GPIO_PAD_DS_SUPPORT		0
#else
#define GPIO_PAD_DS_SUPPORT		1
#endif

#if (CHIP_REV == S2) || (CHIP_REV == S2E)
#define GPIO_DS0_0_OFFSET		0x270
#define GPIO_DS0_1_OFFSET		0x274
#define GPIO_DS0_2_OFFSET		0x278
#define GPIO_DS0_3_OFFSET		0x27C
#define GPIO_DS0_4_OFFSET		0x280
#define GPIO_DS1_0_OFFSET		0x284
#define GPIO_DS1_1_OFFSET		0x288
#define GPIO_DS1_2_OFFSET		0x28C
#define GPIO_DS1_3_OFFSET		0x290
#define GPIO_DS1_4_OFFSET		0x294
#define GPIO_DS0_OFFSET(bank)		(0x270 + ((bank) * 4))
#define GPIO_DS1_OFFSET(bank)		(0x284 + ((bank) * 4))

#else
#define GPIO_DS0_0_OFFSET		0x314
#define GPIO_DS1_0_OFFSET		0x318
#define GPIO_DS0_1_OFFSET		0x31C
#define GPIO_DS1_1_OFFSET		0x320
#define GPIO_DS0_2_OFFSET		0x324
#define GPIO_DS1_2_OFFSET		0x328
#define GPIO_DS0_3_OFFSET		0x32C
#define GPIO_DS1_3_OFFSET		0x330
#define GPIO_DS0_4_OFFSET		0x438
#define GPIO_DS1_4_OFFSET		0x43C
#define GPIO_DS0_5_OFFSET		0x440
#define GPIO_DS1_5_OFFSET		0x444
#define GPIO_DS0_6_OFFSET		0x448
#define GPIO_DS1_6_OFFSET		0x44C
#define GPIO_DS0_OFFSET(bank)		((bank) >= 4 ? \
					(0x438 + (((bank) - 4) * 8)) : \
					(0x314 + ((bank) * 8)))
#define GPIO_DS1_OFFSET(bank)		((bank) >= 4 ? \
					(0x438 + (((bank) - 4) * 8) + 4) : \
					(0x314 + ((bank) * 8) + 4))
#endif

#define GPIO_DS0_0_REG			RCT_REG(GPIO_DS0_0_OFFSET)
#define GPIO_DS1_0_REG			RCT_REG(GPIO_DS1_0_OFFSET)
#define GPIO_DS0_1_REG			RCT_REG(GPIO_DS0_1_OFFSET)
#define GPIO_DS1_1_REG			RCT_REG(GPIO_DS1_1_OFFSET)
#define GPIO_DS0_2_REG			RCT_REG(GPIO_DS0_2_OFFSET)
#define GPIO_DS1_2_REG			RCT_REG(GPIO_DS1_2_OFFSET)
#define GPIO_DS0_3_REG			RCT_REG(GPIO_DS0_3_OFFSET)
#define GPIO_DS1_3_REG			RCT_REG(GPIO_DS1_3_OFFSET)
#define GPIO_DS0_4_REG			RCT_REG(GPIO_DS0_4_OFFSET)
#define GPIO_DS1_4_REG			RCT_REG(GPIO_DS1_4_OFFSET)
#define GPIO_DS0_5_REG			RCT_REG(GPIO_DS0_5_OFFSET)
#define GPIO_DS1_5_REG			RCT_REG(GPIO_DS1_5_OFFSET)
#define GPIO_DS0_6_REG			RCT_REG(GPIO_DS0_6_OFFSET)
#define GPIO_DS1_6_REG			RCT_REG(GPIO_DS1_6_OFFSET)

/* ==========================================================================*/
#define RCT_TIMER_OFFSET		0x254
#define RCT_TIMER_CTRL_OFFSET		0x258

#define RCT_TIMER_REG			RCT_REG(RCT_TIMER_OFFSET)
#define RCT_TIMER_CTRL_REG		RCT_REG(RCT_TIMER_CTRL_OFFSET)

/* ==========================================================================*/

/* Secure and Scratchpad */
#define AHB_SCRATCHPAD_OFFSET		0x1B000
#define AHB_SECURE_OFFSET		0x1D000
#define AHB_SCRATCHPAD_BASE		(AHB_BASE + AHB_SCRATCHPAD_OFFSET)
#define AHB_SECURE_BASE			(AHB_BASE + AHB_SECURE_OFFSET)
#define AHB_SCRATCHPAD_REG(x)		(AHB_SCRATCHPAD_BASE + (x))
#define AHB_SECURE_REG(x)		(AHB_SECURE_BASE + (x))

#if (CHIP_REV == S3L)
#define AHBSP_PRI_IRQ_C0_OFFSET		0x38
#define AHBSP_PRI_IRQ_C1_OFFSET		-1 /* not supported */
#else
#define AHBSP_PRI_IRQ_C0_OFFSET		0x3C
#define AHBSP_PRI_IRQ_C1_OFFSET		0x40
#endif
#define AHBSP_PRI_IRQ_C0_REG		AHB_SCRATCHPAD_REG(AHBSP_PRI_IRQ_C0_OFFSET)
#define AHBSP_PRI_IRQ_C1_REG		AHB_SCRATCHPAD_REG(AHBSP_PRI_IRQ_C1_OFFSET)

/* ==========================================================================*/

#if (CHIP_REV == S3) || (CHIP_REV == S3L)
#define DMA_SUPPORT_SELECT_CHANNEL	1
#else
#define DMA_SUPPORT_SELECT_CHANNEL	0
#endif

#if (CHIP_REV == S3)
#define AHBSP_DMA_CHANNEL_SEL_OFFSET	0x30
#elif (CHIP_REV == S3L)
#define AHBSP_DMA_CHANNEL_SEL_OFFSET	0x2C
#endif
#define AHBSP_DMA_CHANNEL_SEL_REG	AHB_SCRATCHPAD_REG(AHBSP_DMA_CHANNEL_SEL_OFFSET)

#define SSI0_TX_DMA_REQ_IDX		0
#define SSI0_RX_DMA_REQ_IDX		1
#define SSI1_TX_DMA_REQ_IDX		2
#define SSI1_RX_DMA_REQ_IDX		3
#define NOR_SPI_TX_DMA_REQ_IDX		4
#define NOR_SPI_RX_DMA_REQ_IDX		5
#define SSIS0_TX_DMA_REQ_IDX		6
#define SSIS0_RX_DMA_REQ_IDX		7
#define UART_TX_DMA_REQ_IDX		8
#define UART_RX_DMA_REQ_IDX		9
#define I2S_TX_DMA_REQ_IDX		10
#define I2S_RX_DMA_REQ_IDX		11
#define SLIM_TX_DMA_REQ_IDX		12
#define SLIM_RX_DMA_REQ_IDX		13

/* ==========================================================================*/
#if (CHIP_REV == A5S) || (CHIP_REV == A7L)
#define	POC_BOOT_MAP_TYPE		0
#define POC_BOOT_FROM_MASK		0x00010500
#define POC_BOOT_FROM_USB		0x00000000
#define POC_BOOT_FROM_BYPASS		0x00000100
#define POC_BOOT_FROM_NAND		0x00000400
#define POC_BOOT_FROM_EMMC		0xFFFFFFFF /* not supported */
#define POC_BOOT_FROM_SPINOR		0xFFFFFFFF /* not supported */
#define POC_BOOT_FROM_HIF		0xFFFFFFFF /* not supported */
#elif (CHIP_REV == S2) || (CHIP_REV == S2E)
#define	POC_BOOT_MAP_TYPE		0
#define POC_BOOT_FROM_MASK		0x00003140
#define POC_BOOT_FROM_BYPASS		0x00000040
#define POC_BOOT_FROM_USB		0x00000000
#define POC_BOOT_FROM_NAND		0x00000100
#define POC_BOOT_FROM_SPINOR		0x00001000 /* not supported on S2 */
#define POC_BOOT_FROM_EMMC		0x00002000
#define POC_BOOT_FROM_HIF		0xFFFFFFFF /* not supported */
#elif (CHIP_REV == S3)
#define	POC_BOOT_MAP_TYPE		1
#define POC_BOOT_FROM_MASK		0x00000570
#define POC_BOOT_FROM_BYPASS		0x00000100
#define POC_BOOT_FROM_USB		0x00000400
#define POC_BOOT_FROM_SPINOR		0x00000000
#define POC_BOOT_FROM_NAND		0x00000010
#define POC_BOOT_FROM_EMMC		0x00000020
#define POC_BOOT_FROM_HIF		0x00000040
#elif (CHIP_REV == S2L) || (CHIP_REV == S3L)
#define	POC_BOOT_MAP_TYPE		1
#define POC_BOOT_FROM_MASK		0x00000530
#define POC_BOOT_FROM_BYPASS		0x00000100
#define POC_BOOT_FROM_USB		0x00000400
#define POC_BOOT_FROM_SPINOR		0x00000000
#define POC_BOOT_FROM_NAND		0x00000010
#define POC_BOOT_FROM_EMMC		0x00000020
#define POC_BOOT_FROM_HIF		0xFFFFFFFF /* not supported */
#endif

#define RCT_BOOT_FROM_BYPASS		0x80000000
#define RCT_BOOT_FROM_NAND		0x00000001
#define RCT_BOOT_FROM_USB		0x00000002
#define RCT_BOOT_FROM_EMMC		0x00000004
#define RCT_BOOT_FROM_SPINOR		0x00000008
#define RCT_BOOT_FROM_HIF		0x00000010

/* ==========================================================================*/
#if (CHIP_REV == A5S)
#define SYS_CONFIG_NAND_PAGE_SIZE	0x00000020
#define SYS_CONFIG_NAND_READ_CONFIRM	0x00000040
#define SYS_CONFIG_NAND_ECC_BCH_EN	0x00000000
#define SYS_CONFIG_NAND_ECC_SPARE_2X	0x00000000
#elif (CHIP_REV == S2) || (CHIP_REV == S2E)
#define SYS_CONFIG_NAND_PAGE_SIZE	0x00000010
#define SYS_CONFIG_NAND_READ_CONFIRM	0x00000020
#define SYS_CONFIG_NAND_ECC_BCH_EN	0x00000400
#define SYS_CONFIG_NAND_ECC_SPARE_2X	0x00000800
#else
#define SYS_CONFIG_NAND_PAGE_SIZE	0x00040000
#define SYS_CONFIG_NAND_READ_CONFIRM	0x00020000
#define SYS_CONFIG_NAND_ECC_BCH_EN	0x00010000
#define SYS_CONFIG_NAND_ECC_SPARE_2X	0x00008000
#endif

/* these definition are used by software */
#define RCT_BOOT_NAND_AUTO		0x00000000
#define RCT_BOOT_NAND_PAGE_SIZE		0x00000001
#define RCT_BOOT_NAND_READ_CONFIRM	0x00000002
#define RCT_BOOT_NAND_ECC_BCH_EN	0x00000004
#define RCT_BOOT_NAND_ECC_SPARE_2X	0x00000008

/* ==========================================================================*/
#if (CHIP_REV == S2)
#define SYS_CONFIG_MMC_HS		0x00010000
#define SYS_CONFIG_MMC_DDR		0x00020000
#define SYS_CONFIG_MMC_4BIT		0x00040000
#define SYS_CONFIG_MMC_8BIT		0x00080000
#elif (CHIP_REV == S2E)
#define SYS_CONFIG_MMC_HS		0x00010000
#define SYS_CONFIG_MMC_DDR		0x00000000 /* not supported */
#define SYS_CONFIG_MMC_4BIT		0x00040000
#define SYS_CONFIG_MMC_8BIT		0x00080000
#elif (CHIP_REV == S2L) || (CHIP_REV == S3)
#define SYS_CONFIG_MMC_HS		0x00008000
#define SYS_CONFIG_MMC_DDR		0x00004000
#define SYS_CONFIG_MMC_4BIT		0x00020000
#define SYS_CONFIG_MMC_8BIT		0x00010000
#elif (CHIP_REV == S3L)
#define SYS_CONFIG_MMC_HS		0x00000000
#define SYS_CONFIG_MMC_DDR		0x00000000
#define SYS_CONFIG_MMC_4BIT		0x00010000
#define SYS_CONFIG_MMC_8BIT		0x00008000
#else
#define SYS_CONFIG_MMC_HS		0x00000000
#define SYS_CONFIG_MMC_DDR		0x00000000
#define SYS_CONFIG_MMC_4BIT		0x00000000
#define SYS_CONFIG_MMC_8BIT		0x00000000
#endif

/* these definition are used by software */
#define RCT_BOOT_EMMC_AUTO		0x00000000
#define RCT_BOOT_EMMC_HS		0x00000001
#define RCT_BOOT_EMMC_DDR		0x00000002
#define RCT_BOOT_EMMC_4BIT		0x00000004
#define RCT_BOOT_EMMC_8BIT		0x00000008

/* ==========================================================================*/
#ifndef __ASM__
/* ==========================================================================*/

extern void rct_pll_init(void);
extern u32 get_apb_bus_freq_hz(void);
extern u32 get_ahb_bus_freq_hz(void);
extern u32 get_core_bus_freq_hz(void);
extern u32 get_ddr_freq_hz(void);
extern u32 get_idsp_freq_hz(void);
extern u32 get_cortex_freq_hz(void);
extern u32 get_arm_bus_freq_hz(void);

extern u32 rct_boot_from(void);
extern u32 rct_is_eth_enabled(void);
extern u32 rct_get_nand_poc(void);
extern u32 rct_get_emmc_poc(void);

extern void rct_timer_enable();
extern void rct_timer_disable();
extern void rct_timer_reset_count();
extern u32 rct_timer_get_count();
extern u32 rct_timer_get_tick();
extern u32 rct_timer_tick2ms(u32 s_tck, u32 e_tck);
extern void rct_timer_dly_ms(u32 dly_tim);
extern u32 rct_timer_get_frequency_div_1000();
extern void rct_timer_delay_ticks(u32 ticks);

extern u32 get_gtx_freq_hz(void);

extern void amboot_rct_reset_chip_pre(void)
	__attribute__ ((weak));
extern void amboot_rct_reset_chip_sdmmc(void)
	__attribute__ ((weak));
extern void rct_reset_chip(void);
extern void rct_reset_fio(void);

extern void rct_set_uart_pll(void);
extern u32 get_uart_freq_hz(void);


extern void rct_set_ssi_pll(void);
extern u32 get_ssi_freq_hz(void);

extern void rct_set_ssi2_pll(void);
extern u32 get_ssi2_freq_hz(void);

extern void rct_set_ssi3_pll(void);
extern u32 get_ssi3_freq_hz(void);

extern void rct_set_sd_pll(u32 freq_hz);
extern u32 get_sd_freq_hz(void);

extern void rct_set_sdio_pll(u32 freq_hz);
extern u32 get_sdio_freq_hz(void);

extern void rct_set_sdxc_pll(u32 freq_hz);
extern u32 get_sdxc_freq_hz(void);

extern void rct_set_so_pll(void);
extern void rct_set_vo_pll(void);
extern void rct_set_vo2_pll(void);

extern void rct_set_audio_pll(void);

extern void rct_enable_usb(void);
extern void rct_suspend_usb(void);
extern void rct_usb_reset(void);

extern void rct_show_pll(void);
extern void rct_show_boot_from(u32 boot_from);

extern void dma_channel_select(void);

/* ==========================================================================*/
#endif
/* ==========================================================================*/

#endif

