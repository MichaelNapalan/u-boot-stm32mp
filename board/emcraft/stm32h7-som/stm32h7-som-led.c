// SPDX-License-Identifier: GPL-2.0+
/*
 * Emcraft Systems, www.emcraft.com, (C) Copyright 2019
 */

#include <common.h>

DECLARE_GLOBAL_DATA_PTR;

#define PERIPH_BASE		((uint32_t)0x40000000) /* Base address of AHB/ABP */
#define D3_AHB1PERIPH_BASE	(PERIPH_BASE + 0x18020000)
#define GPIOF_BASE		(D3_AHB1PERIPH_BASE + 0x1400)
#define GPIOF			((GPIO_TypeDef *) GPIOF_BASE)

#define RCC_BASE		(D3_AHB1PERIPH_BASE + 0x4400)
#define RCC_AHB4ENR		(RCC_BASE +0xE0)

#define GPIO_MODE		((uint32_t)0x00000003)
#define ANALOG_MODE		((uint32_t)0x00000008)
#define EXTI_MODE		((uint32_t)0x10000000)
#define GPIO_MODE_IT		((uint32_t)0x00010000)
#define GPIO_MODE_EVT		((uint32_t)0x00020000)
#define RISING_EDGE		((uint32_t)0x00100000)
#define FALLING_EDGE		((uint32_t)0x00200000)
#define GPIO_OUTPUT_TYPE	((uint32_t)0x00000010)

/*
 * GPIO pins define
 */
#define GPIO_PIN_0		((uint16_t)0x0001U)  /* Pin 0 selected    */
#define GPIO_PIN_1		((uint16_t)0x0002U)  /* Pin 1 selected    */
#define GPIO_PIN_2		((uint16_t)0x0004U)  /* Pin 2 selected    */
#define GPIO_PIN_3		((uint16_t)0x0008U)  /* Pin 3 selected    */
#define GPIO_PIN_4		((uint16_t)0x0010U)  /* Pin 4 selected    */
#define GPIO_PIN_5		((uint16_t)0x0020U)  /* Pin 5 selected    */
#define GPIO_PIN_6		((uint16_t)0x0040U)  /* Pin 6 selected    */
#define GPIO_PIN_7		((uint16_t)0x0080U)  /* Pin 7 selected    */
#define GPIO_PIN_8		((uint16_t)0x0100U)  /* Pin 8 selected    */
#define GPIO_PIN_9		((uint16_t)0x0200U)  /* Pin 9 selected    */
#define GPIO_PIN_10		((uint16_t)0x0400U)  /* Pin 10 selected   */
#define GPIO_PIN_11		((uint16_t)0x0800U)  /* Pin 11 selected   */
#define GPIO_PIN_12		((uint16_t)0x1000U)  /* Pin 12 selected   */
#define GPIO_PIN_13		((uint16_t)0x2000U)  /* Pin 13 selected   */
#define GPIO_PIN_14		((uint16_t)0x4000U)  /* Pin 14 selected   */
#define GPIO_PIN_15		((uint16_t)0x8000U)  /* Pin 15 selected   */
#define GPIO_PIN_All		((uint16_t)0xFFFFU)  /* All pins selected */

/******************  Bits definition for GPIO_MODER register  *****************/
#define GPIO_MODER_MODER0_Pos            (0U)
#define GPIO_MODER_MODER0_Msk            (0x3U << GPIO_MODER_MODER0_Pos)       /*!< 0x00000003 */
#define GPIO_MODER_MODER0                GPIO_MODER_MODER0_Msk
#define GPIO_MODER_MODER0_0              (0x1U << GPIO_MODER_MODER0_Pos)       /*!< 0x00000001 */
#define GPIO_MODER_MODER0_1              (0x2U << GPIO_MODER_MODER0_Pos)       /*!< 0x00000002 */

#define GPIO_MODER_MODER1_Pos            (2U)
#define GPIO_MODER_MODER1_Msk            (0x3U << GPIO_MODER_MODER1_Pos)       /*!< 0x0000000C */
#define GPIO_MODER_MODER1                GPIO_MODER_MODER1_Msk
#define GPIO_MODER_MODER1_0              (0x1U << GPIO_MODER_MODER1_Pos)       /*!< 0x00000004 */
#define GPIO_MODER_MODER1_1              (0x2U << GPIO_MODER_MODER1_Pos)       /*!< 0x00000008 */

#define GPIO_MODER_MODER2_Pos            (4U)
#define GPIO_MODER_MODER2_Msk            (0x3U << GPIO_MODER_MODER2_Pos)       /*!< 0x00000030 */
#define GPIO_MODER_MODER2                GPIO_MODER_MODER2_Msk
#define GPIO_MODER_MODER2_0              (0x1U << GPIO_MODER_MODER2_Pos)       /*!< 0x00000010 */
#define GPIO_MODER_MODER2_1              (0x2U << GPIO_MODER_MODER2_Pos)       /*!< 0x00000020 */

#define GPIO_MODER_MODER3_Pos            (6U)
#define GPIO_MODER_MODER3_Msk            (0x3U << GPIO_MODER_MODER3_Pos)       /*!< 0x000000C0 */
#define GPIO_MODER_MODER3                GPIO_MODER_MODER3_Msk
#define GPIO_MODER_MODER3_0              (0x1U << GPIO_MODER_MODER3_Pos)       /*!< 0x00000040 */
#define GPIO_MODER_MODER3_1              (0x2U << GPIO_MODER_MODER3_Pos)       /*!< 0x00000080 */

#define GPIO_MODER_MODER4_Pos            (8U)
#define GPIO_MODER_MODER4_Msk            (0x3U << GPIO_MODER_MODER4_Pos)       /*!< 0x00000300 */
#define GPIO_MODER_MODER4                GPIO_MODER_MODER4_Msk
#define GPIO_MODER_MODER4_0              (0x1U << GPIO_MODER_MODER4_Pos)       /*!< 0x00000100 */
#define GPIO_MODER_MODER4_1              (0x2U << GPIO_MODER_MODER4_Pos)       /*!< 0x00000200 */

#define GPIO_MODER_MODER5_Pos            (10U)
#define GPIO_MODER_MODER5_Msk            (0x3U << GPIO_MODER_MODER5_Pos)       /*!< 0x00000C00 */
#define GPIO_MODER_MODER5                GPIO_MODER_MODER5_Msk
#define GPIO_MODER_MODER5_0              (0x1U << GPIO_MODER_MODER5_Pos)       /*!< 0x00000400 */
#define GPIO_MODER_MODER5_1              (0x2U << GPIO_MODER_MODER5_Pos)       /*!< 0x00000800 */

#define GPIO_MODER_MODER6_Pos            (12U)
#define GPIO_MODER_MODER6_Msk            (0x3U << GPIO_MODER_MODER6_Pos)       /*!< 0x00003000 */
#define GPIO_MODER_MODER6                GPIO_MODER_MODER6_Msk
#define GPIO_MODER_MODER6_0              (0x1U << GPIO_MODER_MODER6_Pos)       /*!< 0x00001000 */
#define GPIO_MODER_MODER6_1              (0x2U << GPIO_MODER_MODER6_Pos)       /*!< 0x00002000 */

#define GPIO_MODER_MODER7_Pos            (14U)
#define GPIO_MODER_MODER7_Msk            (0x3U << GPIO_MODER_MODER7_Pos)       /*!< 0x0000C000 */
#define GPIO_MODER_MODER7                GPIO_MODER_MODER7_Msk
#define GPIO_MODER_MODER7_0              (0x1U << GPIO_MODER_MODER7_Pos)       /*!< 0x00004000 */
#define GPIO_MODER_MODER7_1              (0x2U << GPIO_MODER_MODER7_Pos)       /*!< 0x00008000 */

