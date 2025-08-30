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
 * @file task.h
 * @brief Task management structures and definitions.
 * @author fokaz-c
 */

#ifndef KUMOTRAIL_TASK_H
#define KUMOTRAIL_TASK_H

#include<stdint.h>

/*
 * @brief Enumeration of task states.
 */

typedef enum
{
    TASK_UNUSED, // The TCB is free and can be used for a new task.
    TASK_READY,  // The task is ready to run but not currently running.
    TASK_RUNNING // The task is currently executing.
} task_state_e;

/**
 * @brief Task control block structure.
 */

typedef struct {
    volatile uint32_t *stack_pointer;
    task_state_e state;
    volatile task_t *next;
} task_t;

#endif // KUMOTRAIL_TASK_H