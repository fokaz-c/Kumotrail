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

/**
 * @file main.c
 * @brief Main entry point for the KumoTrail koro kernel.
 *
 * This file contains the primary C function that is called after the
 * initial assembly boot sequence. It is responsible for initializing
 * hardware drivers and starting the main application loop.
 */

#include "uart.h" // Include the public API for our UART driver

/**
 * @brief The main function of the KumoTrail OS.
 *
 * This function is called from the assembly startup code in boot.S.
 * It should never return.
 */
void main(void)
{
    // Initialize the UART driver so we can print messages.
    uart_init();

    // Send a "hello world" message from the kernel.
    uart_puts("hello world from kernel\n");

    uart_puts("KumoTrail koro kernel initialized\n");

    // Main should never return in a bare-metal environment.
    // We enter an infinite loop to halt the CPU.
    while (1) {
        // Future kernel tasks will be managed here.
    }
}