#define GPIO_MODER_MODER8_Pos            (16U)
#define GPIO_MODER_MODER8_Msk            (0x3U << GPIO_MODER_MODER8_Pos)       /*!< 0x00030000 */
#define GPIO_MODER_MODER8                GPIO_MODER_MODER8_Msk
#define GPIO_MODER_MODER8_0              (0x1U << GPIO_MODER_MODER8_Pos)       /*!< 0x00010000 */
#define GPIO_MODER_MODER8_1              (0x2U << GPIO_MODER_MODER8_Pos)       /*!< 0x00020000 */

#define GPIO_MODER_MODER9_Pos            (18U)
#define GPIO_MODER_MODER9_Msk            (0x3U << GPIO_MODER_MODER9_Pos)       /*!< 0x000C0000 */
#define GPIO_MODER_MODER9                GPIO_MODER_MODER9_Msk
#define GPIO_MODER_MODER9_0              (0x1U << GPIO_MODER_MODER9_Pos)       /*!< 0x00040000 */
#define GPIO_MODER_MODER9_1              (0x2U << GPIO_MODER_MODER9_Pos)       /*!< 0x00080000 */

#define GPIO_MODER_MODER10_Pos           (20U)
#define GPIO_MODER_MODER10_Msk           (0x3U << GPIO_MODER_MODER10_Pos)      /*!< 0x00300000 */
#define GPIO_MODER_MODER10               GPIO_MODER_MODER10_Msk
#define GPIO_MODER_MODER10_0             (0x1U << GPIO_MODER_MODER10_Pos)      /*!< 0x00100000 */
#define GPIO_MODER_MODER10_1             (0x2U << GPIO_MODER_MODER10_Pos)      /*!< 0x00200000 */

#define GPIO_MODER_MODER11_Pos           (22U)
#define GPIO_MODER_MODER11_Msk           (0x3U << GPIO_MODER_MODER11_Pos)      /*!< 0x00C00000 */
#define GPIO_MODER_MODER11               GPIO_MODER_MODER11_Msk
#define GPIO_MODER_MODER11_0             (0x1U << GPIO_MODER_MODER11_Pos)      /*!< 0x00400000 */
#define GPIO_MODER_MODER11_1             (0x2U << GPIO_MODER_MODER11_Pos)      /*!< 0x00800000 */

#define GPIO_MODER_MODER12_Pos           (24U)
#define GPIO_MODER_MODER12_Msk           (0x3U << GPIO_MODER_MODER12_Pos)      /*!< 0x03000000 */
#define GPIO_MODER_MODER12               GPIO_MODER_MODER12_Msk
#define GPIO_MODER_MODER12_0             (0x1U << GPIO_MODER_MODER12_Pos)      /*!< 0x01000000 */
#define GPIO_MODER_MODER12_1             (0x2U << GPIO_MODER_MODER12_Pos)      /*!< 0x02000000 */

#define GPIO_MODER_MODER13_Pos           (26U)
#define GPIO_MODER_MODER13_Msk           (0x3U << GPIO_MODER_MODER13_Pos)      /*!< 0x0C000000 */
#define GPIO_MODER_MODER13               GPIO_MODER_MODER13_Msk
#define GPIO_MODER_MODER13_0             (0x1U << GPIO_MODER_MODER13_Pos)      /*!< 0x04000000 */
#define GPIO_MODER_MODER13_1             (0x2U << GPIO_MODER_MODER13_Pos)      /*!< 0x08000000 */

#define GPIO_MODER_MODER14_Pos           (28U)
#define GPIO_MODER_MODER14_Msk           (0x3U << GPIO_MODER_MODER14_Pos)      /*!< 0x30000000 */
#define GPIO_MODER_MODER14               GPIO_MODER_MODER14_Msk
#define GPIO_MODER_MODER14_0             (0x1U << GPIO_MODER_MODER14_Pos)      /*!< 0x10000000 */
#define GPIO_MODER_MODER14_1             (0x2U << GPIO_MODER_MODER14_Pos)      /*!< 0x20000000 */

#define GPIO_MODER_MODER15_Pos           (30U)
#define GPIO_MODER_MODER15_Msk           (0x3U << GPIO_MODER_MODER15_Pos)      /*!< 0xC0000000 */
#define GPIO_MODER_MODER15               GPIO_MODER_MODER15_Msk
#define GPIO_MODER_MODER15_0             (0x1U << GPIO_MODER_MODER15_Pos)      /*!< 0x40000000 */
#define GPIO_MODER_MODER15_1             (0x2U << GPIO_MODER_MODER15_Pos)      /*!< 0x80000000 */

/******************  Bits definition for GPIO_OTYPER register  ****************/
#define GPIO_OTYPER_OT_0                 ((uint32_t)0x00000001)
#define GPIO_OTYPER_OT_1                 ((uint32_t)0x00000002)
#define GPIO_OTYPER_OT_2                 ((uint32_t)0x00000004)
#define GPIO_OTYPER_OT_3                 ((uint32_t)0x00000008)
#define GPIO_OTYPER_OT_4                 ((uint32_t)0x00000010)
#define GPIO_OTYPER_OT_5                 ((uint32_t)0x00000020)
#define GPIO_OTYPER_OT_6                 ((uint32_t)0x00000040)
#define GPIO_OTYPER_OT_7                 ((uint32_t)0x00000080)
#define GPIO_OTYPER_OT_8                 ((uint32_t)0x00000100)
#define GPIO_OTYPER_OT_9                 ((uint32_t)0x00000200)
#define GPIO_OTYPER_OT_10                ((uint32_t)0x00000400)
#define GPIO_OTYPER_OT_11                ((uint32_t)0x00000800)
#define GPIO_OTYPER_OT_12                ((uint32_t)0x00001000)
#define GPIO_OTYPER_OT_13                ((uint32_t)0x00002000)
#define GPIO_OTYPER_OT_14                ((uint32_t)0x00004000)
#define GPIO_OTYPER_OT_15                ((uint32_t)0x00008000)

/******************  Bits definition for GPIO_OSPEEDR register  ***************/
#define GPIO_OSPEEDER_OSPEEDR0_Pos       (0U)
#define GPIO_OSPEEDER_OSPEEDR0_Msk       (0x3U << GPIO_OSPEEDER_OSPEEDR0_Pos)  /*!< 0x00000003 */
#define GPIO_OSPEEDER_OSPEEDR0           GPIO_OSPEEDER_OSPEEDR0_Msk
#define GPIO_OSPEEDER_OSPEEDR0_0         (0x1U << GPIO_OSPEEDER_OSPEEDR0_Pos)  /*!< 0x00000001 */
#define GPIO_OSPEEDER_OSPEEDR0_1         (0x2U << GPIO_OSPEEDER_OSPEEDR0_Pos)  /*!< 0x00000002 */

#define GPIO_OSPEEDER_OSPEEDR1_Pos       (2U)
#define GPIO_OSPEEDER_OSPEEDR1_Msk       (0x3U << GPIO_OSPEEDER_OSPEEDR1_Pos)  /*!< 0x0000000C */
#define GPIO_OSPEEDER_OSPEEDR1           GPIO_OSPEEDER_OSPEEDR1_Msk
#define GPIO_OSPEEDER_OSPEEDR1_0         (0x1U << GPIO_OSPEEDER_OSPEEDR1_Pos)  /*!< 0x00000004 */
#define GPIO_OSPEEDER_OSPEEDR1_1         (0x2U << GPIO_OSPEEDER_OSPEEDR1_Pos)  /*!< 0x00000008 */

