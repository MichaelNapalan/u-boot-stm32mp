/*
 * (C) Copyright 2016
 * Sergei Miroshnichenko, Emcraft Systems, sergeimir@emcraft.com
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <asm/io.h>
#include <common.h>
#include <malloc.h>
#include <dma-uclass.h>
#include <dm/fdtaddr.h>
#include <dm/device.h>
#include <asm/arch-stm32/dma.h>
#include <asm/byteorder.h>

struct stm32_dma_driver_data {
	u32		nr_channels;
	u32		iflags[8];
};

static u32 stm32_dma_elements_left(struct stm32_dma_chan *stm_chan)
{
	return readl(&stm_chan->regs->ndtr);
}

static inline int stm32_dma_chan_is_enabled(struct stm32_dma_chan *stm_chan)
{
	return !!(stm_chan->regs->cr & STM32_DMA_CR_EN);
}

static void stm32_dma_dump_regs(struct stm32_dma *dma)
{
	int i;

	printf("DMA regs: lisr 0x%x, hisr 0x%x, lifcr 0x%x, hifcr 0x%x\n",
	       dma->regs->lisr, dma->regs->hisr, dma->regs->lifcr, dma->regs->hifcr);

	for (i = 0; i < dma->chan_num; ++i) {
		struct stm32_dma_ch_regs *stream = &dma->regs->stream[i];
		printf("chan %d: cr 0x%x, ndtr 0x%x, par 0x%x, m0ar 0x%x, m1ar 0x%x, fcr 0x%x\n",
		       i, stream->cr, stream->ndtr, stream->par, stream->m0ar, stream->m1ar, stream->fcr);
	}
}

int stm32_dma_wait_for_completion(struct stm32_dma_chan *stm_chan)
{
	u32 elements_left = STM32_MAX_DMA_SIZE;
	int not_changed_cnt = 0;
	int ret = 0;
	u32* buf = (u32*)stm_chan->prep.m0ar;
	int unprocessed_len = stm_chan->len;

	do {
		u32 prev_elements_left = elements_left;
		elements_left = stm32_dma_elements_left(stm_chan);

		if (!elements_left)
			break;

		if (elements_left == prev_elements_left)
			++not_changed_cnt;
		else
			not_changed_cnt = 0;

		if (not_changed_cnt > 100) {
			pr_err("%s: DMA has failed\n", __func__);
			stm32_dma_dump_regs(stm_chan->dma);
			ret = -1;
			break;
		}

		/*
		 * Implemented only for DMA_SLAVE_BUSWIDTH_2_BYTES:
		 * loop by 32-bit words, but reverse byte order in each
		 * halfword independently.
		 */
		if (stm_chan->convert_endianess) {
			int ready_elements = unprocessed_len - elements_left;
			int i;

			for (i = 0; i < ready_elements; i += 2, ++buf)
				*buf = __arch__swahb32(*buf);
			unprocessed_len = elements_left;
		}
	} while (elements_left);

	if (stm_chan->convert_endianess) {
		int i;

		for (i = 0; i < unprocessed_len; i += 2, ++buf)
			*buf = __arch__swahb32(*buf);

		if (i != unprocessed_len)
			*((u16*)buf) = swab16(*((u16*)buf));

		if ((unprocessed_len - i) > 1) {
			pr_err("%s: internal error: unprocessed_len %d\n",
			      __func__, unprocessed_len);
		}
	}

	stm_chan->regs->cr &= ~STM32_DMA_CR_EN;

	return ret;
}

int stm32_dma_tx_submit(struct stm32_dma_chan *stm_chan)
{
	*stm_chan->ifcr_reg = STM32_INT_MSK << stm_chan->i_ofs;

	/*
	 * Set-up hw using values formed in stm_prep_xxx()
	 */
	stm_chan->regs->m0ar = stm_chan->prep.m0ar;
	stm_chan->regs->par  = stm_chan->prep.par;
	stm_chan->regs->ndtr = stm_chan->prep.ndtr;
	stm_chan->regs->fcr  = stm_chan->prep.fcr;
	stm_chan->regs->cr   = stm_chan->prep.cr | STM32_DMA_CR_EN;

	return 0;
}

