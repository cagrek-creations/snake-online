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

class SlowBoostEffect : public Effect {

    public:
        SlowBoostEffect(Snake &target, float duration) : Effect(duration), m_target(target) {}

        void apply() override {
            m_target.applySlowBoost();
        }

        void expire() override {
            m_target.removeSlowBoost();
        }

        TextureID getType() override {
            return TextureID::SLOW;
        }

    private:
        Snake &m_target; // Should this be moved to another base class?

};

class GhostEffect : public Effect {

    public:
        GhostEffect(Snake &target, float duration) : Effect(duration), m_target(target) {}

        void apply() override {
            m_target.becomeGhost();
        }

        void expire() override {
            m_target.removeGhost();
        }

        TextureID getType() override {
            return TextureID::GHOST;
        }

    private:
        Snake &m_target; // Should this be moved to another base class?

};

class FreezeEffect : public Effect {

    public:
        FreezeEffect(Snake &target, float duration) : Effect(duration), m_target(target) {}

        void apply() override {
            m_target.freeze();
        }

        void expire() override {
            m_target.unfreeze();
        }

        TextureID getType() override {
            return TextureID::FREEZE;
        }

    private:
        Snake &m_target; // Should this be moved to another base class?

};