#define GPIO_OSPEEDER_OSPEEDR2_Pos       (4U)
#define GPIO_OSPEEDER_OSPEEDR2_Msk       (0x3U << GPIO_OSPEEDER_OSPEEDR2_Pos)  /*!< 0x00000030 */
#define GPIO_OSPEEDER_OSPEEDR2           GPIO_OSPEEDER_OSPEEDR2_Msk
#define GPIO_OSPEEDER_OSPEEDR2_0         (0x1U << GPIO_OSPEEDER_OSPEEDR2_Pos)  /*!< 0x00000010 */
#define GPIO_OSPEEDER_OSPEEDR2_1         (0x2U << GPIO_OSPEEDER_OSPEEDR2_Pos)  /*!< 0x00000020 */

#define GPIO_OSPEEDER_OSPEEDR3_Pos       (6U)
#define GPIO_OSPEEDER_OSPEEDR3_Msk       (0x3U << GPIO_OSPEEDER_OSPEEDR3_Pos)  /*!< 0x000000C0 */
#define GPIO_OSPEEDER_OSPEEDR3           GPIO_OSPEEDER_OSPEEDR3_Msk
#define GPIO_OSPEEDER_OSPEEDR3_0         (0x1U << GPIO_OSPEEDER_OSPEEDR3_Pos)  /*!< 0x00000040 */
#define GPIO_OSPEEDER_OSPEEDR3_1         (0x2U << GPIO_OSPEEDER_OSPEEDR3_Pos)  /*!< 0x00000080 */

#define GPIO_OSPEEDER_OSPEEDR4_Pos       (8U)
#define GPIO_OSPEEDER_OSPEEDR4_Msk       (0x3U << GPIO_OSPEEDER_OSPEEDR4_Pos)  /*!< 0x00000300 */
#define GPIO_OSPEEDER_OSPEEDR4           GPIO_OSPEEDER_OSPEEDR4_Msk
#define GPIO_OSPEEDER_OSPEEDR4_0         (0x1U << GPIO_OSPEEDER_OSPEEDR4_Pos)  /*!< 0x00000100 */
#define GPIO_OSPEEDER_OSPEEDR4_1         (0x2U << GPIO_OSPEEDER_OSPEEDR4_Pos)  /*!< 0x00000200 */

#define GPIO_OSPEEDER_OSPEEDR5_Pos       (10U)
#define GPIO_OSPEEDER_OSPEEDR5_Msk       (0x3U << GPIO_OSPEEDER_OSPEEDR5_Pos)  /*!< 0x00000C00 */
#define GPIO_OSPEEDER_OSPEEDR5           GPIO_OSPEEDER_OSPEEDR5_Msk
#define GPIO_OSPEEDER_OSPEEDR5_0         (0x1U << GPIO_OSPEEDER_OSPEEDR5_Pos)  /*!< 0x00000400 */
#define GPIO_OSPEEDER_OSPEEDR5_1         (0x2U << GPIO_OSPEEDER_OSPEEDR5_Pos)  /*!< 0x00000800 */

#define GPIO_OSPEEDER_OSPEEDR6_Pos       (12U)
#define GPIO_OSPEEDER_OSPEEDR6_Msk       (0x3U << GPIO_OSPEEDER_OSPEEDR6_Pos)  /*!< 0x00003000 */
#define GPIO_OSPEEDER_OSPEEDR6           GPIO_OSPEEDER_OSPEEDR6_Msk
#define GPIO_OSPEEDER_OSPEEDR6_0         (0x1U << GPIO_OSPEEDER_OSPEEDR6_Pos)  /*!< 0x00001000 */
#define GPIO_OSPEEDER_OSPEEDR6_1         (0x2U << GPIO_OSPEEDER_OSPEEDR6_Pos)  /*!< 0x00002000 */

#define GPIO_OSPEEDER_OSPEEDR7_Pos       (14U)
#define GPIO_OSPEEDER_OSPEEDR7_Msk       (0x3U << GPIO_OSPEEDER_OSPEEDR7_Pos)  /*!< 0x0000C000 */
#define GPIO_OSPEEDER_OSPEEDR7           GPIO_OSPEEDER_OSPEEDR7_Msk
#define GPIO_OSPEEDER_OSPEEDR7_0         (0x1U << GPIO_OSPEEDER_OSPEEDR7_Pos)  /*!< 0x00004000 */
#define GPIO_OSPEEDER_OSPEEDR7_1         (0x2U << GPIO_OSPEEDER_OSPEEDR7_Pos)  /*!< 0x00008000 */

#define GPIO_OSPEEDER_OSPEEDR8_Pos       (16U)
#define GPIO_OSPEEDER_OSPEEDR8_Msk       (0x3U << GPIO_OSPEEDER_OSPEEDR8_Pos)  /*!< 0x00030000 */
#define GPIO_OSPEEDER_OSPEEDR8           GPIO_OSPEEDER_OSPEEDR8_Msk
#define GPIO_OSPEEDER_OSPEEDR8_0         (0x1U << GPIO_OSPEEDER_OSPEEDR8_Pos)  /*!< 0x00010000 */
#define GPIO_OSPEEDER_OSPEEDR8_1         (0x2U << GPIO_OSPEEDER_OSPEEDR8_Pos)  /*!< 0x00020000 */

#define GPIO_OSPEEDER_OSPEEDR9_Pos       (18U)
#define GPIO_OSPEEDER_OSPEEDR9_Msk       (0x3U << GPIO_OSPEEDER_OSPEEDR9_Pos)  /*!< 0x000C0000 */
#define GPIO_OSPEEDER_OSPEEDR9           GPIO_OSPEEDER_OSPEEDR9_Msk
#define GPIO_OSPEEDER_OSPEEDR9_0         (0x1U << GPIO_OSPEEDER_OSPEEDR9_Pos)  /*!< 0x00040000 */
#define GPIO_OSPEEDER_OSPEEDR9_1         (0x2U << GPIO_OSPEEDER_OSPEEDR9_Pos)  /*!< 0x00080000 */

#define GPIO_OSPEEDER_OSPEEDR10_Pos      (20U)
#define GPIO_OSPEEDER_OSPEEDR10_Msk      (0x3U << GPIO_OSPEEDER_OSPEEDR10_Pos) /*!< 0x00300000 */
#define GPIO_OSPEEDER_OSPEEDR10          GPIO_OSPEEDER_OSPEEDR10_Msk
#define GPIO_OSPEEDER_OSPEEDR10_0        (0x1U << GPIO_OSPEEDER_OSPEEDR10_Pos) /*!< 0x00100000 */
#define GPIO_OSPEEDER_OSPEEDR10_1        (0x2U << GPIO_OSPEEDER_OSPEEDR10_Pos) /*!< 0x00200000 */

#define GPIO_OSPEEDER_OSPEEDR11_Pos      (22U)
#define GPIO_OSPEEDER_OSPEEDR11_Msk      (0x3U << GPIO_OSPEEDER_OSPEEDR11_Pos) /*!< 0x00C00000 */
#define GPIO_OSPEEDER_OSPEEDR11          GPIO_OSPEEDER_OSPEEDR11_Msk
#define GPIO_OSPEEDER_OSPEEDR11_0        (0x1U << GPIO_OSPEEDER_OSPEEDR11_Pos) /*!< 0x00400000 */
#define GPIO_OSPEEDER_OSPEEDR11_1        (0x2U << GPIO_OSPEEDER_OSPEEDR11_Pos) /*!< 0x00800000 */

