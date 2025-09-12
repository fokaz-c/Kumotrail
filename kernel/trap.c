/**
 * @file trap.c
 * @brief C-level trap and interrupt handling for the KumoTrail kernel.
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
uint32_t *trap_handler_c(uint32_t *sp)
{
    uint32_t cause = read_csr(mcause);
    
    // Check if the trap was caused by an interrupt (MSB is set).
    if (cause & 0x80000000)
    {
        uint32_t interrupt_id = cause & 0x7FFFFFFF;
        switch (interrupt_id)
        {
            case 6: // Timer interrupt
                timer_handle_interrupt();
                //uart_puts("Tick\n"); // Debug output
                return schedule(sp); // Call the scheduler
                
            default:
                uart_puts("Unknown interrupt: ");
                // Could print the interrupt ID here
                uart_puts("\n");
                break;
        }
    }
    else
    {
        // Handle synchronous exceptions
        uart_puts("Exception occurred: ");
        // Could print the cause here
        uart_puts("\n");
        
        // For now, just return to the same task
        // In a real OS, you'd handle page faults, illegal instructions, etc.
    }
    
    // If it wasn't handled, return the same stack pointer
    return sp;
}