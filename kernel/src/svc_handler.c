/**
 * @file 
 *
 * @brief      
 *
 * @date       
 *
 * @author     
 */

#include <stdint.h>
#include <debug.h>
#include <svc_num.h>
#include <syscall.h>
#include <printk.h>
#include <uart.h>
#include <lcd_driver.h>

#define UNUSED __attribute__((unused))

void svc_c_handler(uint32_t *frame) {
  uint16_t *svc_instr = (uint16_t *)(frame[6] - 2);  // step back to the svc
  int svc_number = *svc_instr & 0xFF;                // low byte is the number

  switch (svc_number) {
  case SVC_SBRK:
    frame[0] = (uint32_t)sys_sbrk((int)frame[0]);
    break;
  
  case SVC_WRITE:
    frame[0] = (uint32_t)sys_write((int)frame[0], (char*)frame[1], (int)frame[2]);
    break;

  case SVC_CLOSE:
    break; 
  
  case SVC_FSTAT:
    break; 

  case SVC_ISATTY:
    break; 
  
  case SVC_LSEEK:
    break; 
  
  case SVC_READ:
    frame[0] = (uint32_t)sys_read((int)frame[0], (char*)frame[1], (int)frame[2]);
    break;

  case SVC_EXIT:
    frame[0] = (uint32_t)sys_sbrk((int)frame[0]);
    break;
  
  case SVC_KILL:
    break; 

    case SVC_THR_INIT:
    break;

  case SVC_THR_CREATE:
    break;

  case SVC_THR_KILL:
    break;

  case SVC_GET_PID:
    break;

  case SVC_MUT_INIT:
    break;

  case SVC_MUT_LOK:
    break;

  case SVC_MUT_ULK:
    break;

  case SVC_WAIT:
    break;

  case SVC_TIME:
    break;

  case SVC_SCHD_START:
    break;

  case SVC_PRIORITY:
    break;

  case SVC_THR_TIME:
    break;

  case SVC_SLEEP_TILL_INT:
    break;

  case SVC_SERVO_ENABLE:
    frame[0] = (uint32_t)sys_servo_enable((int)frame[0], (int)frame[1]);
    break;

  case SVC_SERVO_SET:
    frame[0] = (uint32_t)sys_servo_set((int)frame[0], (int)frame[1]);
    break;

  default:
    frame[0] = (uint32_t)-1;
    break;
  }
}