/**
 * @file scheduler.c - PRODUCTION VERSION
 * @brief Stable, production-ready scheduler for KumoTrail kernel
 */
#include "scheduler.h"
#include "task.h"
#include "trap.h"
#include <uart.h>
#include <stddef.h>

#define CONFIG_MAX_TASKS 5
#define CONFIG_TASK_STACK_SIZE_WORDS 256

// Ensure proper alignment for RISC-V
#define STACK_ALIGN_MASK 0xFFFFFFF0U

static task_t tasks[CONFIG_MAX_TASKS];
// Ensure stacks are properly aligned
static uint32_t task_stacks[CONFIG_MAX_TASKS][CONFIG_TASK_STACK_SIZE_WORDS] __attribute__((aligned(16)));
static task_t *current_task = NULL;
static int num_tasks = 0;
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

int task_create(task_func_t func)
{
    if (num_tasks >= CONFIG_MAX_TASKS || func == NULL)
    {
        return -1;
    }
    
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
    
    if (!new_task)
    {
        return -1;
    }
    
    new_task->state = TASK_READY;
    new_task->task_id = task_id;
    
    // Set up circular linked list
    if (num_tasks == 0)
    {
        new_task->next = new_task;
        current_task = new_task;
    }
    else
    {
        new_task->next = current_task->next;
        current_task->next = new_task;
    }
    
    // Initialize stack with proper alignment
    uint32_t *stack_top = &task_stacks[task_id][CONFIG_TASK_STACK_SIZE_WORDS];
    
    // Ensure stack is properly aligned before we start
    stack_top = (uint32_t*)((uint32_t)stack_top & STACK_ALIGN_MASK);
    
    // Allocate 124 bytes (31 words) to match trap.S exactly
    stack_top -= 31;
    
    // Zero the entire context frame for clean initialization
    for (int i = 0; i < 31; i++) {
        stack_top[i] = 0;
    }
    
    // Set the task entry point at offset 0 (ra register position)
    stack_top[0] = (uint32_t)func;
    
    // Verify stack alignment
    if ((uint32_t)stack_top & 0xF) {
        uart_puts("WARNING: Stack not 16-byte aligned!\n");
    }
    
    new_task->stack_pointer = stack_top;
    num_tasks++;
    
    return task_id;
}

uint32_t *schedule(uint32_t *old_sp)
{
    if (num_tasks == 0 || current_task == NULL)
    {
        return old_sp;
    }

    // First task switch - don't save old context
    if (!scheduler_started)
    {
        scheduler_started = 1;
        current_task->state = TASK_RUNNING;
        return current_task->stack_pointer;
    }
    
    // Save current task context
    if (current_task->state == TASK_RUNNING)
    {
        // Verify the saved stack pointer is reasonable
        if ((uint32_t)old_sp < 0x3FC88000 || (uint32_t)old_sp > 0x3FCA0000) {
            uart_puts("WARNING: Suspicious stack pointer!\n");
        }
        
        current_task->stack_pointer = old_sp;
        current_task->state = TASK_READY;
    }
    
    // Find next ready task (simple round-robin)
    task_t *next_task = current_task->next;
    int attempts = 0;
    
    while (next_task->state != TASK_READY && attempts < CONFIG_MAX_TASKS)
    {
        next_task = next_task->next;
        attempts++;
    }
    
    // If we found a ready task different from current, switch to it
    if (next_task->state == TASK_READY)
    {
        current_task = next_task;
        current_task->state = TASK_RUNNING;
        
        // Verify the new stack pointer is reasonable  
        if ((uint32_t)current_task->stack_pointer < 0x3FC88000 || 
            (uint32_t)current_task->stack_pointer > 0x3FCA0000) {
            uart_puts("WARNING: Invalid new stack pointer!\n");
        }
        
        return current_task->stack_pointer;
    }
    
    // No other ready task, continue with current
    current_task->state = TASK_RUNNING;
    return current_task->stack_pointer;
}

void scheduler_start(void)
{
    if (num_tasks == 0 || current_task == NULL)
    {
        uart_puts("No tasks to start!\n");
        while(1) asm volatile("wfi");
    }
    
    uart_puts("Starting scheduler...\n");
    enable_interrupts();
    
    while(1)
    {
        asm volatile("wfi");
    }
}