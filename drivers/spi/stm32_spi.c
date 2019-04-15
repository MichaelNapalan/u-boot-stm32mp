/*
 * (C) Copyright 2016
 * Sergei Miroshnichenko, Emcraft Systems, sergeimir@emcraft.com
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <malloc.h>
#include <spi.h>
#include <asm/io.h>
#include <asm/gpio.h>
#include <dm.h>
#include <dma.h>
#include <errno.h>
#include <asm/arch/stm32.h>
#include <asm/arch-stm32/dma.h>
#include <asm/byteorder.h>

DECLARE_GLOBAL_DATA_PTR;

struct stm32_spi_regs {
	u16	cr1;
	u16	align1;
	u16	cr2;
	u16	align2;
	u16	sr;
	u16	align3;
	u16	dr;
	u16	align4;
	u16	crcpr;
	u16	align5;
	u16	rxcrcr;
	u16	align6;
	u16	txcrcr;
	u16	align7;
	u16	i2scfgr;
	u16	align8;
	u16	i2spr;
};

#define SPI_CR1_DFF			(1<<11)
#define SPI_CR1_RX_ONLY			(1<<10)
#define SPI_CR1_SSM			(1<<9)
#define SPI_CR1_SSI			(1<<8)
#define SPI_CR1_SPE			(1<<6)
#define SPI_CR1_BR(x)			((x)<<3)
#define SPI_CR1_MSTR			(1<<2)
#define SPI_CR1_CPOL			(1<<1)
#define SPI_CR1_CPHA			(1<<0)
#define SPI_CR2_TXEIE			(1<<7)
#define SPI_CR2_RXNEIE			(1<<6)
#define SPI_CR2_ERRIE			(1<<5)
#define SPI_CR2_FRF			(1<<4)
#define SPI_CR2_SSOE			(1<<2)
#define SPI_CR2_TXDMAEN			(1<<1)
#define SPI_CR2_RXDMAEN			(1<<0)
#define SPI_SR_FTLVL_FULL		(3<<11)
#define SPI_SR_FRE			(1<<8)
#define SPI_SR_BSY			(1<<7)
#define SPI_SR_OVR			(1<<6)
#define SPI_SR_UDR			(1<<3)
#define SPI_SR_TXE			(1<<1)
#define SPI_SR_RXNE			(1<<0)

#define SPI_TIMEOUT_MS			100

struct stm32_spi_platdata {
	u32 base;
	u32 max_hz;
	u32 bus_freq;
	int dma_number;
	int dma_channel;
	int rx_dma_stream;
	int tx_dma_stream;
};

struct stm32_spi_priv {
	struct stm32_spi_regs *regs;
	int cs_gpio;
	u32 max_hz;
	u32 bus_freq;
	u32 mode;
	struct stm32_dma *dma;
	int dma_number;
	int dma_channel;
	int rx_dma_stream;
	int tx_dma_stream;
	struct stm32_dma_chan *rx_chan;
	struct stm32_dma_chan *tx_chan;
	u32 *empty_buf;
	struct gpio_desc nss;
};

static int stm32_spi_ofdata_to_platdata(struct udevice *bus)
{
	/*
	 * SPI1, SPI4-6 are on the APB2 clock (90/84 MHz)
	 * SPI2-3 are on the APB1 clock (45/40 MHz)
	 */
	static const int bus_freqs[6] = { 90000000, 45000000, 45000000, 90000000, 90000000, 90000000 };
	static const int dma_numbers[6] = { 1, 0, 0, 1, 1, 1 };
	static const int dma_channels[6] = { 3, 0, 0, 4, 2, 1 };
	static const int rx_dma_streams[6] = { 0, 3, 0, 0, 3, 6 };
	static const int tx_dma_streams[6] = { 3, 4, 5, 1, 4, 5 };

	struct stm32_spi_platdata *plat = bus->platdata;
	struct fdt_resource res_regs, res_mem;
	const void *blob = gd->fdt_blob;
	int node = dev_of_offset(bus);
	int ret;

	ret = fdt_get_named_resource(blob, node, "reg", "reg-names",
				     "QuadSPI", &res_regs);
	if (ret) {
		pr_err("%s: can't get regs base addresses(ret = %d)!\n", __func__, ret);
		return -ENOMEM;
	}
	ret = fdt_get_named_resource(blob, node, "reg", "reg-names",
				     "QuadSPI-memory", &res_mem);
	if (ret) {
		pr_err("%s: can't get mmap base address(ret = %d)!\n", __func__, ret);
		return -ENOMEM;
	}

	plat->bus_freq = bus_freqs[bus->seq];
	plat->max_hz = fdtdec_get_int(blob, node, "spi-max-frequency",
				      plat->bus_freq / 2);

	plat->base = res_regs.start;

	plat->dma_number = dma_numbers[bus->seq];
	plat->dma_channel = dma_channels[bus->seq];
	plat->rx_dma_stream = rx_dma_streams[bus->seq];
	plat->tx_dma_stream = tx_dma_streams[bus->seq];

	debug("%s: regs=<0x%x> max-frequency=%d\n",
	      __func__, plat->base, plat->max_hz);

	return 0;
}

