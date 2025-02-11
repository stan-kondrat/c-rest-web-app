CC = gcc
CFLAGS = -Wall -Wextra -g
BUILDDIR = build

SOURCES = foo database server router
LIBS_LOCAL = parson picohttpparser
OBJECTS = $(addprefix build/, $(addsuffix .o, $(SOURCES))) $(addprefix build/, $(addsuffix .o, $(LIBS_LOCAL)))

INCLUDES_LOCAL = -Ilibs/ -Ilibs/parson/ -Ilibs/picohttpparser/
# Platform-specific flags for PostgreSQL library (libpq) and libuv
ifeq ($(shell uname), Darwin) # MacOS
    INCLUDES = $(INCLUDES_LOCAL) -I/opt/homebrew/include
    LDFLAGS = -L/opt/homebrew/lib  -lpq -luv
	TEST_LDFLAGS = -lcmocka
else ifeq ($(shell uname), Linux) # Linux
    INCLUDES = $(INCLUDES_LOCAL) \
		-I/usr/include/postgresql \
		-I/usr/include/libuv
    LDFLAGS = -L/usr/lib -lpq -luv
	TEST_LDFLAGS = -lcmocka
else
    $(error Unsupported platform: $(shell uname))
endif

# Default target
all: build/main
run: build/main
	./build/main

# Clean up build files
clean:
	rm -rf $(BUILDDIR)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Local libs
build/parson.o: libs/parson/parson.c libs/parson/parson.h | $(BUILDDIR)
	$(CC) $(CFLAGS) -O0 -std=c89 -pedantic-errors -c libs/parson/parson.c -o $@

build/picohttpparser.o: libs/picohttpparser/picohttpparser.c libs/picohttpparser/picohttpparser.h | $(BUILDDIR)
	$(CC) $(CFLAGS) -c libs/picohttpparser/picohttpparser.c -o $@

# Sources
build/%.o: src/%.c src/%.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c src/$*.c -o $@

# Link object files to create the final executable
build/main: build/main.o $(OBJECTS) | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $(OBJECTS) build/main.o

# Tests

build/test_router.o: test/test_router.c | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c test/test_router.c -o $@

build/test_all.o: test/test_all.c | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c test/test_all.c -o $@

# Link test object files to create the test executable
build/test_all: build/test_all.o build/test_router.o $(OBJECTS) | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $(LDFLAGS) $(TEST_LDFLAGS) -o $@ $(OBJECTS) build/test_all.o

# Run tests
test: build/test_all
	./build/test_all


.vscode/compile_commands.json: Makefile
	@echo "[" > $@
	@for src in $(SOURCES) $(LIBS_LOCAL); do \
		echo "  {\"directory\": \"$(PWD)\", \"command\": \"$(CC) $(CFLAGS) $(INCLUDES) -c src/$$src.c -o build/$$src.o\", \"file\": \"src/$$src.c\"}," >> $@; \
	done
	@for src in $(SOURCES); do \
		echo "  {\"directory\": \"$(PWD)\", \"command\": \"$(CC) $(CFLAGS) $(INCLUDES) -c src/$$src.c -o build/$$src.o\", \"file\": \"src/$$src.c\"}," >> $@; \
	done
	@echo "  {\"directory\": \"$(PWD)\", \"command\": \"$(CC) $(CFLAGS) $(INCLUDES) -c src/main.c -o build/main.o\", \"file\": \"src/main.c\"}" >> $@
	@echo "]" >> $@

format:
	find ./test \( -name "*.c" -or -name "*.h" \) -exec clang-format -i {} \;

.PHONY: clean

