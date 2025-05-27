#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Gui.hpp"

#define BERRY 		0x0
#define SPEED		0x1
#define SWAPAROO	0x2

class Score {
    public:
        Score(SDL_Renderer *renderer, GUI *gui, int width, int height, const std::string type);
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

        SDL_Texture *m_textureScore;
        SDL_Renderer *m_renderer;
        GUI *m_gui;

};

