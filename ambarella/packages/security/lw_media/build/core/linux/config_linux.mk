##
## $(MODULE_NAME_TAG)/build/core/linux/config_linux.mk
##
## History:
##    2013/03/16 - [Zhi He] Create
##
## Copyright (C) 2014 - 2024, the Ambarella, Inc.
##
## All rights reserved. No Part of this file may be reproduced, stored
## in a retrieval system, or transmitted, in any form, or by any means,
## electronic, mechanical, photocopying, recording, or otherwise,
## without the prior consent of the Ambarella, Inc.
##

MAKE_PRINT		=	@

##Cross compile
ENABLE_CROSS_COMPILE := y
CROSS_COMPILE_TARGET_ARCH := ARM
CROSS_COMPILE_HOST_ARCH := X86
CONFIG_LINARO_TOOLCHAIN := y
TOOLCHAIN_PATH ?= /usr/local/linaro-multilib-2014.06-gcc4.9/bin
CROSS_COMPILE_TOOL_CHAIN_PATH := $(TOOLCHAIN_PATH)
CROSS_COMPILE	:=  arm-linux-gnueabihf-

##CPU arch and option
BUILD_CONFIG_CPU_ARCH_X86 := n
BUILD_CONFIG_CPU_ARCH_X64 := n
BUILD_CONFIG_CPU_OPT_MMX := n
BUILD_CONFIG_CPU_OPT_SSE := n
BUILD_CONFIG_CPU_ARCH_X64 := n
BUILD_CONFIG_CPU_ARCH_ARMV5 := n
BUILD_CONFIG_CPU_ARCH_ARMV6 := n
BUILD_CONFIG_CPU_ARCH_ARMV7 := y
BUILD_CONFIG_CPU_OPT_NEON := y

##OS typs
BUILD_CONGIG_OS_LINUX := y
BUILD_CONGIG_OS_ANDROID := n
BUILD_CONGIG_OS_WINDOWS := n
BUILD_CONGIG_OS_IOS := n

##HW related
BUILD_CONGIG_DSP_AMBA_A5S := n
BUILD_CONGIG_DSP_AMBA_I1 := n
BUILD_CONGIG_DSP_AMBA_S2 := n
BUILD_CONGIG_DSP_AMBA_S2L := n
BUILD_CONGIG_DSP_AMBA_S2E := y

##SW module related
BUILD_CONFIG_MODULE_FFMPEG := y
BUILD_CONFIG_MODULE_LIBXML2 := n
BUILD_CONFIG_MODULE_ALSA := n
BUILD_CONFIG_MODULE_LIBAAC := n
BUILD_CONFIG_MODULE_AMBA_DSP := n
BUILD_CONFIG_MODULE_LINUX_FB := y

##internal module
BUILD_CONFIG_MODULE_DEMUXER_RTSP := y
BUILD_CONFIG_MODULE_DEMUXER_MP4 := y
BUILD_CONFIG_MODULE_DEMUXER_TS := y
BUILD_CONFIG_MODULE_MUXER_MP4 := y
BUILD_CONFIG_MODULE_MUXER_TS := y

LWMD_TOPDIR		=	$(BUILDSYSTEM_DIR)/$(MODULE_NAME_TAG)
LWMD_OBJDIR		=	$(LWMD_TOPDIR)/out/linux/objs
LWMD_INTERNAL_BINARY	=	$(LWMD_TOPDIR)/out/linux/binary
LWMD_INTERNAL_LIBDIR	=	$(LWMD_TOPDIR)/out/linux/lib
LWMD_INTERNAL_TARDIR	=	$(LWMD_TOPDIR)/out/linux/tar
LWMD_INTERNAL_INCDIR	=	$(LWMD_TOPDIR)/out/linux/include

LWMD_CFLAGS		= -I$(LWMD_TOPDIR)/include -I$(LWMD_TOPDIR)/source/internal_include -I$(DEVICE_DIR)/include 

LWMD_LDFLAGS	=

ifeq ($(BUILD_CONFIG_CPU_ARCH_ARMV6), y)
LWMD_CFLAGS += -march=armv6k -mtune=arm1136j-s -msoft-float -mlittle-endian
else
ifeq ($(BUILD_CONFIG_CPU_ARCH_ARMV7), y)
LWMD_CFLAGS += -march=armv7-a -mlittle-endian
ifeq ($(BUILD_CONFIG_CPU_OPT_NEON), y)

ifeq ($(CONFIG_LINARO_TOOLCHAIN), y)
LWMD_CFLAGS += -mfloat-abi=hard -mfpu=neon -fno-rtti -fno-exceptions -fstrict-aliasing
THIRD_PARTY_DIR := $(BUILDSYSTEM_DIR)/prebuild/third-party/armv7-a-hf
else
LWMD_CFLAGS += -mfloat-abi=softfp -mfpu=neon
THIRD_PARTY_DIR := $(BUILDSYSTEM_DIR)/prebuild/third-party/armv7-a
endif

else
LWMD_CFLAGS += -msoft-float
endif
else
LWMD_CFLAGS += -march=armv5te -mtune=arm9tdmi
endif
endif

LWMD_CFLAGS += $(call cc-option,-mno-unaligned-access,)

ifeq ($(BUILD_AMBARELLA_LWMD_DEBUG), y)
LWMD_CFLAGS += -g
else
LWMD_CFLAGS += -O3
endif
LWMD_CFLAGS += -Wall -Werror -fPIC -D_REENTRENT

