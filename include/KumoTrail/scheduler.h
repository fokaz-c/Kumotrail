#ifndef KUMOTRAIL_SCHEDULER_H
#define KUMOTRAIL_SCHEDULER_H

#include <stdint.h>

// Define the type for a task's main function.
typedef void (*task_func_t)(void);

/**
 * @brief Initializes the task scheduler.
 *
 * Sets up the initial task structures and prepares the scheduler to run.
 * Must be called once before creating any tasks.
 */
void scheduler_init(void);

/**
 * @brief Creates a new task and adds it to the scheduler.
 *
 * @param func A pointer to the function that the task will execute.
 * @return 0 on success, -1 on failure (e.g., max tasks reached).
 */
int task_create(task_func_t func);

/**
 * @brief Starts the multitasking scheduler.
 *
 * This function enables interrupts and jumps to the first task.
 * It does not return.
 */
void scheduler_start(void);

#endif /* KUMOTRAIL_SCHEDULER_H */