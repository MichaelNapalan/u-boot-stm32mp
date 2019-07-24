// SPDX-License-Identifier: GPL-2.0+
/*
 * Emcraft Systems, www.emcraft.com, (C) Copyright 2019
 */

#include <common.h>
#include <dm.h>
#include <spl.h>
#include <asm/arch/gpio.h>
#include <asm/gpio.h>

DECLARE_GLOBAL_DATA_PTR;

#define GPIOA	0
#define GPIOB	(GPIOA + 16)
#define GPIOC	(GPIOB + 16)
#define GPIOD	(GPIOC + 16)
#define GPIOE	(GPIOD + 16)
#define GPIOF	(GPIOE + 16)
#define GPIOG	(GPIOF + 16)
#define GPIOH	(GPIOG + 16)
#define GPIOI	(GPIOH + 16)
#define GPIOJ	(GPIOI + 16)
#define GPIOK	(GPIOJ + 16)

static void custom_gpio_setup(void)
{
	unsigned int gpio;
	int ret = 0;
	int gpio_pin = GPIOB + 0; /* PB0 */
	char gpio_name[4];

	sprintf(gpio_name, "%d", gpio_pin);
	if (gpio_lookup_name(gpio_name, NULL, NULL, &gpio)) {
		printf("GPIO'%s' not found\n", gpio_name);
		return;
	}

	ret = gpio_request(gpio, "gpio_cfg");
	if (ret && ret != -EBUSY) {
		printf("gpio: requesting pin %u failed\n", gpio);
		return;
	}

	if (gpio_direction_output(gpio, 1)) {
		printf("GPIO `%d' gpio_direction_output error\n", gpio);
		return;
	}
}

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
	custom_gpio_setup();
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
