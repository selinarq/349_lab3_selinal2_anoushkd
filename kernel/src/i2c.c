/*
 * i2c.c
 *
 * Polled master-mode driver for I2C1. Used by lcd_driver.c to talk to the
 * PCF8574 extender board on the LCD.
 *
 * Author: Selina Liao (selinal2@andrew.cmu.edu)
   Author: Anoushka De (anoushkd@andrew.cmu.edu)
 */

#include <unistd.h>
#include <gpio.h>
#include <i2c.h>
#include <rcc.h>

//register map to control I2C hardware
struct i2c_reg_map {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t OAR1;
    volatile uint32_t OAR2;
    volatile uint32_t DR;
    volatile uint32_t SR1;
    volatile uint32_t SR2;
    volatile uint32_t CCR;
    volatile uint32_t TRISE;
    volatile uint32_t FLTR;
};

//connect reg map to location of I2C1 channel
#define I2C1_BASE (struct i2c_reg_map *) 0x40005400
#define I2C1_SCL_PORT GPIO_B
#define I2C1_SCL_PIN  8
#define I2C1_SDA_PORT GPIO_B
#define I2C1_SDA_PIN  9
#define I2C1_ALT 4

//according to manual which bits represent what
#define I2C1_CLK_EN (1 << 21)
#define I2C1_PERIPH_EN (1 << 0)
#define START (1 << 8)
#define STOP (1 << 9)
#define SB_BIT (1 << 0)
#define ADDR_BIT (1 << 1)
#define BTF_BIT (1 << 2)
#define TXE_BIT (1 << 7)
#define ACK_BIT (1 << 10)

#define I2C_ACK 0
#define I2C_NACK -1
#define AF_BIT (1 << 10)
#define BUSY_BIT (1 << 1)
#define CR2_VALUE 16
#define CLK_VALUE 100
#define TRISE_VALUE 17

/*
 * Configures I2C1 as a master and sets up pins PB8 (SCL) and PB9 (SDA).
 * Args: clk - desired SCL frequency in kHz (e.g. 100). Returns: nothing.
 * Side effects: enables the I2C1 clock and the peripheral.
 * Assumes the APB1 clock is 16 MHz.
 */
void i2c_master_init(uint16_t clk){
    (void) clk;
    struct rcc_reg_map *rcc = RCC_BASE;

    //SCL setup
    gpio_init(I2C1_SCL_PORT, I2C1_SCL_PIN, MODE_ALT, OUTPUT_OPEN_DRAIN,
              OUTPUT_SPEED_LOW, PUPD_NONE, I2C1_ALT);

    //SDA setip
    gpio_init(I2C1_SDA_PORT, I2C1_SDA_PIN, MODE_ALT, OUTPUT_OPEN_DRAIN, 
              OUTPUT_SPEED_LOW, PUPD_NONE, I2C1_ALT);

    rcc->apb1_enr |= I2C1_CLK_EN;

    struct i2c_reg_map *i2c = I2C1_BASE;
    i2c->CR2 |= CR2_VALUE;
    i2c->CCR |= CLK_VALUE;
    i2c->TRISE = TRISE_VALUE;
    i2c->CR1 |= I2C1_PERIPH_EN;
    i2c->CR1 |= ACK_BIT;

    return;
}

/*
 * Generates a Start condition and waits until it is sent.
 * Args: none. Returns: nothing.
 * Side effects: blocks until the bus is free and SB is set.
 */
void i2c_master_start() {
    struct i2c_reg_map *i2c = I2C1_BASE;
    while (i2c->SR2 & BUSY_BIT) {
        //wait for bus to be free
    };

    i2c->CR1 |= START;

    //wait for start
    while (!(i2c->SR1 & SB_BIT)) {
        
    };

    return;
}

/*
 * Generates a Stop condition and waits until it is sent.
 * Args: none. Returns: nothing.
 * Side effects: releases the bus.
 */
void i2c_master_stop() {
    struct i2c_reg_map *i2c = I2C1_BASE;
    //make stop condition
    i2c->CR1 |= STOP;
    while (i2c->CR1 & STOP) {
        //wait for stop to finish
    };
    return;
}

/*
 * Writes len bytes from buf to a slave in one transaction.
 * Args: buf - bytes to send; len - number of bytes (must be at least 1);
 * slave_addr - 7-bit address, not shifted.
 * Returns: I2C_ACK (0) on success, I2C_NACK (-1) if the slave did not
 * acknowledge (a Stop is sent in that case).
 */
int i2c_master_write(uint8_t *buf, uint16_t len, uint8_t slave_addr){
    struct i2c_reg_map *i2c = I2C1_BASE;
    i2c_master_start();
    i2c->DR = slave_addr << 1;

    // Wait for address
    while (!(i2c->SR1 & ADDR_BIT)){
        if (i2c->SR1 & AF_BIT) {
            // AF must be cleared by software, and the bus released with a Stop
            i2c->SR1 &= ~AF_BIT;
            i2c_master_stop();
            return I2C_NACK;
        }
    };

    // questionable?
    // read SR1 and read SR2 to clear ADDR bit
    volatile uint32_t dummy = i2c->SR1;
    dummy = i2c->SR2;
    (void)dummy; // Prevent unused variable warning

    // Send the data
    for (uint16_t i = 0; i < len; i++) {

        // Wait until ready
        while (!(i2c->SR1 & TXE_BIT)) {
            if (i2c->SR1 & AF_BIT) {
                // AF must be cleared by software, and the bus released with a Stop
                i2c->SR1 &= ~AF_BIT;
                i2c_master_stop();
                return I2C_NACK;
            }
        }

        // Send one byte
        i2c->DR = buf[i];
    }

    // Wait until last byte is finished
    while (!(i2c->SR1 & BTF_BIT)) {
        if (i2c->SR1 & AF_BIT) {
            // AF must be cleared by software, and the bus released with a Stop
            i2c->SR1 &= ~AF_BIT;
            i2c_master_stop();
            return I2C_NACK;
        }
    };

    i2c_master_stop();
    return I2C_ACK;
}

/*
 * Not implemented; the lab only requires i2c_master_write.
 * Args: unused. Returns: 0.
 */
int i2c_master_read(uint8_t *buf, uint16_t len, uint8_t slave_addr){
    (void) buf;
    (void) len;
    (void) slave_addr;

    return 0;
}