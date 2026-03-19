CC = gcc
CFLAGS = -Wall -Wextra -fPIC
LDFLAGS = -ldl

# Shared libraries
LIBS = libone.so libtwo.so

# Executables
TARGETS = dltest

all: $(LIBS) $(TARGETS)

# Shared libraries
libone.so: libone.o
	$(CC) -shared -o $@ $^

libtwo.so: libtwo.o
	$(CC) -shared -o $@ $^

# Object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Executables
dltest: dltest.o
	$(CC) -o $@ $< $(LDFLAGS)

clean:
	rm -f *.o $(LIBS) $(TARGETS)

.PHONY: all clean