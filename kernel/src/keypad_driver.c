/*
 * keypad_driver.c
 *
 * Keypad driver for STM32.
 * Author: Selina Liao (selinal2@andrew.cmu.edu)
 *   Author: Anoushka De (anoushkd@andrew.cmu.edu)
 */

#include <unistd.h>

#include <gpio.h>
#include <keypad_driver.h>

//defining all the keypad connections to stm32
#define KEYPAD_ROW3_PORT GPIO_C
#define KEYPAD_ROW3_PIN  7
#define KEYPAD_ROW2_PORT GPIO_B
#define KEYPAD_ROW2_PIN  6
#define KEYPAD_ROW4_PORT GPIO_A
#define KEYPAD_ROW4_PIN  7
#define KEYPAD_ROW1_PORT GPIO_A
#define KEYPAD_ROW1_PIN  6

#define KEYPAD_COL2_PORT GPIO_A
#define KEYPAD_COL2_PIN  10
#define KEYPAD_COL1_PORT GPIO_B
#define KEYPAD_COL1_PIN  3
#define KEYPAD_COL3_PORT GPIO_B
#define KEYPAD_COL3_PIN  5

/*
 * Initializes the keypad driver.
 * Args: none. Returns: nothing.
 * Side effects: configures GPIO pins for keypad operation.
 */
void keypad_init() {
    // D12 to D9
    // row 3 connected to d9
    gpio_init(KEYPAD_ROW3_PORT, KEYPAD_ROW3_PIN, MODE_GP_OUTPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_PULL_DOWN, ALT0);

    // row 2 connected d10
    gpio_init(KEYPAD_ROW2_PORT, KEYPAD_ROW2_PIN, MODE_GP_OUTPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_PULL_DOWN, ALT0);

    // row 4 connected d11
    gpio_init(KEYPAD_ROW4_PORT, KEYPAD_ROW4_PIN, MODE_GP_OUTPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_PULL_DOWN, ALT0);

    // row 1 connected d12
    gpio_init(KEYPAD_ROW1_PORT, KEYPAD_ROW1_PIN, MODE_GP_OUTPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_PULL_DOWN, ALT0);


    // D2 to D4
    // col 2 connected to d2
    gpio_init(KEYPAD_COL2_PORT, KEYPAD_COL2_PIN, MODE_INPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_PULL_DOWN, ALT0);
    // col 1 connected to d3
    gpio_init(KEYPAD_COL1_PORT, KEYPAD_COL1_PIN, MODE_INPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_PULL_DOWN, ALT0);
    // col 3 conncted to d4
    gpio_init(KEYPAD_COL3_PORT, KEYPAD_COL3_PIN, MODE_INPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_PULL_DOWN, ALT0);

    return;
}

/*
 * Reads the keypad and returns the pressed key.
 * Args: none. Returns: the pressed key (0-9, *, #) or '\0' if none.
 * Assumes keypad_init has been called first.
 */
char keypad_read() {
    for (int i = 0; i < 4; i++) {
        
        // intialize all rows as low
        gpio_clr(KEYPAD_ROW3_PORT, KEYPAD_ROW3_PIN);
        gpio_clr(KEYPAD_ROW2_PORT, KEYPAD_ROW2_PIN);
        gpio_clr(KEYPAD_ROW4_PORT, KEYPAD_ROW4_PIN);
        gpio_clr(KEYPAD_ROW1_PORT, KEYPAD_ROW1_PIN);
        
        // sets current row to high base don iteration
        if (i == 0) gpio_set(KEYPAD_ROW3_PORT, KEYPAD_ROW3_PIN); // Row 3 (7, 8, 9)
        if (i == 1) gpio_set(KEYPAD_ROW2_PORT, KEYPAD_ROW2_PIN); // Row 2 (4, 5, 6)
        if (i == 2) gpio_set(KEYPAD_ROW4_PORT, KEYPAD_ROW4_PIN); // Row 4 (*, 0, #)
        if (i == 3) gpio_set(KEYPAD_ROW1_PORT, KEYPAD_ROW1_PIN); // Row 1 (1, 2, 3)
        
        // columns and debouncing logic
        int d2_col2 = gpio_read(KEYPAD_COL2_PORT, KEYPAD_COL2_PIN);
        for (int db = 0; db < 50000; db++);
        int d3_col1 = gpio_read(KEYPAD_COL1_PORT, KEYPAD_COL1_PIN);
        for (int db = 0; db < 50000; db++);
        int d4_col3 = gpio_read(KEYPAD_COL3_PORT, KEYPAD_COL3_PIN);
        for (int db = 0; db < 50000; db++);

        // return char on button based on col and row connection
        if (i == 0) {
            if (d2_col2) return '8';
            if (d3_col1) return '7';
            if (d4_col3) return '9';
        } else if (i == 1) {
            if (d2_col2) return '5';
            if (d3_col1) return '4';
            if (d4_col3) return '6';
        } else if (i == 2) {
            if (d2_col2) return '0';
            if (d3_col1) return '*';
            if (d4_col3) return '#';
        } else if (i == 3) {
            if (d2_col2) return '2';
            if (d3_col1) return '1';
            if (d4_col3) return '3';
        }
    }

    // return null if nothing
    return '\0'; 
}