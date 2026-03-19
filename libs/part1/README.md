<!--
Copyright (c) 2024 Techveda
Author: Raghu Bharadwaj
SPDX-License-Identifier: MIT
-->

This directory demonstrates the concepts of static and dynamic linkage in Linux systems, showing how to create and use both static and shared libraries.

## Overview

### Static Linkage
Static linkage occurs at compile time where the library code is copied directly into the executable. The resulting binary is self-contained but larger in size.

### Dynamic Linkage
Dynamic linkage occurs at runtime where the library code is loaded when the program executes. This results in smaller executables and allows multiple programs to share the same library in memory.

## Files Description

- `lib1.c`, `lib2.c` - Source files containing library functions
- `lib.h` - Header file with function declarations
- `main.c` - Test application that uses the static library
- `libinit.c` - Shared library with constructor/destructor functions
- `runtime.c` - Application demonstrating runtime dynamic loading
- `Makefile` - Build configuration

## Building Libraries

### Creating Static Library with `ar` tool

The `ar` (archiver) tool is used to create static libraries from object files:

```bash
raghu@techveda$ gcc -Wall -Wextra -fPIC -c lib1.c -o lib1.o
raghu@techveda$ gcc -Wall -Wextra -fPIC -c lib2.c -o lib2.o
raghu@techveda$ ar rcs libtest.a lib1.o lib2.o
```

**ar command options:**
- `r` - Insert files into archive
- `c` - Create archive if it doesn't exist
- `s` - Write an index (equivalent to running ranlib)

### Creating Shared Library with `gcc`

GCC is used to create shared libraries (.so files):

```bash
raghu@techveda$ gcc -Wall -Wextra -fPIC -c libinit.c -o libinit.o
raghu@techveda$ gcc -shared -o libinit.so libinit.o
```

**gcc options for shared libraries:**
- `-fPIC` - Generate Position Independent Code
- `-shared` - Create a shared library

## Building and Running

### Build all targets
```bash
raghu@techveda$ make all
```

### Static Linking Example
```bash
raghu@techveda$ gcc -o main main.o -L. -ltest
raghu@techveda$ ./main
```

The `-L.` flag tells the linker to look in the current directory, and `-ltest` links against `libtest.a`.

### Dynamic Linking at Load Time
```bash
raghu@techveda$ export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
raghu@techveda$ ./main
```

### Runtime Dynamic Loading
```bash
raghu@techveda$ gcc -o runtime runtime.o -ldl
raghu@techveda$ ./runtime
```

The `-ldl` flag links against the dynamic loading library.

## Key Differences

| Aspect | Static Linking | Dynamic Linking |
|--------|----------------|-----------------|
| **Size** | Larger executable | Smaller executable |
| **Dependencies** | Self-contained | Requires library files |
| **Memory Usage** | Each process has its own copy | Shared among processes |
| **Load Time** | Faster (no library loading) | Slower (library loading required) |
| **Updates** | Requires recompilation | Library can be updated independently |
| **Portability** | More portable | Less portable (dependency issues) |

## Library Loading Mechanisms

### 1. Load-time Dynamic Linking
- Libraries are loaded when the program starts
- Resolved by the dynamic linker (`ld.so`)
- Uses `LD_LIBRARY_PATH` environment variable

### 2. Runtime Dynamic Loading
- Libraries are loaded during program execution
- Uses `dlopen()`, `dlsym()`, `dlclose()` functions
- Requires linking with `-ldl`

## Advanced Concepts

### Constructor/Destructor Functions
The `libinit.c` demonstrates library initialization and cleanup:

```c
void __attribute__((constructor)) init_routine(void);
void __attribute__((destructor)) fini_routine(void);
```

These functions are automatically called when the library is loaded/unloaded.

### Position Independent Code (PIC)
- Required for shared libraries
- Allows code to run at any memory address
- Generated with `-fPIC` compiler flag

## Debugging and Analysis Tools

### Check library dependencies
```bash
raghu@techveda$ ldd ./main
raghu@techveda$ objdump -p ./main
```

### List symbols in libraries
```bash
raghu@techveda$ nm libtest.a
raghu@techveda$ nm -D libinit.so
```

### Check library loading at runtime
```bash
raghu@techveda$ strace -e openat ./main
```

## Clean up
```bash
raghu@techveda$ make clean
```

This removes all object files, libraries, and executables.

## Notes

- Static libraries have `.a` extension (archive)
- Shared libraries have `.so` extension (shared object)
- Use `pkg-config` for complex library management in larger projects
- Consider using `rpath` for embedded library paths in production
