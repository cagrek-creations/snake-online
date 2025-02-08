
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
CFLAGS= -I$(IDIR) $(LIBS) $(EXTRA_LIBS) 

IDIR = ./src/headers
_DEPS = Gui.hpp Snake.hpp Grid.hpp Menu.hpp Score.hpp Controller.hpp Soundmanager.hpp comm_util.hpp Observer.hpp Game.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

ODIR = ./obj
_OBJ =  Gui.o Snake.o Grid.o main.o Menu.o Score.o Soundmanager.o comm_util.o Game.o GameLogic.o Controller.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: snake

./obj/%.o: ./src/%.cpp $(DEPS)
	$(CC) -c $(OPTIMIZATION) -o $@ $< -I./src/headers

snake: $(OBJ)
	$(CC) $(OPTIMIZATION) -o ./debug/$@ $^ $(CFLAGS) 

static: $(OBJ)
	$(CC) $(OPTIMIZATION) -o ./debug/$@ $^ $(CFLAGS) $(LINKER_FLAGS) -mconsole

run: snake
	./debug/snake

clean: remove snake

remove:
	./remove.py

