#pragma once

#include <SDL2/SDL_render.h>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <unordered_map>
#include <vector>
#include <memory>
#include <thread>

#include "Menu.hpp"
#include "Sprite.hpp"
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
    GRIDTILE,
    VINJETTE,
    BERRY_GLOW,
    GOLDEN_BERRY,
    GHOST,
    SLOW,
    FREEZE,
    A_PURPLE_SNAKE,
    A_YELLOW_SNAKE,
    A_GREEN_SNAKE,
    A_RED_SNAKE,
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

        SpriteSheet *loadAtlas(TextureID name, const std::string &filePath, int frameWidth, int frameHeight, int numFrames);
        SpriteSheet *getAtlas(TextureID key);

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
        std::unordered_map<TextureID, std::unique_ptr<SpriteSheet>> m_atlasMap;
        std::vector<std::unique_ptr<Menu>> menus;
};
