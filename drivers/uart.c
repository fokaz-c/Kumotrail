/**
 * @file uart.c
 * @brief Bare-metal UART0 driver for ESP32-C3 (KumoTrail kernel)
 *
 * Provides direct register access for serial communication (8N1, 115200 bps)
 * with polling-based TX FIFO management. No SDK/HAL dependencies.
 *
 * Hardware:
 * - UART0, 128-byte TX/RX FIFOs, APB clock domain (~80MHz)
 * - Registers: FIFO, CLKDIV, CONF0, STATUS, INT_CLR
 *
 * Version: 1.0 | Date: 23-08-2025 | Author/Maintainer: ClariS--
 * Notes: Refers to ESP32-C3 TRM Chapter 26. Direct register access requires
 * proper timing and initialization.
 */

#include "uart.h"
#include <stdint.h>

// --- Hardware Register Definitions ---
#define UART0_BASE_ADDR    0x60000000U
#define UART_FIFO_REG      (*(volatile uint32_t*)(UART0_BASE_ADDR + 0x00))
#define UART_INT_CLR_REG   (*(volatile uint32_t*)(UART0_BASE_ADDR + 0x10))
#define UART_CLKDIV_REG    (*(volatile uint32_t*)(UART0_BASE_ADDR + 0x14))
#define UART_STATUS_REG    (*(volatile uint32_t*)(UART0_BASE_ADDR + 0x1C))
#define UART_CONF0_REG     (*(volatile uint32_t*)(UART0_BASE_ADDR + 0x20))

// --- Bit Masks and Constants ---

// CONF0 bits
#define UART_PARITY_EN_BIT  1U
#define UART_PARITY_EN      (1U << UART_PARITY_EN_BIT)
#define UART_BIT_NUM_SHIFT  2U
#define UART_STOP_BIT_SHIFT 4U
#define UART_TXFIFO_RST_BIT 18U
#define UART_TXFIFO_RST     (1U << UART_TXFIFO_RST_BIT)
#define UART_RXFIFO_RST_BIT 17U
#define UART_RXFIFO_RST     (1U << UART_RXFIFO_RST_BIT)

// STATUS bits
#define UART_TXFIFO_CNT_SHIFT 16U
#define UART_TXFIFO_CNT_MASK  0x1FFU

// UART config
#define ESP32C3_APB_CLK_FREQ  80000000U // This is a hardware-specific constant for the driver
#define UART_FIFO_DEPTH       128U
#define UART_FIFO_THRESHOLD   (UART_FIFO_DEPTH - 1U)

// Data format
#define UART_DATA_BITS_8   3U
#define UART_STOP_BITS_1   1U
#define UART_INT_CLEAR_ALL 0x3FFFFFU

/**
 * @brief Sends a single character over UART0.
 *
 * This function waits for space to be available in the hardware transmit
 * buffer (TX FIFO) by polling the status register. Once space is free,
 * it writes the given character to the FIFO register, which the hardware
 * then sends serially.
 *
 * @param c The character to send.
 */
static void uart_putc(char c)
{
    uint32_t fifo_count;

    do
    {
        fifo_count = (UART_STATUS_REG >> UART_TXFIFO_CNT_SHIFT) & UART_TXFIFO_CNT_MASK;
    } while (fifo_count >= UART_FIFO_THRESHOLD);

    UART_FIFO_REG = (uint32_t)c;
}

/**
 * @brief Initialize UART0 peripheral for serial communication.
 *
 * This function configures the UART0 hardware for operation. It performs
 * the following steps:
 * 1. Calculates and sets the baud rate divisor using the public
 * KUMOTRAIL_UART_BAUD_RATE constant from uart.h.
 * 2. Configures the data frame format to 8N1 (8 data bits, no parity,
 * 1 stop bit).
 * 3. Resets the hardware transmit (TX) and receive (RX) FIFOs.
 * 4. Clears all pending UART interrupts.
 *
 * @note Must be called once before any other UART functions.
 */
void uart_init(void)
{
    uint32_t divisor_integer = ESP32C3_APB_CLK_FREQ / KUMOTRAIL_UART_BAUD_RATE;
    uint32_t remainder = ESP32C3_APB_CLK_FREQ % KUMOTRAIL_UART_BAUD_RATE;
    uint32_t divisor_fractional = (remainder * 16U) / KUMOTRAIL_UART_BAUD_RATE;
    UART_CLKDIV_REG = (divisor_fractional << 20U) | divisor_integer;

    uint32_t conf0_value = (UART_DATA_BITS_8 << UART_BIT_NUM_SHIFT)
                         | (UART_STOP_BITS_1 << UART_STOP_BIT_SHIFT);
    conf0_value &= ~UART_PARITY_EN;
    UART_CONF0_REG = conf0_value;

    UART_CONF0_REG |= (UART_TXFIFO_RST | UART_RXFIFO_RST);
    UART_CONF0_REG &= ~(UART_TXFIFO_RST | UART_RXFIFO_RST);

    UART_INT_CLR_REG = UART_INT_CLEAR_ALL;
}

/**
 * @brief Transmit a null-terminated string via UART0.
 *
 * This function iterates through a given string and sends each character
 * one by one using the `uart_putc` function. The loop continues until
 * the null terminator ('\0') is reached. A safety check is included to
 * prevent crashing if a NULL pointer is passed.
 *
 * @param s Pointer to the null-terminated string to transmit.
 */
void uart_puts(const char *s)
{
    if (!s)
    {
        return;
    }

    while (*s)
    {
        uart_putc(*s++);
    }
}