ifeq ($(BUILD_CONGIG_OS_LINUX), y)
LWMD_CFLAGS += -DBUILD_OS_LINUX=1
else
ifeq ($(BUILD_CONGIG_OS_WINDOWS), y)
LWMD_CFLAGS += -DBUILD_OS_WINDOWS=1
else
ifeq ($(BUILD_CONGIG_OS_ANDROID), y)
LWMD_CFLAGS += -DBUILD_OS_ANDROID=1
else
ifeq ($(BUILD_CONGIG_OS_IOS), y)
LWMD_CFLAGS += -DBUILD_OS_IOS=1
endif
endif
endif
endif

ifeq ($(BUILD_CONFIG_MODULE_AMBA_DSP), y)
LWMD_CFLAGS += -DBUILD_MODULE_AMBA_DSP=1
endif

ifeq ($(BUILD_CONGIG_DSP_AMBA_A5S), y)
LWMD_CFLAGS += -DBUILD_DSP_AMBA_A5S=1
endif

ifeq ($(BUILD_CONGIG_DSP_AMBA_I1), y)
LWMD_CFLAGS += -DBUILD_DSP_AMBA_I1=1
endif

ifeq ($(BUILD_CONGIG_DSP_AMBA_S2), y)
LWMD_CFLAGS += -DBUILD_DSP_AMBA_S2=1
endif

ifeq ($(BUILD_CONGIG_DSP_AMBA_S2L), y)
LWMD_CFLAGS += -DBUILD_DSP_AMBA_S2L=1
endif

ifeq ($(BUILD_CONGIG_DSP_AMBA_S2E), y)
LWMD_CFLAGS += -DBUILD_DSP_AMBA_S2E=1
endif

ifeq ($(BUILD_CONFIG_MODULE_LINUX_FB), y)
LWMD_CFLAGS += -DBUILD_MODULE_LINUX_FB=1
endif

ifeq ($(BUILD_CONFIG_MODULE_FFMPEG), y)
LWMD_CFLAGS += -DBUILD_MODULE_FFMPEG=1
endif

ifeq ($(BUILD_CONFIG_MODULE_LIBXML2), y)
LWMD_CFLAGS += -DBUILD_MODULE_LIBXML2=1
endif

ifeq ($(BUILD_CONFIG_MODULE_ALSA), y)
LWMD_CFLAGS += -DBUILD_MODULE_ALSA=1
endif

ifeq ($(BUILD_CONFIG_MODULE_LIBAAC), y)
LWMD_CFLAGS += -DBUILD_MODULE_LIBAAC=1
endif

ifeq ($(BUILD_CONFIG_MODULE_DEMUXER_RTSP), y)
LWMD_CFLAGS += -DBUILD_MODULE_DEMUXER_RTSP=1
endif

ifeq ($(BUILD_CONFIG_MODULE_DEMUXER_MP4), y)
LWMD_CFLAGS += -DBUILD_MODULE_DEMUXER_MP4=1
endif

ifeq ($(BUILD_CONFIG_MODULE_DEMUXER_TS), y)
LWMD_CFLAGS += -DBUILD_MODULE_DEMUXER_TS=1
endif

ifeq ($(BUILD_CONFIG_MODULE_MUXER_MP4), y)
LWMD_CFLAGS += -DBUILD_MODULE_MUXER_MP4=1
endif

ifeq ($(BUILD_CONFIG_MODULE_MUXER_TS), y)
LWMD_CFLAGS += -DBUILD_MODULE_MUXER_TS=1
endif

ifeq ($(BUILD_WITH_UNDER_DEVELOP_COMPONENT), y)
LWMD_CFLAGS += -DBUILD_WITH_UNDER_DEVELOP_COMPONENT=1
endif

######################################################################
#		tool chain config
######################################################################
ifeq ($(ENABLE_CROSS_COMPILE), y)
ifeq ($(CROSS_COMPILE_TARGET_ARCH), ARM)
CC    = $(CROSS_COMPILE_TOOL_CHAIN_PATH)/$(CROSS_COMPILE)gcc
CXX    = $(CROSS_COMPILE_TOOL_CHAIN_PATH)/$(CROSS_COMPILE)g++
GCC    = $(CROSS_COMPILE_TOOL_CHAIN_PATH)/$(CROSS_COMPILE)gcc
LD     = $(CROSS_COMPILE_TOOL_CHAIN_PATH)/$(CROSS_COMPILE)ld
AS     = $(CROSS_COMPILE_TOOL_CHAIN_PATH)/$(CROSS_COMPILE)as
AR     = $(CROSS_COMPILE_TOOL_CHAIN_PATH)/$(CROSS_COMPILE)ar
STRIP  = $(CROSS_COMPILE_TOOL_CHAIN_PATH)/$(CROSS_COMPILE)strip
RANLIB = $(CROSS_COMPILE_TOOL_CHAIN_PATH)/$(CROSS_COMPILE)ranlib
endif
else
CC    = gcc
CXX    = g++
GCC    = gcc
LD     = ld
AS     = as
AR     = ar
STRIP  = strip
RANLIB = ranlib
endif

export BUILDSYSTEM_DIR
export LWMD_TOPDIR
export LWMD_OBJDIR
export LWMD_CFLAGS
export LWMD_LDFLAGS
export LWMD_INTERNAL_BINARY
export LWMD_INTERNAL_LIBDIR
export LWMD_INTERNAL_INCDIR
export THIRD_PARTY_DIR

