CC = gcc
CFLAGS = -Wall -Wextra -fPIC
LDFLAGS = -ldl

# Shared libraries
LIBS = liba.so libb.so

# Executables
TARGETS = app

all: $(LIBS) $(TARGETS)

# Shared libraries
liba.so: liba.o
	$(CC) -shared -o $@ $^

libb.so: libb.o liba.so
	$(CC) -shared -o $@ libb.o -L. -la

# Object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Executables
app: app.o
	$(CC) -o $@ $< $(LDFLAGS)

clean:
	rm -f *.o $(LIBS) $(TARGETS)

.PHONY: all clean
