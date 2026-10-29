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
#include <gpio.h>
#include <systick.h>
#include <timer.h>
#include <nvic.h>

#define PULSE_FREQUENCY 20
#define SERVO_PORT GPIO_A
// 0.6 = 0 degrees
// 1.5 = 90 degrees
// 2.4 = 180 degrees

// timer driving the PWM and the NVIC IRQ number
#define SERVO_TIMER           3
#define SERVO_TIMER_IRQ       29

//16 MHz / 16 = 1 MHz timer clock
#define SERVO_PRESCALER       16

// microseconds between timer interrupts
#define SERVO_TICK_US         20u

// time between the start of one pulse and another (20 ms)
#define SERVO_PERIOD_US       20000u

// 0 degrees = 0.6ms
#define SERVO_MIN_PULSE_US    600u

// 180 degrees = 2.4 ms
#define SERVO_MAX_PULSE_US    2400u

// number of ticks in one full period (20ms /20 us) = 1000
#define SERVO_PERIOD_TICKS    (SERVO_PERIOD_US / SERVO_TICK_US)

// pulse width in ticks for 0 degrees (600 us / 20 us = 30)
#define SERVO_MIN_TICKS       (SERVO_MIN_PULSE_US / SERVO_TICK_US)

// pulse width in ticks for 0 degrees (2400 us / 20 us = 120)
#define SERVO_MAX_TICKS       (SERVO_MAX_PULSE_US / SERVO_TICK_US)

#define SERVO_MAX_ANGLE 180

#define NUM_SERVOS 2

#define SERVO_CENTER_TICKS    ((SERVO_MIN_TICKS + SERVO_MAX_TICKS) / 2)

/** @brief Pins of the two PWM outputs; check against the schematic. */
static const uint8_t servo_pins[NUM_SERVOS] = {4, 5};

// array that shows if channel is enabled or disabled
static volatile uint8_t  servo_enabled[NUM_SERVOS];

static volatile uint32_t pulse_ticks[NUM_SERVOS] = {SERVO_CENTER_TICKS,
                                                    SERVO_CENTER_TICKS};

// period tick time within the period                                                    
static volatile uint32_t period_tick;

// nonzero once the PWM timer has been starter
static int timer_running;

void TIM3_IRQHandler(void) {
  timer_clear_interrupt_bit(SERVO_TIMER);

  for (int i = 0; i < NUM_SERVOS; i++) {
    if (servo_enabled[i] && period_tick < pulse_ticks[i]) {
        gpio_set(SERVO_PORT, servo_pins[i]);
    } else {
      gpio_clr(SERVO_PORT, servo_pins[i]);
    }
  }

  period_tick++;
  if (period_tick == SERVO_PERIOD_TICKS) {
    period_tick = 0;
  }
}

/**
* @brief Enable or disable servo motor control
*
* @param channel channel to enable or disable
* @param enabled true to enable, false to disable
*
* @return 0 on success or-1 on failure
*/
int sys_servo_enable(uint8_t channel, uint8_t enabled) {
  if (channel < 1 || channel > NUM_SERVOS) {
    return -1;
  }

  if (enabled && !timer_running) {
    for (int i = 0; i < NUM_SERVOS; i++) {
      gpio_init(SERVO_PORT, servo_pins[i], MODE_GP_OUTPUT, OUTPUT_PUSH_PULL,
                OUTPUT_SPEED_LOW, PUPD_NONE, ALT0);
    }
    timer_init(SERVO_TIMER, SERVO_PRESCALER, SERVO_TICK_US);
    nvic_irq(SERVO_TIMER_IRQ, IRQ_ENABLE);
    timer_running = 1;
  }

  servo_enabled[channel - 1] = enabled ? 1 : 0;
  return 0;
}

/**
* @brief Set a servo motor to a given position
*
* @param channel channel to control
* @param angle
servo angle in degrees (0-180)
*
* @return 0 on success or-1 on failure
*/
int sys_servo_set(uint8_t channel, uint8_t angle){
  int index = channel - 1;
  if (channel < 1 || channel > 2) {
    return -1;
  }

  uint32_t span = SERVO_MAX_TICKS - SERVO_MIN_TICKS;
  pulse_ticks[index] = SERVO_MIN_TICKS
                     + (angle * span + SERVO_MAX_ANGLE / 2) / SERVO_MAX_ANGLE;

  return 0;
}