# Program Loading Process

<!--
Copyright (c) 2024 Techveda
Author: Raghu Bharadwaj
SPDX-License-Identifier: MIT
-->

This document outlines the four-phase process that occurs when a program is executed in a Linux system, from user interface to main() function execution.

## Overview

Program loading is a complex multi-stage process involving the shell, kernel, dynamic linker, and C runtime. Each phase has specific responsibilities in preparing the program for execution.

## Loading Phases

### 1. UI Phase (Shell)

The user interface (typically the shell) initiates the program loading process:

- **File Verification**: Verify physical existence of the file
- **Binary Compatibility Check**: Check if the file is a compatible executable binary
- **Process Creation**: Invoke `fork()` to set up address-space and PCB (Process Control Block)
- **Kernel Handoff**: Invokes `execve("./main", ...)` to pass the control to kernel's loader

### 2. Kernel Loader

The kernel takes control and prepares the process memory layout:

- **Program Header Analysis**: Lookup binary's program header table
  - Learn about size of code and data areas
- **Memory Mapping**: Map code and data blocks to address-space
- **Dynamic Linker Setup**: Bind link-loader and invoke its entry routine

### 3. Link-loader (Dynamic Linker)

The dynamic linker handles shared library dependencies:

- **Dynamic Section Analysis**: Lookup into dynamic section of the executable binary header
- **Shared Object Linking**: Links specified shared objects (`.so` files)
- **Runtime Handoff**: Pass control to `_start()` (C-runtime)

### 4. _start() (C Runtime)

The C runtime performs final initialization before calling main():

- **Environment Setup**:
  - Setup stack
  - Initialize threading
  - Parse command line arguments
  - Setup global data
  - Initialize heap

- **Constructor Execution**: Invoke constructors (functions marked with `__attribute__((constructor))`)

- **Main Invocation**: Call `main()` function

## Process Flow Diagram

```
User Command
     │
     ▼
┌─────────────┐
│  UI Phase   │ ← Shell/UI verifies file and creates process
│   (Shell)   │
└─────────────┘
     │ execve()
     ▼
┌─────────────┐
│   Kernel    │ ← Maps memory, loads binary, starts dynamic linker
│   Loader    │
└─────────────┘
     │
     ▼
┌─────────────┐
│    Link     │ ← Resolves shared libraries and dependencies
│   Loader    │
└─────────────┘
     │
     ▼
┌─────────────┐
│  _start()   │ ← Sets up runtime environment
│ C Runtime   │
└─────────────┘
     │
     ▼
┌─────────────┐
│   main()    │ ← User's main function begins execution
│  Function   │
└─────────────┘
```

## Key Components

### Process Control Block (PCB)
- Contains process state information
- Created during `fork()` system call
- Manages process resources and scheduling

### Program Headers
- ELF format structure describing memory segments
- Contains information about loadable segments
- Specifies virtual addresses and permissions

### Dynamic Section
- Contains information about shared library dependencies
- Lists required shared objects
- Specifies symbol tables and relocation information

### Constructors
- Functions executed before `main()`
- Typically used for library initialization
- Marked with `__attribute__((constructor))`


```bash
# Trace system calls during program loading
strace -e execve,mmap,openat ./main

# Show dynamic dependencies
ldd ./main

# Display program headers
readelf -l ./main

# Show dynamic section
readelf -d ./main
```

## Notes

- This process happens for every program execution
- Static binaries skip the link-loader phase
- Some steps may be optimized or cached by the system
- Modern systems may use additional security features (ASLR, PIE, etc.)
