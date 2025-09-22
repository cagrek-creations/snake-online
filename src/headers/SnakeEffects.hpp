#pragma once

#include <SDL2/SDL_render.h>
#include <iostream>
#include <vector>

#include "Effect.hpp"
#include "Snake.hpp"
#include "Gui.hpp"

inline const std::vector<TextureID> snakeEffectTextures = {
    TextureID::FREEZE,
    TextureID::GHOST,
    TextureID::SPEED,
    TextureID::SLOW,
    TextureID::RAGE,
};

class SnakeEffect : public Effect {
    public:
        SnakeEffect(Snake &target, float duration) : Effect(duration), m_target(target) {}
    protected:
        Snake &m_target;
        // TODO: add type here?
};

class InvertControlsEffect : public SnakeEffect {

    public: 
        InvertControlsEffect(Snake &target, float duration) : SnakeEffect(target, duration) {}

        void apply() override {
            m_target.invertControls();
        }

        void expire() override {
            m_target.invertControls();
        }

        TextureID getType() override {
            return TextureID::SWAPAROO;
        }

        SDL_Color getColor() override {
            return color::DEFAULT;
        }

};

class SpeedBoostEffect : public SnakeEffect {

    public:
        SpeedBoostEffect(Snake &target, float duration) : SnakeEffect(target, duration) {}

        void apply() override {
            m_target.applySpeedBoost();
        }

        void expire() override {
            m_target.removeSpeedBoost();
        }

        TextureID getType() override {
            return TextureID::SPEED;
        }

        SDL_Color getColor() override {
            return color::ORANGE;
        }

};

class SlowBoostEffect : public SnakeEffect {

    public:
        SlowBoostEffect(Snake &target, float duration) : SnakeEffect(target, duration) {}

        void apply() override {
            m_target.applySlowBoost();
        }

        void expire() override {
            m_target.removeSlowBoost();
        }

        TextureID getType() override {
            return TextureID::SLOW;
        }

        SDL_Color getColor() override {
            return color::LIGHTBLUE;
        }

};

class GhostEffect : public SnakeEffect {

    public:
        GhostEffect(Snake &target, float duration) : SnakeEffect(target, duration) {}

        void apply() override {
            m_target.becomeGhost();
        }

        void expire() override {
            m_target.removeGhost();
        }

        TextureID getType() override {
            return TextureID::GHOST;
        }

        SDL_Color getColor() override {
            return color::LIGHTGRAY;
        }

};

class FreezeEffect : public SnakeEffect {

    public:
        FreezeEffect(Snake &target, float duration) : SnakeEffect(target, duration) {}

        void apply() override {
            m_target.freeze();
        }

        void expire() override {
            m_target.unfreeze();
        }

        // TODO: Change function name
        TextureID getType() override {
            return TextureID::FREEZE;
        }

        SDL_Color getColor() override {
            return color::BLUE;
        }

};