#define GPIO_OSPEEDER_OSPEEDR12_Pos      (24U)
#define GPIO_OSPEEDER_OSPEEDR12_Msk      (0x3U << GPIO_OSPEEDER_OSPEEDR12_Pos) /*!< 0x03000000 */
#define GPIO_OSPEEDER_OSPEEDR12          GPIO_OSPEEDER_OSPEEDR12_Msk
#define GPIO_OSPEEDER_OSPEEDR12_0        (0x1U << GPIO_OSPEEDER_OSPEEDR12_Pos) /*!< 0x01000000 */
#define GPIO_OSPEEDER_OSPEEDR12_1        (0x2U << GPIO_OSPEEDER_OSPEEDR12_Pos) /*!< 0x02000000 */

#define GPIO_OSPEEDER_OSPEEDR13_Pos      (26U)
#define GPIO_OSPEEDER_OSPEEDR13_Msk      (0x3U << GPIO_OSPEEDER_OSPEEDR13_Pos) /*!< 0x0C000000 */
#define GPIO_OSPEEDER_OSPEEDR13          GPIO_OSPEEDER_OSPEEDR13_Msk
#define GPIO_OSPEEDER_OSPEEDR13_0        (0x1U << GPIO_OSPEEDER_OSPEEDR13_Pos) /*!< 0x04000000 */
#define GPIO_OSPEEDER_OSPEEDR13_1        (0x2U << GPIO_OSPEEDER_OSPEEDR13_Pos) /*!< 0x08000000 */

#define GPIO_OSPEEDER_OSPEEDR14_Pos      (28U)
#define GPIO_OSPEEDER_OSPEEDR14_Msk      (0x3U << GPIO_OSPEEDER_OSPEEDR14_Pos) /*!< 0x30000000 */
#define GPIO_OSPEEDER_OSPEEDR14          GPIO_OSPEEDER_OSPEEDR14_Msk
#define GPIO_OSPEEDER_OSPEEDR14_0        (0x1U << GPIO_OSPEEDER_OSPEEDR14_Pos) /*!< 0x10000000 */
#define GPIO_OSPEEDER_OSPEEDR14_1        (0x2U << GPIO_OSPEEDER_OSPEEDR14_Pos) /*!< 0x20000000 */

#define GPIO_OSPEEDER_OSPEEDR15_Pos      (30U)
#define GPIO_OSPEEDER_OSPEEDR15_Msk      (0x3U << GPIO_OSPEEDER_OSPEEDR15_Pos) /*!< 0xC0000000 */
#define GPIO_OSPEEDER_OSPEEDR15          GPIO_OSPEEDER_OSPEEDR15_Msk
#define GPIO_OSPEEDER_OSPEEDR15_0        (0x1U << GPIO_OSPEEDER_OSPEEDR15_Pos) /*!< 0x40000000 */
#define GPIO_OSPEEDER_OSPEEDR15_1        (0x2U << GPIO_OSPEEDER_OSPEEDR15_Pos) /*!< 0x80000000 */

/******************  Bits definition for GPIO_PUPDR register  *****************/
#define GPIO_PUPDR_PUPDR0_Pos            (0U)
#define GPIO_PUPDR_PUPDR0_Msk            (0x3U << GPIO_PUPDR_PUPDR0_Pos)       /*!< 0x00000003 */
#define GPIO_PUPDR_PUPDR0                GPIO_PUPDR_PUPDR0_Msk
#define GPIO_PUPDR_PUPDR0_0              (0x1U << GPIO_PUPDR_PUPDR0_Pos)       /*!< 0x00000001 */
#define GPIO_PUPDR_PUPDR0_1              (0x2U << GPIO_PUPDR_PUPDR0_Pos)       /*!< 0x00000002 */

#define GPIO_PUPDR_PUPDR1_Pos            (2U)
#define GPIO_PUPDR_PUPDR1_Msk            (0x3U << GPIO_PUPDR_PUPDR1_Pos)       /*!< 0x0000000C */
#define GPIO_PUPDR_PUPDR1                GPIO_PUPDR_PUPDR1_Msk
#define GPIO_PUPDR_PUPDR1_0              (0x1U << GPIO_PUPDR_PUPDR1_Pos)       /*!< 0x00000004 */
#define GPIO_PUPDR_PUPDR1_1              (0x2U << GPIO_PUPDR_PUPDR1_Pos)       /*!< 0x00000008 */

#define GPIO_PUPDR_PUPDR2_Pos            (4U)
#define GPIO_PUPDR_PUPDR2_Msk            (0x3U << GPIO_PUPDR_PUPDR2_Pos)       /*!< 0x00000030 */
#define GPIO_PUPDR_PUPDR2                GPIO_PUPDR_PUPDR2_Msk
#define GPIO_PUPDR_PUPDR2_0              (0x1U << GPIO_PUPDR_PUPDR2_Pos)       /*!< 0x00000010 */
#define GPIO_PUPDR_PUPDR2_1              (0x2U << GPIO_PUPDR_PUPDR2_Pos)       /*!< 0x00000020 */

#define GPIO_PUPDR_PUPDR3_Pos            (6U)
#define GPIO_PUPDR_PUPDR3_Msk            (0x3U << GPIO_PUPDR_PUPDR3_Pos)       /*!< 0x000000C0 */
#define GPIO_PUPDR_PUPDR3                GPIO_PUPDR_PUPDR3_Msk
#define GPIO_PUPDR_PUPDR3_0              (0x1U << GPIO_PUPDR_PUPDR3_Pos)       /*!< 0x00000040 */
#define GPIO_PUPDR_PUPDR3_1              (0x2U << GPIO_PUPDR_PUPDR3_Pos)       /*!< 0x00000080 */

#define GPIO_PUPDR_PUPDR4_Pos            (8U)
#define GPIO_PUPDR_PUPDR4_Msk            (0x3U << GPIO_PUPDR_PUPDR4_Pos)       /*!< 0x00000300 */
#define GPIO_PUPDR_PUPDR4                GPIO_PUPDR_PUPDR4_Msk
#define GPIO_PUPDR_PUPDR4_0              (0x1U << GPIO_PUPDR_PUPDR4_Pos)       /*!< 0x00000100 */
#define GPIO_PUPDR_PUPDR4_1              (0x2U << GPIO_PUPDR_PUPDR4_Pos)       /*!< 0x00000200 */

#define GPIO_PUPDR_PUPDR5_Pos            (10U)
#define GPIO_PUPDR_PUPDR5_Msk            (0x3U << GPIO_PUPDR_PUPDR5_Pos)       /*!< 0x00000C00 */
#define GPIO_PUPDR_PUPDR5                GPIO_PUPDR_PUPDR5_Msk
#define GPIO_PUPDR_PUPDR5_0              (0x1U << GPIO_PUPDR_PUPDR5_Pos)       /*!< 0x00000400 */
#define GPIO_PUPDR_PUPDR5_1              (0x2U << GPIO_PUPDR_PUPDR5_Pos)       /*!< 0x00000800 */

#define GPIO_PUPDR_PUPDR6_Pos            (12U)
#define GPIO_PUPDR_PUPDR6_Msk            (0x3U << GPIO_PUPDR_PUPDR6_Pos)       /*!< 0x00003000 */
#define GPIO_PUPDR_PUPDR6                GPIO_PUPDR_PUPDR6_Msk
#define GPIO_PUPDR_PUPDR6_0              (0x1U << GPIO_PUPDR_PUPDR6_Pos)       /*!< 0x00001000 */
#define GPIO_PUPDR_PUPDR6_1              (0x2U << GPIO_PUPDR_PUPDR6_Pos)       /*!< 0x00002000 */

