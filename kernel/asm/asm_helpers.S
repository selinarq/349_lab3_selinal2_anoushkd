.cpu cortex-m4
.syntax unified
.thumb

.section .text

.extern __psp_stack_top 
.extern _crt0

.thumb_func
.global enter_user_mode
enter_user_mode:
  LDR R0, __psp_stack_top
  MSR PSP, R0
  b _crt0





  