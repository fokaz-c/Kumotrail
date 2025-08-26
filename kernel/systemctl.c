/**
 * @file sysctl.c
 * @brief Implementation of the system control module.
 */

#include "systemctl.h"    // Public API for this module
#include <stdint.h>    // For explicit integer types like uint32_t

// --- Private Hardware Register Definitions ---

/*
 * System Register Base Address
 * (Reference: ESP32-C3 TRM, Chapter 16, Table 3.3-3)
 */
#define SYSREG_BASE_ADDR 0x600C0000U

/*
 * Peripheral Clock Enable and Reset Registers
 */
#define SYSTEM_PERIP_CLK_EN0_REG (*(volatile uint32_t*)(SYSREG_BASE_ADDR + 0x0010))
#define SYSTEM_PERIP_RST_EN0_REG (*(volatile uint32_t*)(SYSREG_BASE_ADDR + 0x0018))

/*
 * Bitmasks for SYSTEM_PERIP_CLK_EN0_REG and SYSTEM_PERIP_RST_EN0_REG
 */
#define SYSTEM_UART_CLK_EN        (1U << 2)
#define SYSTEM_UART1_CLK_EN       (1U << 5)
#define SYSTEM_TIMERGROUP_CLK_EN  (1U << 13)
#define SYSTEM_TIMERGROUP1_CLK_EN (1U << 15)

#define SYSTEM_UART_RST           (1U << 2)
#define SYSTEM_UART1_RST          (1U << 5)
#define SYSTEM_TIMERGROUP_RST     (1U << 13)
#define SYSTEM_TIMERGROUP1_RST    (1U << 15)


/**
 * @brief Enables the clock for a specified peripheral.
 */
void sysctl_enable_clock(peripheral_t peripheral)
{
    uint32_t enable_bit = 0;

    switch (peripheral)
    {
        case PERIPH_UART0:
            enable_bit = SYSTEM_UART_CLK_EN;
            break;
        case PERIPH_UART1:
            enable_bit = SYSTEM_UART1_CLK_EN;
            break;
        case PERIPH_TIMG0:
            enable_bit = SYSTEM_TIMERGROUP_CLK_EN;
            break;
        case PERIPH_TIMG1:
            enable_bit = SYSTEM_TIMERGROUP1_CLK_EN;
            break;
    }

    if (enable_bit)
    {
        SYSTEM_PERIP_CLK_EN0_REG |= enable_bit;
    }
}

/**
 * @brief Resets a specified peripheral.
 */
void sysctl_reset_peripheral(peripheral_t peripheral)
{
    uint32_t reset_bit = 0;

    switch (peripheral)
    {
        case PERIPH_UART0:
            reset_bit = SYSTEM_UART_RST;
            break;
        case PERIPH_UART1:
            reset_bit = SYSTEM_UART1_RST;
            break;
        case PERIPH_TIMG0:
            reset_bit = SYSTEM_TIMERGROUP_RST;
            break;
        case PERIPH_TIMG1:
            reset_bit = SYSTEM_TIMERGROUP1_RST;
            break;
    }

    if (reset_bit)
    {
        // Assert the reset signal
        SYSTEM_PERIP_RST_EN0_REG |= reset_bit;
        // De-assert the reset signal
        SYSTEM_PERIP_RST_EN0_REG &= ~reset_bit;
    }
}