static int smt32_dma_prepare_spi_rx(struct stm32_spi_priv *priv, struct stm32_dma *dma, void *dst, int len)
{
	struct dma_slave_config config;

	memset(&config, 0, sizeof(config));
	config.direction = DMA_DEV_TO_MEM;
	config.src_addr_width = DMA_SLAVE_BUSWIDTH_2_BYTES;
	config.dst_addr_width = DMA_SLAVE_BUSWIDTH_2_BYTES;
	config.dst_addr = (dma_addr_t)(dst ? dst : priv->empty_buf);
	config.src_addr = (dma_addr_t)&priv->regs->dr;
	config.src_maxburst = 1;

	priv->rx_chan =
		stm32_dma_prepare(priv->dma,
				  priv->rx_dma_stream, priv->dma_channel,
				  &config, config.direction,
				  config.dst_addr, len,
				  !!dst,
				  !!dst);

	return priv->rx_chan ? 0 : -EIO;
}

static int smt32_dma_prepare_spi_tx(struct stm32_spi_priv *priv, struct stm32_dma *dma, const void *src, int len)
{
	struct dma_slave_config config;

	memset(&config, 0, sizeof(config));
	config.direction = DMA_MEM_TO_DEV;
	config.src_addr_width = DMA_SLAVE_BUSWIDTH_2_BYTES;
	config.dst_addr_width = DMA_SLAVE_BUSWIDTH_2_BYTES;
	config.src_addr = (dma_addr_t)(src ? src : priv->empty_buf);
	config.dst_addr = (dma_addr_t)&priv->regs->dr;
	config.src_maxburst = 1;
	*priv->empty_buf = 0xFFFFFFFF;

	priv->tx_chan =
		stm32_dma_prepare(priv->dma,
				  priv->tx_dma_stream, priv->dma_channel,
				  &config, config.direction,
				  config.src_addr, len,
				  !!src,
				  false);

	return priv->tx_chan ? 0 : -EIO;
}

static int stm32_spi_probe(struct udevice *bus)
{
	struct stm32_spi_platdata *plat = dev_get_platdata(bus);
	struct stm32_spi_priv *priv = dev_get_priv(bus);
	struct dm_spi_bus *dm_spi_bus;
	int err = -EIO;

	err = gpio_request_by_name(bus, "cs-gpios", 0, &priv->nss, GPIOD_IS_OUT);
	if (err) {
		pr_err("%s: No cs-gpios property: %d\n", __func__, err);
		return err;
	}

	priv->regs = (struct stm32_spi_regs *)(uintptr_t)plat->base;

	dm_spi_bus = bus->uclass_priv;
	dm_spi_bus->max_hz = plat->max_hz;
	priv->max_hz = plat->max_hz;
	priv->bus_freq = plat->bus_freq;

	priv->dma_number = plat->dma_number;
	priv->dma_channel = plat->dma_channel;
	priv->rx_dma_stream = plat->rx_dma_stream;
	priv->tx_dma_stream = plat->tx_dma_stream;
	priv->empty_buf = (u32*)(CONFIG_SYS_SPI_DUMMY_ADDR);

	writew(SPI_CR1_MSTR, &priv->regs->cr1);
	setbits_le16(&priv->regs->cr1, SPI_CR1_SSM | SPI_CR1_SSI);

	{
		struct udevice *dma_dev;
		int i;

		for (i = 0, err = uclass_first_device(UCLASS_DMA, &dma_dev);
		     i < priv->dma_number && dma_dev && !err;
		     ++i, uclass_next_device(&dma_dev));

		if ((err < 0) || (i != priv->dma_number) || !dma_dev) {
			pr_err("%s: can't get dma device: %d\n", __func__, err);
			return err;
		}

		priv->dma = dev_get_priv(dma_dev);
	}

	return err;
}

static void stm32_spi_cs_activate(struct stm32_spi_priv *priv)
{
	dm_gpio_set_value(&priv->nss, 0);
}

static void stm32_spi_cs_deactivate(struct stm32_spi_priv *priv)
{
	dm_gpio_set_value(&priv->nss, 1);
}

