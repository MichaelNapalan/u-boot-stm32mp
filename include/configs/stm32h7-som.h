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

#define CONFIG_SYS_FLASH_BASE		0x08000000
#define CONFIG_STM32H7_FLASH
#define CONFIG_SYS_MAX_FLASH_SECT	1
#define CONFIG_SYS_MAX_FLASH_BANKS	1
#define CONFIG_SYS_FLASH_SECT_SIZE	0x20000		/* 128 KB */

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
			CONFIG_MTDPARTS_DEFAULT "\0" 		\
			BOOTENV
/*
 * Command line configuration.
 */
#define CONFIG_BOARD_LATE_INIT

#endif /* __CONFIG_H */
