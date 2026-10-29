/**
 * @file
 *
 * @brief
 *
 * @date
 *
 * @author
 */

#include <unistd.h>
#include <timer.h>
#include <rcc.h>
#include <printk.h>

#define TIM_BASE (struct tim2_5 *) 0x40000000
#define TIM_EN_2 0
#define TIM_EN_3 1
#define TIM_EN_4 2
#define TIM_EN_5 3
/** @brief Update interrupt flag (UIF) bit in the timer status register. */
#define TIM_SR_UIF  (1 << 0)

/** @brief tim2_5 */
struct tim2_5 {
  volatile uint32_t cr1; /**< 00 Control Register 1 */
  volatile uint32_t cr2; /**< 04 Control Register 2 */
  volatile uint32_t smcr; /**< 08 Slave Mode Control */
  volatile uint32_t dier; /**< 0C DMA/Interrupt Enable */
  volatile uint32_t sr; /**< 10 Status Register */
  volatile uint32_t egr; /**< 14 Event Generation */
  volatile uint32_t ccmr[2]; /**< 18-1C Capture/Compare Mode */
  volatile uint32_t ccer; /**< 20 Capture/Compare Enable */
  volatile uint32_t cnt; /**< 24 Counter Register */
  volatile uint32_t psc; /**< 28 Prescaler Register */
  volatile uint32_t arr; /**< 2C Auto-Reload Register */
  volatile uint32_t reserved_1; /**< 30 */
  volatile uint32_t ccr[4]; /**< 34-40 Capture/Compare */
  volatile uint32_t reserved_2; /**< 44 */
  volatile uint32_t dcr; /**< 48 DMA Control Register */
  volatile uint32_t dmar; /**< 4C DMA address for full transfer Register */
  volatile uint32_t or; /**< 50 Option Register */
};

struct tim2_5* const timer_base[] = {(void *)0x0,   // N/A - Don't fill out
                                     (void *)0x0,   // N/A - Don't fill out
                                     (void *)0x40000000,  // TIM2
                                     (void *)0x40000400,  // TIM3
                                     (void *)0x40000800,  // TIM4
                                     (void *)0x40000C00}; // TIM5


void timer_init(int timer, uint32_t prescalar, uint32_t period) {
  struct rcc_reg_map *rcc = RCC_BASE;
  if (timer < 2 || timer > 5 || prescalar==0 || period==0){
    return;
  }
  switch (timer) {
    case 2:
      rcc->apb1_enr |= (1 << TIM_EN_2);
      break;
    case 3:
      rcc->apb1_enr |= (1 << TIM_EN_3);
      break;
    case 4:
      rcc->apb1_enr |= (1 << TIM_EN_4);
      break;
    case 5:
      rcc->apb1_enr |= (1 << TIM_EN_5);
      break;
    default:
      printk("error");
      break;
  }

  struct tim2_5 *tim = timer_base[timer];
  tim->cr1 |= 1;
  tim->dier |= 1;
  tim->psc = prescalar - 1;
  tim->arr = period - 1;
}

void timer_disable(int timer) {
  struct rcc_reg_map *rcc = RCC_BASE;
  switch (timer) {
    case 2:
      rcc->apb1_enr &= ~(1 << TIM_EN_2);
      break;
    case 3:
      rcc->apb1_enr &= ~(1 << TIM_EN_3);
      break;
    case 4:
      rcc->apb1_enr &= ~(1 << TIM_EN_4);
      break;
    case 5:
      rcc->apb1_enr &= ~(1 << TIM_EN_5);
      break;
    default:
      break;
  }
}

void timer_clear_interrupt_bit(int timer) {
  if (2 <= timer && timer <= 5) {
    timer_base[timer]->sr &= ~TIM_SR_UIF; 
  }
}