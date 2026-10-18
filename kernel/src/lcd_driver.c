/*
 * lcd_driver.c
 *
 * Driver for the 16x2 HD44780 LCD. Sends commands and characters through
 * i2c.c to the PCF8574 extender board.
 *
 * Author: Selina Liao (selinal2@andrew.cmu.edu)
   Author: Anoushka De (anoushkd@andrew.cmu.edu)
 */

#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>

#include <i2c.h>
#include <lcd_driver.h>
#include <systick.h>


//  shifted to the right 
#define LCD_SLAVE_ADDR_READ 0x4F >> 1
#define LCD_SLAVE_ADDR_WRITE 0x4E >> 1

// the wait times
#define LCD_POWER_ON_DELAY 15
#define LCD_FIRST_DELAY 5
#define LCD_SECOND_DELAY 1
#define LCD_CLEAR_DELAY 2

/*
 * Sends one byte to the LCD as two 4-bit nibbles in a single I2C transaction.
 * Args: data - instruction or character; rs - 0 for instruction, 1 for data.
 * Returns: nothing. Errors from the I2C write are not checked.
 * Assumes the LCD is already in 4-bit mode.
 */
void lcd_helper_function(uint8_t data, bool rs) {
    // split it into 4 bits with the enable
    // call i2c_master_write() to send the data to the LCD
    // 0 0 1 1 RS to DB4 0 0 0 0 three times

    uint8_t first_half = (data >> 4) & 0x0F;
    uint8_t second_half = data & 0x0F;

    uint8_t first_half_enable, first_half_disable, second_half_enable, second_half_disable;

    if (rs == 0) {
        // enable bit is 1100
        // unenable bit is 1000
        first_half_enable = (first_half << 4) | 0x0C;
        first_half_disable = (first_half << 4) | 0x08;

        // enable bit is 1100
        // unenable bit is 1000
        second_half_enable = (second_half << 4) | 0x0C;
        second_half_disable = (second_half << 4) | 0x08;

    } else {
        // enable bit is 1101
        // unenable bit is 1001
        first_half_enable = (first_half << 4) | 0x0D;
        first_half_disable = (first_half << 4) | 0x09;

        // enable bit is 1101
        // unenable bit is 1001
        second_half_enable = (second_half << 4) | 0x0D;
        second_half_disable = (second_half << 4) | 0x09;
    }

    uint8_t buf[4] = {first_half_enable, first_half_disable, second_half_enable, second_half_disable};
    i2c_master_write(buf, 4, LCD_SLAVE_ADDR_WRITE);

    return;
}

/*
 * Initializes the LCD for 4-bit, 2-line operation with the display and
 * blinking cursor on, then clears the screen.
 * Args: none. Returns: nothing.
 * Assumes i2c_master_init has been called first.
 */
void lcd_driver_init() {
    systick_delay(LCD_POWER_ON_DELAY); // wait for power on
    // 0 0 1 0 0 0 0 0  for 3 times
    // 0 0 1 0 0 0 0 0  for 1 time
    // clear lcd display with 0 0 0 0  0 0 0 1
	lcd_helper_function(0x03, 0);
    systick_delay(LCD_FIRST_DELAY);
    lcd_helper_function(0x03, 0);
    systick_delay(LCD_SECOND_DELAY);
    lcd_helper_function(0x03, 0);
    systick_delay(LCD_SECOND_DELAY);
    // switch to 4 bit mode
    lcd_helper_function(0x02, 0);
    systick_delay(LCD_SECOND_DELAY);
    

    lcd_helper_function(0x28, 0);         // 4-bit, 2 lines
    systick_delay(LCD_SECOND_DELAY);
    lcd_helper_function(0x0F, 0);         // display on, blinking cursor
    systick_delay(LCD_SECOND_DELAY);
    lcd_helper_function(0x06, 0);         // entry mode
    systick_delay(LCD_SECOND_DELAY);
    lcd_clear();                            // clear display
}

/*
 * Prints a string at the current cursor position.
 * Args: input - null-terminated string. Returns: nothing.
 * Side effects: advances the cursor. Does not wrap past 16 characters.
 */
void lcd_print(char *input){
    (void) input;
    // in order to write things, it is 
    // RS R/W
    //  1  0 WRITE DATA follows
    // this doesnt matter allegedly

    for (int i = 0; input[i] != '\0'; i++) {
        uint8_t data = (uint8_t)input[i];
        // set RS to 1 for data mode
        // data |= 0x01 << 0; // Set RS bit to 1
        lcd_helper_function(data, 1);
    }
    return;
}

/*
 * Moves the cursor to the given position.
 * Args: row - 0 or 1; col - 0 to 15. Returns: nothing.
 * Error case: an invalid row is ignored and the cursor does not move.
 */
void lcd_set_cursor(uint8_t row, uint8_t col){
    (void) row;
    (void) col;
    
    uint8_t DDRAM_address = 0x00;
    if (row == 0) {
        DDRAM_address = 0x00 + col;
    } else if (row == 1) {
        DDRAM_address = 0x40 + col;
    } else {
        // Invalid row, handle error
        return;
    }

    // set DDRAM address
    // first half is 1 _ _ _  _ _ _ _
    // make sure that addresses have 1 in the first bit
    DDRAM_address |= 0x80; // Set the first bit to 1
    lcd_helper_function(DDRAM_address, 0);
}

/*
 * Clears the display and returns the cursor to row 0, column 0.
 * Args: none. Returns: nothing.
 * Side effects: blocks briefly while the LCD finishes clearing.
 */
void lcd_clear() {
    // 0 0 0 0  0 0 0 1
    lcd_helper_function(0x01, 0);
    systick_delay(LCD_CLEAR_DELAY);
    return;
}