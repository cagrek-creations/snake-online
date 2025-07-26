#pragma once

#include <SDL2/SDL_render.h>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <vector>
#include <memory>
#include <thread>

#include "Menu.hpp"
#include "utils.hpp"


namespace color {
    const SDL_Color DEFAULT = {0,   255, 0,   255};
    const SDL_Color RED     = {255, 0,   0,   255};
    const SDL_Color GREEN   = {0,   255, 0,   255};
    const SDL_Color BLUE    = {0,   0,   255, 255};
    const SDL_Color WHITE   = {255, 255, 255, 255};
}

enum class TextureID {
    ERR,
    BERRY,
    SPEED,
    SPEED_O,
    SWAPAROO,
    SWAPAROO_O,
    SNAKEHEAD,
    SNAKEBODY,
    SNAKECURVE,
    SNAKETAIL,
    GRIDTILE,
    VINJETTE,
    BERRY_GLOW,
    GOLDEN_BERRY,
};

class GUI : public Observer {

    public:
        GUI(const char *title, int windowWidth, int windowHeight, bool fullscreen);
        ~GUI();

        void render();
        void update();

        void onEvent(const SDL_Event& event) override;

        template <typename T>
        void render(T &object) {
            object.render();
        }
        void clearRenderer();

        bool getWindowClose();

        int getWindowWidth();
        int getWindowHeight();

        int getWindowCenterX();
        int getWindowCenterY();

        SDL_Color getColor(std::string colorName);
        SDL_Texture *loadTexture(TextureID name, const std::string &filePath);
        SDL_Texture *loadTextureAlpha(TextureID name, const std::string &filePath, int alpha, bool cache);
        SDL_Texture *getTexture(TextureID key);
        SDL_Texture *copyTexture(TextureID key);
        void unloadTexture(TextureID key);

        SDL_Renderer *getRenderer();
        TTF_Font *getFont();

    private:
        int m_windowWidth; 
        int m_windowHeight;
        
        bool m_windowClose;

        // std::thread m_guiThread;

        SDL_Renderer *m_renderer;
        SDL_Window *m_window;
        SDL_Event m_event;

        TTF_Font *m_font;

        std::unordered_map<TextureID, SDL_Texture*> m_textureMap;
        std::vector<std::unique_ptr<Menu>> menus;
};
