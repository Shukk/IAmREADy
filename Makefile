# Setting the compiler and the default linker program
CC = gcc

# options for pre-processor (-I, -include, -D ... )
CPPFLAGS = `pkg-config --cflags sdl` `pkg-config --cflags gtk+-3.0` -MMD
# main compilation options
CFLAGS = -Wall -Wextra -Werror -std=c99 -O3
# Linker options (probably always empty)
LDFLAGS =
# libs and path for linker
LDLIBS = `pkg-config --libs sdl` `pkg-config --libs gtk+-3.0` -lSDL_image -lm

OBJ = $(patsubst %.c, %.o, $(shell find . -name "*.c"))
DEP = ${SRC:.o=.d}

all: main

main: ${OBJ}

.PHONY: clean

clean:
	${RM} ${OBJ}
	$(shell find . -type f -name '*.d' -exec rm {} +)
	${RM} main


-include ${DEP}

# END