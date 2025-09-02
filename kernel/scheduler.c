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
 * @file scheduler.c
 * @brief A simple, preemptive round-robin task scheduler for the KumoTrail kernel.
 * @version 1.0
 * @date 30-08-2025
 * @author fokaz-c
 *
 * 
 */

#include <scheduler.h>
#include <task.h>
#include <stddef.h>

#define CONFIG_MAX_TASKS 3
#define CONFIG_TASK_STACK_SIZE_WORDS 256 // 256 words = 1KB stack

// --- Private Scheduler Data ---

/** Array to hold the TCBs for all tasks. */
static task_t tasks[CONFIG_MAX_TASKS];

/** Array to hold the memory for each task's stack. */
static uint32_t task_stacks[CONFIG_MAX_TASKS][CONFIG_TASK_STACK_SIZE_WORDS];

/** Pointer to the TCB of the currently running task. */
static task_t *current_task = NULL;

/** The number of tasks currently created. */
static int num_tasks = 0;

/**
 * @brief Initializes the task scheduler.
 */
void scheduler_init(void)
{
    for (int i = 0; i < CONFIG_MAX_TASKS; i++)
    {
        tasks[i].state = TASK_UNUSED;
    }
    current_task = NULL;
    num_tasks = 0;
}

/**
 * @brief Creates a new task and adds it to the scheduler's run queue.
 */
int task_create(task_func_t func)
{
    if (num_tasks >= CONFIG_MAX_TASKS)
    {
        return -1; // No free TCBs
    }

    // Find the first unused TCB
    task_t *new_task = NULL;
    int task_id = -1;
    for (int i = 0; i < CONFIG_MAX_TASKS; i++)
    {
        if (tasks[i].state == TASK_UNUSED)
        {
            new_task = &tasks[i];
            task_id = i;
            break;
        }
    }

    if (!new_task) return -1;

    // --- Initialize the TCB ---
    new_task->state = TASK_READY;

    // --- Set up the circular linked list for round-robin ---
    if (num_tasks == 0)
    {
        new_task->next = new_task; // First task points to itself
    }
    else
    {
        // Insert the new task at the end of the list
        task_t *last_task = tasks[0].next;
        while (last_task->next != tasks[0].next)
        {
             last_task = last_task->next;
        }
        last_task->next = new_task;
        new_task->next = tasks[0].next;
    }

    // --- Initialize the task's stack ---
    uint32_t *stack_top = &task_stacks[task_id][CONFIG_TASK_STACK_SIZE_WORDS];
    stack_top--; *stack_top = (uint32_t)func; // ra (x1) - The return address is the task's function
    stack_top--; *stack_top = 0x0;            // gp (x3)
    stack_top--; *stack_top = 0x0;            // tp (x4)
    for (int i = 0; i < 28; i++)
    {
        stack_top--; *stack_top = 0x0; // t0-t6, s0-s11, a0-a7
    }

    new_task->stack_pointer = stack_top;
    num_tasks++;
    return task_id;
}

/**
 * @brief The core scheduler function, called from the timer interrupt.
 */
uint32_t *schedule(uint32_t *old_sp)
{
    // If there's no current task, start with the first one.
    if (!current_task)
    {
        current_task = &tasks[0];
        current_task->state = TASK_RUNNING;
        return current_task->stack_pointer;
    }

    // Save the stack pointer of the task that was just interrupted.
    current_task->stack_pointer = old_sp;
    current_task->state = TASK_READY;

    // Move to the next task in the circular list.
    current_task = current_task->next;
    current_task->state = TASK_RUNNING;

    // Return the stack pointer of the new task to run.
    return current_task->stack_pointer;
}

/**
 * @brief Starts the scheduler. This function does not return.
 */
void scheduler_start(void)
{
    // The actual start happens on the first timer interrupt.
    // We just need to enable interrupts and wait.
    // The trap handler will call schedule() which will launch the first task.
    enable_interrupts();
    while(1);
}
