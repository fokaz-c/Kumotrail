/*
 * Copyright 2025 fokaz-c
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __KUMOTRAIL_UART_H__
#define __KUMOTRAIL_UART_H__

/**
 * @file uart.h
 * @brief UART driver public interface for ESP32-C3 serial communication
 *
 * This header defines the public API for UART communication functionality
 * within the KumoTrail kernel. The interface provides essential serial
 * communication operations for debugging, logging, and external device
 * communication on the ESP32-C3 platform.
 *
 * Hardware Support:
 * - ESP32-C3 UART0 peripheral (primary debug interface)
 * - Standard GPIO pin assignments (TX: GPIO21, RX: GPIO20)
 * - 115200 baud rate with 8N1 configuration
 * - Polling-based transmission for bare-metal reliability
 *
 * @version 1.0
 * @date 23-08-2025
 * @author fokaz-c
 * @maintainer fokaz-c
 *
 * @changelog
 * Version 1.0 - 23-08-2025 - fokaz-c
 *   - Initial UART driver interface definition
 *   - Basic initialization and transmission functions
 *   - ESP32-C3 hardware integration support
 *
 * @note This interface is designed for single-threaded bare-metal usage.
 *       For multi-threaded environments, external synchronization is required.
 */

/*
 * Standard Library Dependencies
 *
 * Include explicit integer type definitions to ensure consistent data sizes
 * across different compilation environments and maintain hardware compatibility.
 */
#include <stdint.h>

/*
 * UART Driver Configuration Constants
 *
 * These constants define the operational parameters for the UART driver.
 * Modify these values to adjust communication settings or hardware assignments.
 */

/** Default UART baud rate for serial communication */
#define KUMOTRAIL_UART_BAUD_RATE    115200U

/** Maximum string length supported by uart_puts() function */
#define KUMOTRAIL_UART_MAX_STRING   1024U

/** GPIO pin number for UART transmit signal */
#define KUMOTRAIL_UART_TX_PIN       21U

/** GPIO pin number for UART receive signal */
#define KUMOTRAIL_UART_RX_PIN       20U

/*
 * Public Function Declarations
 *
 * The following functions comprise the complete public interface for UART
 * operations within the KumoTrail kernel. All functions are designed for
 * bare-metal operation without dynamic memory allocation dependencies.
 */

/**
 * @brief Initialize UART0 peripheral for serial communication
 *
 * Performs complete hardware initialization of the ESP32-C3 UART0 peripheral
 * including clock configuration, GPIO pin assignment, communication format
 * setup, and transmitter enablement. This function must be called once during
 * system boot before attempting any UART communication operations.
 *
 * Initialization Process:
 * - Configures UART clock divisor for target baud rate
 * - Sets communication format to 8N1 (8 data bits, no parity, 1 stop bit)
 * - Routes UART signals to designated GPIO pins
 * - Enables transmit FIFO and clears interrupt state
 * - Validates hardware register configuration
 *
 * Hardware Requirements:
 * - ESP32-C3 system clock must be stable (typically 80MHz)
 * - GPIO pins 20 and 21 must be available for UART usage
 * - UART0 peripheral must not be used by bootloader or other code
 *
 * @pre System clock initialization must be completed
 * @pre GPIO system must be functional
 * @post UART0 is ready for transmission operations
 * @post GPIO pins are configured for UART signals
 *
 * @note This function performs polling-based hardware verification
 * @note Initialization failure results in undefined UART behavior
 * @note Function execution time: approximately 10-50 microseconds
 *
 * @see uart_puts() for string transmission after initialization
 */
void uart_init(void);

/**
 * @brief Transmit null-terminated string via UART0
 *
 * Sends each character of the input string sequentially over the UART
 * interface using polling-based transmission with hardware flow control.
 * The function blocks until the entire string has been transmitted or
 * a null terminator is encountered.
 *
 * Transmission Characteristics:
 * - Character-by-character sequential transmission
 * - Hardware FIFO flow control prevents data loss
 * - Automatic null-termination detection
 * - Blocking operation until completion
 * - No character encoding conversion (raw byte transmission)
 *
 * Performance Considerations:
 * - Transmission rate limited by configured baud rate (115200 bps)
 * - CPU blocks during entire transmission process
 * - Typical character transmission time: ~87 microseconds at 115200 baud
 * - String length directly affects blocking duration
 *
 * Error Handling:
 * - NULL pointer input results in immediate return (safe no-op)
 * - No buffer overflow protection (relies on null termination)
 * - Hardware transmission errors are not detected or reported
 *
 * @param s Pointer to null-terminated string for transmission
 *          Must point to valid memory containing null-terminated data
 *          Maximum practical length limited by system responsiveness needs
 *
 * @pre uart_init() must have been called successfully
 * @pre Input string must be null-terminated
 * @pre String memory must remain valid during function execution
 *
 * @post All characters transmitted via UART0 (excluding null terminator)
 * @post CPU execution blocked until transmission completion
 *
 * @note Function is not reentrant - external synchronization required
 * @note No flow control from receiving device is implemented
 * @note Consider transmission time impact on real-time system requirements
 *
 * @warning Long strings may cause system responsiveness issues
 * @warning NULL pointer input performs no operation (silent failure)
 *
 * Usage Example:
 * @code
 * uart_init();
 * uart_puts("KumoTrail kernel initialized\n");
 * uart_puts("System ready for operation\n");
 * @endcode
 *
 * @see uart_init() for required initialization before usage
 */
void uart_puts(const char *s);

/*
 * Future Enhancement Opportunities
 *
 * The current UART interface provides essential functionality for bare-metal
 * kernel operation. Consider these enhancements for expanded capability:
 *
 * Potential Additions:
 * - uart_putc() for single character transmission
 * - uart_printf() for formatted output support
 * - uart_gets() for string reception functionality
 * - uart_set_baud_rate() for runtime baud rate modification
 * - uart_get_status() for hardware status monitoring
 * - Interrupt-driven transmission for non-blocking operation
 * - DMA support for high-performance bulk data transfer
 * - Multi-UART instance support (UART1, UART2)
 * - Hardware flow control (RTS/CTS) implementation
 * - Error detection and reporting mechanisms
 */

#endif /* __KUMOTRAIL_UART_H__ */

/*
 * End of UART Driver Public Interface
 *
 * This header provides the complete public API for UART communication within
 * the KumoTrail kernel environment. The interface is designed for simplicity,
 * reliability, and bare-metal operation without external dependencies beyond
 * standard C library integer types.
 *
 * Integration Notes:
 * - Include this header in any source file requiring UART functionality
 * - Link against uart.c implementation for complete driver functionality
 * - Ensure proper initialization sequence in system boot code
 * - Consider timing implications of blocking transmission operations
 */