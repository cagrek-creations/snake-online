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

    private:
        Snake &m_target;

};