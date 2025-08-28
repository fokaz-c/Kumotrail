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
 * @brief Main entry point for the KumoTrail kernel.
 */

#include "uart.h"
#include "timer.h"
#include "trap.h"
#include "sysctl.h"

/**
 * @brief The kernel's tick handler.
 *
 * This function is registered as a callback and is executed by the timer
 * driver on every timer interrupt.
 */
void kernel_tick_handler(void)
{
    uart_puts("Tick!\n");
}

/**
 * @brief The main function of the KumoTrail OS.
 */
void main(void)
{
    // Initialize all hardware drivers and kernel modules
    uart_init();
    timer_init();
    trap_init();

    // Register our tick handler function with the timer driver.
    timer_set_callback(kernel_tick_handler);

    // Enable interrupts globally. The system is now live.
    enable_interrupts();

    uart_puts("KumoTrail has booted. Interrupts are enabled.\n");

    // The CPU will now idle here. The timer interrupt will periodically
    // call our handler and print "Tick!".
    while (1)
    {
        // This is the idle loop.
    }
}
