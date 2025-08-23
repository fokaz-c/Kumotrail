/**
 * @file linker_script.ld
 * @brief KumoTrail Koro kernel linker script for ESP32-C3 bare-metal execution
 * 
 * This GNU ld linker script defines the memory layout and section organization
 * for the KumoTrail Koro real-time kernel running on the ESP32-C3 microcontroller.
 * The script transforms compiled object files into the final executable binary
 * with precise control over memory placement and symbol generation.
 *
 * Target Hardware: ESP32-C3 (RISC-V single-core, 32-bit)
 * Memory Model: Unified address space using internal SRAM
 * Execution Model: Bare-metal (no bootloader dependencies)
 *
 * @version 1.0
 * @date 2025
 * @author ClariS--
 * 
 * @changelog
 * Version 1.0 - 23-08-2025 - ClariS--
 *   - Initial creation of ESP32-C3 linker script
 *   - Implemented unified memory layout for bare-metal execution
 *   - Added comprehensive documentation and comments
 * 
 * @note For modification history, see git log or version control system
 */

/*
 * Program Entry Point Declaration
 *
 * The ENTRY directive specifies the symbol representing the program's initial
 * execution address. Upon hardware reset or boot, the processor will transfer
 * control to this symbol's address.
 *
 * Symbol Requirements:
 * - Must be defined in assembly source (typically boot.S)
 * - Responsible for hardware initialization and C runtime setup
 * - Should establish stack pointer before calling C functions
 */
ENTRY(_start)

/*
 * Physical Memory Region Definitions
 *
 * The ESP32-C3 memory architecture provides multiple SRAM regions accessible
 * through different bus interfaces. For this bare-metal kernel implementation,
 * we utilize the instruction bus access to Internal SRAM 0 to minimize
 * complexity and maximize performance.
 *
 * Memory Selection Rationale:
 * - Single unified memory region simplifies linker script logic
 * - Instruction bus provides optimal access for code execution
 * - 400KB capacity sufficient for real-time kernel requirements
 */
MEMORY
{
    /*
     * Primary RAM Region: ESP32-C3 Internal SRAM 0
     * 
     * Physical Characteristics:
     * Base Address:    0x4037C000 (instruction bus mapping)
     * Size:            400KB (409,600 bytes)
     * Access Pattern:  Read/Write/Execute permissions
     * Bus Interface:   Accessible via instruction and data buses
     * 
     * This region will contain all program sections including executable
     * code, initialized data, uninitialized variables, and runtime stack.
     */
    RAM (rwx) : ORIGIN = 0x4037C000, LENGTH = 400K
}

/*
 * Section Layout and Memory Organization
 *
 * The SECTIONS directive controls the placement of compiled code and data
 * sections within the target memory regions. Each section serves a specific
 * purpose in the program's memory layout and runtime behavior.
 *
 * Layout Strategy:
 * Sequential placement from low to high addresses ensures optimal memory
 * utilization and predictable section boundaries for runtime operations.
 */
