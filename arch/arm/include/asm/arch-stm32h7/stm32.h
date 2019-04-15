/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2017, STMicroelectronics - All Rights Reserved
 * Author(s): Patrice Chotard, <patrice.chotard@st.com> for STMicroelectronics.
 */

#ifndef _ASM_ARCH_HARDWARE_H
#define _ASM_ARCH_HARDWARE_H

#define STM32H7_FLASH_CNTL_BASE	0x52002000

static const u32 sect_sz_kb[CONFIG_SYS_MAX_FLASH_SECT] = {
	[0 ... CONFIG_SYS_MAX_FLASH_SECT-1] = CONFIG_SYS_FLASH_SECT_SIZE
};

#endif /* _ASM_ARCH_HARDWARE_H */
