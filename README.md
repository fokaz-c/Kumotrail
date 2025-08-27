# Kumotrail ☁️

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](https://github.com/fokaz-c/Kumotrail)
[![License](https://img.shields.io/badge/license-MIT-blue)](LICENSE)
[![RISC-V](https://img.shields.io/badge/arch-RISC--V-orange)](https://riscv.org/)
[![ESP32-C3](https://img.shields.io/badge/target-ESP32--C3-red)](https://www.espressif.com/en/products/socs/esp32-c3)

*A tiny, custom kernel for a weather-aware smart clock, built from scratch for the ESP32-C3.*

---

## 📖 Introduction

**Kumotrail** (a wordplay combining the Japanese kumo, 雲, meaning cloud, with "contrail") is an educational project to build a complete, bare-metal operating system for a smart desk clock. The system runs on an ESP32-C3 (RISC-V) and is developed entirely from the ground up, without relying on any vendor SDKs, HALs, or existing real-time operating systems.

The primary goal of this project is to explore and learn the fundamentals of operating system design, low-level driver development, and embedded systems programming.

---

## ✨ Core Concepts

- **Bare-Metal C:** The entire kernel and all drivers are written in C, directly accessing hardware registers.
- **Custom Kernel:** A minimal, preemptive kernel with a tick-based scheduler will be built from scratch.
- **Hardware Abstraction:** Drivers for peripherals (UART, Timers, GPIO, etc.) are written to be modular and cleanly separated from the kernel logic.
- **QEMU First:** Initial development is done in the QEMU emulator to separate software logic from hardware-specific debugging.

---

## 🚀 Project Roadmap

### **Phase 1: Foundation** - ✅ **Complete**
- [x] Bare-metal bootstrap - QEMU execution environment
- [x] UART driver - Debug output and communication
- [x] Basic build system - Makefile with cross-compilation

### **Phase 2: Kernel Core** - 🚧 **In Progress**
- [ ] Timer interrupts - System tick generation
- [ ] Trap handling - Exception and interrupt management
- [ ] Task scheduler - Preemptive multitasking

### **Phase 3: System Services & Application** - 📋 **Planned**
- [ ] GPIO, I2C/SPI, WiFi, and Display drivers
- [ ] Real-Time Clock, Weather API integration, and UI

---

## 🛠️ Development Setup

To build and run KumoTrail, you will need the following tools:

| Tool | Purpose |
|------|---------|
| `riscv32-unknown-elf-gcc` | Cross-compiler for RISC-V |
| `qemu-system-riscv32` | ESP32-C3 emulation |
| `make` | Build automation |
| `bear` | LSP support for IDEs (Optional) |

---

## ⚙️ Building and Running

### 1. **Clone the repository:**

```bash
git clone https://github.com/fokaz-c/Kumotrail.git
cd Kumotrail
```

### 2. **Build the kernel:**

```bash
make
```

### 3. **Run in QEMU:**

```bash
make run
```

### 4. **Run with GDB for debugging:**

```bash
make debug
```

Then, in another terminal, connect GDB:
```bash
riscv32-unknown-elf-gdb build/KumoTrail-Koro.elf
```

### 5. **Clean the build:**

```bash
make clean
```

---

## 📁 Project Structure

KumoTrail uses a modular, single-platform structure to keep code organized.

```
KumoTrail/
├── 📁 app/                  # Application layer (main.c)
├── 📁 arch/                 # Architecture-specific code (RISC-V)
│   └── riscv/
├── 📁 drivers/              # Hardware drivers (.c, .h, and private _regs.h)
│   └── include/
├── 📁 include/              # Public API headers
│   └── KumoTrail/
├── 📁 kernel/               # Core OS functionality
├── 📁 scripts/              # Build configuration (linker.ld)
├── 📄 Makefile              # Build system configuration
└── 📄 README.md             # This file
```

---

## 🤝 Contributing

We welcome contributions! Please see our contributing guidelines for details.

### Development Workflow

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes following the coding standards
4. Test thoroughly in QEMU
5. Submit a pull request

---

## 📄 License

This project is licensed under the Apache License 2.0 - see the LICENSE file for details.

---

## 🌟 Acknowledgments

- **Espressif Systems** for detailed ESP32-C3 documentation
- **The QEMU Community** for excellent emulation support
- **The OSdev Community** for invaluable bare-metal programming resources

---

<div align="center">

**Built with ❤️ for learning and exploration**

[⭐ Star this repo](../../stargazers) | [🐛 Report issues](../../issues) | [💡 Request features](../../issues/new)

</div>
