#include "Sprite.hpp"
#include <SDL2/SDL_render.h>

Sprite::~Sprite() {
    
}

SpriteSheet::SpriteSheet(SDL_Renderer *renderer, const std::string& path, int frameWidth, int frameHeight, int numFrames) : m_texture(nullptr) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "IMG_Load failed: " << std::string(IMG_GetError());
    }

    m_texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (!m_texture) {
        std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << std::endl;
    }

    SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND);

    int sheetWidth = surface->w;
    int sheetHeight = surface->h;
    SDL_FreeSurface(surface);

    int framesPerRow = sheetWidth / frameWidth;
    int framesPerCol = sheetHeight / frameHeight;

    int count = 0;
    for (int y = 0; y < framesPerCol && count < numFrames; y++) {
        for (int x = 0; x < framesPerRow && count < numFrames; x++) {
            SDL_Rect rect = { x * frameWidth, y * frameHeight, frameWidth, frameHeight };
            m_clips.push_back(rect);
            count++;
        }
    }
}

SpriteSheet::~SpriteSheet() {
    if (m_texture) {
        SDL_DestroyTexture(m_texture);
    }
}

const SDL_Rect &SpriteSheet::getClip(int index) const {
    if (index < 0 || index >= (int)m_clips.size()) {
        std::cerr << "Invalid clip index";
    }
    return m_clips[index];
}

SDL_Texture *SpriteSheet::getTexture() const { 
    return m_texture; 
}