static int stm32_spi_claim_bus(struct udevice *dev)
{
	struct udevice *bus = dev->parent;
	struct stm32_spi_priv *priv = dev_get_priv(bus);

	setbits_le16(&priv->regs->cr1, SPI_CR1_SPE);

	return 0;
}

static int stm32_spi_release_bus(struct udevice *dev)
{
	struct udevice *bus = dev->parent;
	struct stm32_spi_priv *priv = dev_get_priv(bus);

	clrbits_le16(&priv->regs->cr1, SPI_CR1_SPE);

	return 0;
}

static inline int stm32_spi_hw_txfifo_full(struct stm32_spi_priv *priv)
{
	return (readw(&priv->regs->sr) & SPI_SR_FTLVL_FULL) == SPI_SR_FTLVL_FULL;
}

static inline int stm32_spi_hw_rxfifo_error(struct stm32_spi_priv *priv)
{
	return readw(&priv->regs->sr) & (SPI_SR_FRE | SPI_SR_UDR);
}

static inline int stm32_spi_hw_rxfifo_empty(struct stm32_spi_priv *priv)
{
	return !(readw(&priv->regs->sr) & SPI_SR_RXNE);
}

static int stm32_dma_portion(struct stm32_spi_priv *priv, const u8 *dout, u8 *din, int len)
{
	int ret;
	u32 *dout_converted = NULL;

	if (len & 1) {
		pr_err("%s: DMA is not supported for odd buffer lengths: %d\n",
		      __func__, len);
		return -EINVAL;
	}

	/*
	 * Need to convert the endianess for 16-bit SPI data
	 * frame format:
	 * loop by 32-bit words, but reverse byte order in each
	 * halfword independently.
	 */
	if (dout)
	{
		int i;
		u32 *dout_tmp = malloc(len);
		if (!dout_tmp) {
			pr_err("%s: unable to alloc %d bytes for swapping the out buffer\n",
			      __func__, len);
			ret = -ENOMEM;
			goto done;
		}
		dout_converted = dout_tmp;

		for (i = 0; i + 4 <= len;
		     i += 4, ++dout_tmp, dout += 4)
			*dout_tmp = __arch__swahb32(*((u32*)dout));

		if (i < len)
			*((u16*)dout_tmp) = swab16(*((u16*)dout));

		dout = (u8*)dout_converted;
	}

	ret = smt32_dma_prepare_spi_rx(priv, priv->dma, din, len);
	if (ret) {
		pr_err("%s: unable to set up RX DMA\n", __func__);
		goto done;
	}

	ret = smt32_dma_prepare_spi_tx(priv, priv->dma, dout, len);
	if (ret) {
		pr_err("%s: unable to set up TX DMA\n", __func__);
		goto done;
	}

	ret = stm32_dma_tx_submit(priv->rx_chan);
	if (ret) {
		pr_err("%s: unable to submit RX DMA\n", __func__);
		goto done;
	}

	ret = stm32_dma_tx_submit(priv->tx_chan);
	if (ret) {
		pr_err("%s: unable to submit RX DMA\n", __func__);
		goto done;
	}

	ret = stm32_dma_wait_for_completion(priv->rx_chan);
	if (ret) {
		pr_err("%s: unable to finish RX DMA\n", __func__);
		goto done;
	}

	ret = stm32_dma_wait_for_completion(priv->tx_chan);
	if (ret) {
		pr_err("%s: unable to finish TX DMA\n", __func__);
		goto done;
	}

	if (dout_converted)
		free(dout_converted);

done:
	return ret;
}

