#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_Image.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

class SpriteSheet {

    public:
        SpriteSheet(SDL_Renderer *renderer, const std::string &path, int frameWidth, int frameHeight, int numFrames);
        ~SpriteSheet();

        const SDL_Rect &getClip(int index) const;

    private:
        SDL_Texture *m_texture;
        std::vector<SDL_Rect> m_clips;

};

