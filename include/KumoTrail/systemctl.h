#ifndef __KUMOTRAIL_SYSCTL_H__
#define __KUMOTRAIL_SYSCTL_H__

/**
 * @file sysctl.h
 * @brief Public interface for system-level peripheral control.
 *
 * This module provides a centralized way to manage the clocks and resets
 * for various hardware peripherals in the KumoTrail kernel.
 */

#include <stdint.h>

/**
 * @brief Enumeration of all controllable hardware peripherals.
 */
typedef enum
{
    PERIPH_UART0,
    PERIPH_UART1,
    PERIPH_TIMG0,
    PERIPH_TIMG1,
} peripheral_t;

/**
 * @brief Enables the clock for a specified peripheral.
 *
 * A peripheral's clock must be enabled before its registers can be accessed.
 *
 * @param peripheral The peripheral to enable.
 */
void sysctl_enable_clock(peripheral_t peripheral);

/**
 * @brief Resets a specified peripheral.
 *
 * This function asserts and then de-asserts the reset signal for a
 * peripheral, returning it to a clean, default state.
 *
 * @param peripheral The peripheral to reset.
 */
void sysctl_reset_peripheral(peripheral_t peripheral);

#endif /* __KUMOTRAIL_SYSCTL_H__ */
