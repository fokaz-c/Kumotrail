/*
 * @file trap.S
 * @brief RISC-V assembly trap handler for the KumoTrail kernel.
 */

.macro save_all_registers
    addi sp, sp, -124
    sw ra, 0(sp)   # x1
    sw gp, 8(sp)   # x3
    sw tp, 12(sp)  # x4
    sw t0, 16(sp)  # x5
    sw t1, 20(sp)  # x6
    sw t2, 24(sp)  # x7
    sw s0, 28(sp)  # x8
    sw s1, 32(sp)  # x9
    sw a0, 36(sp)  # x10
    sw a1, 40(sp)  # x11
    sw a2, 44(sp)  # x12
    sw a3, 48(sp)  # x13
    sw a4, 52(sp)  # x14
    sw a5, 56(sp)  # x15
    sw a6, 60(sp)  # x16
    sw a7, 64(sp)  # x17
    sw s2, 68(sp)  # x18
    sw s3, 72(sp)  # x19
    sw s4, 76(sp)  # x20
    sw s5, 80(sp)  # x21
    sw s6, 84(sp)  # x22
    sw s7, 88(sp)  # x23
    sw s8, 92(sp)  # x24
    sw s9, 96(sp)  # x25
    sw s10, 100(sp) # x26
    sw s11, 104(sp) # x27
    sw t3, 108(sp) # x28
    sw t4, 112(sp) # x29
    sw t5, 116(sp) # x30
    sw t6, 120(sp) # x31
.endm

.macro restore_all_registers
    lw ra, 0(sp)   # x1
    lw gp, 8(sp)   # x3
    lw tp, 12(sp)  # x4
    lw t0, 16(sp)  # x5
    lw t1, 20(sp)  # x6
    lw t2, 24(sp)  # x7
    lw s0, 28(sp)  # x8
    lw s1, 32(sp)  # x9
    lw a0, 36(sp)  # x10
    lw a1, 40(sp)  # x11
    lw a2, 44(sp)  # x12
    lw a3, 48(sp)  # x13
    lw a4, 52(sp)  # x14
    lw a5, 56(sp)  # x15
    lw a6, 60(sp)  # x16
    lw a7, 64(sp)  # x17
    lw s2, 68(sp)  # x18
    lw s3, 72(sp)  # x19
    lw s4, 76(sp)  # x20
    lw s5, 80(sp)  # x21
    lw s6, 84(sp)  # x22
    lw s7, 88(sp)  # x23
    lw s8, 92(sp)  # x24
    lw s9, 96(sp)  # x25
    lw s10, 100(sp) # x26
    lw s11, 104(sp) # x27
    lw t3, 108(sp) # x28
    lw t4, 112(sp) # x29
    lw t5, 116(sp) # x30
    lw t6, 120(sp) # x31
    addi sp, sp, 124
.endm

.global _trap_handler
.align 2

_trap_handler:
    save_all_registers
    
    # Pass the current stack pointer (sp) to trap_handler_c as an argument.
    # The argument goes in register a0.
    mv a0, sp

    call trap_handler_c

    # After trap_handler_c returns, the address of the *new* task's stack
    # will be in the return value register, a0.
    # We now switch stacks by moving that address into sp.
    mv sp, a0

    restore_all_registers
    mret
