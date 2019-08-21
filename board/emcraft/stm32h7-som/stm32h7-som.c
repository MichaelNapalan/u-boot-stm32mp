// SPDX-License-Identifier: GPL-2.0+
/*
 * Emcraft Systems, www.emcraft.com, (C) Copyright 2019
 */

#include <common.h>
#include <dm.h>
#include <dm/uclass-internal.h>
#include <spl.h>
#include <asm/arch/gpio.h>
#include <asm/gpio.h>
#include <dt-bindings/memory/stm32-sdram.h>
#include <ubifs_uboot.h>
#include <ubi_uboot.h>
#include <environment.h>
#ifdef CONFIG_SPLASH_SCREEN
#include <splash.h>
#include <video.h>
#include <panel.h>
#endif

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
	int ret = 0;
	unsigned int gpio_pin = GPIOB + 0; /* PB0 */

	ret = gpio_request(gpio_pin, "gpio_cfg");
	if (ret && ret != -EBUSY) {
		printf("gpio: requesting pin %u failed\n", gpio_pin);
		return;
	}

	if (gpio_direction_output(gpio_pin, 1)) {
		printf("GPIO `%u' gpio_direction_output error\n", gpio_pin);
		return;
	}
}

static int detect_sdram_size(void)
{
	int ret = 0;
	int gpio_pin = GPIOE + 2; /* PE2 */
	u32 * gpioe_pullup_reg = (u32 *)(0x5802100C);

	ret = gpio_request(gpio_pin, "dram_cfg");
	if (ret && ret != -EBUSY) {
		debug("gpio: requesting pin %u failed\n", gpio_pin);
		return -1;
	}
	gpio_direction_input(gpio_pin);
	* gpioe_pullup_reg |= 0x10;
	ret = gpio_get_value(gpio_pin);
	* gpioe_pullup_reg &= ~0x10;
	return ret;
}

static void dts_fixup(struct udevice *dev)
{
	ofnode ofnode;
	u8 * sdram_control;
	char * sdram_prop = "st,sdram-control";
	u32 * mem;
	int size;

	ofnode = ofnode_find_subnode(dev->node, "bank@1");
	size = ofnode_read_size(ofnode, sdram_prop);
	sdram_control = (u8 *)ofnode_read_u8_array_ptr(ofnode, sdram_prop, size);
	sdram_control[2] = MWIDTH_32;
	ofnode = ofnode_path("/memory");
	if (ofnode_valid(ofnode)) {
		mem = (u32 *)ofnode_get_property(ofnode, "reg", &size);
		mem[1] = 0x4;
	}
}

#ifdef CONFIG_SPLASH_SCREEN
static int splash_screen_display(void)
{
#ifdef CONFIG_DM_VIDEO
	struct udevice *dev;
#endif
	int res = 0;
	ulong addr = CONFIG_SYS_LOAD_ADDR;

	res = ubi_volume_read("splash", (char *)addr, 0);
	if (res) {
		printf("%s: ubi_volume_read error=%d\n", __func__, res);
		return res;
	}
#ifdef CONFIG_DM_VIDEO
	res = uclass_first_device_err(UCLASS_VIDEO, &dev);
	if (res) {
		printf("%s: uclass_first_device_err error=%d\n", __func__, res);
		return res;
	}
	video_bmp_display(dev, addr, 0, 0, false);

	uclass_first_device(UCLASS_PANEL, &dev);
	res = panel_enable_backlight(dev);
	if (res) {
		printf("%s: panel %s enable backlight error %d\n", __func__, dev->name, res);
		return res;
	}
#endif
	return res;
}
#endif

int dram_init(void)
{
	struct udevice *dev;
	int ret;

	ret = uclass_find_device(UCLASS_RAM, 0, &dev);
	if (ret) {
		debug("DRAM find device failed: %d\n", ret);
		return ret;
	}

	if (detect_sdram_size() == 1) {
		dts_fixup(dev);
	}

	ret = uclass_get_device_tail(dev, ret, &dev);
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

#ifndef CONFIG_SPL_BUILD
struct uboot_abi {
	unsigned int magic;
#define UBOOT_ABI_MAGIC		0xB00DB00D
	unsigned int version;
#define UBOOT_ABI_VERSION	0x00000001
	volatile void *gd;
	void (*prepare_abi)(void);
	int (*ubi_part)(char *part_name, const char *vid_header_offset);
	int (*ubifs_mount)(char *vol_name);
	int (*ubifs_umount)(void);
	int (*ubifs_read)(const char *filename, void *buf, loff_t offset,
			loff_t size, loff_t *actread);
	int (*ubifs_ls)(const char *filename);
	int (*ubifs_write)(const char *filename, void *buf, loff_t offset,
			loff_t size, loff_t *actwritten);
	int (*ubifs_mkdir)(const char *filename);
	int (*ubifs_rmdir)(const char *filename);
	int (*ubifs_unlink)(const char *filename);
	char * (*getenv)(const char *varname);
	int (*setenv)(const char *varname, const char *value);
	int (*saveenv)(void);
};

static void prepare_abi(void)
{
	/* Function table is located at the start of ITCM */
	struct uboot_abi *s = (void *)0x00000000;
	gd = s->gd;
}
#endif

int board_late_init(void)
{
#ifdef CONFIG_SPLASH_SCREEN
	splash_screen_display();
#endif
	custom_gpio_setup();
#ifndef CONFIG_SPL_BUILD
	/* Function table is located at the start of ITCM */
	struct uboot_abi *s = (void *)0x00000000;

	s->magic = UBOOT_ABI_MAGIC;
	s->version = UBOOT_ABI_VERSION;
	s->gd = gd;
	s->prepare_abi = prepare_abi;
	s->ubi_part = ubi_part;
	s->ubifs_mount = cmd_ubifs_mount;
	s->ubifs_umount = cmd_ubifs_umount;
	s->ubifs_read = ubifs_read;
	s->ubifs_ls = ubifs_ls;
	s->ubifs_write = ubifs_write;
	s->ubifs_mkdir = ubifs_mkdir;
	s->ubifs_rmdir = ubifs_rmdir;
	s->ubifs_unlink = ubifs_unlink;
	s->getenv = env_get;
	s->setenv = env_set;
	s->saveenv = env_save;
#endif
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
