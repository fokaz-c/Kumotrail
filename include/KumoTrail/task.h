#ifndef KUMOTRAIL_TASK_H
#define KUMOTRAIL_TASK_H

#include <stdint.h>

/**
 * @brief Represents the possible states of a task.
 */
typedef enum
{
    TASK_UNUSED,   // The TCB is free
    TASK_READY,    // The task is ready to run
    TASK_RUNNING,  // The task is currently running
    TASK_BLOCKED   // The task is blocked (for future use)
} task_state_e;

// Forward-declare the task_t struct so we can use a pointer to it inside itself.
typedef struct task_t task_t;

/**
 * @brief Task Control Block (TCB).
 * 
 * This structure holds all the necessary information to manage a task,
 * including its current state, stack pointer, and a link to the next
 * task in the scheduler's list.
 */
struct task_t
{
    uint32_t *stack_pointer;   // Pointer to the top of the task's stack
    task_state_e state;        // The current state of the task
    task_t *next;              // Pointer to the next task in the circular list
    int task_id;               // Task identifier
};

#endif /* KUMOTRAIL_TASK_H */