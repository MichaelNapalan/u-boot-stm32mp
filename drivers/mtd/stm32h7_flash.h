struct stm32h7_flash_regs {
	u32 acr;
	u32 key;
	u32 optkeyr;
	u32 cr;
	u32 sr;
	u32 ccr;
	u32 optcr;
	u32 optsr_cur;
	u32 optsr_prg;
	u32 optccr;
	u32 prar_cur;
	u32 prar_prg;
	u32 scar_cur;
	u32 scar_prg;
};

#define STM32H7_FLASH_KEY1		0x45670123
#define STM32H7_FLASH_KEY2		0xCDEF89AB

#define STM32H7_FLASH_SR_BSY		(1)

#define STM32H7_FLASH_CR_PG		(1 << 1)
#define STM32H7_FLASH_CR_SER		(1 << 2)
#define STM32H7_FLASH_CR_STRT		(1 << 16)
#define STM32H7_FLASH_CR_LOCK		(1)
#define STM32H7_FLASH_CR_SNB_OFFSET	3
#define STM32H7_FLASH_SCAR_DMES1	(1 << 31)
#define STM32H7_FLASH_CR_SNB_MASK	(7 << 8)
#define STM32H7_FLASH_SR_QW1		(1 << 2)
