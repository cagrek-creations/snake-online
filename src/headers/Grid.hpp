#pragma once

#include <SDL2/SDL_render.h>
#include <iostream>
#include <vector>
#include <algorithm>

#include <SDL2/SDL.h>

#include "Vector2.hpp"
#include "Gui.hpp"

class Gridpoint {

    public:
        Gridpoint(GUI *gui, Vector2 pos, int width, int height);
        ~Gridpoint();

        bool operator==(const Gridpoint& other) const {
            return (m_gridPointX == other.m_gridPointX) && (m_gridPointX == other.m_gridPointX);
        }

        void render();
        void renderTexture();

        bool contains(int x, int y);

        bool isEmpty();

        void setNotEmpty();

        void setEmpty();

        void setScore();
        void removeScore();

        bool hasScore();

        Vector2 getGridPointPos();

        int getGridPointX();
        int getGridPointY();

        int getGridPointWidth();
        int getGridPointHeight();

    private:
        SDL_Renderer *m_renderer;
        GUI *m_gui;

        int m_gridPointX;
        int m_gridPointY;
        int m_gridWidth;
        int m_gridHeight;
        bool m_gridPointIsEmpty = true;
        bool m_gridPointHasScore = false;

        SDL_Texture *m_texture;
        SDL_Rect m_destRect;

        Vector2 m_pos;
    
};

class Grid {

    public:
        Grid(GUI *gui, int width, int height, int granularityX, int granularityY, int rows, int columns, Vector2 pos);
        ~Grid();

        void render();

        int getGridPointWidth();
        int getGridPointHeight();

        int getWidth();
        int getHeight();

        void setSize(int width, int height);

        Gridpoint *getPoint(int x, int y);
        bool isPointEmpty(Gridpoint point);

        int getGridPointGranularityX();
        int getGridPointGranularityY();

    private: 
        SDL_Renderer *m_renderer;
        SDL_Texture *m_gridTexture;
        GUI *m_gui;

        int m_gridWidth;
        int m_gridHeight;
        int m_granularity;
        int m_granularityX;
        int m_granularityY;
        int m_gridPointWidth;
        int m_gridPointHeight;

        Vector2 m_pos;

        std::vector<Gridpoint> m_gridpoints;
};
