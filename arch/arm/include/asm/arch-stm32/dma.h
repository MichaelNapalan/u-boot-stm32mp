#ifndef _MACH_STM32_DMA_H_
#define _MACH_STM32_DMA_H_

typedef u32 dma_addr_t;

struct dma_slave_config {
	enum dma_direction direction;
	dma_addr_t src_addr;
	dma_addr_t dst_addr;
	enum dma_slave_buswidth src_addr_width;
	enum dma_slave_buswidth dst_addr_width;
	u32 src_maxburst;
	u32 dst_maxburst;
	bool device_fc;
	unsigned int slave_id;
};

struct stm32_dma_ch_regs {
	u32	cr;
	u32	ndtr;
	u32	par;
	u32	m0ar;
	u32	m1ar;
	u32	fcr;
};

struct stm32_dma_regs {
	u32	lisr;
	u32	hisr;
	u32	lifcr;
	u32	hifcr;

	struct stm32_dma_ch_regs stream[8];
};

struct stm32_dma;

struct stm32_dma_chan {
	int				idx;

	struct stm32_dma_ch_regs __iomem *regs;	/* pointer to hw registers    */
	struct stm32_dma_ch_regs	prep;	/* prepared register values   */

	u32 __iomem			*isr_reg;
	u32 __iomem			*ifcr_reg;
	u32				i_ofs;
	u32				i_val;

	u32				len;
	struct stm32_dma		*dma;
	bool				convert_endianess;
};

struct stm32_dma {
	struct stm32_dma_regs __iomem	*regs;
	struct clk			*clk;

	int				chan_num;
	/* Must be the last */
	struct stm32_dma_chan		chan[0];
};

#define STM32_INT_TCIF			(1 << 5)
#define STM32_INT_HTIF			(1 << 4)
#define STM32_INT_FEIF			(1 << 0)
#define STM32_INT_MSK			(STM32_INT_TCIF | STM32_INT_HTIF |     \
					 STM32_INT_FEIF)
#define STM32_DMA_CR_CHSEL_OFS		25
#define STM32_DMA_CR_MBURST_OFS		23
#define STM32_DMA_CR_PBURST_OFS		21
#define STM32_DMA_CR_PL_HI		(3 << 16)
#define STM32_DMA_CR_MSIZE_OFS		13
#define STM32_DMA_CR_PSIZE_OFS		11
#define STM32_DMA_CR_MINC		(1 << 10)
#define STM32_DMA_CR_PINC		(1 << 9)
#define STM32_DMA_CR_CIRC		(1 << 8)
#define STM32_DMA_CR_DIR_P2M		(0 << 6)
#define STM32_DMA_CR_DIR_M2P		(1 << 6)
#define STM32_DMA_CR_PFCTRL		(1 << 5)
#define STM32_DMA_CR_TCIE		(1 << 4)
#define STM32_DMA_CR_HTIE		(1 << 3)
#define STM32_DMA_CR_EN			(1 << 0)

/*
 * FCR register bits
 */
#define STM32_DMA_FCR_FEIE		(1 << 7)
#define STM32_DMA_FCR_DMDIS		(1 << 2)
#define STM32_DMA_FCR_FTH_OFS		0

#define SMT32_DMA_ISR_ERROR_MASK	((1 << 24) | (1 << 18) | (1 << 8) |  (1 << 2))

#define STM32_MAX_DMA_SIZE		(0xFFFE)

struct stm32_dma_chan* stm32_dma_prepare(struct stm32_dma *dma,
					 int stream, int channel,
					 struct dma_slave_config *sconfig,
					 enum dma_direction direction,
					 dma_addr_t buf_addr,
					 size_t buf_len,
					 bool mem_increment,
					 bool convert_endianess);

int stm32_dma_tx_submit(struct stm32_dma_chan *stm_chan);
int stm32_dma_wait_for_completion(struct stm32_dma_chan *stm_chan);

#endif /* _MACH_STM32_DMA_H_ */
