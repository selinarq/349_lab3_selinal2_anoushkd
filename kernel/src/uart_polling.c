#include <unistd.h>

#include <gpio.h>
#include <rcc.h>
#include <uart_polling.h>

/** @brief The UART register map. */
struct uart_reg_map {
    volatile uint32_t SR;   /**< Status Register */
    volatile uint32_t DR;   /**<  Data Register */
    volatile uint32_t BRR;  /**<  Baud Rate Register */
    volatile uint32_t CR1;  /**<  Control Register 1 */
    volatile uint32_t CR2;  /**<  Control Register 2 */
    volatile uint32_t CR3;  /**<  Control Register 3 */
    volatile uint32_t GTPR; /**<  Guard Time and Prescaler Register */
};

/** @brief Base address for UART2 */
#define UART2_BASE  (struct uart_reg_map *) 0x40004400

/** @brief Enable  Bit for UART Config register */
#define UART_EN (1 << 13)

/** @brief Enable  Bit for UART clock */
#define RCC_APB1ENR_USART2 (1 << 17)

/** @brief Enable bit for TX RX 2nd and 3rd bit */
#define TXRX_EN (3 << 2) // 

/** @brief USARTDIV */
#define USARTDIV (8 << 4) | 11   // 0x0000008B

/**
 * @brief initializes UART to given baud rate with 8-bit word length, 1 stop bit, 0 parity bits
 *
 * @param baud Baud rate
 */
void uart_polling_init (int baud){
    (void) baud; /* This line is simply here to suppress the Unused Variable Error. */
                 /* You should remove this line in your final implementation */
    // TX PA_2, alternate function pins, push pull, lowest speed rate, none, 1 2 3 (7) 15
    gpio_init(GPIO_A, 2, MODE_ALT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_NONE, ALT7);

    // RX PA_3, alternate function pins, open drain, lowest speed rate, none, 1 2 3 (7) 15
    gpio_init(GPIO_A, 3, MODE_ALT, OUTPUT_OPEN_DRAIN, OUTPUT_SPEED_LOW, PUPD_NONE, ALT7);


    // what is the name of thing that has a bunch of registers: register map
    struct rcc_reg_map *rcc = RCC_BASE;
    rcc->apb1_enr |= RCC_APB1ENR_USART2; /* enable USART2 clock */  

    struct uart_reg_map *uart = UART2_BASE;

    // written into the BRR register
    uart->BRR = USARTDIV;

    // enabling TX RX (2nd 3rd bit)
    uart->CR1 |= TXRX_EN;

    // enable UART
    uart->CR1 |= UART_EN;

    return;
}

/**
 * @brief transmits a byte over UART
 *
 * @param c character to be sent
 */
void uart_polling_put_byte (char c){
    struct uart_reg_map *uart = UART2_BASE;
    // bit 7 is 0, means empty
    while (!(uart->SR & (1 << 7))) {
        
    }
    uart->DR = c;
    return;
}

/**
 * @brief receives a byte over UART
 */
char uart_polling_get_byte () {
    struct uart_reg_map *uart = UART2_BASE;
    // bit 5 is 0, means not empty
    while (!(uart->SR & (1 << 5))) {
        // wait
    }

    return (char)(uart->DR);
}