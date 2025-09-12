/**
 * @file scheduler.c
 * @brief A simple, preemptive round-robin task scheduler for the KumoTrail kernel.
 * @version 1.1
 * @date 12-09-2025
 * @author ClariS--
 */
#include "scheduler.h"
#include "task.h"
#include "trap.h"
#include <uart.h>
#include <stddef.h> // For NULL

#define CONFIG_MAX_TASKS 5
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

/** Flag to indicate if the scheduler has been started. */
static int scheduler_started = 0;

void scheduler_init(void)
{
    for (int i = 0; i < CONFIG_MAX_TASKS; i++)
    {
        tasks[i].state = TASK_UNUSED;
        tasks[i].next = NULL;
        tasks[i].task_id = i;
        tasks[i].stack_pointer = NULL;
    }
    current_task = NULL;
    num_tasks = 0;
    scheduler_started = 0;
    uart_puts("Scheduler initialized\n");
}

/**
 * @brief Task wrapper function that calls the actual task function.
 * This ensures tasks never return.
 */
static void task_wrapper(void)
{
    // Get the current task
    if (current_task && current_task->stack_pointer)
    {
        // Extract the task function from the context frame
        // The task function address is stored at offset 0 (ra register)
        uint32_t *context = current_task->stack_pointer;
        task_func_t task_func = (task_func_t)context[0];
        
        // Call the actual task function
        if (task_func)
        {
            task_func();
        }
    }
    
    // If the task function returns, enter an infinite loop
    while (1)
    {
        // Could implement task cleanup here
        asm volatile("nop");
    }
}

int task_create(task_func_t func)
{
    if (num_tasks >= CONFIG_MAX_TASKS || func == NULL)
    {
        return -1; // No free TCBs or invalid function
    }
    
    task_t *new_task = NULL;
    int task_id = -1;
    
    // Find the first available TCB slot
    for (int i = 0; i < CONFIG_MAX_TASKS; i++)
    {
        if (tasks[i].state == TASK_UNUSED)
        {
            new_task = &tasks[i];
            task_id = i;
            break;
        }
    }
    
    if (!new_task)
    {
        return -1;
    }
    
    // Initialize the TCB
    new_task->state = TASK_READY;
    new_task->task_id = task_id;
    
    // Set up the circular linked list for round-robin
    if (num_tasks == 0)
    {
        new_task->next = new_task; // First task points to itself
        current_task = new_task;
    }
    else
    {
        // Insert the new task into the circular list
        new_task->next = current_task->next;
        current_task->next = new_task;
    }
    
    // Initialize the task's stack with a context frame that matches trap.S
    // Your assembly saves registers in this exact order and layout:
    
    // Get the top of the stack (grows downward)
    uint32_t *stack_top = &task_stacks[task_id][CONFIG_TASK_STACK_SIZE_WORDS];
    
    // Create the context frame that matches your save_all_registers macro
    // The stack frame size is 31 registers * 4 bytes = 124 bytes
    // But we work with word indices, so 31 words
    
    stack_top -= 31; // Allocate space for 31 registers
    
    // Initialize all registers to safe values
    // This matches the exact layout in your trap.S:
    stack_top[0] = (uint32_t)func;        // ra (x1) - return address to task function
    // stack_top[1] is x2 (sp) - not saved/restored in your assembly
    stack_top[1] = 0;                     // gap for x2
    stack_top[2] = 0;                     // gp (x3) 
    stack_top[3] = 0;                     // tp (x4)
    stack_top[4] = 0;                     // t0 (x5)
    stack_top[5] = 0;                     // t1 (x6)
    stack_top[6] = 0;                     // t2 (x7)
    stack_top[7] = 0;                     // s0 (x8)
    stack_top[8] = 0;                     // s1 (x9)
    stack_top[9] = 0;                     // a0 (x10)
    stack_top[10] = 0;                    // a1 (x11)
    stack_top[11] = 0;                    // a2 (x12)
    stack_top[12] = 0;                    // a3 (x13)
    stack_top[13] = 0;                    // a4 (x14)
    stack_top[14] = 0;                    // a5 (x15)
    stack_top[15] = 0;                    // a6 (x16)
    stack_top[16] = 0;                    // a7 (x17)
    stack_top[17] = 0;                    // s2 (x18)
    stack_top[18] = 0;                    // s3 (x19)
    stack_top[19] = 0;                    // s4 (x20)
    stack_top[20] = 0;                    // s5 (x21)
    stack_top[21] = 0;                    // s6 (x22)
    stack_top[22] = 0;                    // s7 (x23)
    stack_top[23] = 0;                    // s8 (x24)
    stack_top[24] = 0;                    // s9 (x25)
    stack_top[25] = 0;                    // s10 (x26)
    stack_top[26] = 0;                    // s11 (x27)
    stack_top[27] = 0;                    // t3 (x28)
    stack_top[28] = 0;                    // t4 (x29)
    stack_top[29] = 0;                    // t5 (x30)
    stack_top[30] = 0;                    // t6 (x31)
    
    // Set the stack pointer to point to our prepared context frame
    new_task->stack_pointer = stack_top;
    
    num_tasks++;
    return task_id;
}

uint32_t *schedule(uint32_t *old_sp)
{
    // Handle the case where we have no tasks
    if (num_tasks == 0 || current_task == NULL)
    {
        return old_sp; // No tasks to schedule
    }

    uart_puts("Scheduling...\n");

    // If this is the first call to schedule, we're switching from the
    // initial kernel context to the first task
    if (!scheduler_started)
    {
        uart_puts("First task start\n");

        scheduler_started = 1;
        current_task->state = TASK_RUNNING;
        return current_task->stack_pointer;
    }
    
    // Save the current task's context
    if (current_task->state == TASK_RUNNING)
    {   
        uart_puts("Saving current task context\n");
        current_task->stack_pointer = old_sp;
        current_task->state = TASK_READY;
    }
    
    uart_puts("Looking for next task\n");
    // Find the next ready task (round-robin)
    task_t *next_task = current_task->next;
    int attempts = 0;
    
    // Look for a ready task, but don't loop forever
    while (next_task->state != TASK_READY && attempts < CONFIG_MAX_TASKS)
    {
        uart_puts("Skipping non-ready task\n");
        next_task = next_task->next;
        attempts++;
    }
    
    // If no ready task found, stay with current task
    if (next_task->state != TASK_READY)
    {
        uart_puts("No other ready tasks, continuing current task\n");
        current_task->state = TASK_RUNNING;
        return current_task->stack_pointer;
    }
    
    // Switch to the next task
    current_task = next_task;
    current_task->state = TASK_RUNNING;
    uart_puts("Switching to next task\n");
    return current_task->stack_pointer;
}

void scheduler_start(void)
{
    if (num_tasks == 0 || current_task == NULL)
    {
        // No tasks to run, just halt
        while(1)
        {
            asm volatile("wfi"); // Wait for interrupt
        }
    }
    
    uart_puts("Starting scheduler\n");
    // Enable interrupts to start receiving timer interrupts
    enable_interrupts();
    
    // The scheduler will start on the first timer interrupt
    // Until then, we wait in this loop
    while(1)
    {
        asm volatile("wfi"); // Wait for interrupt - saves power
    }
}