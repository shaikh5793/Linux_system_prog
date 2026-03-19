# Symbol Resolution and RTLD_DEEPBIND

<!--
Copyright (c) 2024 Techveda
Author: Raghu Bharadwaj
SPDX-License-Identifier: MIT
-->

This directory demonstrates the importance of `RTLD_DEEPBIND` flag in resolving symbol conflicts during dynamic library loading, particularly when multiple libraries define functions with the same name.

## Overview

When multiple shared libraries define symbols with identical names, the dynamic linker must decide which implementation to use. By default, symbols are resolved using the global symbol table in load order (first loaded takes precedence). `RTLD_DEEPBIND` changes this behavior by prioritizing symbols from within the library itself and its direct dependencies.

## Problem Statement

Symbol name conflicts occur when:
1. Multiple libraries define functions with the same name
2. A library calls a function that exists both locally and in another library
3. The default symbol resolution may not choose the intended implementation

## Files Description

- `dltest.c` - Main application demonstrating RTLD_DEEPBIND behavior
- `libone.c` - First library with `common()` and `libone()` functions
- `libtwo.c` - Second library with `common()` and `libtwo()` functions, calls `libone()`
- `Makefile` - Build configuration for shared libraries and executable

## Symbol Conflict Scenario

### Library Implementations

**libone.c** contains:
```c
void common() {
    printf("libone common\n");
}

void libone() {
    common();  // Calls libone's version of common()
}
```

**libtwo.c** contains:
```c
extern void libone(void);

void common() {
    printf("libtwo common\n");
}

void libtwo() {
    libone();   // Calls libone() from libone.so
    common();   // Which common() gets called?
}
```

### The Conflict
Both libraries define a `common()` function. When `libtwo()` calls `common()`, which implementation should be used?

## Default Symbol Resolution (Without RTLD_DEEPBIND)

### Building and Testing
```bash
raghu@techveda$ make all
raghu@techveda$ ./dltest
```

### Default Loading Behavior
```c
ptr1 = dlopen("./libone.so", RTLD_NOW | RTLD_GLOBAL);
ptr2 = dlopen("./libtwo.so", RTLD_NOW);
```

**Expected Output (without RTLD_DEEPBIND):**
```
dltest:process registered with pid 1234
libone common
libone common
```

**Explanation:**
1. `libone.so` loaded first with `RTLD_GLOBAL`
2. `libone`'s `common()` symbol added to global symbol table
3. When `libtwo()` calls `common()`, it resolves to `libone`'s version
4. `libtwo`'s own `common()` function is never called

## Symbol Resolution with RTLD_DEEPBIND

### Modified Loading Behavior
```c
ptr1 = dlopen("./libone.so", RTLD_NOW | RTLD_GLOBAL);
ptr2 = dlopen("./libtwo.so", RTLD_NOW | RTLD_DEEPBIND);
```

**Expected Output (with RTLD_DEEPBIND):**
```
dltest:process registered with pid 1234
libone common
libtwo common
```

**Explanation:**
1. `libone.so` loaded with `RTLD_GLOBAL`
2. `libtwo.so` loaded with `RTLD_DEEPBIND`
3. When `libtwo()` calls `libone()`, it finds it in global table
4. When `libtwo()` calls `common()`, `RTLD_DEEPBIND` prioritizes `libtwo`'s own version

## RTLD_DEEPBIND Behavior

### Symbol Resolution Priority (with RTLD_DEEPBIND)
1. **Local symbols first**: Symbols defined within the library itself
2. **Direct dependencies**: Symbols from libraries this library directly depends on
3. **Global symbols last**: Symbols from the global symbol table

### Symbol Resolution Priority (without RTLD_DEEPBIND)
1. **Global symbols first**: Symbols from global table (load order dependent)
2. **Local symbols**: Symbols from the library itself
3. **Dependencies**: Symbols from direct dependencies

## Practical Examples

### Example 1: Plugin Architecture
```bash
raghu@techveda$ # Plugin needs its own utility functions
raghu@techveda$ dlopen("plugin.so", RTLD_NOW | RTLD_DEEPBIND);
```

**Use Case**: Plugin has its own implementation of common utilities that shouldn't be overridden by host application's versions.

