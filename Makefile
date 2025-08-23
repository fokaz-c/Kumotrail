# KumoTrail Project Makefile
TARGET = build/KumoTrail-Koro.elf
TOOLCHAIN_PREFIX = riscv32-unknown-elf-
CC = $(TOOLCHAIN_PREFIX)gcc
AS = $(TOOLCHAIN_PREFIX)as
LD = $(TOOLCHAIN_PREFIX)ld
OBJCOPY = $(TOOLCHAIN_PREFIX)objcopy
QEMU_XTENSA = tools/qemu/bin/qemu-system-xtensa
QEMU_RISCV = tools/qemu/bin/qemu-system-riscv32

# Compiler flags
CFLAGS = -march=rv32imc -mabi=ilp32 -nostdlib -ffreestanding -g -Wall -I drivers/include
ASFLAGS = -march=rv32imc -mabi=ilp32
LDFLAGS = -T scripts/linker.ld

# Source files
ASM_SOURCES = arch/riscv/boot.S
C_SOURCES = app/main.c \
	drivers/uart.c

# Create build directory object paths
OBJECTS = $(patsubst %.c,build/%.o,$(C_SOURCES)) $(patsubst %.S,build/%.o,$(ASM_SOURCES))

# Ensure build directory exists
$(shell mkdir -p build/app build/drivers build/arch/riscv)

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
		$(QEMU_RISCV) -M esp32c3 -nographic -bios none -kernel $(TARGET); \
	elif command -v qemu-system-riscv32 >/dev/null 2>&1; then \
		qemu-system-riscv32 -M esp32c3 -nographic -bios none -kernel $(TARGET); \
	else \
		echo "Error: ESP32 QEMU not found. Run './download_esp_qemu.sh' first"; \
		exit 1; \
	fi

# Run in QEMU (ESP32 Xtensa) - if you need ESP32 classic support
run-esp32: $(TARGET)
	@echo "RUN $(TARGET) in ESP32 QEMU"
	@if [ -x "$(QEMU_XTENSA)" ]; then \
		$(QEMU_XTENSA) -M esp32 -nographic -kernel $(TARGET); \
	else \
		echo "Error: ESP32 QEMU not found. Run './download_esp_qemu.sh' first"; \
		exit 1; \
	fi

# Debug in QEMU with GDB
debug: $(TARGET)
	@echo "RUN $(TARGET) in ESP32-C3 QEMU for debugging"
	@if [ -x "$(QEMU_RISCV)" ]; then \
		$(QEMU_RISCV) -M esp32c3 -nographic -kernel $(TARGET) -s -S; \
	elif command -v qemu-system-riscv32 >/dev/null 2>&1; then \
		qemu-system-riscv32 -M esp32c3 -nographic -kernel $(TARGET) -s -S; \
	else \
		echo "Error: ESP32 QEMU not found. Run './download_esp_qemu.sh' first"; \
		exit 1; \
	fi

# Clean build
clean:
	@echo "CLEAN"
	rm -rf build

.PHONY: all run run-esp32 debug clean