/**
 * @file trap.c - FIXED VERSION
 * @brief C-level trap and interrupt handling that properly handles mepc for task switching
 */
#include "trap.h"
#include "timer.h"
#include "uart.h"
#include "scheduler.h"
#include <stdint.h>

// --- Private CSR Definitions ---
#define CSR_MSTATUS 0x300
#define CSR_MTVEC 0x305
#define CSR_MCAUSE 0x342
#define CSR_MEPC 0x341
#define MSTATUS_MIE_BIT (1U << 3)

// --- Forward declaration of the assembly trap handler ---
extern void _trap_handler(void);

// --- CSR Access Macros ---
#define write_csr(csr, val) \
    asm volatile("csrw " #csr ", %0" : : "r"(val))

#define read_csr(csr) ({ \
    uint32_t val; \
    asm volatile("csrr %0, " #csr : "=r"(val)); \
    val; \
})

// Flag to track if this is the first task switch
static int first_task_switch = 1;

void trap_init(void)
{
    write_csr(mtvec, (uint32_t)_trap_handler);
}

void enable_interrupts(void)
{
    uint32_t mstatus = read_csr(mstatus);
    mstatus |= MSTATUS_MIE_BIT;
    write_csr(mstatus, mstatus);
}

/**
 * @brief The C-level trap handler called from assembly.
 * @param sp The stack pointer of the interrupted task.
 * @return The stack pointer of the next task to run.
 */
static int need_mepc_update = 0;
static uint32_t new_mepc_value = 0;

uint32_t *trap_handler_c(uint32_t *sp)
{
    uint32_t cause = read_csr(mcause);
    
    if (cause & 0x80000000)
    {
        uint32_t interrupt_id = cause & 0x7FFFFFFF;
        switch (interrupt_id)
        {
            case 6: // Timer interrupt
                timer_handle_interrupt();
                
                uint32_t *new_sp = schedule(sp);
                
                // Always set mepc when switching tasks (safe approach)
                if (new_sp != sp) {
                    uint32_t task_entry = *new_sp;
                    write_csr(mepc, task_entry);
                }
                
                return new_sp;
                
            default:
                uart_puts("Unknown interrupt\n");
                break;
        }
    }
    else
    {
        uart_puts("Exception occurred\n");
    }
    
    return sp;
}