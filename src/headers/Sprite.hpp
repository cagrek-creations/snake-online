#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

class Sprite {
    public:
        Sprite(SDL_Texture* texture, const SDL_Rect& rect) : m_texture(texture), m_rect(rect) {}
        ~Sprite();

        SDL_Texture *getTexture() const { return m_texture; }
        const SDL_Rect &getRect() const { return m_rect; }

    private:
        SDL_Texture *m_texture;
        SDL_Rect m_rect;
};

class SpriteSheet {

    public:
        SpriteSheet(SDL_Renderer *renderer, const std::string &path, int frameWidth, int frameHeight, int numFrames);
        ~SpriteSheet();

        const SDL_Rect &getClip(int index) const;
        SDL_Texture* getTexture() const;

    private:
        SDL_Texture *m_texture;
        std::vector<SDL_Rect> m_clips;

};
