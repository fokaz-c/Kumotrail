/**
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
 * @file trap.c
 * @brief C-level trap and interrupt handling for the KumoTrail kernel.
 * @version 1.0
 * @date 29-08-2025
 * @author fokaz-c
 */

#include "trap.h"
#include "timer.h"
#include "uart.h"
#include <stdint.h>

/* RISC-V CSR register definitions */
#define CSR_MSTATUS 0x300
#define CSR_MTVEC 0x305
#define CSR_MCAUSE 0x342
#define MSTATUS_MIE_BIT (1U << 3)

/* Assembly trap handler forward declaration */
extern void _trap_handler(void);

/* CSR write macro - requires compile-time constant CSR address */
#define write_csr(csr, value) \
({ \
    uint32_t __v = (uint32_t)(value); \
    asm volatile ("csrw " #csr ", %0" \
                  : : "r"(__v) \
                  : "memory"); \
})

/* CSR read macro - requires compile-time constant CSR address */
#define read_csr(csr) \
({ \
    uint32_t __v; \
    asm volatile ("csrr %0, " #csr \
                  : "=r"(__v) : : "memory"); \
    __v; \
})

/**
 * Initialize the trap system by setting machine trap vector
 */
void trap_init(void)
{
    asm volatile ("csrw mtvec, %0" : : "r"((uint32_t)_trap_handler));
}

/**
 * Enable machine-level interrupts by setting MIE bit in mstatus
 */
void enable_interrupts(void)
{
    uint32_t mstatus = read_csr(mstatus);
    mstatus |= MSTATUS_MIE_BIT;
    write_csr(mstatus, mstatus);
}

/**
 * Main C-level trap handler called from assembly
 * Determines trap cause and dispatches to appropriate handler
 */
void trap_handler_c(void)
{
    uint32_t cause = read_csr(mcause);
    
    if (cause & 0x80000000)
    {
        uint32_t interrupt_id = cause & 0x7FFFFFFF;
        switch (interrupt_id)
        {
            case 6:
                timer_handle_interrupt();
                break;
            default:
                uart_puts("Unknown interrupt occurred\n");
                break;
        }
    }
    else
    {
        uart_puts("An exception occurred\n");
    }
}