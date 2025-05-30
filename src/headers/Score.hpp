#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Gui.hpp"

#define ERR         0x99
#define BERRY 		0x00
#define SPEED		0x10
#define SPEED_O     0x11
#define SWAPAROO	0x20
#define SWAPAROO_O  0x22

class Score {
    public:
        Score(SDL_Renderer *renderer, GUI *gui, int width, int height, int type);
        ~Score();

        void render();
        void move(int xPos, int yPos);

        int getX() {
            return m_xPos;
        }

        int getY() {
            return m_yPos;
        }

    private:

        int m_xPos;
        int m_yPos;
        int m_width;
        int m_height;
        int m_type;

        SDL_Texture *m_textureScore;
        SDL_Renderer *m_renderer;
        GUI *m_gui;

};

