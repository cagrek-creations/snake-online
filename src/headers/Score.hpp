#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Gui.hpp"
#include "Effect.hpp"

class Score {
    public:
        Score(SDL_Renderer *renderer, GUI *gui, int width, int height, TextureID type);
        ~Score();

        void render();
        void update(float deltaTime);
        void move(int xPos, int yPos);

        void addEffect(std::unique_ptr<Effect> effect);

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
        TextureID m_type;

        std::vector<std::unique_ptr<Effect>> m_effects;

        SDL_Texture *m_textureScore;
        SDL_Renderer *m_renderer;
        GUI *m_gui;

};

class GlowEffect : public Effect {

    public:
        GlowEffect(float duration, SDL_Texture *texture, SDL_Renderer *renderer, SDL_Rect target)
            : Effect(duration), m_texture(texture), m_renderer(renderer), m_target(target) {}

        void render() override {
            if (!m_active || !m_texture) return;

            float frequency = 0.5f;
            float alphaFactor = (sinf((m_elapsed / 1000) * frequency * 2.0f * M_PI) + 1.0f) / 2.0f;
            Uint8 alpha = static_cast<Uint8>(alphaFactor * 255);

            SDL_SetTextureAlphaMod(m_texture, alpha);

            int glowPadding = 100;
            SDL_Rect glowRect = {
                m_target.x - glowPadding,
                m_target.y - glowPadding,
                m_target.w + glowPadding * 2,
                m_target.h + glowPadding * 2
            };

            SDL_RenderCopy(m_renderer, m_texture, nullptr, &glowRect);
        }

        void move(int xPos, int yPos) override {
            m_target.x = xPos;
            m_target.y = yPos;
        }

    private:
        SDL_Texture *m_texture;
        SDL_Renderer *m_renderer;
        SDL_Rect m_target;
};
