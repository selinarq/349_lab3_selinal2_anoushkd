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
#include <printk.h>
#include <uart_polling.h>

int kernel_main( void ) {
  init_349(); // DO NOT REMOVE THIS LINE
  uart_polling_init(115200);
  systick_init();
  while (1) {
    systick_delay(1000);
    printk("Interrupt!\n");
  }

  return 0;
}
