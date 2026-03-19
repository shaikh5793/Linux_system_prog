# Dynamic Library Loading with RTLD Flags

<!--
Copyright (c) 2024 Techveda
Author: Raghu Bharadwaj
SPDX-License-Identifier: MIT
-->

This directory demonstrates advanced concepts of runtime dynamic library loading, focusing on different RTLD (Run-Time Loader) flags and their impact on symbol resolution and library behavior.

## Overview

Runtime dynamic loading allows programs to load shared libraries during execution using the `dlopen()` family of functions. The behavior of library loading and symbol resolution is controlled by various RTLD flags passed to `dlopen()`.

## Files Description

- `app.c` - Main application demonstrating different RTLD flag behaviors
- `liba.c` - First shared library containing functions `a()` and `b()`
- `libb.c` - Second shared library containing functions `x()` and `y()`, with `y()` calling external `b()`
- `Makefile` - Build configuration for creating shared libraries and executable

## RTLD Flags Overview

### RTLD_NOW vs RTLD_LAZY

#### RTLD_NOW
- **Immediate Resolution**: All undefined symbols are resolved immediately when `dlopen()` is called
- **Fail Fast**: If any symbol cannot be resolved, `dlopen()` fails immediately
- **Performance**: Higher load time overhead but no runtime resolution delays
- **Reliability**: Catches missing symbols early, preventing runtime failures

```c
// All symbols must be resolvable at load time
ptr = dlopen("./liba.so", RTLD_NOW);
```

#### RTLD_LAZY
- **Deferred Resolution**: Symbols are resolved only when first accessed
- **Lazy Loading**: `dlopen()` succeeds even if some symbols are unresolvable
- **Performance**: Faster load time but potential runtime resolution overhead
- **Risk**: May fail at runtime when accessing unresolved symbols

```c
// Symbols resolved on first use
ptr = dlopen("./liba.so", RTLD_LAZY);
```

### RTLD_GLOBAL vs Local (Default)

#### RTLD_GLOBAL
- **Global Symbol Table**: Library symbols are added to the global symbol namespace
- **Visibility**: Symbols become available to subsequently loaded libraries
- **Symbol Sharing**: Allows symbol resolution between dynamically loaded libraries
- **Dependency Resolution**: Essential when libraries have interdependencies

```c
// Make liba.so symbols globally available
ptr1 = dlopen("./liba.so", RTLD_NOW | RTLD_GLOBAL);
// libb.so can now find symbols from liba.so
ptr2 = dlopen("./libb.so", RTLD_NOW);
```

#### Local Loading (Default)
- **Private Symbol Table**: Library symbols remain in local scope
- **Isolation**: Symbols are only accessible through explicit `dlsym()` calls
- **Encapsulation**: Prevents symbol conflicts between libraries
- **Limited Sharing**: Libraries cannot directly access each other's symbols

```c
// liba.so symbols remain private
ptr1 = dlopen("./liba.so", RTLD_NOW);
// libb.so cannot access liba.so symbols directly
ptr2 = dlopen("./libb.so", RTLD_NOW); // May fail if libb needs liba symbols
```

## Practical Examples

### Building the Example
```bash
raghu@techveda$ make all
```

This creates:
- `liba.so` - Contains functions `a()` and `b()`
- `libb.so` - Contains functions `x()` and `y()`, where `y()` calls `b()`
- `app` - Executable that demonstrates different loading strategies

### Example 1: Local Loading (Will Fail)
```bash
raghu@techveda$ ./app
```

If the code uses local loading:
```c
ptr1 = dlopen("./liba.so", RTLD_NOW);        // Local loading
ptr2 = dlopen("./libb.so", RTLD_NOW);        // Fails - can't find b()
```

**Result**: `dlopen()` for `libb.so` fails because `y()` function cannot find the `b()` symbol from `liba.so`.

### Example 2: Global Loading (Will Succeed)
```c
ptr1 = dlopen("./liba.so", RTLD_NOW | RTLD_GLOBAL);  // Global loading
ptr2 = dlopen("./libb.so", RTLD_NOW);                // Succeeds - finds b()
```

**Result**: `libb.so` loads successfully because `b()` symbol from `liba.so` is globally available.

## Symbol Resolution Process

### Global Loading Flow
1. Load `liba.so` with `RTLD_GLOBAL`
2. Add `liba.so` symbols (`a`, `b`) to global symbol table
3. Load `libb.so` with `RTLD_NOW`
4. Resolve `libb.so`'s undefined symbol `b` from global table
5. Successfully complete loading

### Local Loading Flow
1. Load `liba.so` locally
2. Keep `liba.so` symbols (`a`, `b`) in private scope
3. Attempt to load `libb.so`
4. Fail to resolve `libb.so`'s undefined symbol `b`
5. `dlopen()` fails

## Advanced Flag Combinations

### RTLD_LAZY | RTLD_GLOBAL
```c
ptr = dlopen("./liba.so", RTLD_LAZY | RTLD_GLOBAL);
```
- Symbols added to global table immediately
- Individual symbol resolution deferred until first use
- Good for large libraries with many symbols

### RTLD_NOW | RTLD_GLOBAL
```c
ptr = dlopen("./liba.so", RTLD_NOW | RTLD_GLOBAL);
```
- All symbols resolved and added to global table immediately
- Highest reliability but highest load-time cost
- Recommended for critical dependencies

## Debugging and Analysis

### Check Symbol Dependencies
```bash
raghu@techveda$ nm -D liba.so
raghu@techveda$ nm -D libb.so
raghu@techveda$ objdump -T libb.so | grep UND
```

### Monitor Dynamic Loading
```bash
raghu@techveda$ strace -e openat,mmap ./app
raghu@techveda$ ltrace -e dlopen,dlsym ./app
```

### Check Runtime Symbol Resolution
```bash
raghu@techveda$ LD_DEBUG=symbols ./app
raghu@techveda$ LD_DEBUG=bindings ./app
```

## Best Practices

### When to Use RTLD_GLOBAL
- Libraries have interdependencies
- Plugin architectures where plugins need to share symbols
- When creating a shared runtime environment

### When to Use Local Loading
- Independent library modules
- When symbol isolation is important
- To prevent symbol conflicts

### When to Use RTLD_NOW
- Production environments where reliability is critical
- When you want to catch missing dependencies early
- Real-time systems where runtime delays are unacceptable

### When to Use RTLD_LAZY
- Development and testing environments
- Large libraries where not all symbols are used
- When load time performance is critical

## Common Issues and Solutions

### Issue: `dlopen()` fails with "undefined symbol"
**Solution**: Use `RTLD_GLOBAL` for dependency libraries

### Issue: Symbol conflicts between libraries
**Solution**: Use local loading and explicit `dlsym()` calls

### Issue: Runtime symbol resolution failures with `RTLD_LAZY`
**Solution**: Switch to `RTLD_NOW` for early error detection

## Memory Management

```bash
raghu@techveda$ valgrind --tool=memcheck ./app
```

Always pair `dlopen()` with `dlclose()`:
```c
if (ptr) {
    dlclose(ptr);
    ptr = NULL;
}
```

## Clean up
```bash
raghu@techveda$ make clean
```

## Additional Notes

- `RTLD_GLOBAL` makes symbols available to the entire process
- Multiple libraries can be loaded with `RTLD_GLOBAL` 
- Symbol precedence follows load order (first loaded has priority)
- Use `dladdr()` to determine which library a symbol comes from
