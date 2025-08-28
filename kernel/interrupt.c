
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
 * @file interrupt.c
 * @brief Driver for the ESP32-C3 Interrupt Matrix (KumoTrail kernel)
 * @version 1.0
 * @date 29-08-2025
 * @author fokaz-c
 */

#include "interrupt.h"
#include <stdint.h>

/**
 * @brief Base address for the interrupt matrix hardware.
 */
#define INTERRUPT_MATRIX_BASE_ADDR  0x600C2000U

/**
 * @brief Macro to access the interrupt source mapping register.
 */
#define INTERRUPT_SOURCE_MAP_REG(source) \
    (*(volatile uint32_t*)(INTERRUPT_MATRIX_BASE_ADDR + ((source) * 4)))

/**
 * @brief Register for enabling CPU interrupt lines.
 */
#define INTERRUPT_CORE0_CPU_INT_ENABLE_REG \
    (*(volatile uint32_t*)(INTERRUPT_MATRIX_BASE_ADDR + 0x0104))

/**
 * @brief Route a hardware interrupt source to a CPU interrupt line.
 * @param source Hardware interrupt source.
 * @param cpu_line Target CPU interrupt line (1-31).
 */
void interrupt_route(interrupt_source_t source, int cpu_line)
{
    INTERRUPT_SOURCE_MAP_REG(source) = cpu_line;
}

/**
 * @brief Enable a specific CPU interrupt line.
 * @param cpu_line CPU interrupt line to enable.
 */
void interrupt_enable(int cpu_line)
{
    if (cpu_line >= 0 && cpu_line < 32) {
        INTERRUPT_CORE0_CPU_INT_ENABLE_REG |= (1 << cpu_line);
    }
}

/**
 * @brief Disable a specific CPU interrupt line.
 * @param cpu_line CPU interrupt line to disable.
 */
void interrupt_disable(int cpu_line)
{
    if (cpu_line >= 0 && cpu_line < 32) {
        INTERRUPT_CORE0_CPU_INT_ENABLE_REG &= ~(1 << cpu_line);
    }
}
