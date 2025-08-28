
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


#ifndef KUMOTRAIL_INTERRUPT_H
#define KUMOTRAIL_INTERRUPT_H


/**
 * @file interrupt.h
 * @brief Interrupt controller interface for KumoTrail kernel (ESP32-C3)
 * @version 1.0
 * @date 29-08-2025
 * @author fokaz-c
 */




/**
 * @brief Hardware interrupt sources (TRM values).
 */
typedef enum {
    INTERRUPT_SOURCE_TIMG0_T0 = 32 /**< Timer Group 0, Timer 0 interrupt */
} interrupt_source_t;


/**
 * @brief Route a hardware interrupt source to a CPU interrupt line.
 * @param source Hardware interrupt source.
 * @param cpu_line Target CPU interrupt line (1-31).
 */
void interrupt_route(interrupt_source_t source, int cpu_line);

/**
 * @brief Enable a specific CPU interrupt line.
 * @param cpu_line CPU interrupt line to enable.
 */
void interrupt_enable(int cpu_line);

/**
 * @brief Disable a specific CPU interrupt line.
 * @param cpu_line CPU interrupt line to disable.
 */
void interrupt_disable(int cpu_line);

#endif // KUMOTRAIL_INTERRUPT_H
