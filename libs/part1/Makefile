CC = gcc
CFLAGS = -Wall -Wextra -fPIC
LDFLAGS = -ldl

# Static library
STATIC_LIB = libtest.a
STATIC_OBJS = lib1.o lib2.o

# Shared library
SHARED_LIB = libinit.so
SHARED_OBJS = libinit.o

# Executables
TARGETS = main runtime

all: $(STATIC_LIB) $(SHARED_LIB) $(TARGETS)

# Static library
$(STATIC_LIB): $(STATIC_OBJS)
	ar rcs $@ $^

# Shared library
$(SHARED_LIB): $(SHARED_OBJS)
	$(CC) -shared -o $@ $^

# Object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Executables
main: main.o $(STATIC_LIB)
	$(CC) -o $@ $< -L. -ltest

runtime: runtime.o
	$(CC) -o $@ $< $(LDFLAGS)

clean:
	rm -f *.o $(STATIC_LIB) $(SHARED_LIB) $(TARGETS)

.PHONY: all clean