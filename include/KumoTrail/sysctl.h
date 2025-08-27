#ifndef __KUMOTRAIL_SYSCTL_H__
#define __KUMOTRAIL_SYSCTL_H__

/**
 * @file sysctl.h
 * @brief Public interface for system-level peripheral control.
 *
 * This module provides a centralized and abstracted API for managing the
 * clocks and reset signals of various hardware peripherals on the ESP32-C3.
 * It ensures that peripherals are properly enabled and initialized before use.
 *
 * @version 1.0
 * @date 28-08-2025
 * @author ClariS--
 * @maintainer ClariS--
 *
 * @note This module is essential for the proper functioning of all drivers
 * that interact with hardware peripherals.
 */

#include <stdint.h>

/**
 * @brief Enumeration of all controllable hardware peripherals.
 *
 * This enum defines a unique identifier for each peripheral managed by the
 * system control module. These identifiers are used to specify the target
 * for clock and reset operations.
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
 * This function writes to the appropriate system register to gate the clock
 * on for the target peripheral. It also handles shared resources, such as
 * the UART memory clock.
 *
 * @param peripheral The peripheral to enable (e.g., PERIPH_UART0).
 * @pre The system and APB clocks must be running.
 * @post The specified peripheral is clocked and can be configured.
 */
void sysctl_enable_clock(peripheral_t peripheral);

/**
 * @brief Resets a specified peripheral.
 *
 * This function asserts and then de-asserts the reset signal for a
 * peripheral via the system control registers. This action returns the
 * peripheral's hardware state to its default values, which is a critical
 * step during initialization.
 *
 * @param peripheral The peripheral to reset (e.g., PERIPH_TIMG0).
 * @pre The peripheral's clock must be enabled.
 * @post The specified peripheral's registers are reset to their default state.
 */
void sysctl_reset_peripheral(peripheral_t peripheral);

#endif /* __KUMOTRAIL_SYSCTL_H__ */
