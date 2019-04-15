/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2017, STMicroelectronics - All Rights Reserved
 * Author(s): Patrice Chotard, <patrice.chotard@st.com> for STMicroelectronics.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <config.h>

#define CONFIG_SYS_INIT_SP_ADDR		0x24040000

/*
 * Configuration of the external SDRAM memory
 */
#define CONFIG_SYS_LOAD_ADDR		0xD0400000
#define CONFIG_LOADADDR			0xD0400000

#define CONFIG_SYS_FLASH_BASE		0x08000000
#define CONFIG_STM32H7_FLASH
#define CONFIG_SYS_MAX_FLASH_SECT	8
#define CONFIG_SYS_MAX_FLASH_BANKS	1
#define CONFIG_SYS_FLASH_SECT_SIZE	0x20000		/* 128 KB */
#define CONFIG_ENV_SECT_SIZE		(CONFIG_SYS_FLASH_SECT_SIZE)
#define CONFIG_ENV_OFFSET		(3 * CONFIG_ENV_SECT_SIZE)
#define CONFIG_ENV_ADDR			(CONFIG_SYS_FLASH_BASE + CONFIG_ENV_OFFSET)
#define CONFIG_ENV_SIZE			(CONFIG_ENV_SECT_SIZE)

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
			"fdtfile=stm32h743i-eval.dtb\0"	\
			"fdt_addr_r=0xD0700000\0"		\
			"scriptaddr=0xD0800000\0"		\
			"pxefile_addr_r=0xD0800000\0" \
			"fdt_high=0xffffffffffffffff\0"		\
			"initrd_high=0xffffffffffffffff\0"	\
			"ramdisk_addr_r=0xD0900000\0"		\
			BOOTENV
/*
 * Command line configuration.
 */
#define CONFIG_BOARD_LATE_INIT

#endif /* __CONFIG_H */
