// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2019
 * Alexander Dyachenko <sasha_d@emcraft.com>
 */
#include <common.h>
#include <spl.h>
#ifdef CONFIG_SPL_SPI_LOAD
#include <spi.h>
#include <spi_flash.h>
#endif

int spl_icptf(ulong flash, void *addr)
{
	int err = 0;
	struct image_header *header = addr;

	if (image_get_magic(header) != IH_MAGIC) {
		puts("uImage not found.\n");
		return -1;
	}

	/*
	 * Load U-Boot image from RAM into Flash
	 */

#ifdef CONFIG_SPL_SPI_LOAD
	if (flash == 1) {
		err = spl_spi_save_image(
				CONFIG_SYS_SPI_U_BOOT_OFFS,
				sizeof(*header) + image_get_data_size(header),
				addr);
		if (err) {
			puts("SPI save image failed.\n");
			return err;
		}
		return 0;
	}
#endif

	puts("Flash not found.\n");
	return -1;
}