#define GPIO_PUPDR_PUPDR7_Pos            (14U)
#define GPIO_PUPDR_PUPDR7_Msk            (0x3U << GPIO_PUPDR_PUPDR7_Pos)       /*!< 0x0000C000 */
#define GPIO_PUPDR_PUPDR7                GPIO_PUPDR_PUPDR7_Msk
#define GPIO_PUPDR_PUPDR7_0              (0x1U << GPIO_PUPDR_PUPDR7_Pos)       /*!< 0x00004000 */
#define GPIO_PUPDR_PUPDR7_1              (0x2U << GPIO_PUPDR_PUPDR7_Pos)       /*!< 0x00008000 */

#define GPIO_PUPDR_PUPDR8_Pos            (16U)
#define GPIO_PUPDR_PUPDR8_Msk            (0x3U << GPIO_PUPDR_PUPDR8_Pos)       /*!< 0x00030000 */
#define GPIO_PUPDR_PUPDR8                GPIO_PUPDR_PUPDR8_Msk
#define GPIO_PUPDR_PUPDR8_0              (0x1U << GPIO_PUPDR_PUPDR8_Pos)       /*!< 0x00010000 */
#define GPIO_PUPDR_PUPDR8_1              (0x2U << GPIO_PUPDR_PUPDR8_Pos)       /*!< 0x00020000 */

#define GPIO_PUPDR_PUPDR9_Pos            (18U)
#define GPIO_PUPDR_PUPDR9_Msk            (0x3U << GPIO_PUPDR_PUPDR9_Pos)       /*!< 0x000C0000 */
#define GPIO_PUPDR_PUPDR9                GPIO_PUPDR_PUPDR9_Msk
#define GPIO_PUPDR_PUPDR9_0              (0x1U << GPIO_PUPDR_PUPDR9_Pos)       /*!< 0x00040000 */
#define GPIO_PUPDR_PUPDR9_1              (0x2U << GPIO_PUPDR_PUPDR9_Pos)       /*!< 0x00080000 */

#define GPIO_PUPDR_PUPDR10_Pos           (20U)
#define GPIO_PUPDR_PUPDR10_Msk           (0x3U << GPIO_PUPDR_PUPDR10_Pos)      /*!< 0x00300000 */
#define GPIO_PUPDR_PUPDR10               GPIO_PUPDR_PUPDR10_Msk
#define GPIO_PUPDR_PUPDR10_0             (0x1U << GPIO_PUPDR_PUPDR10_Pos)      /*!< 0x00100000 */
#define GPIO_PUPDR_PUPDR10_1             (0x2U << GPIO_PUPDR_PUPDR10_Pos)      /*!< 0x00200000 */

#define GPIO_PUPDR_PUPDR11_Pos           (22U)
#define GPIO_PUPDR_PUPDR11_Msk           (0x3U << GPIO_PUPDR_PUPDR11_Pos)      /*!< 0x00C00000 */
#define GPIO_PUPDR_PUPDR11               GPIO_PUPDR_PUPDR11_Msk
#define GPIO_PUPDR_PUPDR11_0             (0x1U << GPIO_PUPDR_PUPDR11_Pos)      /*!< 0x00400000 */
#define GPIO_PUPDR_PUPDR11_1             (0x2U << GPIO_PUPDR_PUPDR11_Pos)      /*!< 0x00800000 */

#define GPIO_PUPDR_PUPDR12_Pos           (24U)
#define GPIO_PUPDR_PUPDR12_Msk           (0x3U << GPIO_PUPDR_PUPDR12_Pos)      /*!< 0x03000000 */
#define GPIO_PUPDR_PUPDR12               GPIO_PUPDR_PUPDR12_Msk
#define GPIO_PUPDR_PUPDR12_0             (0x1U << GPIO_PUPDR_PUPDR12_Pos)      /*!< 0x01000000 */
#define GPIO_PUPDR_PUPDR12_1             (0x2U << GPIO_PUPDR_PUPDR12_Pos)      /*!< 0x02000000 */

#define GPIO_PUPDR_PUPDR13_Pos           (26U)
#define GPIO_PUPDR_PUPDR13_Msk           (0x3U << GPIO_PUPDR_PUPDR13_Pos)      /*!< 0x0C000000 */
#define GPIO_PUPDR_PUPDR13               GPIO_PUPDR_PUPDR13_Msk
#define GPIO_PUPDR_PUPDR13_0             (0x1U << GPIO_PUPDR_PUPDR13_Pos)      /*!< 0x04000000 */
#define GPIO_PUPDR_PUPDR13_1             (0x2U << GPIO_PUPDR_PUPDR13_Pos)      /*!< 0x08000000 */

#define GPIO_PUPDR_PUPDR14_Pos           (28U)
#define GPIO_PUPDR_PUPDR14_Msk           (0x3U << GPIO_PUPDR_PUPDR14_Pos)      /*!< 0x30000000 */
#define GPIO_PUPDR_PUPDR14               GPIO_PUPDR_PUPDR14_Msk
#define GPIO_PUPDR_PUPDR14_0             (0x1U << GPIO_PUPDR_PUPDR14_Pos)      /*!< 0x10000000 */
#define GPIO_PUPDR_PUPDR14_1             (0x2U << GPIO_PUPDR_PUPDR14_Pos)      /*!< 0x20000000 */

#define GPIO_PUPDR_PUPDR15_Pos           (30U)
#define GPIO_PUPDR_PUPDR15_Msk           (0x3U << GPIO_PUPDR_PUPDR15_Pos)      /*!< 0xC0000000 */
#define GPIO_PUPDR_PUPDR15               GPIO_PUPDR_PUPDR15_Msk
#define GPIO_PUPDR_PUPDR15_0             (0x1U << GPIO_PUPDR_PUPDR15_Pos)      /*!< 0x40000000 */
#define GPIO_PUPDR_PUPDR15_1             (0x2U << GPIO_PUPDR_PUPDR15_Pos)      /*!< 0x80000000 */

/******************  Bits definition for GPIO_IDR register  *******************/
#define GPIO_IDR_IDR_0                   ((uint32_t)0x00000001)
#define GPIO_IDR_IDR_1                   ((uint32_t)0x00000002)
#define GPIO_IDR_IDR_2                   ((uint32_t)0x00000004)
#define GPIO_IDR_IDR_3                   ((uint32_t)0x00000008)
#define GPIO_IDR_IDR_4                   ((uint32_t)0x00000010)
#define GPIO_IDR_IDR_5                   ((uint32_t)0x00000020)
#define GPIO_IDR_IDR_6                   ((uint32_t)0x00000040)
#define GPIO_IDR_IDR_7                   ((uint32_t)0x00000080)
#define GPIO_IDR_IDR_8                   ((uint32_t)0x00000100)
#define GPIO_IDR_IDR_9                   ((uint32_t)0x00000200)
#define GPIO_IDR_IDR_10                  ((uint32_t)0x00000400)
#define GPIO_IDR_IDR_11                  ((uint32_t)0x00000800)
#define GPIO_IDR_IDR_12                  ((uint32_t)0x00001000)
#define GPIO_IDR_IDR_13                  ((uint32_t)0x00002000)
#define GPIO_IDR_IDR_14                  ((uint32_t)0x00004000)
#define GPIO_IDR_IDR_15                  ((uint32_t)0x00008000)
/* Old GPIO_IDR register bits definition, maintained for legacy purpose */
#define GPIO_OTYPER_IDR_0                    GPIO_IDR_IDR_0
#define GPIO_OTYPER_IDR_1                    GPIO_IDR_IDR_1
#define GPIO_OTYPER_IDR_2                    GPIO_IDR_IDR_2
#define GPIO_OTYPER_IDR_3                    GPIO_IDR_IDR_3
#define GPIO_OTYPER_IDR_4                    GPIO_IDR_IDR_4
#define GPIO_OTYPER_IDR_5                    GPIO_IDR_IDR_5
#define GPIO_OTYPER_IDR_6                    GPIO_IDR_IDR_6
#define GPIO_OTYPER_IDR_7                    GPIO_IDR_IDR_7
#define GPIO_OTYPER_IDR_8                    GPIO_IDR_IDR_8
#define GPIO_OTYPER_IDR_9                    GPIO_IDR_IDR_9
#define GPIO_OTYPER_IDR_10                   GPIO_IDR_IDR_10
#define GPIO_OTYPER_IDR_11                   GPIO_IDR_IDR_11
#define GPIO_OTYPER_IDR_12                   GPIO_IDR_IDR_12
#define GPIO_OTYPER_IDR_13                   GPIO_IDR_IDR_13
#define GPIO_OTYPER_IDR_14                   GPIO_IDR_IDR_14
#define GPIO_OTYPER_IDR_15                   GPIO_IDR_IDR_15

