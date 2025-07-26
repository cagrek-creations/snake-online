#pragma once

#include <iostream>

#include "Effect.hpp"
#include "Snake.hpp"

class InvertControlsEffect : public Effect {

    public: 
        InvertControlsEffect(Snake &target, float duration) : Effect(duration), m_target(target) {}

        void apply() override {
            m_target.invertControls();
        }

        void expire() override {
            m_target.invertControls();
        }

        TextureID getType() override {
            return TextureID::SWAPAROO;
        }

    private:
        Snake &m_target; // Should this be moved to another base class?

};

class SpeedBoostEffect : public Effect {

    public:
        SpeedBoostEffect(Snake &target, float duration) : Effect(duration), m_target(target) {}

        void apply() override {
            m_target.applySpeedBoost();
        }

        void expire() override {
            m_target.removeSpeedBoost();
        }

        TextureID getType() override {
            return TextureID::SPEED;
        }

    private:
        Snake &m_target; // Should this be moved to another base class?

};