static int stm32_spi_xfer(struct udevice *dev, unsigned int bitlen,
			  const void *data_out, void *data_in, unsigned long flags)
{
	struct udevice *bus = dev->parent;
	struct stm32_spi_priv *priv = dev_get_priv(bus);
	int num_bytes = bitlen / 8;
	const u8 *dout = data_out;
	u8 *din = data_in;
	int ret = -1;
	int leftover = num_bytes & 1;

	debug("%s: slave %u:%u dout %p din %p bitlen %u (%u bytes), flags 0x%lx, leftover %d\n",
	      __func__, bus->seq, spi_chip_select(dev),
	      dout, din, bitlen, num_bytes, flags, leftover);

	if (bitlen % 8)
		return -1;

	if (!bitlen)
		return 0;

	if (!dout && !din)
		return -EINVAL;

	if (flags & SPI_XFER_BEGIN) {
		stm32_spi_cs_activate(priv);
	}

	num_bytes &= ~1;

	if (num_bytes) {
		clrbits_le16(&priv->regs->cr1, SPI_CR1_SPE);
		setbits_le16(&priv->regs->cr2, SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN);
		setbits_le16(&priv->regs->cr1, SPI_CR1_DFF);
		setbits_le16(&priv->regs->cr1, SPI_CR1_SPE);
	}

	while (num_bytes > STM32_MAX_DMA_SIZE) {
		ret = stm32_dma_portion(priv, dout, din, STM32_MAX_DMA_SIZE);
		if (ret)
			goto out;
		num_bytes -= STM32_MAX_DMA_SIZE;
		if (din)
			din += STM32_MAX_DMA_SIZE;
		if (dout)
			dout += STM32_MAX_DMA_SIZE;
	}

	if (num_bytes) {
		ret = stm32_dma_portion(priv, dout, din, num_bytes);
		if (ret)
			goto out;

		if (din)
			din += num_bytes;
		if (dout)
			dout += num_bytes;
	}

	if (leftover) {
		unsigned int timer_start;

		clrbits_le16(&priv->regs->cr1, SPI_CR1_SPE);
		clrbits_le16(&priv->regs->cr2, SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN);
		clrbits_le16(&priv->regs->cr1, SPI_CR1_DFF);
		setbits_le16(&priv->regs->cr1, SPI_CR1_SPE);

		writeb(dout ? dout[0] : 0xFF, &priv->regs->dr);

		if (stm32_spi_hw_rxfifo_error(priv)) {
			pr_err("%s: rxfifo_error\n", __func__);
			ret = -EIO;
			goto out;
		}

		timer_start = get_timer(0);

		while (stm32_spi_hw_rxfifo_empty(priv)) {
			if (get_timer(timer_start) > SPI_TIMEOUT_MS) {
				pr_err("%s: timeout\n", __func__);
				ret = -ETIMEDOUT;
				goto out;
			}
		}

		if (din != NULL)
			din[0] = readb(&priv->regs->dr);
		else
			*priv->empty_buf = readb(&priv->regs->dr);
	}

	ret = 0;

out:
	if (flags & SPI_XFER_END) {
		stm32_spi_cs_deactivate(priv);
	}

	return ret;
}

static int stm32_spi_set_speed(struct udevice *bus, uint speed)
{
	struct stm32_spi_priv *priv = dev_get_priv(bus);
	int i;

	/*
	 * Calculate the clock rate that works for this slave
	 */
	for (i = 1; i <= 8; i ++) {
		if (priv->bus_freq / (1 << i) <= speed)
			break;
	}

	if (i == 9) {
		pr_err("%s: Can't provide a rate that is slow enough (%d, %d)\n",
		       __func__, speed, priv->bus_freq / (1 << 8));
		return -EIO;
	}

	clrsetbits_le16(&priv->regs->cr1, SPI_CR1_BR(7), SPI_CR1_BR(i - 1));

	return 0;
}

static int stm32_spi_set_mode(struct udevice *bus, uint mode)
{
	struct stm32_spi_priv *priv = dev_get_priv(bus);

	/*
	 * Disable the SPI contoller
	 */
	clrbits_le16(&priv->regs->cr1, SPI_CR1_SPE);

	/*
	 * Set the mode
	 */
	if (mode & SPI_CPHA)
		setbits_le16(&priv->regs->cr1, SPI_CR1_CPHA);
	else
		clrbits_le16(&priv->regs->cr1, SPI_CR1_CPHA);

	if (mode & SPI_CPOL)
		setbits_le16(&priv->regs->cr1, SPI_CR1_CPOL);
	else
		clrbits_le16(&priv->regs->cr1, SPI_CR1_CPOL);

	/*
	 * Re-enable the SPI contoller
	 */
	setbits_le16(&priv->regs->cr1, SPI_CR1_SPE);
	return 0;
}

static const struct dm_spi_ops stm32_spi_ops = {
	.claim_bus	= stm32_spi_claim_bus,
	.release_bus	= stm32_spi_release_bus,
	.xfer		= stm32_spi_xfer,
	.set_speed	= stm32_spi_set_speed,
	.set_mode	= stm32_spi_set_mode,
};

static const struct udevice_id stm32_spi_ids[] = {
	{
		.compatible = "st,stm32-spi",
	}, {
		/* sentinel */
	}
};

U_BOOT_DRIVER(stm32_spi) = {
	.name	= "stm32_spi",
	.id	= UCLASS_SPI,
	.of_match = stm32_spi_ids,
	.ops	= &stm32_spi_ops,
	.ofdata_to_platdata = stm32_spi_ofdata_to_platdata,
	.platdata_auto_alloc_size = sizeof(struct stm32_spi_platdata),
	.priv_auto_alloc_size = sizeof(struct stm32_spi_priv),
	.probe	= stm32_spi_probe,
};