/******************  Bits definition for GPIO_ODR register  *******************/
#define GPIO_ODR_ODR_0                   ((uint32_t)0x00000001)
#define GPIO_ODR_ODR_1                   ((uint32_t)0x00000002)
#define GPIO_ODR_ODR_2                   ((uint32_t)0x00000004)
#define GPIO_ODR_ODR_3                   ((uint32_t)0x00000008)
#define GPIO_ODR_ODR_4                   ((uint32_t)0x00000010)
#define GPIO_ODR_ODR_5                   ((uint32_t)0x00000020)
#define GPIO_ODR_ODR_6                   ((uint32_t)0x00000040)
#define GPIO_ODR_ODR_7                   ((uint32_t)0x00000080)
#define GPIO_ODR_ODR_8                   ((uint32_t)0x00000100)
#define GPIO_ODR_ODR_9                   ((uint32_t)0x00000200)
#define GPIO_ODR_ODR_10                  ((uint32_t)0x00000400)
#define GPIO_ODR_ODR_11                  ((uint32_t)0x00000800)
#define GPIO_ODR_ODR_12                  ((uint32_t)0x00001000)
#define GPIO_ODR_ODR_13                  ((uint32_t)0x00002000)
#define GPIO_ODR_ODR_14                  ((uint32_t)0x00004000)
#define GPIO_ODR_ODR_15                  ((uint32_t)0x00008000)
/* Old GPIO_ODR register bits definition, maintained for legacy purpose */
#define GPIO_OTYPER_ODR_0                    GPIO_ODR_ODR_0
#define GPIO_OTYPER_ODR_1                    GPIO_ODR_ODR_1
#define GPIO_OTYPER_ODR_2                    GPIO_ODR_ODR_2
#define GPIO_OTYPER_ODR_3                    GPIO_ODR_ODR_3
#define GPIO_OTYPER_ODR_4                    GPIO_ODR_ODR_4
#define GPIO_OTYPER_ODR_5                    GPIO_ODR_ODR_5
#define GPIO_OTYPER_ODR_6                    GPIO_ODR_ODR_6
#define GPIO_OTYPER_ODR_7                    GPIO_ODR_ODR_7
#define GPIO_OTYPER_ODR_8                    GPIO_ODR_ODR_8
#define GPIO_OTYPER_ODR_9                    GPIO_ODR_ODR_9
#define GPIO_OTYPER_ODR_10                   GPIO_ODR_ODR_10
#define GPIO_OTYPER_ODR_11                   GPIO_ODR_ODR_11
#define GPIO_OTYPER_ODR_12                   GPIO_ODR_ODR_12
#define GPIO_OTYPER_ODR_13                   GPIO_ODR_ODR_13
#define GPIO_OTYPER_ODR_14                   GPIO_ODR_ODR_14
#define GPIO_OTYPER_ODR_15                   GPIO_ODR_ODR_15

/******************  Bits definition for GPIO_BSRR register  ******************/
#define GPIO_BSRR_BS_0                   ((uint32_t)0x00000001)
#define GPIO_BSRR_BS_1                   ((uint32_t)0x00000002)
#define GPIO_BSRR_BS_2                   ((uint32_t)0x00000004)
#define GPIO_BSRR_BS_3                   ((uint32_t)0x00000008)
#define GPIO_BSRR_BS_4                   ((uint32_t)0x00000010)
#define GPIO_BSRR_BS_5                   ((uint32_t)0x00000020)
#define GPIO_BSRR_BS_6                   ((uint32_t)0x00000040)
#define GPIO_BSRR_BS_7                   ((uint32_t)0x00000080)
#define GPIO_BSRR_BS_8                   ((uint32_t)0x00000100)
#define GPIO_BSRR_BS_9                   ((uint32_t)0x00000200)
#define GPIO_BSRR_BS_10                  ((uint32_t)0x00000400)
#define GPIO_BSRR_BS_11                  ((uint32_t)0x00000800)
#define GPIO_BSRR_BS_12                  ((uint32_t)0x00001000)
#define GPIO_BSRR_BS_13                  ((uint32_t)0x00002000)
#define GPIO_BSRR_BS_14                  ((uint32_t)0x00004000)
#define GPIO_BSRR_BS_15                  ((uint32_t)0x00008000)
#define GPIO_BSRR_BR_0                   ((uint32_t)0x00010000)
#define GPIO_BSRR_BR_1                   ((uint32_t)0x00020000)
#define GPIO_BSRR_BR_2                   ((uint32_t)0x00040000)
#define GPIO_BSRR_BR_3                   ((uint32_t)0x00080000)
#define GPIO_BSRR_BR_4                   ((uint32_t)0x00100000)
#define GPIO_BSRR_BR_5                   ((uint32_t)0x00200000)
#define GPIO_BSRR_BR_6                   ((uint32_t)0x00400000)
#define GPIO_BSRR_BR_7                   ((uint32_t)0x00800000)
#define GPIO_BSRR_BR_8                   ((uint32_t)0x01000000)
#define GPIO_BSRR_BR_9                   ((uint32_t)0x02000000)
#define GPIO_BSRR_BR_10                  ((uint32_t)0x04000000)
#define GPIO_BSRR_BR_11                  ((uint32_t)0x08000000)
#define GPIO_BSRR_BR_12                  ((uint32_t)0x10000000)
#define GPIO_BSRR_BR_13                  ((uint32_t)0x20000000)
#define GPIO_BSRR_BR_14                  ((uint32_t)0x40000000)
#define GPIO_BSRR_BR_15                  ((uint32_t)0x80000000)

