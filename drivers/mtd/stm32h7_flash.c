// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2015
 * Kamil Lulko, <kamil.lulko@gmail.com>
 * (C) Copyright Emcraft Systems (www.emcraft.com)
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/stm32.h>
#include "stm32h7_flash.h"

flash_info_t flash_info[CONFIG_SYS_MAX_FLASH_BANKS];

#define STM32H7_FLASH	((struct stm32h7_flash_regs *)STM32H7_FLASH_CNTL_BASE)

static void stm32_flash_lock(u8 lock)
{
	if (lock) {
		setbits_le32(&STM32H7_FLASH->cr, STM32H7_FLASH_CR_LOCK);
	} else {
		writel(STM32H7_FLASH_KEY1, &STM32H7_FLASH->key);
		writel(STM32H7_FLASH_KEY2, &STM32H7_FLASH->key);
	}
}

unsigned long flash_init(void)
{
	unsigned long total_size = 0;
	u8 i, j;

	for (i = 0; i < CONFIG_SYS_MAX_FLASH_BANKS; i++) {
		flash_info[i].flash_id = FLASH_STM32;
		flash_info[i].sector_count = CONFIG_SYS_MAX_FLASH_SECT;
		flash_info[i].start[0] = CONFIG_SYS_FLASH_BASE + (i << 20);
		flash_info[i].size = sect_sz_kb[0];
		for (j = 1; j < CONFIG_SYS_MAX_FLASH_SECT; j++) {
			flash_info[i].start[j] = flash_info[i].start[j - 1]
				+ (sect_sz_kb[j - 1]);
			flash_info[i].size += sect_sz_kb[j];
		}
		total_size += flash_info[i].size;
	}

	clrbits_le32(&STM32H7_FLASH->scar_prg, STM32H7_FLASH_SCAR_DMES1);
	setbits_le32(&STM32H7_FLASH->ccr, (0xFFF << 16));

	return total_size;
}

void flash_print_info(flash_info_t *info)
{
	int i;

	if (info->flash_id == FLASH_UNKNOWN) {
		printf("missing or unknown FLASH type\n");
		return;
	} else if (info->flash_id == FLASH_STM32) {
		printf("stm32 Embedded Flash\n");
	}

	printf("  Size: %ld MB in %d Sectors\n",
	       info->size >> 20, info->sector_count);

	printf("  Sector Start Addresses:");
	for (i = 0; i < info->sector_count; ++i) {
		if ((i % 5) == 0)
			printf("\n   ");
		printf(" %08lX%s",
		       info->start[i],
			info->protect[i] ? " (RO)" : "     ");
	}
	printf("\n");
	return;
}

int flash_erase(flash_info_t *info, int first, int last)
{
	u8 bank = 0xFF;
	int i;

	for (i = 0; i < CONFIG_SYS_MAX_FLASH_BANKS; i++) {
		if (info == &flash_info[i]) {
			bank = i;
			break;
		}
	}
	if (bank == 0xFF)
		return -1;

	stm32_flash_lock(0);

	for (i = first; i <= last; i++) {
		clrsetbits_le32(&STM32H7_FLASH->cr,
				STM32H7_FLASH_CR_SNB_MASK,
				((i << 8) | STM32H7_FLASH_CR_SER));
		setbits_le32(&STM32H7_FLASH->cr, (1 << 7));
		while (readl(&STM32H7_FLASH->sr) & STM32H7_FLASH_SR_BSY);
	}

	stm32_flash_lock(1);
	return 0;
}

int write_buff(flash_info_t *info, uchar *src, ulong addr, ulong cnt)
{
	ulong i;

	while (readl(&STM32H7_FLASH->sr) & STM32H7_FLASH_SR_BSY)
		;

	stm32_flash_lock(0);

	setbits_le32(&STM32H7_FLASH->cr, STM32H7_FLASH_CR_PG);

	/* To make things simple use byte writes only */
	for (i = 0; i < cnt; i++) {
		*(uchar *)(addr + i) = src[i];
		/*  avoid re-ordering flash data write and busy status
		 *  check as flash memory space attributes are generally Normal
		 */
		mb();
		while (readl(&STM32H7_FLASH->sr) & STM32H7_FLASH_SR_QW1);
	}
	clrbits_le32(&STM32H7_FLASH->cr, STM32H7_FLASH_CR_PG);
	stm32_flash_lock(1);

	return 0;
}
