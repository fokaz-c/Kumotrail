/**
 * @file main.c
 * @brief Main entry point for the KumoTrail kernel.
 */

#include <stdint.h>
#include <uart.h>
#include <timer.h>
#include <trap.h>
#include <scheduler.h>

// --- Task Functions ---

// In your main function:
void task1(void) {
    while(1) {
        uart_puts("Task 1 running\n");
        // Do some work
        for(volatile int i = 0; i < 100000; i++);
    }
}

void task2(void) {
    while(1) {
        uart_puts("Task 2 running\n");
        // Do some work  
        for(volatile int i = 0; i < 100000; i++);
    }
}

int main(void) {
    // Initialize system
    uart_init();
    timer_init();
    trap_init();
    
    // Initialize scheduler
    scheduler_init();
    
    // Create tasks
    task_create(task1);
    task_create(task2);
    
    uart_puts("Starting scheduler...\n");
    
    // Start multitasking (never returns)
    scheduler_start();
    
    return 0;
}

