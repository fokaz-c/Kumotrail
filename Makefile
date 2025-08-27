# KumoTrail Project Makefile
# This file automates the build process for the KumoTrail OS.

# -----------------------------------------------------------------------------
# Architecture Configuration
# -----------------------------------------------------------------------------
ARCH ?= riscv

# -----------------------------------------------------------------------------
# Variables
# -----------------------------------------------------------------------------

TARGET = build/KumoTrail-Koro.elf
TOOLCHAIN_PREFIX = riscv32-unknown-elf-

# Toolchain executables
CC = $(TOOLCHAIN_PREFIX)gcc
AS = $(TOOLCHAIN_PREFIX)as
LD = $(TOOLCHAIN_PREFIX)ld
OBJCOPY = $(TOOLCHAIN_PREFIX)objcopy

# QEMU paths and command
QEMU_RISCV = tools/qemu/bin/qemu-system-riscv32
# CORRECTED: Changed -M esp32c3-builtin to -M esp32c3
QEMU_CMD = -M esp32c3 -nographic -kernel $(TARGET)

# -----------------------------------------------------------------------------
# Compiler / Linker Flags
# -----------------------------------------------------------------------------

# C compiler flags.
CFLAGS = -MMD -MP -march=rv32imc -mabi=ilp32 -nostdlib -ffreestanding -g -Wall
CFLAGS += -I drivers/include
CFLAGS += -I include/KumoTrail
CFLAGS += -I arch/$(ARCH)/include/plat

# Assembly flags
ASFLAGS = -march=rv32imc -mabi=ilp32

# Linker flags
LDFLAGS = -T scripts/linker.ld

# -----------------------------------------------------------------------------
# Source Files
# -----------------------------------------------------------------------------

# Update wildcard paths to search in the correct directories.
C_SOURCES   = $(wildcard app/*.c drivers/*.c kernel/*.c arch/$(ARCH)/*.c)
ASM_SOURCES = $(wildcard arch/$(ARCH)/*.S)

# Map source files to object files in the build directory
OBJECTS     = $(patsubst %.c,build/%.o,$(C_SOURCES)) $(patsubst %.S,build/%.o,$(ASM_SOURCES))
DEPS        = $(OBJECTS:.o=.d)

# -----------------------------------------------------------------------------
# Build Rules
# -----------------------------------------------------------------------------

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "LD $@"
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

# Compile C files
build/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "CC $<"
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble assembly files
build/%.o: %.S
	@mkdir -p $(dir $@)
	@echo "AS $<"
	$(AS) $(ASFLAGS) -o $@ $<

# Run in QEMU (ESP32-C3 RISC-V)
run: $(TARGET)
	@echo "RUN $(TARGET) in ESP32-C3 QEMU"
	@if [ -x "$(QEMU_RISCV)" ]; then \
		$(QEMU_RISCV) $(QEMU_CMD); \
	elif command -v qemu-system-riscv32 >/dev/null 2>&1; then \
		qemu-system-riscv32 $(QEMU_CMD); \
	else \
		echo "Error: qemu-system-riscv32 not found."; \
		exit 1; \
	fi

# Debug in QEMU with GDB
debug: $(TARGET)
	@echo "RUN $(TARGET) in ESP32-C3 QEMU for debugging"
	@if [ -x "$(QEMU_RISCV)" ]; then \
		$(QEMU_RISCV) $(QEMU_CMD) -s -S; \
	elif command -v qemu-system-riscv32 >/dev/null 2>&1; then \
		qemu-system-riscv32 $(QEMU_CMD) -s -S; \
	else \
		echo "Error: qemu-system-riscv32 not found."; \
		exit 1; \
	fi

# Clean build
clean:
	@echo "CLEAN"
	rm -rf build

# Include generated dependency files
-include $(DEPS)

.PHONY: all run debug clean
