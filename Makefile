CC = gcc
CFLAGS = -Wall -Wextra -g
BUILDDIR = build

SOURCES = database http router server foo app_users
LIBS_LOCAL = parson picohttpparser
OBJECTS = $(addprefix build/, $(addsuffix .o, $(LIBS_LOCAL))) $(addprefix build/, $(addsuffix .o, $(SOURCES)))
TESTS = test_router
TEST_OBJECTS = $(addprefix build/, $(addsuffix .o, $(TESTS)))

INCLUDES_LOCAL = -Isrc/ -Ilibs/ -Ilibs/parson/ -Ilibs/picohttpparser/
# Platform-specific flags for PostgreSQL library (libpq) and libuv
ifeq ($(shell uname), Darwin) # MacOS
    INCLUDES = $(INCLUDES_LOCAL) -I/opt/homebrew/include
    LDFLAGS = -L/opt/homebrew/lib -lpq -luv
else ifeq ($(shell uname), Linux) # Linux
    INCLUDES = $(INCLUDES_LOCAL) -I/usr/include -I/usr/include/uv -I/usr/include/postgresql 
    LDFLAGS =  -lpq -luv
else
    $(error Unsupported platform: $(shell uname))
endif


# Default target
all: build/main

foo:
	ls /usr/lib/aarch64-linux-gnu/libpq*

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
build/main: src/main.c $(OBJECTS) | $(BUILDDIR)
	$(CC) $(CFLAGS) src/main.c $(OBJECTS) $(INCLUDES) $(LDFLAGS) -o $@

run: build/main
	./build/main

# Tests
build/%.o: test/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c test/$*.c -o $@

# Link test object files to create the test executable
build/test_all: test/test_all.c $(OBJECTS) ${TEST_OBJECTS} | $(BUILDDIR)
	$(CC) $(CFLAGS) test/test_all.c $(OBJECTS) $(INCLUDES) $(LDFLAGS) -lcmocka -o $@

# Run tests
test: build/test_all
	./build/test_all

test-docker: 
	docker build -t temp-image . && docker run --rm temp-image

test-docker-linux-amd64: 
	docker build --platform linux/amd64 -t temp-image . && docker run --platform linux/amd64 --rm temp-image

test-docker-linux-arm64: 
	docker build --platform linux/arm64 -t temp-image . && docker run --platform linux/arm64 --rm temp-image

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

