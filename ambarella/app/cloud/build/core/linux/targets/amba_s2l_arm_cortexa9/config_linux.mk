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
BUILD_CONGIG_DSP_AMBA_S2L := y

##SW module related
BUILD_CONFIG_MODULE_FFMPEG := n
BUILD_CONFIG_MODULE_LIBXML2 := n
BUILD_CONFIG_MODULE_ALSA := n
BUILD_CONFIG_MODULE_LIBAAC := n
BUILD_CONFIG_MODULE_AMBA_DSP := n
BUILD_CONFIG_MODULE_WINDESKDUP := n

BUILDSYSTEM_DIR	?=	$(word 1, $(subst /$(MODULE_NAME_TAG), ,$(PWD)))
MWCG_TOPDIR		=	$(BUILDSYSTEM_DIR)/$(MODULE_NAME_TAG)
MWCG_BINDIR		=	$(BUILDSYSTEM_DIR)/rootfs/fakeroot/usr/local/bin
MWCG_LIBDIR		=	$(BUILDSYSTEM_DIR)/rootfs/fakeroot/usr/lib
MWCG_CONFDIR	=	$(BUILDSYSTEM_DIR)/rootfs/fakeroot/etc/mwcg
MWCG_OBJDIR		=	$(MWCG_TOPDIR)/out/linux/objs
MWCG_INTERNAL_BINARY	=	$(MWCG_TOPDIR)/out/linux/binary
MWCG_INTERNAL_LIBDIR	=	$(MWCG_TOPDIR)/out/linux/lib
MWCG_INTERNAL_TARDIR	=	$(MWCG_TOPDIR)/out/linux/tar
MWCG_INTERNAL_INCDIR	=	$(MWCG_TOPDIR)/out/linux/include

#include $(BUILDSYSTEM_DIR)/build/core/functions.mk

MWCG_CFLAGS		= -I$(MWCG_TOPDIR)/common/include -I$(MWCG_TOPDIR)/security_utils/include -I$(MWCG_TOPDIR)/cloud_lib/include -I$(MWCG_TOPDIR)/storage_lib/include -I$(MWCG_TOPDIR)/media_mw/include -I$(MWCG_TOPDIR)/im_mw/include -I$(MWCG_TOPDIR)/lightweight_database/include -I$(MWCG_TOPDIR)/directsharing/include -I$(MWCG_TOPDIR)/extern_security_lib_wrapper/include

MWCG_LDFLAGS	=

ifeq ($(BUILD_CONFIG_CPU_ARCH_ARMV6), y)
MWCG_CFLAGS += -march=armv6k -mtune=arm1136j-s -msoft-float -mlittle-endian
else
ifeq ($(BUILD_CONFIG_CPU_ARCH_ARMV7), y)
MWCG_CFLAGS += -march=armv7-a -mlittle-endian
ifeq ($(BUILD_CONFIG_CPU_OPT_NEON), y)

ifeq ($(CONFIG_LINARO_TOOLCHAIN), y)
MWCG_CFLAGS += -mfloat-abi=hard -mfpu=neon -fno-rtti -fno-exceptions -fstrict-aliasing
MWCG_CFLAGS += -DNEW_AAC_CODEC_VERSION=1
THIRD_PARTY_DIR := $(BUILDSYSTEM_DIR)/prebuild/third-party/armv7-a-hf
MWCG_FFMPEG_LIBDIR := lib-hf
else
MWCG_CFLAGS += -mfloat-abi=softfp -mfpu=neon
THIRD_PARTY_DIR := $(BUILDSYSTEM_DIR)/prebuild/third-party/armv7-a
MWCG_FFMPEG_LIBDIR := lib
endif

else
MWCG_CFLAGS += -msoft-float
endif
else
MWCG_CFLAGS += -march=armv5te -mtune=arm9tdmi
endif
endif

MWCG_CFLAGS += $(call cc-option,-mno-unaligned-access,)

ifeq ($(BUILD_AMBARELLA_MWCG_DEBUG), y)
MWCG_CFLAGS += -g -O2
else
MWCG_CFLAGS += -O3
endif
MWCG_CFLAGS += -Wall -Werror -fPIC -D_REENTRENT

