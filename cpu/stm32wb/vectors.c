/*
 * Copyright (C) 2014-2017 Freie Universität Berlin
 *               2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32wb
 * @{
 *
 * @file
 * @brief       Interrupt vector definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

WEAK_DEFAULT void isr_adc1(void);
WEAK_DEFAULT void isr_aes1(void);
WEAK_DEFAULT void isr_aes2(void);
WEAK_DEFAULT void isr_c2sev_pwr_c2h(void);
WEAK_DEFAULT void isr_comp(void);
WEAK_DEFAULT void isr_crs(void);
WEAK_DEFAULT void isr_dma1_channel1(void);
WEAK_DEFAULT void isr_dma1_channel2(void);
WEAK_DEFAULT void isr_dma1_channel3(void);
WEAK_DEFAULT void isr_dma1_channel4(void);
WEAK_DEFAULT void isr_dma1_channel5(void);
WEAK_DEFAULT void isr_dma1_channel6(void);
WEAK_DEFAULT void isr_dma1_channel7(void);
WEAK_DEFAULT void isr_dma2_channel1(void);
WEAK_DEFAULT void isr_dma2_channel2(void);
WEAK_DEFAULT void isr_dma2_channel3(void);
WEAK_DEFAULT void isr_dma2_channel4(void);
WEAK_DEFAULT void isr_dma2_channel5(void);
WEAK_DEFAULT void isr_dma2_channel6(void);
WEAK_DEFAULT void isr_dma2_channel7(void);
WEAK_DEFAULT void isr_dmamux1_ovr(void);
WEAK_DEFAULT void isr_exti(void);
WEAK_DEFAULT void isr_flash(void);
WEAK_DEFAULT void isr_fpu(void);
WEAK_DEFAULT void isr_hsem(void);
WEAK_DEFAULT void isr_i2c1_er(void);
WEAK_DEFAULT void isr_i2c1_ev(void);
WEAK_DEFAULT void isr_i2c3_er(void);
WEAK_DEFAULT void isr_i2c3_ev(void);
WEAK_DEFAULT void isr_ipcc_c1_rx(void);
WEAK_DEFAULT void isr_ipcc_c1_tx(void);
WEAK_DEFAULT void isr_lcd(void);
WEAK_DEFAULT void isr_lptim1(void);
WEAK_DEFAULT void isr_lptim2(void);
WEAK_DEFAULT void isr_lpuart1(void);
WEAK_DEFAULT void isr_pka(void);
WEAK_DEFAULT void isr_pvd_pvm(void);
WEAK_DEFAULT void isr_quadspi(void);
WEAK_DEFAULT void isr_rcc(void);
WEAK_DEFAULT void isr_rng(void);
WEAK_DEFAULT void isr_rtc_alarm(void);
WEAK_DEFAULT void isr_rtc_wkup(void);
WEAK_DEFAULT void isr_sai1(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_spi2(void);
WEAK_DEFAULT void isr_tamp_stamp_lsecss(void);
WEAK_DEFAULT void isr_tim1_brk(void);
WEAK_DEFAULT void isr_tim1_cc(void);
WEAK_DEFAULT void isr_tim1_trg_com_tim17(void);
WEAK_DEFAULT void isr_tim1_up_tim16(void);
WEAK_DEFAULT void isr_tim2(void);
WEAK_DEFAULT void isr_tsc(void);
WEAK_DEFAULT void isr_usart1(void);
WEAK_DEFAULT void isr_usb_hp(void);
WEAK_DEFAULT void isr_usb_lp(void);
WEAK_DEFAULT void isr_wwdg(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    /* shared vectors for all family members */
    [ 0] = isr_wwdg,                 /* [ 0] Window WatchDog Interrupt */
    [ 1] = isr_pvd_pvm,              /* [ 1] PVD and PVM detector */
    [ 2] = isr_tamp_stamp_lsecss,    /* [ 2] RTC Tamper and TimeStamp Interrupts and LSECSS Interrupts */
    [ 3] = isr_rtc_wkup,             /* [ 3] RTC Wakeup Interrupt */
    [ 4] = isr_flash,                /* [ 4] FLASH (CFI)  global Interrupt */
    [ 5] = isr_rcc,                  /* [ 5] RCC Interrupt */
    [ 6] = isr_exti,                 /* [ 6] EXTI Line 0 Interrupt */
    [ 7] = isr_exti,                 /* [ 7] EXTI Line 1 Interrupt */
    [ 8] = isr_exti,                 /* [ 8] EXTI Line 2 Interrupt */
    [ 9] = isr_exti,                 /* [ 9] EXTI Line 3 Interrupt */
    [10] = isr_exti,                 /* [10] EXTI Line 4 Interrupt */
    [11] = isr_dma1_channel1,        /* [11] DMA1 Channel 1 Interrupt */
    [12] = isr_dma1_channel2,        /* [12] DMA1 Channel 2 Interrupt */
    [13] = isr_dma1_channel3,        /* [13] DMA1 Channel 3 Interrupt */
    [14] = isr_dma1_channel4,        /* [14] DMA1 Channel 4 Interrupt */
    [15] = isr_dma1_channel5,        /* [15] DMA1 Channel 5 Interrupt */
    [16] = isr_dma1_channel6,        /* [16] DMA1 Channel 6 Interrupt */
    [17] = isr_dma1_channel7,        /* [17] DMA1 Channel 7 Interrupt */
    [18] = isr_adc1,                 /* [18] ADC1 Interrupt */
    [19] = isr_usb_hp,               /* [19] USB High Priority Interrupt */
    [20] = isr_usb_lp,               /* [20] USB Low Priority Interrupt (including USB wakeup) */
    [21] = isr_c2sev_pwr_c2h,        /* [21] CPU2 SEV Interrupt */
    [22] = isr_comp,                 /* [22] COMP1 and COMP2 Interrupts */
    [23] = isr_exti,                 /* [23] EXTI Lines [9:5] Interrupt */
    [24] = isr_tim1_brk,             /* [24] TIM1 Break Interrupt */
    [25] = isr_tim1_up_tim16,        /* [25] TIM1 Update and TIM16 global Interrupts */
    [26] = isr_tim1_trg_com_tim17,   /* [26] TIM1 Trigger and Communication and TIM17 global Interrupts */
    [27] = isr_tim1_cc,              /* [27] TIM1 Capture Compare Interrupt */
    [28] = isr_tim2,                 /* [28] TIM2 Global Interrupt */
    [29] = isr_pka,                  /* [29] PKA Interrupt */
    [30] = isr_i2c1_ev,              /* [30] I2C1 Event Interrupt */
    [31] = isr_i2c1_er,              /* [31] I2C1 Error Interrupt */
    [32] = isr_i2c3_ev,              /* [32] I2C3 Event Interrupt */
    [33] = isr_i2c3_er,              /* [33] I2C3 Error Interrupt */
    [34] = isr_spi1,                 /* [34] SPI1 Interrupt */
    [35] = isr_spi2,                 /* [35] SPI2 Interrupt */
    [36] = isr_usart1,               /* [36] USART1 Interrupt */
    [37] = isr_lpuart1,              /* [37] LPUART1 Interrupt */
    [38] = isr_sai1,                 /* [38] SAI1 A and B global interrupt */
    [39] = isr_tsc,                  /* [39] TSC Interrupt */
    [40] = isr_exti,                 /* [40] EXTI Lines1[15:10 ]Interrupts */
    [41] = isr_rtc_alarm,            /* [41] RTC Alarms (A and B) Interrupt */
    [42] = isr_crs,                  /* [42] CRS interrupt */
    [44] = isr_ipcc_c1_rx,           /* [44] IPCC RX Occupied Interrupt */
    [45] = isr_ipcc_c1_tx,           /* [45] IPCC TX Free Interrupt */
    [46] = isr_hsem,                 /* [46] HSEM Interrupt */
    [47] = isr_lptim1,               /* [47] LPTIM1 Interrupt */
    [48] = isr_lptim2,               /* [48] LPTIM2 Interrupt */
    [49] = isr_lcd,                  /* [49] LCD Interrupt */
    [50] = isr_quadspi,              /* [50] QUADSPI Interrupt */
    [51] = isr_aes1,                 /* [51] AES1 Interrupt */
    [52] = isr_aes2,                 /* [52] AES2 Interrupt */
    [53] = isr_rng,                  /* [53] RNG Interrupt */
    [54] = isr_fpu,                  /* [54] FPU Interrupt */
    [55] = isr_dma2_channel1,        /* [55] DMA2 Channel 1 Interrupt */
    [56] = isr_dma2_channel2,        /* [56] DMA2 Channel 2 Interrupt */
    [57] = isr_dma2_channel3,        /* [57] DMA2 Channel 3 Interrupt */
    [58] = isr_dma2_channel4,        /* [58] DMA2 Channel 4 Interrupt */
    [59] = isr_dma2_channel5,        /* [59] DMA2 Channel 5 Interrupt */
    [60] = isr_dma2_channel6,        /* [60] DMA2 Channel 6 Interrupt */
    [61] = isr_dma2_channel7,        /* [61] DMA2 Channel 7 Interrupt */
    [62] = isr_dmamux1_ovr,          /* [62] DMAMUX1 overrun Interrupt */
};
