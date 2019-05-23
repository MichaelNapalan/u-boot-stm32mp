// SPDX-License-Identifier: GPL-2.0+
/*
 * Emcraft Systems, www.emcraft.com, (C) Copyright 2019
 */

#include <common.h>
#include <dm.h>
#include <spl.h>

DECLARE_GLOBAL_DATA_PTR;

int dram_init(void)
{
	struct udevice *dev;
	int ret;

	ret = uclass_get_device(UCLASS_RAM, 0, &dev);
	if (ret) {
		debug("DRAM init failed: %d\n", ret);
		return ret;
	}

	if (fdtdec_setup_mem_size_base() != 0)
		ret = -EINVAL;

	return ret;
}

int dram_init_banksize(void)
{
	fdtdec_setup_memory_banksize();

	return 0;
}

int board_early_init_f(void)
{
	return 0;
}

u32 get_board_rev(void)
{
	return 0;
}

int board_late_init(void)
{
	return 0;
}

int board_init(void)
{
	gd->bd->bi_boot_params = gd->bd->bi_dram[0].start + 0x100;
	return 0;
}

#ifdef CONFIG_SPL_BUILD
static int spl_dram_init(void)
{
	struct udevice *dev;
	int rv;

	rv = uclass_get_device(UCLASS_RAM, 0, &dev);
	if (rv) {
		debug("DRAM init failed: %d\n", rv);
		return rv;
	}
	if (fdtdec_setup_mem_size_base() != 0)
		rv = -EINVAL;
	return rv;
}

void spl_board_init(void)
{
	/* serial console initialization */
	preloader_console_init();
	/* configure mpu for sdram rw permissions */
	arch_cpu_init();
	/* DDR initialization */
	spl_dram_init();
	dram_init_banksize();
}

u32 spl_boot_device(void)
{
	return BOOT_DEVICE_SPI;
}
#endif /* CONFIG_SPL_BUILD */