/****************** Bit definition for GPIO_LCKR register *********************/
#define GPIO_LCKR_LCK0_Pos               (0U)
#define GPIO_LCKR_LCK0_Msk               (0x1U << GPIO_LCKR_LCK0_Pos)          /*!< 0x00000001 */
#define GPIO_LCKR_LCK0                   GPIO_LCKR_LCK0_Msk
#define GPIO_LCKR_LCK1_Pos               (1U)
#define GPIO_LCKR_LCK1_Msk               (0x1U << GPIO_LCKR_LCK1_Pos)          /*!< 0x00000002 */
#define GPIO_LCKR_LCK1                   GPIO_LCKR_LCK1_Msk
#define GPIO_LCKR_LCK2_Pos               (2U)
#define GPIO_LCKR_LCK2_Msk               (0x1U << GPIO_LCKR_LCK2_Pos)          /*!< 0x00000004 */
#define GPIO_LCKR_LCK2                   GPIO_LCKR_LCK2_Msk
#define GPIO_LCKR_LCK3_Pos               (3U)
#define GPIO_LCKR_LCK3_Msk               (0x1U << GPIO_LCKR_LCK3_Pos)          /*!< 0x00000008 */
#define GPIO_LCKR_LCK3                   GPIO_LCKR_LCK3_Msk
#define GPIO_LCKR_LCK4_Pos               (4U)
#define GPIO_LCKR_LCK4_Msk               (0x1U << GPIO_LCKR_LCK4_Pos)          /*!< 0x00000010 */
#define GPIO_LCKR_LCK4                   GPIO_LCKR_LCK4_Msk
#define GPIO_LCKR_LCK5_Pos               (5U)
#define GPIO_LCKR_LCK5_Msk               (0x1U << GPIO_LCKR_LCK5_Pos)          /*!< 0x00000020 */
#define GPIO_LCKR_LCK5                   GPIO_LCKR_LCK5_Msk
#define GPIO_LCKR_LCK6_Pos               (6U)
#define GPIO_LCKR_LCK6_Msk               (0x1U << GPIO_LCKR_LCK6_Pos)          /*!< 0x00000040 */
#define GPIO_LCKR_LCK6                   GPIO_LCKR_LCK6_Msk
#define GPIO_LCKR_LCK7_Pos               (7U)
#define GPIO_LCKR_LCK7_Msk               (0x1U << GPIO_LCKR_LCK7_Pos)          /*!< 0x00000080 */
#define GPIO_LCKR_LCK7                   GPIO_LCKR_LCK7_Msk
#define GPIO_LCKR_LCK8_Pos               (8U)
#define GPIO_LCKR_LCK8_Msk               (0x1U << GPIO_LCKR_LCK8_Pos)          /*!< 0x00000100 */
#define GPIO_LCKR_LCK8                   GPIO_LCKR_LCK8_Msk
#define GPIO_LCKR_LCK9_Pos               (9U)
#define GPIO_LCKR_LCK9_Msk               (0x1U << GPIO_LCKR_LCK9_Pos)          /*!< 0x00000200 */
#define GPIO_LCKR_LCK9                   GPIO_LCKR_LCK9_Msk
#define GPIO_LCKR_LCK10_Pos              (10U)
#define GPIO_LCKR_LCK10_Msk              (0x1U << GPIO_LCKR_LCK10_Pos)         /*!< 0x00000400 */
#define GPIO_LCKR_LCK10                  GPIO_LCKR_LCK10_Msk
#define GPIO_LCKR_LCK11_Pos              (11U)
#define GPIO_LCKR_LCK11_Msk              (0x1U << GPIO_LCKR_LCK11_Pos)         /*!< 0x00000800 */
#define GPIO_LCKR_LCK11                  GPIO_LCKR_LCK11_Msk
#define GPIO_LCKR_LCK12_Pos              (12U)
#define GPIO_LCKR_LCK12_Msk              (0x1U << GPIO_LCKR_LCK12_Pos)         /*!< 0x00001000 */
#define GPIO_LCKR_LCK12                  GPIO_LCKR_LCK12_Msk
#define GPIO_LCKR_LCK13_Pos              (13U)
#define GPIO_LCKR_LCK13_Msk              (0x1U << GPIO_LCKR_LCK13_Pos)         /*!< 0x00002000 */
#define GPIO_LCKR_LCK13                  GPIO_LCKR_LCK13_Msk
#define GPIO_LCKR_LCK14_Pos              (14U)
#define GPIO_LCKR_LCK14_Msk              (0x1U << GPIO_LCKR_LCK14_Pos)         /*!< 0x00004000 */
#define GPIO_LCKR_LCK14                  GPIO_LCKR_LCK14_Msk
#define GPIO_LCKR_LCK15_Pos              (15U)
#define GPIO_LCKR_LCK15_Msk              (0x1U << GPIO_LCKR_LCK15_Pos)         /*!< 0x00008000 */
#define GPIO_LCKR_LCK15                  GPIO_LCKR_LCK15_Msk
#define GPIO_LCKR_LCKK_Pos               (16U)
#define GPIO_LCKR_LCKK_Msk               (0x1U << GPIO_LCKR_LCKK_Pos)          /*!< 0x00010000 */
#define GPIO_LCKR_LCKK                   GPIO_LCKR_LCKK_Msk

/****************** Bit definition for GPIO_AFRL register  ********************/
#define GPIO_AFRL_AFRL0_Pos              (0U)
#define GPIO_AFRL_AFRL0_Msk              (0xFU << GPIO_AFRL_AFRL0_Pos)         /*!< 0x0000000F */
#define GPIO_AFRL_AFRL0                  GPIO_AFRL_AFRL0_Msk
#define GPIO_AFRL_AFRL1_Pos              (4U)
#define GPIO_AFRL_AFRL1_Msk              (0xFU << GPIO_AFRL_AFRL1_Pos)         /*!< 0x000000F0 */
#define GPIO_AFRL_AFRL1                  GPIO_AFRL_AFRL1_Msk
#define GPIO_AFRL_AFRL2_Pos              (8U)
#define GPIO_AFRL_AFRL2_Msk              (0xFU << GPIO_AFRL_AFRL2_Pos)         /*!< 0x00000F00 */
#define GPIO_AFRL_AFRL2                  GPIO_AFRL_AFRL2_Msk
#define GPIO_AFRL_AFRL3_Pos              (12U)
#define GPIO_AFRL_AFRL3_Msk              (0xFU << GPIO_AFRL_AFRL3_Pos)         /*!< 0x0000F000 */
#define GPIO_AFRL_AFRL3                  GPIO_AFRL_AFRL3_Msk
#define GPIO_AFRL_AFRL4_Pos              (16U)
#define GPIO_AFRL_AFRL4_Msk              (0xFU << GPIO_AFRL_AFRL4_Pos)         /*!< 0x000F0000 */
#define GPIO_AFRL_AFRL4                  GPIO_AFRL_AFRL4_Msk
#define GPIO_AFRL_AFRL5_Pos              (20U)
#define GPIO_AFRL_AFRL5_Msk              (0xFU << GPIO_AFRL_AFRL5_Pos)         /*!< 0x00F00000 */
#define GPIO_AFRL_AFRL5                  GPIO_AFRL_AFRL5_Msk
#define GPIO_AFRL_AFRL6_Pos              (24U)
#define GPIO_AFRL_AFRL6_Msk              (0xFU << GPIO_AFRL_AFRL6_Pos)         /*!< 0x0F000000 */
#define GPIO_AFRL_AFRL6                  GPIO_AFRL_AFRL6_Msk
#define GPIO_AFRL_AFRL7_Pos              (28U)
#define GPIO_AFRL_AFRL7_Msk              (0xFU << GPIO_AFRL_AFRL7_Pos)         /*!< 0xF0000000 */
#define GPIO_AFRL_AFRL7                  GPIO_AFRL_AFRL7_Msk

