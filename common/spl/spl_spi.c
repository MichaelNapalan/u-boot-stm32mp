// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2011 OMICRON electronics GmbH
 *
 * based on drivers/mtd/nand/raw/nand_spl_load.c
 *
 * Copyright (C) 2011
 * Heiko Schocher, DENX Software Engineering, hs@denx.de.
 */

#include <common.h>
#include <spi.h>
#include <spi_flash.h>
#include <errno.h>
#include <spl.h>

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_SPL_OS_BOOT
/*
 * Load the kernel, check for a valid header we can parse, and if found load
 * the kernel and then device tree.
 */
static int spi_load_image_os(struct spl_image_info *spl_image,
			     struct spi_flash *flash,
			     struct image_header *header)
{
	int err;

	/* Read for a header, parse or error out. */
	spi_flash_read(flash, CONFIG_SYS_SPI_KERNEL_OFFS, sizeof(*header),
		       (void *)header);

	if (image_get_magic(header) != IH_MAGIC)
		return -1;

	err = spl_parse_image_header(spl_image, header);
	if (err)
		return err;

	spi_flash_read(flash, CONFIG_SYS_SPI_KERNEL_OFFS,
		       spl_image->size, (void *)spl_image->load_addr);

	/* Read device tree. */
	spi_flash_read(flash, CONFIG_SYS_SPI_ARGS_OFFS,
		       CONFIG_SYS_SPI_ARGS_SIZE,
		       (void *)CONFIG_SYS_SPL_ARGS_ADDR);

	return 0;
}
#endif

static ulong spl_spi_fit_read(struct spl_load_info *load, ulong sector,
			      ulong count, void *buf)
{
	struct spi_flash *flash = load->dev;
	ulong ret;

	ret = spi_flash_read(flash, sector, count, buf);
	if (!ret)
		return count;
	else
		return 0;
}
/*
 * The main entry for SPI booting. It's necessary that SDRAM is already
 * configured and available since this code loads the main U-Boot image
 * from SPI into SDRAM and starts it from there.
 */
static int spl_spi_load_image(struct spl_image_info *spl_image,
			      struct spl_boot_device *bootdev)
{
	int err = 0;
	unsigned payload_offs = CONFIG_SYS_SPI_U_BOOT_OFFS;
	struct spi_flash *flash;
	struct image_header *header;

	/*
	 * Load U-Boot image from SPI flash into RAM
	 */

#ifndef CONFIG_SF_DUMMY
	flash = spi_flash_probe(CONFIG_SF_DEFAULT_BUS,
				CONFIG_SF_DEFAULT_CS,
				CONFIG_SF_DEFAULT_SPEED,
				CONFIG_SF_DEFAULT_MODE);
	if (!flash) {
		puts("SPI probe failed.\n");
		return -ENODEV;
	}
#endif

	header = spl_get_load_buffer(-sizeof(*header), sizeof(*header));

#ifndef CONFIG_SF_DUMMY
#if CONFIG_IS_ENABLED(OF_CONTROL) && !CONFIG_IS_ENABLED(OF_PLATDATA)
	payload_offs = fdtdec_get_config_int(gd->fdt_blob,
					     "u-boot,spl-payload-offset",
					     payload_offs);
#endif
#endif

#ifdef CONFIG_SPL_OS_BOOT
	if (spl_start_uboot() || spi_load_image_os(spl_image, flash, header))
#endif
	{
		/* Load u-boot, mkimage header is 64 bytes. */
#ifndef CONFIG_SF_DUMMY
		err = spi_flash_read(flash, payload_offs, sizeof(*header),
				     (void *)header);
		if (err) {
			debug("%s: Failed to read from SPI flash (err=%d)\n",
			      __func__, err);
			return err;
		}
#else
		memcpy((void *)header, (void *)CONFIG_SF_DUMMY + payload_offs, sizeof(*header));
#endif

		if (IS_ENABLED(CONFIG_SPL_LOAD_FIT_FULL) &&
		    image_get_magic(header) == FDT_MAGIC) {
			err = spi_flash_read(flash, payload_offs,
					     roundup(fdt_totalsize(header), 4),
					     (void *)CONFIG_SYS_LOAD_ADDR);
			if (err)
				return err;
			err = spl_parse_image_header(spl_image,
					(struct image_header *)CONFIG_SYS_LOAD_ADDR);
		} else if (IS_ENABLED(CONFIG_SPL_LOAD_FIT) &&
			   image_get_magic(header) == FDT_MAGIC) {
			struct spl_load_info load;

			debug("Found FIT\n");
			load.dev = flash;
			load.priv = NULL;
			load.filename = NULL;
			load.bl_len = 1;
			load.read = spl_spi_fit_read;
			err = spl_load_simple_fit(spl_image, &load,
						  payload_offs,
						  header);
		} else {
#ifdef CONFIG_CPU_V7M
			/*
			 * Without this flag, the image entry point is ignored
			 * and forcibly set to TEXT_BASE, which is not correct for CPU_V7M
			 */
			spl_image->flags |= SPL_COPY_PAYLOAD_ONLY;

			payload_offs += sizeof(*header);
#endif
			err = spl_parse_image_header(spl_image, header);
			if (err)
				return err;
#ifndef CONFIG_SF_DUMMY
			err = spi_flash_read(flash, payload_offs,
					     spl_image->size,
					     (void *)spl_image->load_addr);
#else
			memcpy((void *)spl_image->load_addr, (void *)CONFIG_SF_DUMMY + payload_offs, spl_image->size);
#endif
		}
	}

	return err;
}
/* Use priorty 1 so that boards can override this */
SPL_LOAD_IMAGE_METHOD("SPI", 1, BOOT_DEVICE_SPI, spl_spi_load_image);

/*
 * Write a binary image to SPI Flash
 */
int spl_spi_save_image(ulong offset, ulong size, void *addr)
{
	int err = 0;
	struct spi_flash *flash;
	ulong erase_size;

	flash = spi_flash_probe(CONFIG_SF_DEFAULT_BUS,
				CONFIG_SF_DEFAULT_CS,
				CONFIG_SF_DEFAULT_SPEED,
				CONFIG_SF_DEFAULT_MODE);
	if (!flash) {
		puts("SPI probe failed.\n");
		return -ENODEV;
	}

	if (offset + size > flash->size) {
		puts("Incorrect image size.\n");
		return -EINVAL;
	}

	if (offset % flash->erase_size) {
		puts("Image offset not on sector boundary.\n");
		return -EINVAL;
	}

	erase_size = size / flash->erase_size * flash->erase_size;
	if (erase_size < size) erase_size += flash->erase_size;

	err = spi_flash_erase(flash, offset, erase_size);
	if (!flash) {
		puts("SPI Flash erase failed.\n");
		return err;
	}

	err = spi_flash_write(flash, offset, size, addr);
	if (err) {
		puts("SPI Flash write failed.\n");
		return err;
	}

	return 0;
}
