
OBJS = ./src/*.cpp

CC=g++

OPTIMIZATION= #-O3

STATIC= -static -static-libgcc -static-libstdc++ 

# https://www.reddit.com/r/gamedev/comments/5pl8vs/sdl2_staticlly_link/
LINKER_FLAGS= -static
LINKER_FLAGS += `pkg-config --libs --static SDL2`
LINKER_FLAGS += `pkg-config --libs --static SDL2_ttf`
LINKER_FLAGS += `pkg-config --libs --static SDL2_Image`
LINKER_FLAGS += `pkg-config --libs --static SDL2_Mixer`
LINKER_FLAGS += -lbrotlicommon -lsharpyuv


LIBS= -lmingw32 -lws2_32 -lSDL2main -lSDL2 -lSDL2_Image -lSDL2_TTF -lSDL2_Mixer

IDIR = ./src/headers
CFLAGS= -I$(IDIR) $(LIBS) $(EXTRA_LIBS)
# _DEPS = Gui.hpp Snake.hpp Grid.hpp Menu.hpp Score.hpp Controller.hpp Soundmanager.hpp comm_util.hpp Observer.hpp Game.hpp
DEPS = $(wildcard $(IDIR)/*.hpp)

ODIR = ./obj
_OBJ =  Gui.o Snake.o Grid.o main.o Menu.o Score.o Soundmanager.o comm_util.o Game.o GameLogic.o Controller.o Vector2.o Sprite.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

LIBSRC = ./src/cpp-lib

.PHONY: all install 

all: snake

$(ODIR)/%.o: ./src/%.cpp $(DEPS)
	$(CC) -c $(OPTIMIZATION) -o $@ $< -I./src/headers -I./src/cpp-lib

$(ODIR)/%.o: $(LIBSRC)/%.cpp
	$(CC) -c $(OPTIMIZATION) -o $@ $< -I./src/cpp-lib

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
	cp tmp/snake-assets/* assets/

	# Setup bin
	mkdir -p bin/
	mkdir -p bin/gfx
	mkdir -p bin/sfx
	cp gfx/* bin/gfx
	cp sfx/* bin/sfx
	cp font.ttf bin/

	# Clean tmp
	rm -rf tmp
