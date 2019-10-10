/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Emcraft Systems, www.emcraft.com, (C) Copyright 2019
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <config.h>

#define CONFIG_SYS_INIT_SP_ADDR		0x20020000

/*
 * Configuration of the external SDRAM memory
 */
#define CONFIG_SYS_UBOOT_START		0x240013FC
#define CONFIG_SYS_LOAD_ADDR		0xD0400000
#define CONFIG_LOADADDR			0xD0400000
#define CONFIG_PRAM			0

#define CONFIG_SYS_FLASH_BASE		0x08000000
#define CONFIG_STM32H7_FLASH
#define CONFIG_SYS_MAX_FLASH_SECT	1
#define CONFIG_SYS_MAX_FLASH_BANKS	1
#define CONFIG_SYS_FLASH_SECT_SIZE	0x20000		/* 128 KB */

/* For splashcreen */
#ifdef CONFIG_DM_VIDEO
#define CONFIG_VIDEO_BMP_RLE8
#define CONFIG_BMP_16BPP
#define CONFIG_BMP_24BPP
#define CONFIG_BMP_32BPP
#undef CONFIG_VIDEO_STM32_MAX_BPP
#define CONFIG_VIDEO_STM32_MAX_BPP	32
#define CONFIG_FB_ADDR			0xD0000000
#define CONFIG_SPLASH_SCREEN
#define CONFIG_SPLASH_SCREEN_ALIGN
#endif

#define CONFIG_ENV_SIZE			(0x4000)

/* environment organization */
#define CONFIG_ENV_UBI_PART		"system"
#define CONFIG_ENV_UBI_VOLUME		"env1"
#define CONFIG_ENV_UBI_VOLUME_REDUND	"env2"

/*
 * SPL configuration
 */
#define CONFIG_SPL_STACK		(0x20010000)
#define CONFIG_SPL_TEXT_BASE		CONFIG_SYS_FLASH_BASE

#define CONFIG_SYS_SPI_U_BOOT_OFFS	0x00000000

#define CONFIG_SYS_HZ_CLOCK		1000000

#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG
#define CONFIG_REVISION_TAG

#define CONFIG_SYS_MAXARGS		16
#define CONFIG_SYS_MALLOC_LEN		(1 * 1024 * 1024)

#define BOOT_TARGET_DEVICES(func) \
	func(MMC, mmc, 0)

#include <config_distro_bootcmd.h>
#define CONFIG_EXTRA_ENV_SETTINGS				\
			"kernel_addr_r=0xD0008000\0"		\
			"fdtfile=stm32h7_som.dtb\0"		\
			"fdt_addr_r=0xD0700000\0"		\
			"scriptaddr=0xD0800000\0"		\
			"pxefile_addr_r=0xD0800000\0" \
			"fdt_high=0xffffffffffffffff\0"		\
			"initrd_high=0xffffffffffffffff\0"	\
			"ramdisk_addr_r=0xD0900000\0"		\
			"bootcmd=run reliable_boot;"			\
			" if test x$upgrade_available = x1; then reset; fi" \
			"\0"						\
									\
			"altbootcmd=run reliable_altboot\0"		\
									\
			"ubiboot=ubi read $loadaddr rtos$boot_num && bootm\0" \
									\
			"reliable_boot="				\
			" setenv boot_num 0;"				\
			" if test x$boot2_active != x1 -a x$boot1_valid = x1; then" \
			"   setenv boot_num 1;"				\
			" fi;"						\
			" if test x$boot2_active  = x1 -a x$boot2_valid = x1; then" \
			"   setenv boot_num 2;"				\
			" fi;"						\
			" if test x$boot_num != x0; then"		\
			"   echo \"Booting Image #${boot_num}\";"	\
			"   run ubiboot ||"				\
			"       echo ERROR: ubiboot($boot_num) failed;" \
			" else"						\
			"   echo ERROR: Active image set is invalid;"	\
			" fi;"						\
			"\0"						\
									\
			"reliable_altboot="				\
			" if test x$boot2_active != x1; then"		\
			"   setenv boot2_active 1;"			\
			"   setenv boot1_valid 0;"			\
			" else"						\
			"   setenv boot2_active 0;"			\
			"   setenv boot2_valid 0;"			\
			" fi;"						\
			" setenv upgrade_available 0;"			\
			" setenv bootcount 0;"				\
			" saveenv;"					\
			" reset;"					\
			"\0"						\
									\
			CONFIG_MTDPARTS_DEFAULT "\0"			\
			BOOTENV
/*
 * Command line configuration.
 */
#define CONFIG_BOARD_LATE_INIT

#define CONFIG_DMAMEM_START            0xD0000000
#define CONFIG_DMAMEM_SIZE_ORDER       22

#define CONFIG_SYS_MEMTEST_START	0xD0000000
#define CONFIG_SYS_MEMTEST_END		0xD1f00000

#endif /* __CONFIG_H */
