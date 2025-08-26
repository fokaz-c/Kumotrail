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
 * Version: 1.2 | Date: 24-08-2025 | Author/Maintainer: ClariS--
 * Notes: Refers to ESP32-C3 TRM Chapter 26. Direct register access requires
 * proper timing and initialization.
 */

#include "uart.h"
#include <stdint.h>

// --- Private Hardware Register Definitions ---

// System Registers (for Clock and Reset)
#define SYSREG_BASE_ADDR              0x600C0000U
#define SYSTEM_PERIP_CLK_EN0_REG      (*(volatile uint32_t*)(SYSREG_BASE_ADDR + 0x0010))
#define SYSTEM_PERIP_RST_EN0_REG      (*(volatile uint32_t*)(SYSREG_BASE_ADDR + 0x0018))

#define SYSTEM_UART_MEM_CLK_EN        (1U << 24)
#define SYSTEM_UART_CLK_EN            (1U << 2)
#define SYSTEM_UART_RST               (1U << 2)

// UART0 Register Definitions (TRM Chapter 26)
#define UART0_BASE_ADDR               0x60000000U
#define UART_FIFO_REG                 (*(volatile uint32_t*)(UART0_BASE_ADDR + 0x00))
#define UART_INT_CLR_REG              (*(volatile uint32_t*)(UART0_BASE_ADDR + 0x10))
#define UART_CLKDIV_REG               (*(volatile uint32_t*)(UART0_BASE_ADDR + 0x14))
#define UART_STATUS_REG               (*(volatile uint32_t*)(UART0_BASE_ADDR + 0x1C))
#define UART_CONF0_REG                (*(volatile uint32_t*)(UART0_BASE_ADDR + 0x20))
#define UART_CONF1_REG                (*(volatile uint32_t*)(UART0_BASE_ADDR + 0x24))
#define UART_CLK_CONF_REG             (*(volatile uint32_t*)(UART0_BASE_ADDR + 0x78))
#define UART_ID_REG                   (*(volatile uint32_t*)(UART0_BASE_ADDR + 0x80))

// CONF0 bits
#define UART_PARITY_EN                (1U << 1)
#define UART_BIT_NUM_SHIFT            2U
#define UART_STOP_BIT_SHIFT           4U
#define UART_TXFIFO_RST               (1U << 18)
#define UART_RXFIFO_RST               (1U << 17)

// CONF1 bits
#define UART_TXFIFO_EMPTY_THRHD_SHIFT 9U

// CLK_CONF bits
#define UART_SCLK_SEL_SHIFT           20U // 1 for APB_CLK
#define UART_SCLK_EN                  (1U << 22)
#define UART_RST_CORE                 (1U << 23)
#define UART_TX_SCLK_EN               (1U << 24)

// ID bits
#define UART_UPDATE_CTRL              (1U << 30)
#define UART_REG_UPDATE               (1U << 31)

// STATUS bits
#define UART_TXFIFO_CNT_SHIFT         16U
#define UART_TXFIFO_CNT_MASK          0x1FFU

// UART config
#define ESP32C3_APB_CLK_FREQ          80000000U
#define UART_FIFO_DEPTH               128U
#define UART_FIFO_THRESHOLD           (UART_FIFO_DEPTH - 1U)

// Data format
#define UART_DATA_BITS_8              3U
#define UART_STOP_BITS_1              1U
#define UART_INT_CLEAR_ALL            0x3FFFFFU


/**
 * @brief Sends a single character over UART0.
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
 * @note Must be called once before any other UART functions.
 */
void uart_init(void)
{
    // --- 1. System-Level Clock and Reset ---
    SYSTEM_PERIP_CLK_EN0_REG |= (SYSTEM_UART_CLK_EN | SYSTEM_UART_MEM_CLK_EN);
    
    // --- 2. UART Core Reset ---
    UART_CLK_CONF_REG |= UART_RST_CORE;
    SYSTEM_PERIP_RST_EN0_REG |= SYSTEM_UART_RST;
    SYSTEM_PERIP_RST_EN0_REG &= ~SYSTEM_UART_RST;
    UART_CLK_CONF_REG &= ~UART_RST_CORE;

    // --- 3. UART Register Synchronization Setup ---
    while (UART_ID_REG & UART_REG_UPDATE);
    UART_ID_REG &= ~UART_UPDATE_CTRL;

    // --- 4. Clock Source Selection and TX Enable ---
    UART_CLK_CONF_REG = (1 << UART_SCLK_SEL_SHIFT) | UART_SCLK_EN | UART_TX_SCLK_EN;

    // --- 5. Baud Rate Configuration ---
    uint32_t divisor_integer = ESP32C3_APB_CLK_FREQ / KUMOTRAIL_UART_BAUD_RATE;
    uint32_t remainder = ESP32C3_APB_CLK_FREQ % KUMOTRAIL_UART_BAUD_RATE;
    uint32_t divisor_fractional = (remainder * 16U) / KUMOTRAIL_UART_BAUD_RATE;
    UART_CLKDIV_REG = (divisor_fractional << 20U) | divisor_integer;

    // --- 6. Data Frame Format (8N1) ---
    uint32_t conf0_value = (UART_DATA_BITS_8 << UART_BIT_NUM_SHIFT)
                         | (UART_STOP_BITS_1 << UART_STOP_BIT_SHIFT);
    conf0_value &= ~UART_PARITY_EN;
    UART_CONF0_REG = conf0_value;

    // --- 7. TX FIFO Empty Threshold (Optional but Recommended) ---
    UART_CONF1_REG = (10 << UART_TXFIFO_EMPTY_THRHD_SHIFT);

    // --- 8. Final Synchronization ---
    UART_ID_REG |= UART_REG_UPDATE;
    while (UART_ID_REG & UART_REG_UPDATE);

    // --- 9. Local FIFO Reset and Interrupt Clear ---
    UART_CONF0_REG |= (UART_TXFIFO_RST | UART_RXFIFO_RST);
    UART_CONF0_REG &= ~(UART_TXFIFO_RST | UART_RXFIFO_RST);
    UART_INT_CLR_REG = UART_INT_CLEAR_ALL;
}

/**
 * @brief Transmit a null-terminated string via UART0.
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