SECTIONS
{
    /*
     * Initialize location counter to RAM region base address.
     * Subsequent sections will be placed sequentially from this point.
     */
    . = ORIGIN(RAM);

    /*
     * Executable Code Section (.text)
     *
     * Contains all executable machine instructions including:
     * - Compiled C/C++ function implementations
     * - Hand-written assembly routines
     * - Interrupt service routines and exception handlers
     * - Compiler-generated runtime support code
     *
     * Placement Rationale:
     * Located at memory base for optimal instruction fetch performance
     * and simplified address calculation during program execution.
     */
    .text :
    {
        /*
         * Include all text sections from input object files.
         * The wildcard pattern captures both primary .text sections
         * and compiler-generated subsections (.text.function_name).
         */
        *(.text .text.*)
    } > RAM

    /*
     * Read-Only Data Section (.rodata)
     *
     * Contains compile-time constant data that remains immutable during
     * program execution:
     * - String literals and character arrays
     * - Const-qualified global variables
     * - Lookup tables and configuration data
     * - Function pointer arrays and jump tables
     *
     * Memory Characteristics:
     * While logically read-only, the ESP32-C3 hardware does not enforce
     * write protection. Software discipline must maintain data integrity.
     */
    .rodata :
    {
        /*
         * Aggregate all read-only data sections from input files.
         * Compiler may generate multiple subsections for optimization.
         */
        *(.rodata .rodata.*)
    } > RAM

    /*
     * Initialized Data Section (.data)
     *
     * Contains global and static variables with explicit non-zero initial
     * values that must be established during program startup:
     * - Global variables: uint32_t system_clock = 80000000;
     * - Static variables: static uint8_t buffer[256] = {0xAA, 0x55, ...};
     * - Initialized structures and arrays
     *
     * Runtime Initialization:
     * Initial values are stored in the executable file and must be copied
     * to RAM by startup code before main() execution begins. This copying
     * operation is typically performed in the _start routine.
     */
    .data :
    {
        /*
         * Combine all initialized data sections from object files.
         * Includes both primary sections and compiler subsections.
         */
        *(.data .data.*)
    } > RAM

    /*
     * Uninitialized Data Section (.bss)
     *
     * Contains global and static variables that are either declared without
     * initializers or explicitly initialized to zero:
     * - Uninitialized globals: uint32_t error_count;
     * - Zero-initialized arrays: static char log_buffer[1024] = {0};
     * - NULL pointer initializations: static void *handle = NULL;
     *
     * Space Optimization:
     * This section consumes no space in the executable file, only reserves
     * memory addresses. Startup code must zero-fill this region before
     * executing any C code that might access these variables.
     */
    .bss :
    {
        /*
         * Enforce 8-byte alignment for efficient memory operations.
         * RISC-V architecture benefits from aligned data access patterns.
         */
        . = ALIGN(8);
        
        /*
         * Generate symbol marking BSS section start address.
         * Startup code uses this symbol to locate the beginning of the
         * memory region that requires zero-initialization.
         */
        __bss_start = .;
        
        /*
         * Include all BSS sections from input object files.
         * Compiler may create multiple BSS subsections for optimization.
         */
        *(.bss .bss.*)
        
        /*
         * Generate symbol marking BSS section end address.
         * Combined with __bss_start, enables startup code to calculate
         * the exact memory range requiring zero-initialization.
         *
         * Example usage in C startup code:
         * extern uint8_t __bss_start[], __bss_end[];
         * memset(__bss_start, 0, __bss_end - __bss_start);
         */
        __bss_end = .;
        
    } > RAM

    /*
     * Runtime Stack Configuration
     *
     * The ESP32-C3 RISC-V implementation uses a descending stack that grows
     * from high memory addresses toward lower addresses. This configuration
     * establishes the initial stack pointer value and reserves space for
     * function call frames, local variables, and interrupt handling.
     *
     * Stack Growth Pattern:
     * - Initial SP = __stack_top (highest available RAM address)
     * - Function calls decrement SP (push operations)
     * - Function returns increment SP (pop operations)
     * - Stack overflow occurs when SP drops below BSS end
     */
    .stack_dummy (NOLOAD) :
    {
        /*
         * Ensure stack pointer alignment meets RISC-V ABI requirements.
         * The architecture specification mandates 8-byte stack alignment
         * for optimal performance and compatibility.
         */
        . = ALIGN(8);
        
        /*
         * Calculate and assign initial stack pointer value.
         * Places stack at the highest available RAM address, providing
         * maximum separation from other memory sections.
         *
         * Stack Pointer Initialization:
         * Assembly startup code should load this symbol into the stack
         * pointer register before calling any C functions:
         * 
         * la sp, __stack_top
         */
        __stack_top = ORIGIN(RAM) + LENGTH(RAM);
        
    } > RAM

    /*
     * Section Elimination
     *
     * Remove unnecessary sections that would otherwise appear in the final
     * executable, reducing binary size and eliminating potential conflicts.
     * These sections are primarily used by hosted environments and debugging
     * tools that are not applicable in bare-metal embedded systems.
     */
    /DISCARD/ :
    {
        /*
         * Exception handling frame information used by C++ runtime
         * and debugging tools. Not required for bare-metal C programs.
         */
        *(.eh_frame)
        
        /*
         * Compiler identification and build information comments.
         * Provides no runtime value in embedded deployments.
         */
        *(.comment)
    }
}

/*
 * Final Memory Layout Documentation
 *
 * The linker will arrange sections in the following memory organization:
 *
 * Address Range: 0x4037C000 - 0x403E3FFF (400KB total)
 *
 * Memory Layout (Low to High Address):
 * 
 * 0x4037C000    .text section (executable machine code)
 *               |
 * text_end      .rodata section (constants and literals) 
 *               |
 * rodata_end    .data section (initialized global variables)
 *               |
 * data_end      .bss section (uninitialized variables)
 *               |
 * __bss_end     Available heap space (if dynamic allocation used)
 *               |
 *               |    <-- Stack grows downward
 *               |
 *               v
 * 0x403E3FFF    __stack_top (initial stack pointer location)
 *

 * Generated Symbols:
 * _start          - Program entry point (must be defined in boot.S)
 * __bss_start     - Beginning of zero-initialized data section
 * __bss_end       - End of zero-initialized data section
 * __stack_top     - Initial stack pointer value
 *
 * Critical Implementation Notes:
 * 1. Boot assembly code must initialize stack pointer using __stack_top
 * 2. Startup code must zero-fill BSS section before main() execution
 * 3. No hardware memory protection - all sections have RWX permissions
 * 4. Stack overflow detection requires software implementation
 * 5. Dynamic memory allocation (if used) should operate between BSS end
 *    and current stack pointer location
 */