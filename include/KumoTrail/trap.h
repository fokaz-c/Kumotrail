#ifndef __KUMOTRAIL_TRAP_H__
#define __KUMOTRAIL_TRAP_H__

/**
 * @file trap.h
 * @brief Public interface for trap and interrupt handling.
 * @version 1.0
 * @date 29-08-2025
 * @author fokaz-c
 */

/**
 * @brief Initializes the trap vector.
 *
 * Sets the CPU's machine trap vector (mtvec) to point to the low-level
 * assembly trap handler. This must be called once before interrupts
 * are enabled.
 */
void trap_init(void);

/**
 * @brief Enables machine-level interrupts globally.
 *
 * This function sets the Machine Interrupt Enable (MIE) bit in the
 * mstatus register, allowing the CPU to begin accepting interrupts.
 */
void enable_interrupts(void);

#endif /* __KUMOTRAIL_TRAP_H__ */