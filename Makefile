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
    LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lws2_32
else
    UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S), Linux)
		LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
	endif
endif

IDIR = ./src/headers
CFLAGS= -I$(IDIR) $(LIBS) $(EXTRA_LIBS)
DEPS = $(wildcard $(IDIR)/*.hpp)

ODIR = ./obj
_OBJ =  Gui.o Snake.o Grid.o main.o Menu.o Score.o Soundmanager.o comm_util.o Game.o GameLogic.o Controller.o Vector2.o Sprite.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

LIBSRC = ./src/cpp-lib

.PHONY: all install 

all: snake

$(ODIR)/%.o: ./src/%.cpp $(DEPS)
	$(CC) -c $(OPTIMIZATION) -o $@ $< -I./src/cpp-lib $(CFLAGS)

$(ODIR)/%.o: $(LIBSRC)/%.cpp
	$(CC) -c $(OPTIMIZATION) -o $@ $< -I./src/cpp-lib $(CFLAGS)

snake: $(OBJ)
	$(CC) $(OPTIMIZATION) -o ./bin/$@ $^ $(CFLAGS)

static: $(OBJ)
	$(CC) $(OPTIMIZATION) -o ./bin/$@ $^ $(CFLAGS) $(LINKER_FLAGS) -mconsole

run: snake
	./bin/snake

clean: remove snake

clangd:
	compiledb make

remove:
	rm obj/*

install::
	mkdir -p obj/

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

prepare-windows-x86_64::
	pacman -S --needed --noconfirm \
	  mingw-w64-x86_64-SDL2 \
	  mingw-w64-x86_64-SDL2_image \
	  mingw-w64-x86_64-SDL2_ttf \
	  mingw-w64-x86_64-SDL2_mixer \
	  mingw-w64-x86_64-yaml-cpp