ifeq ($(BUILD_CONGIG_OS_LINUX), y)
MWCG_CFLAGS += -DBUILD_OS_LINUX=1
else
ifeq ($(BUILD_CONGIG_OS_WINDOWS), y)
MWCG_CFLAGS += -DBUILD_OS_WINDOWS=1
else
ifeq ($(BUILD_CONGIG_OS_ANDROID), y)
MWCG_CFLAGS += -DBUILD_OS_ANDROID=1
else
ifeq ($(BUILD_CONGIG_OS_IOS), y)
MWCG_CFLAGS += -DBUILD_OS_IOS=1
endif
endif
endif
endif

ifeq ($(BUILD_CONFIG_MODULE_AMBA_DSP), y)
MWCG_CFLAGS += -DBUILD_MODULE_AMBA_DSP=1

ifeq ($(BUILD_CONGIG_DSP_AMBA_I1), y)
MWCG_CFLAGS += -DBUILD_DSP_AMBA_I1=1
endif

ifeq ($(BUILD_CONGIG_DSP_AMBA_S2), y)
MWCG_CFLAGS += -DBUILD_DSP_AMBA_S2=1
endif

else

BUILD_CONGIG_DSP_AMBA_A5S := n
BUILD_CONGIG_DSP_AMBA_I1 := n
BUILD_CONGIG_DSP_AMBA_S2 := n

endif

ifeq ($(BUILD_CONFIG_MODULE_FFMPEG), y)
MWCG_CFLAGS += -DBUILD_MODULE_FFMPEG=1
endif

ifeq ($(BUILD_CONFIG_MODULE_LIBXML2), y)
MWCG_CFLAGS += -DBUILD_MODULE_LIBXML2=1
endif

ifeq ($(BUILD_CONFIG_MODULE_ALSA), y)
MWCG_CFLAGS += -DBUILD_MODULE_ALSA=1
endif

ifeq ($(BUILD_CONFIG_MODULE_LIBAAC), y)
MWCG_CFLAGS += -DBUILD_MODULE_LIBAAC=1
endif

ifeq ($(BUILD_WITH_UNDER_DEVELOP_COMPONENT), y)
MWCG_CFLAGS += -DBUILD_WITH_UNDER_DEVELOP_COMPONENT=1
endif

######################################################################
#		tool chain config
######################################################################
ifeq ($(ENABLE_CROSS_COMPILE), y)
ifeq ($(CROSS_COMPILE_TARGET_ARCH), ARM)
CC    = $(CROSS_COMPILE_TOOL_CHAIN_PATH)/$(CROSS_COMPILE)gcc
CXX    = $(CROSS_COMPILE_TOOL_CHAIN_PATH)/$(CROSS_COMPILE)g++
LD     = $(CROSS_COMPILE_TOOL_CHAIN_PATH)/$(CROSS_COMPILE)ld
AS     = $(CROSS_COMPILE_TOOL_CHAIN_PATH)/$(CROSS_COMPILE)as
AR     = $(CROSS_COMPILE_TOOL_CHAIN_PATH)/$(CROSS_COMPILE)ar
STRIP  = $(CROSS_COMPILE_TOOL_CHAIN_PATH)/$(CROSS_COMPILE)strip
RANLIB = $(CROSS_COMPILE_TOOL_CHAIN_PATH)/$(CROSS_COMPILE)ranlib
endif
else
CC    = gcc
CXX    = g++
LD     = ld
AS     = as
AR     = ar
STRIP  = strip
RANLIB = ranlib
endif

export BUILDSYSTEM_DIR
export MWCG_TOPDIR
export MWCG_BINDIR
export MWCG_LIBDIR
export MWCG_CONFDIR
export MWCG_OBJDIR
export MWCG_INTERNAL_BINARY
export MWCG_INTERNAL_LIBDIR
export MWCG_INTERNAL_INCDIR
export MWCG_CFLAGS
export MWCG_LDFLAGS
export THIRD_PARTY_DIR
export MWCG_FFMPEG_LIBDIR

