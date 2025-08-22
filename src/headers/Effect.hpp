#pragma once

#include "Gui.hpp"

class Effect {

    public:
        Effect(float duration) : m_duration(duration), m_elapsed(0.0f), m_active(true), m_applied(false) {}
        virtual ~Effect() = default;

        bool isActive() const { return m_active; }
        float getElapsed() const { return m_elapsed; }
        float getDuration() const { return m_duration; }

        virtual void apply() {};
        virtual void expire() {};
        virtual void update(float dt) {
            if (!m_applied) {
                apply();
                m_applied = true;
            }

            m_elapsed += dt;
            if (m_elapsed >= m_duration && m_duration != -1) {
                expire();
                m_active = false;
            }
        }

        virtual TextureID getType() { return TextureID::ERR; }
        virtual SDL_Color getColor() { return color::DEFAULT; }

        virtual void render() {};

        virtual void move(int xPos, int yPos) {};

    protected:
        float m_elapsed;
        float m_duration;
        bool m_active;
        bool m_applied;

};