/****************** Bit definition for GPIO_AFRH register  ********************/
#define GPIO_AFRH_AFRH0_Pos              (0U)
#define GPIO_AFRH_AFRH0_Msk              (0xFU << GPIO_AFRH_AFRH0_Pos)         /*!< 0x0000000F */
#define GPIO_AFRH_AFRH0                  GPIO_AFRH_AFRH0_Msk
#define GPIO_AFRH_AFRH1_Pos              (4U)
#define GPIO_AFRH_AFRH1_Msk              (0xFU << GPIO_AFRH_AFRH1_Pos)         /*!< 0x000000F0 */
#define GPIO_AFRH_AFRH1                  GPIO_AFRH_AFRH1_Msk
#define GPIO_AFRH_AFRH2_Pos              (8U)
#define GPIO_AFRH_AFRH2_Msk              (0xFU << GPIO_AFRH_AFRH2_Pos)         /*!< 0x00000F00 */
#define GPIO_AFRH_AFRH2                  GPIO_AFRH_AFRH2_Msk
#define GPIO_AFRH_AFRH3_Pos              (12U)
#define GPIO_AFRH_AFRH3_Msk              (0xFU << GPIO_AFRH_AFRH3_Pos)         /*!< 0x0000F000 */
#define GPIO_AFRH_AFRH3                  GPIO_AFRH_AFRH3_Msk
#define GPIO_AFRH_AFRH4_Pos              (16U)
#define GPIO_AFRH_AFRH4_Msk              (0xFU << GPIO_AFRH_AFRH4_Pos)         /*!< 0x000F0000 */
#define GPIO_AFRH_AFRH4                  GPIO_AFRH_AFRH4_Msk
#define GPIO_AFRH_AFRH5_Pos              (20U)
#define GPIO_AFRH_AFRH5_Msk              (0xFU << GPIO_AFRH_AFRH5_Pos)         /*!< 0x00F00000 */
#define GPIO_AFRH_AFRH5                  GPIO_AFRH_AFRH5_Msk
#define GPIO_AFRH_AFRH6_Pos              (24U)
#define GPIO_AFRH_AFRH6_Msk              (0xFU << GPIO_AFRH_AFRH6_Pos)         /*!< 0x0F000000 */
#define GPIO_AFRH_AFRH6                  GPIO_AFRH_AFRH6_Msk
#define GPIO_AFRH_AFRH7_Pos              (28U)
#define GPIO_AFRH_AFRH7_Msk              (0xFU << GPIO_AFRH_AFRH7_Pos)         /*!< 0xF0000000 */
#define GPIO_AFRH_AFRH7                  GPIO_AFRH_AFRH7_Msk

/*
 * GPIO mode define ( brief GPIO Configuration Mode)
 */
#define  GPIO_MODE_INPUT		((uint32_t)0x00000000U) /* Input Floating Mode */
#define  GPIO_MODE_OUTPUT_PP		((uint32_t)0x00000001U) /* Output Push Pull Mode */
#define  GPIO_MODE_OUTPUT_OD		((uint32_t)0x00000011U) /* Output Open Drain Mode */
#define  GPIO_MODE_AF_PP		((uint32_t)0x00000002U) /* Alternate Function Push Pull Mode */
#define  GPIO_MODE_AF_OD		((uint32_t)0x00000012U)/* Alternate Function Open Drain Mode */
#define  GPIO_MODE_ANALOG		((uint32_t)0x00000003U)   /*!< Analog Mode  */

 /** @defgroup GPIO_pull_define  GPIO pull define
   * @brief GPIO Pull-Up or Pull-Down Activation
   * @{
   */
#define  GPIO_NOPULL        ((uint32_t)0x00000000U)   /*!< No Pull-up or Pull-down activation  */
#define  GPIO_PULLUP        ((uint32_t)0x00000001U)   /*!< Pull-up activation                  */
#define  GPIO_PULLDOWN      ((uint32_t)0x00000002U)   /*!< Pull-down activation                */

typedef struct
{
	uint32_t MODER;    /* GPIO port mode register,               Address offset: 0x00      */
	uint32_t OTYPER;   /* GPIO port output type register,        Address offset: 0x04      */
	uint32_t OSPEEDR;  /* GPIO port output speed register,       Address offset: 0x08      */
	uint32_t PUPDR;    /* GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
	uint32_t IDR;      /* GPIO port input data register,         Address offset: 0x10      */
	uint32_t ODR;      /* GPIO port output data register,        Address offset: 0x14      */
	uint16_t BSRRL;    /* GPIO port bit set/reset low register,  Address offset: 0x18      */
	uint16_t BSRRH;    /* GPIO port bit set/reset high register, Address offset: 0x1A      */
	uint32_t LCKR;     /* GPIO port configuration lock register, Address offset: 0x1C      */
	uint32_t AFR[2];   /* GPIO alternate function registers,     Address offset: 0x20-0x24 */
} GPIO_TypeDef;

static void gpio_write_pin(GPIO_TypeDef* volatile port, uint16_t gpio_pin,
		uint16_t pin_state)
{
	if (pin_state) {
		port->BSRRL = gpio_pin;
	}
	else {
		port->BSRRH = gpio_pin;
	}
}

static void gpio_init(GPIO_TypeDef * volatile port, uint32_t pin,
		uint32_t mode, uint32_t pull)
{
	uint32_t position = 0x00;
	uint32_t ioposition = 0x00;
	uint32_t iocurrent = 0x00;
	uint32_t temp = 0x00;

	/* Configure the port pins */
	for (position = 0; position < 16; position++)
	{
		/* Get the IO position */
		ioposition = ((uint32_t)0x01) << position;
		/* Get the current IO position */
		iocurrent = (uint32_t)(pin) & ioposition;

		if (iocurrent == ioposition) {
			/* In case of Alternate function mode selection */
			if ((mode == GPIO_MODE_AF_PP) ||
			    (mode == GPIO_MODE_AF_OD)) {
				/* Configure Alternate function mapped with the current IO */
				temp = port->AFR[position >> 3];
				temp &= ~((uint32_t)0xF <<
						((uint32_t)(position & (uint32_t)0x07) * 4)) ;
#if 0
                                temp |= ((uint32_t)(GPIO_Init->Alternate) <<
						(((uint32_t)position & (uint32_t)0x07) * 4));
#endif
				port->AFR[position >> 3] = temp;
			}

			/* Configure IO Direction mode (Input, Output, Alternate or Analog) */
			temp = port->MODER;
			temp &= ~(GPIO_MODER_MODER0 << (position * 2));
			temp |= ((mode & GPIO_MODE) << (position * 2));
			port->MODER = temp;

			/* In case of Output or Alternate function mode selection */
			if ((mode == GPIO_MODE_OUTPUT_PP) ||
			    (mode == GPIO_MODE_AF_PP) ||
			    (mode == GPIO_MODE_OUTPUT_OD) ||
			    (mode == GPIO_MODE_AF_OD)) {
			/* Configure the IO Speed */
				temp = port->OSPEEDR;
				temp &= ~(GPIO_OSPEEDER_OSPEEDR0 << (position * 2));
				temp |= ((uint32_t)0x03 << (position * 2));
				port->OSPEEDR = temp;

			/* Configure the IO Output Type */
				temp = port->OTYPER;
				temp &= ~(GPIO_OTYPER_OT_0 << position) ;
				temp |= (((mode & GPIO_OUTPUT_TYPE) >> 4) << position);
				port->OTYPER = temp;
			}

			/* Activate the Pull-up or Pull down resistor for the current IO */
			temp = port->PUPDR;
			temp &= ~(GPIO_PUPDR_PUPDR0 << (position * 2));
			temp |= ((pull) << (position * 2));
			port->PUPDR = temp;
		}
	}
}

void stm32h7_led1_init(void)
{
	* (uint32_t * volatile)(RCC_AHB4ENR) |= (1 << 5);
	gpio_init(GPIOF, GPIO_PIN_10, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP);
	gpio_write_pin(GPIOF, GPIO_PIN_10, 1);
}

void stm32h7_led1_on(void)
{
	gpio_write_pin(GPIOF, GPIO_PIN_10, 0);
}

void stm32h7_led1_off(void)
{
        gpio_write_pin(GPIOF, GPIO_PIN_10, 1);
}
