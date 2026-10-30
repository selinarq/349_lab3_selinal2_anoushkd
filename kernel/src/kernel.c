/**
 * @file kernel.c
 *
 * @brief      Kernel entry point
 *
 * @date
 * @author
 */

#include <arm.h>
#include <kernel.h>
#include <systick.h>
#include <gpio.h>
#include <i2c.h>
#include <printk.h>
#include <uart_polling.h>
#include <uart.h>
#include <unistd.h>
#include <lcd_driver.h>
#include <keypad_driver.h>
#include "lcd_driver.h"
#include <timer.h>
#include <nvic.h>


//defining all the keypad connections to stm32
#define KEYPAD_ROW3_PORT GPIO_C
#define KEYPAD_ROW3_PIN  7
#define KEYPAD_ROW2_PORT GPIO_B
#define KEYPAD_ROW2_PIN  6
#define KEYPAD_ROW4_PORT GPIO_A
#define KEYPAD_ROW4_PIN  7
#define KEYPAD_ROW1_PORT GPIO_A
#define KEYPAD_ROW1_PIN  6

#define LED_PORT GPIO_A
#define LED_PIN 4

/** @brief Timer used for the LED blink test. */
#define TIMER       2
/** @brief NVIC IRQ number for TIM2. */
#define TIM2_IRQ_NUM      28
/** @brief 16 MHz / 16000 = 1 kHz timer clock (1 count per ms). */
#define PRESCALAR   16000
/** @brief 1000 counts at 1 kHz = one interrupt per second. */
#define PERIOD      1000

static volatile int led_on = 0;

#define CLK_VALUE 100

void TIM2_IRQHandler(void) {
  timer_clear_interrupt_bit(TIMER);   /* clear UIF or this re-fires forever */
  led_on = !led_on;
  if (led_on) {
    gpio_set(LED_PORT, LED_PIN);
  } else {
    gpio_clr(LED_PORT, LED_PIN);
  }
}

int kernel_main( void ) {
  init_349(); // DO NOT REMOVE THIS LINE
  uart_init(115200);

  printk("hello\n");

  systick_init();
  keypad_init();
  i2c_master_init(CLK_VALUE);
  lcd_driver_init();
  lcd_clear();

  // enter_user_mode();

  // start by setting all rows for keypad high 
  gpio_set(KEYPAD_ROW3_PORT, KEYPAD_ROW3_PIN);
  gpio_set(KEYPAD_ROW2_PORT, KEYPAD_ROW2_PIN);
  gpio_set(KEYPAD_ROW4_PORT, KEYPAD_ROW4_PIN);
  gpio_set(KEYPAD_ROW1_PORT, KEYPAD_ROW1_PIN);
  
  // init the LED
  gpio_init(LED_PORT, LED_PIN, MODE_GP_OUTPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_NONE, ALT0);

  // initialize the timer
  timer_init(TIMER, PRESCALAR, PERIOD);
  nvic_irq(TIM2_IRQ_NUM, IRQ_ENABLE);

  
  uint8_t cursor_row = 0;
  while (1) {
    systick_delay(1000);
    printk("Interrupt!\n");
    enter_user_mode();
    

    // read keypad input always and print pressed key
    char key = keypad_read();

    if (key == '*') {
      cursor_row = !cursor_row;
      lcd_set_cursor(cursor_row, 0);
    } else if (key == '#') {
      lcd_clear();
      cursor_row = 0;
      lcd_set_cursor(cursor_row, 0);
    } else if (key != '\0') {
      char key_string[2] = {key, '\0'};
      lcd_print(key_string);
    }

  }

  return 0;
}
