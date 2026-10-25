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
#include <syscall.h>
#include <printk.h>
#include <uart.h>
#include <lcd_driver.h>

extern char __heap_low;
extern char __heap_top;

void *sys_sbrk(int incr){
  static char *program_break = &__heap_low;   // persists between calls
  // variable keep track of current heap position
  
  char *old_break = program_break; // store the old break value

  if (program_break + incr > &__heap_top) {
    return (void *)-1; // Not enough memory
  }
  if (incr >= 0) {
    program_break += incr;
    return (void *)(old_break);
  } else {
    return (void *)-1; // Decrementing the heap is not supported
  }
}


//print characters from user program into terminal through UART
int sys_write(int file, char *ptr, int len){
    //only support stdout, can't write anywhere else
    if (file != 1)
        return -1;
    //send each char to buffer
    for (int i = 0; i < len; i++) {
        while (uart_put_byte(ptr[i]) == -1){
          //wait
        }
    }
    return len;
}

//chars from UART and stores in array so program can use them
int sys_read(int file, char *ptr, int len){
    if (file != 0) return -1;

    //how many chars read and stored
    int count = 0;
    char c;

    while (count < len) {
      //takes char from RX buffer and checks if succesful
        if (uart_get_byte(&c) == 0) {
            if (c == 4){
                break;
            }

            if (c == '\b') {
                if (count > 0) {
                    count--;
                    uart_put_byte('\b');
                    uart_put_byte(' ');
                    uart_put_byte('\b');
                }
            }
            //sotre char and display in terminal
            else {
              //change return to enter
                if (c == '\r'){
                    c = '\n';
                }
                ptr[count] = c;
                count++;
                //sned char back to terminal so user can see what is typed
                uart_put_byte(c);
                if (c == '\n'){
                    break;
                }
            }
        }
    }

    return count;
}

void sys_exit(int status){
  printk("exit(%d)\n", status);
  // flush the UART buffer
  uart_flush();
  // display the exit code on the LED screen and sleep with interrupts disabled
  lcd_clear();
  lcd_set_cursor(0, 0);
  char status_text[2] = { (char)(status + '0'), '\0' };
  lcd_print(status_text);

  __asm volatile("cpsid i");         // disable interrupts
  while (1) {
      __asm volatile("wfi");         // sleep; nothing can wake us
  }
}