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

/*
* @file - timer.h
* @purpose - public api for the timer driver
* @version 1.0
* @date 29-08-2025
* @author fokaz-c
*/
#ifndef TIMER_H
#define TIMER_H

/*
* @brief This function will initialize the peripheral to use the TIMER GROUP (TIMG)
* for an interrupt signal generation. 
*
*/
void timer_init(void);

/**
 * @brief Handles timer interrupts.
 */
void timer_handle_interrupt(void);

/**
 * @brief Sets a callback function to be executed on each timer tick.
 * @param callback The function pointer to the callback.
 */
void timer_set_callback(void (*callback)(void));


#endif