### Example 2: Library Versioning
```bash
raghu@techveda$ # Load specific version with its own implementations
raghu@techveda$ dlopen("libmath_v2.so", RTLD_NOW | RTLD_DEEPBIND);
```

**Use Case**: Different library versions with conflicting symbol names need to coexist.

### Example 3: Namespace Isolation
```bash
raghu@techveda$ # Isolate library's internal symbols
raghu@techveda$ dlopen("isolated_lib.so", RTLD_NOW | RTLD_DEEPBIND);
```

**Use Case**: Prevent external libraries from interfering with internal library functions.

## Testing Different Scenarios

### Scenario 1: Default Loading
```c
// Both libraries loaded normally
ptr1 = dlopen("./libone.so", RTLD_NOW | RTLD_GLOBAL);
ptr2 = dlopen("./libtwo.so", RTLD_NOW);
```

### Scenario 2: DEEPBIND on Second Library
```c
// libtwo.so with DEEPBIND prioritizes its own symbols
ptr1 = dlopen("./libone.so", RTLD_NOW | RTLD_GLOBAL);
ptr2 = dlopen("./libtwo.so", RTLD_NOW | RTLD_DEEPBIND);
```

### Scenario 3: Both with DEEPBIND
```c
// Each library prioritizes its own symbols
ptr1 = dlopen("./libone.so", RTLD_NOW | RTLD_DEEPBIND);
ptr2 = dlopen("./libtwo.so", RTLD_NOW | RTLD_DEEPBIND);
```

## Debugging Symbol Resolution

### Check Symbol Tables
```bash
raghu@techveda$ nm -D libone.so | grep common
raghu@techveda$ nm -D libtwo.so | grep common
raghu@techveda$ objdump -T libone.so
raghu@techveda$ objdump -T libtwo.so
```

### Monitor Symbol Resolution
```bash
raghu@techveda$ LD_DEBUG=symbols ./dltest
raghu@techveda$ LD_DEBUG=bindings ./dltest
raghu@techveda$ LD_DEBUG=all ./dltest 2>&1 | grep common
```

### Trace Library Loading
```bash
raghu@techveda$ strace -e openat ./dltest
raghu@techveda$ ltrace -e dlopen,dlsym ./dltest
```

## When to Use RTLD_DEEPBIND

### Recommended Use Cases
- **Plugin Systems**: When plugins need to use their own implementations
- **Library Versioning**: When multiple versions of same library coexist
- **Symbol Isolation**: When library's internal functions must not be overridden
- **Third-party Libraries**: When integrating libraries with potential naming conflicts

### Avoid RTLD_DEEPBIND When
- **Intentional Override**: When you want global symbols to override local ones
- **Shared Implementations**: When libraries should share common implementations
- **Simple Applications**: When symbol conflicts are not a concern
- **Performance Critical**: DEEPBIND adds slight overhead to symbol resolution

## Performance Considerations

### Symbol Resolution Overhead
- `RTLD_DEEPBIND` adds slight overhead during symbol lookup
- More complex resolution algorithm
- Cache misses more likely with multiple symbol tables

### Memory Usage
- Each library maintains its own symbol resolution context
- Slight increase in metadata overhead
- Potential for symbol duplication

## Best Practices

### Library Design
- Use unique function prefixes to avoid conflicts
- Consider symbol visibility attributes
- Document symbol dependencies clearly

### Application Design
- Load dependent libraries first
- Use `RTLD_DEEPBIND` judiciously
- Always check `dlopen()` return values and `dlerror()`

### Testing
- Test with different loading orders
- Verify symbol resolution with debugging tools
- Test in environments with conflicting libraries

## Clean up
```bash
raghu@techveda$ make clean
```

## Additional Resources

### Related Concepts
- **Symbol versioning**: `@GLIBC_2.34` style versioning
- **Namespace isolation**: Using different symbol namespaces
- **Link maps**: Understanding loader's internal structures
- **RTLD_NODELETE**: Preventing library unloading

### Further Reading
- ELF specification for symbol resolution
- glibc dynamic loader implementation
- Linux loader documentation (`man ld.so`)
- Symbol visibility and versioning guides

## Example Output Analysis

The key insight from this example is understanding when and why symbol resolution matters. The `common()` function conflict demonstrates how `RTLD_DEEPBIND` provides control over which implementation gets called, enabling proper library isolation and preventing unintended symbol interposition.
