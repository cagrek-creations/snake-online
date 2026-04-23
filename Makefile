CC=g++
OPTIMIZATION= #-O2

# https://www.reddit.com/r/gamedev/comments/5pl8vs/sdl2_staticlly_link/
LINKER_FLAGS= -static
LINKER_FLAGS += `pkg-config --libs --static SDL2`
LINKER_FLAGS += `pkg-config --libs --static SDL2_ttf`
LINKER_FLAGS += `pkg-config --libs --static SDL2_Image`
LINKER_FLAGS += `pkg-config --libs --static SDL2_Mixer`
LINKER_FLAGS += -lbrotlicommon -lsharpyuv

ifeq ($(OS), Windows_NT)
    LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lws2_32 -MMD -MP
else
    UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S), Linux)
		LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
	endif
endif

CFLAGS= $(LIBS) $(EXTRA_LIBS)

SRC_DIR = src
BUILD_DIR = build

SRC = $(wildcard $(SRC_DIR)/*.cpp)
LIBSRC = $(wildcard $(SRC_DIR)/cpp-lib/*.cpp)
OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC))
OBJ += $(patsubst $(SRC_DIR)/cpp-lib/%.cpp,$(BUILD_DIR)/%.o,$(LIBSRC))
DEP = $(OBJ:.o=.d)

snake: $(OBJ)
	$(CC) $(OPTIMIZATION) -o ./bin/$@ $^ $(CFLAGS) -I./src/cpp-lib

static: $(OBJ)
	$(CC) $(OPTIMIZATION) -o ./bin/$@ $^ $(CFLAGS) $(LINKER_FLAGS) -mconsole

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $< -o $@ -I./src/cpp-lib -MMD -MP

$(BUILD_DIR)/%.o: $(SRC_DIR)/cpp-lib/%.cpp
	$(CC) -c $< -o $@ -I./src/cpp-lib -MMD -MP

-include $(DEP)

run: snake
	./bin/snake

clean: remove

clangd:
	compiledb make

remove:
	rm build/*

install:
	mkdir -p build/

	# Setup bin
	mkdir -p bin/
	mkdir -p bin/gfx
	mkdir -p bin/gfx/scores
	mkdir -p bin/gfx/snakes
	mkdir -p bin/sfx
	mkdir -p bin/fonts

	# Clean tmp
	rm -rf tmp
	mkdir -p tmp
	# Clone library
	mkdir -p external
	mkdir -p src/cpp-lib
	git clone https://github.com/cagrek-creations/cpp-lib external/cpp-lib
	cp external/cpp-lib/src/* src/cpp-lib
	rm -rf external

	# Clone assets
	mkdir -p assets
	git clone https://github.com/cagrek-creations/snake-assets.git tmp/snake-assets
	cp -r tmp/snake-assets/* bin/

prepare-windows-x86_64:
	pacman -S --noconfirm \
	  mingw-w64-x86_64-SDL2 \
	  mingw-w64-x86_64-SDL2_image \
	  mingw-w64-x86_64-SDL2_ttf \
	  mingw-w64-x86_64-SDL2_mixer \
	  mingw-w64-x86_64-yaml-cpp

.PHONY: all install prepare-windows-x86_64 remove clean run

all: snake