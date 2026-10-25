/**
 * @file   swi_stubs.S
 *
 * @brief  Stub functions for newlib and 349libc syscalls
 *
 * @date
 * @author
 */

.cpu cortex-m4
.syntax unified
.section .swi_stub
.thumb

#include "../../kernel/include/svc_num.h"

.global _start
_start:
  bkpt

.global _sbrk
_sbrk:
  svc SVC_SBRK
  bx lr

.thumb_func
.global _write
_write:
  svc SVC_WRITE
  bx lr

.thumb_func
.global _close
_close:
  svc SVC_CLOSE
  bx lr

.thumb_func
.global _fstat
_fstat:
  svc SVC_FSTAT
  bx lr

.thumb_func
.global _isatty
_isatty:
  svc SVC_ISATTY
  bx lr

.thumb_func
.global _lseek
_lseek:
  svc SVC_LSEEK
  bx lr

.thumb_func
.global _read
_read:
  svc SVC_READ
  bx lr

.thumb_func
.global _gettimeofday
_gettimeofday:
  mvn r0, #0             @ no kernel support: report failure (-1)
  bx lr

.thumb_func
.global _times
_times:
  mvn r0, #0             @ no kernel support: report failure (-1)
  bx lr

.thumb_func
.global _getpid
_getpid:
  svc SVC_GET_PID
  bx lr

.thumb_func
.global _kill
_kill:
  svc SVC_KILL
  bx lr

.thumb_func
.global _exit
_exit:
  svc SVC_EXIT
  b .                    @ sys_exit never returns; spin if it somehow does

.thumb_func
.global servo_enable
servo_enable:
  svc SVC_SERVO_ENABLE
  bx lr

.thumb_func
.global servo_set
servo_set:
  svc SVC_SERVO_SET
  bx lr