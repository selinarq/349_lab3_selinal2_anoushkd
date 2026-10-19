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
#include <systick.h>

#define SYSTICK_BASE 0xE000E010

/** SysTick register map */
typedef struct {
    volatile uint32_t STK_CTRL;   /**< STK_CTRL: control and status */
    volatile uint32_t STK_LOAD;   /**< STK_LOAD: reload value */
    volatile uint32_t STK_VAL;    /**< STK_VAL: current value */
    volatile uint32_t STK_CALIB;  /**< STK_CALIB: calibration */
} systick_regs_t;

#define SYSTICK ((systick_regs_t *)SYSTICK_BASE)

#define STK_CTRL_ENABLE (1 << 0)
#define STK_CTRL_TICKINT (1 << 1)
#define STK_CTRL_CLKSOURCE (1 << 2)

#define CLK_FREQ 16000000  // 16 MHz
#define TICKS_PER_SECOND 1000  // 1 ms per tick
#define SYSTICK_RELOAD ((CLK_FREQ / TICKS_PER_SECOND) - 1) 

static volatile uint32_t elapsed_ms = 0;

void systick_init() {
    
    // set the reload value
    // enable the counter
    // enable the SysTick interrupr
    SYSTICK->STK_CTRL = 0;
    SYSTICK->STK_LOAD = SYSTICK_RELOAD;
    SYSTICK->STK_VAL = 0; // clear current value
    SYSTICK->STK_CTRL = STK_CTRL_ENABLE | STK_CTRL_TICKINT | STK_CTRL_CLKSOURCE;
}

uint32_t systick_get_ticks() {
    return elapsed_ms;
}

void systick_c_handler() {
    elapsed_ms++;
}

void systick_delay(uint32_t ticks) {
    uint32_t start = systick_get_ticks();
    while (systick_get_ticks() - start < ticks);
}