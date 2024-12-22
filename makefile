.RECIPEPREFIX = >

COMPILER = clang
CFLAGS = -Wall -Werror -I/usr/local/include -L/usr/local/lib -lraylib
SRC = $(wildcard src/*.c)
TESTS = $(wildcard test/*.c)

.PHONY: all run build test clean

all: build

run:
> @echo "\nRunning the project...\n"
> ./build/vis.o

build:
> @echo "\nBuilding the project...\n"
> @mkdir -p build/
> $(COMPILER) $(SRC) -o build/vis.o $(CFLAGS)

clean:
> @echo "\nCleaning up...\n"
> rm -rf build/