struct stm32_dma_chan* stm32_dma_prepare(struct stm32_dma *dma,
					 int stream, int channel,
					 struct dma_slave_config *sconfig,
					 enum dma_direction direction,
					 dma_addr_t buf_addr,
					 size_t buf_len,
					 bool mem_increment,
					 bool convert_endianess)
{
	struct stm32_dma_chan *stm_chan = &dma->chan[stream];

	if ((sconfig->src_addr_width != DMA_SLAVE_BUSWIDTH_1_BYTE
	     && sconfig->src_addr_width != DMA_SLAVE_BUSWIDTH_2_BYTES
	     && sconfig->src_addr_width != DMA_SLAVE_BUSWIDTH_4_BYTES))
	{
		pr_err("%s: unsupported add size for source: %d bytes\n",
		      __func__, sconfig->src_addr_width);
		return NULL;
	}
	if ((sconfig->dst_addr_width != DMA_SLAVE_BUSWIDTH_1_BYTE)
	    && (sconfig->dst_addr_width != DMA_SLAVE_BUSWIDTH_2_BYTES)
	    && (sconfig->dst_addr_width != DMA_SLAVE_BUSWIDTH_4_BYTES)) {
		pr_err("%s: unsupported add size for destination: %d bytes\n",
		      __func__, sconfig->dst_addr_width);
		return NULL;
	}

	debug("%s: %s buf@0x%08x - %d\n", __func__,
	      direction == DMA_MEM_TO_DEV ? "TO DEVICE" : "FROM DEVICE",
	      buf_addr, buf_len);

	stm_chan->prep.ndtr = buf_len;
	stm_chan->prep.par = sconfig->src_addr;
	stm_chan->prep.m0ar = buf_addr;
	stm_chan->prep.cr = STM32_DMA_CR_PL_HI
		| (mem_increment ? STM32_DMA_CR_MINC : 0);

	stm_chan->prep.cr |= channel << STM32_DMA_CR_CHSEL_OFS;
	stm_chan->prep.ndtr = buf_len;
	stm_chan->prep.fcr = 0;

	stm_chan->convert_endianess = false;

	switch (direction) {
	case DMA_MEM_TO_DEV:
		stm_chan->prep.par = sconfig->dst_addr;
		stm_chan->prep.ndtr /= sconfig->dst_addr_width;
		stm_chan->prep.cr |= STM32_DMA_CR_DIR_M2P |
				     (__ffs(sconfig->src_addr_width) <<
				      STM32_DMA_CR_MSIZE_OFS) |
				     (__ffs(sconfig->dst_addr_width) <<
				      STM32_DMA_CR_PSIZE_OFS);
		if (convert_endianess)
			pr_err("%s: endianess converting is not supported for MEM_TO_DEV, ignoring\n",
			      __func__);
		break;
	case DMA_DEV_TO_MEM:
		stm_chan->prep.par = sconfig->src_addr;
		stm_chan->prep.ndtr /= sconfig->src_addr_width;
		stm_chan->prep.cr |= STM32_DMA_CR_DIR_P2M |
				     (__ffs(sconfig->src_addr_width) <<
				      STM32_DMA_CR_PSIZE_OFS) |
				     (__ffs(sconfig->dst_addr_width) <<
				      STM32_DMA_CR_MSIZE_OFS);
		if (convert_endianess) {
			if (sconfig->src_addr_width != DMA_SLAVE_BUSWIDTH_2_BYTES)
				pr_err("%s: endianess converting is not implented for selected bus width, ignoring\n",
				      __func__);
			else if (!mem_increment)
				pr_err("%s: endianess converting is meaningless without incrementing memory, ignoring\n",
				      __func__);
			else
				stm_chan->convert_endianess = true;
		}
		break;
	default:
		return NULL;
	}

	stm_chan->len = stm_chan->prep.ndtr;

	return stm_chan;
}

static void stm_dma_off(struct stm32_dma *stm_dma)
{
	int	i;

	for (i = 0; i < stm_dma->chan_num; i++)
		stm_dma->regs->stream[i].cr &= ~STM32_DMA_CR_EN;
}

static int stm32_dma_probe(struct udevice *dev)
{
	struct stm32_dma_driver_data *plat =
		(struct stm32_dma_driver_data *)dev_get_driver_data(dev);
	struct dma_dev_priv *uc_priv = dev_get_uclass_priv(dev);
	struct stm32_dma *dma = dev_get_priv(dev);
	int i;

	dma->chan_num = plat->nr_channels;
	stm_dma_off(dma);

	for (i = 0; i < dma->chan_num; i++) {
		struct stm32_dma_chan	*ch = &dma->chan[i];
		ch->idx = i;
		ch->regs = &dma->regs->stream[i];

		if (plat->iflags[i] / 32) {
			ch->isr_reg = &dma->regs->hisr;
			ch->ifcr_reg = &dma->regs->hifcr;
		} else {
			ch->isr_reg = &dma->regs->lisr;
			ch->ifcr_reg = &dma->regs->lifcr;
		}
		ch->i_ofs = plat->iflags[i] % 32;
		ch->dma = dma;
	}

	uc_priv->supported =
		DMA_SUPPORTS_MEM_TO_DEV |
		DMA_SUPPORTS_DEV_TO_MEM;

	return 0;
}

static int stm32_dma_transfer(struct udevice *dev, int direction, void *dst,
			     void *src, size_t len)
{
	pr_err("%s: NOT IMPLEMENTED\n", __func__);

	return -1;
}

static int stm32_dma_ofdata_to_platdata(struct udevice *dev)
{
	struct stm32_dma *dma = dev_get_priv(dev);

	dma->regs = (void*)devfdt_get_addr(dev);

	return 0;
}

static struct stm32_dma_driver_data stm32_config = {
	.nr_channels = 8,
	.iflags = {0, 6, 16, 22, 32, 38, 48, 54},
};

static const struct dma_ops stm32_dma_ops = {
	.transfer	= stm32_dma_transfer,
};


static const struct udevice_id stm32_dma_ids[] = {
	{
		.compatible = "st,stm32-dma",
		.data = (ulong)&stm32_config
	}, {
		/* sentinel */
	}
};

U_BOOT_DRIVER(stm32_dma) = {
	.name	= "stm32_dma",
	.id	= UCLASS_DMA,
	.of_match = stm32_dma_ids,
	.ops	= &stm32_dma_ops,
	.ofdata_to_platdata = stm32_dma_ofdata_to_platdata,
	.probe	= stm32_dma_probe,
	.priv_auto_alloc_size = sizeof(struct stm32_dma) + 8 * sizeof(struct stm32_dma_chan),
};
