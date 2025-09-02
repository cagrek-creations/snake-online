#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>

#include "Vector2.hpp"

class GUIElement {
    public:
        GUIElement(SDL_Renderer *renderer, Vector2 pos);
        ~GUIElement();

        void render();
        void updatePos(Vector2 newPos);

    private:
        SDL_Texture *m_texture;
        SDL_Renderer *m_renderer;
        Vector2 m_pos;
};

class GText : GUIElement {

    public:
        GText();
        ~GText();

    private:
        TTF_Font *m_font;

};

// void render() {
//     SDL_Rect renderQuad = {xPos, yPos, width, height};
//     SDL_RenderCopy(renderer, texture, nullptr, &renderQuad);
// }

// bool GUI::updateTextColor(Text &txt, SDL_Color textColor) {
//     SDL_Surface *textSurface = TTF_RenderText_Solid(txt.font, txt.name.c_str(), textColor);
//     if (!textSurface) {
//         std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
//     }

//     SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
//     if (!textTexture) {
//         std::cerr << "Unable to create texture from rendered text! SDL_Error: " << SDL_GetError() << std::endl;
//         SDL_FreeSurface(textSurface);
//     }
//     SDL_FreeSurface(textSurface);

//     txt.texture = textTexture;
//     return true;
// }

// bool GUI::updateTextValue(Text &txt, std::string newValue) {
//     txt.name = newValue;
//     SDL_Surface *textSurface = TTF_RenderText_Solid(txt.font, txt.name.c_str(), txt.color);
//     if (!textSurface) {
//         std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
//     }

//     // Update text dimensions.
//     txt.width      = textSurface->w;
//     txt.height     = textSurface->h;

//     SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
//     if (!textTexture) {
//         std::cerr << "Unable to create texture from rendered text! SDL_Error: " << SDL_GetError() << std::endl;
//         SDL_FreeSurface(textSurface);
//     }
//     SDL_FreeSurface(textSurface);

//     txt.texture = textTexture;
//     return true;
// }