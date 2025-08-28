/*
 * Copyright 2025 fokaz-c
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file timer.c
 * @brief ESP32-C3 Timer Group 0 driver for kernel tick generation
 * @version 1.0
 * @date 29-08-2025
 * @author fokaz-c
 */

#include "timer.h"
#include "sysctl.h"
#include "interrupt.h"
#include <stdint.h>
#include <stddef.h>

// --- Private Hardware Register Definitions ---

#define TIMG_0_BASE_ADDR 0x6001F000U
#define TIMG_1_BASE_ADDR 0x60020000U

// ===== TIMG_0 Register Definitions =====
#define TIMG_0_T0CONFIG_REG         (*(volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0000))
#define TIMG_0_T0ALARMLO_REG        (*(volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0010))
#define TIMG_0_T0ALARMHI_REG        (*(volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0014))
#define TIMG_0_T0LOAD_REG           (*(volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0020))
#define TIMG_0_WDTFEED_REG          (*(volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0060))
#define TIMG_0_INT_ENA_TIMERS_REG   (*(volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x0070))
#define TIMG_0_INT_CLR_TIMERS_REG   (*(volatile uint32_t*)(TIMG_0_BASE_ADDR + 0x007C))

// ===== TIMG_1 Register Definitions =====
#define TIMG_1_T0CONFIG_REG         (*(volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0000))
#define TIMG_1_T0LO_REG             (*(volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0004))
#define TIMG_1_T0HI_REG             (*(volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0008))
#define TIMG_1_T0UPDATE_REG         (*(volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x000C))
#define TIMG_1_T0ALARMLO_REG        (*(volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0010))
#define TIMG_1_T0ALARMHI_REG        (*(volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0014))
#define TIMG_1_T0LOADLO_REG         (*(volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0018))
#define TIMG_1_T0LOADHI_REG         (*(volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x001C))
#define TIMG_1_T0LOAD_REG           (*(volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0020))
#define TIMG_1_WDTFEED_REG          (*(volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0060))
#define TIMG_1_INT_ENA_TIMERS_REG   (*(volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x0070))
#define TIMG_1_INT_CLR_TIMERS_REG   (*(volatile uint32_t*)(TIMG_1_BASE_ADDR + 0x007C))


// --- Bit Masks for TIMG0 ---
#define TIMG_0_T0_EN                  (1U << 31)
#define TIMG_0_T0_INCREASE            (1U << 30)
#define TIMG_0_T0_AUTORELOAD          (1U << 29)
#define TIMG_0_T0_DIVIDER_SHIFT       13
#define TIMG_0_T0_ALARM_EN            (1U << 10)
#define TIMG_0_T0_INT_ENA             (1U << 0)
#define TIMG_0_T0_INT_CLR             (1U << 0)

// --- Configuration Constants ---
#define KERNEL_TICK_HZ              100
#define TIMG_CLOCK_FREQ             80000000
#define TIMG_PRESCALER              1600
#define TIMG_ALARM_VALUE            (TIMG_CLOCK_FREQ / TIMG_PRESCALER / KERNEL_TICK_HZ)
#define TIMER_INTERRUPT_LINE        6

/** @brief Static callback function pointer for timer interrupts */
static void (*timer_callback)(void) = NULL;

/**
 * @brief Set the timer interrupt callback function
 * @param callback Function to call on each timer interrupt, or NULL to disable
 */
void timer_set_callback(void (*callback)(void))
{
    timer_callback = callback;
}

/**
 * @brief Initialize Timer Group 0 Timer 0 for 100Hz periodic interrupts
 * 
 * Configures TIMG0_T0 with prescaler and alarm value to generate interrupts
 * at KERNEL_TICK_HZ frequency. Sets up interrupt routing and enables the timer.
 */
void timer_init(void)
{
   sysctl_enable_clock(PERIPH_TIMG0);
   sysctl_reset_peripheral(PERIPH_TIMG0);

   TIMG_0_T0CONFIG_REG &= ~TIMG_0_T0_EN;

   TIMG_0_T0ALARMLO_REG = TIMG_ALARM_VALUE;
   TIMG_0_T0ALARMHI_REG = 0;

   TIMG_0_T0CONFIG_REG = (TIMG_0_T0CONFIG_REG & ~(0xFFFF << TIMG_0_T0_DIVIDER_SHIFT)) |
                         (TIMG_PRESCALER << TIMG_0_T0_DIVIDER_SHIFT) |
                         TIMG_0_T0_INCREASE | TIMG_0_T0_AUTORELOAD;

   interrupt_route(INTERRUPT_SOURCE_TIMG0_T0, TIMER_INTERRUPT_LINE);
   interrupt_enable(TIMER_INTERRUPT_LINE);
   TIMG_0_INT_ENA_TIMERS_REG |= TIMG_0_T0_INT_ENA;

   TIMG_0_T0LOAD_REG = 0;
   TIMG_0_T0CONFIG_REG |= (TIMG_0_T0_EN | TIMG_0_T0_ALARM_EN);
}

/**
 * @brief Timer interrupt handler
 * 
 * Called when TIMG0_T0 alarm fires. Feeds the watchdog, clears the interrupt,
 * calls the user callback if set, and re-enables the alarm.
 */
void timer_handle_interrupt(void)
{
    TIMG_0_WDTFEED_REG = 1;

    TIMG_0_INT_CLR_TIMERS_REG = TIMG_0_T0_INT_CLR;

    if (timer_callback) {
        timer_callback();
    }
    TIMG_0_T0CONFIG_REG |= TIMG_0_T0_ALARM_EN;
}