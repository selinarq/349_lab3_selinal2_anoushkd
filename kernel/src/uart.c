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
#include <rcc.h>
#include <uart.h>
#include <uart_polling.h>
#include <nvic.h>
#include <gpio.h>

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

#define BUFFER_SIZE 100

/** @brief Enable  Bit for UART Config register */
#define UART_EN (1 << 13)

/** @brief Enable  Bit for UART clock */
#define RCC_APB1ENR_USART2 (1 << 17)

/** @brief Enable bit for TX RX 2nd and 3rd bit */
#define TXRX_EN (3 << 2) 
#define RXNEIE_EN (1 << 5)
#define TXEIE_EN (1 << 7)
#define RXNE_EN (1 << 5)
#define TXE_EN (1 << 7)
#define TC_EN (1 << 6)

/** @brief USARTDIV */
#define USARTDIV (8 << 4) | 11   // 0x0000008B

void uart_init(int baud){
    (void) baud;
    // TX PA_2, alternate function pins, push pull, lowest speed rate, none, 1 2 3 (7) 15
    gpio_init(GPIO_A, 2, MODE_ALT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_NONE, ALT7);

    // RX PA_3, alternate function pins, open drain, lowest speed rate, none, 1 2 3 (7) 15
    gpio_init(GPIO_A, 3, MODE_ALT, OUTPUT_OPEN_DRAIN, OUTPUT_SPEED_LOW, PUPD_NONE, ALT7);


    // what is the name of thing that has a bunch of registers: register map
    struct rcc_reg_map *rcc = RCC_BASE;
    rcc->apb1_enr |= RCC_APB1ENR_USART2; /* enable USART2 clock */  

    struct uart_reg_map *uart = UART2_BASE;

    /* configure GPIOA pins for PA2/PA3 as AF, ensure gpio_init() called */
    // struct uart_reg_map *uart = UART2_BASE;
    // uart->CR1 |= UART_EN;

    // written into the BRR register
    uart->BRR = USARTDIV;

    // enabling TX RX (2nd 3rd bit)
    uart->CR1 |= TXRX_EN;

    // enable UART
    uart->CR1 |= UART_EN;

    // Enable receive interrupt when data ready
    uart->CR1 |= RXNEIE_EN;

    //clear odl requests and inteerupt CPU
    // nvic_clear_pending(38);
    nvic_irq(38, IRQ_ENABLE);

    return;
}

//initializing variables for buffer of transmitting chars
char tx_buffer[BUFFER_SIZE];
int tx_buf_head = 0;
int tx_buf_tail = 0;
int tx_len = 0;

//store char to send, adds to buffer
int uart_put_byte(char c){
    struct uart_reg_map *uart = UART2_BASE;

    uart->CR1 &= ~TXEIE_EN;

    int next = tx_buf_head + 1;
    if (next == BUFFER_SIZE) {
        next = 0;
    }

    if (tx_len == BUFFER_SIZE) {
        return -1;
    }

    tx_buffer[tx_buf_head] = c;
    tx_buf_head = next;
    tx_len++;
    uart->CR1 |= TXEIE_EN;

    return 0;
}

//initializing variables for buffer of recieving chars
char rx_buffer[BUFFER_SIZE];
int rx_buf_head = 0;
int rx_buf_tail = 0;
int rx_len = 0;


//gets char that was already recieved
int uart_get_byte(char *c){
  if (rx_len == BUFFER_SIZE) {
        return -1;
    }

    *c = rx_buffer[rx_buf_tail];

    rx_buf_tail = (rx_buf_tail + 1) % BUFFER_SIZE;
    rx_len--;

    return 0;
}

//takes char from buffer and sends through UART
void USART2_TX_IRQHandler() {
    struct uart_reg_map *uart = UART2_BASE;

    if (tx_len > 0) {
        uart->DR = tx_buffer[tx_buf_tail];
        tx_buf_tail = (tx_buf_tail + 1) % BUFFER_SIZE;
        tx_len--;
    }

    if (tx_len == 0) {
        uart->CR1 &= ~TXEIE_EN;   // nothing left to send, stop TXE interrupts
    }
    
}

//recieved char from buffer and puts in RX buffer
void USART2_RX_IRQHandler() {
    struct uart_reg_map *uart = UART2_BASE;

    if (rx_len < BUFFER_SIZE) {
        rx_buffer[rx_buf_head] = (char)uart->DR;
        rx_buf_head = (rx_buf_head + 1) % BUFFER_SIZE;
        rx_len++;
    }
}

//TX or RX interrupt handler
void USART2_IRQHandler() {
    struct uart_reg_map *uart = UART2_BASE;

    if ((uart->SR & RXNE_EN) && (uart->CR1 & RXNEIE_EN)) {
        USART2_RX_IRQHandler();
    }
    if ((uart->SR & TXE_EN) && (uart->CR1 & TXEIE_EN)) {
        USART2_TX_IRQHandler();
    }

    nvic_clear_pending(38);
}

//send all chars still in TX buffer
void uart_flush(){
    struct uart_reg_map *uart = UART2_BASE;
    uart->CR1 &= ~TXEIE_EN;

    //sned all remaining chars
    while (tx_len > 0) {
        while (!(uart->SR & TXE_EN)){
          //wait
        };

        uart->DR = tx_buffer[tx_buf_tail];

        tx_buf_tail = (tx_buf_tail + 1) % BUFFER_SIZE;
        tx_len--;
    }
    //wait for last char to be sent
    while (!(uart->SR & TC_EN))
    {
      //wait
    };
}