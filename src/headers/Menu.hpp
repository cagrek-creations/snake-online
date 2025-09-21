// TODO: refactor this (move things to GUI if possible)

#pragma once

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <cstddef>
#include <iostream>
#include <memory>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <ostream>
#include <psdk_inc/_ip_types.h>
#include <vector>
#include <thread>
#include <functional>
#include <unordered_map>

#include "GuiElements.hpp"
#include "Observer.hpp"
#include "Gui.hpp"
#include "Vector2.hpp"

#define MENU_STATE      0x0
#define MENU_OPTION     0x1
#define MENU_BAR        0x2
#define MENU_ON_OFF     0x3


#define KEY_ENTER       -1
#define KEY_LEFT        0x0
#define KEY_RIGHT       0x1
#define KEY_UP          0x2
#define KEY_DOWN        0x3

namespace menuc {
    const SDL_Color RED     = {255, 0,   0,   255};
    const SDL_Color GREEN   = {0,   255, 0,   255};
    const SDL_Color BLUE    = {0,   0,   255, 255};
    const SDL_Color WHITE   = {255, 255, 255, 255};
}

enum class MenuItemType {
    M_BAR,
    M_BUTTON,
};

class GMenuItem {
    public:
        GMenuItem(Vector2 pos, SDL_Color color) : m_pos(pos), m_color(color) {}

        virtual void render() = 0;
        virtual void renderHighlighted() = 0;
        virtual void bind(std::function<void()> f) {};
        virtual void bind(std::function<void()> l, std::function<void()> r) {};
        virtual void trigger() {}
        virtual void triggerLeft() {}
        virtual void triggerRight() {}
        // TODO: Move the triggering to the menu items and let them choose based on events.
        virtual void update() {}
        virtual MenuItemType type() = 0;

        std::shared_ptr<GMenuItem> up;
        std::shared_ptr<GMenuItem> down;
        std::shared_ptr<GMenuItem> left;
        std::shared_ptr<GMenuItem> right;

    protected:
        Vector2 m_pos;
        SDL_Color m_color;

};

class GMenuItemButton : public GMenuItem {
    public:
        GMenuItemButton(GUI *gui, Vector2 pos, std::string content, std::string font, SDL_Color color, SDL_Color highlighted) : GMenuItem(pos, color) {
            m_text = gui->createText(m_pos, content, font, color, TEXT_CENTRALIZED);
            m_highlighted = highlighted;
        }

        void render() override {
            m_text->render();
        }

        void renderHighlighted() override {
            m_text->renderColor(m_highlighted);
        }

        void bind(std::function<void()> f) override {
            m_select = f;
        }

        void trigger() override {
            if(m_select) m_select();
        }

        MenuItemType type() override {
            return MenuItemType::M_BUTTON;
        }

    private:
        std::shared_ptr<GText> m_text;
        std::function<void()> m_select;
        SDL_Color m_highlighted;
};

// TODO: Move the creation of text to outside the menuitem?
class GMenuItemBar : public GMenuItem {
    public:
        GMenuItemBar(GUI *gui, Vector2 pos, Vector2 dim, int step, float scale, std::string content, std::string font, SDL_Color color, SDL_Color highlighted) : GMenuItem(pos, color) {
            m_text = gui->createText(m_pos, content, font, color, TEXT_CENTRALIZED);
            m_highlighted = highlighted;
            m_renderer = gui->getRenderer();
            m_scale = scale;
            m_barStep = step;
            m_barSizeMax = dim.x;
            m_barSize = m_barSizeMax / 2;
            m_bar.w = m_barSize;
            m_bar.h = dim.y;
            m_bar.x = pos.x - m_barSizeMax / 2;
            m_bar.y = pos.y;
        }

        void render() override {
            m_text->render();

            SDL_SetRenderDrawColor(m_renderer, m_color.r, m_color.g, m_color.b, 255);
            SDL_RenderFillRect(m_renderer, &m_bar);
        }

        void renderHighlighted() override {
            m_text->renderColor(m_highlighted);

            SDL_SetRenderDrawColor(m_renderer, m_highlighted.r, m_highlighted.g, m_highlighted.b, 255);
            SDL_RenderFillRect(m_renderer, &m_bar);
        }

        void bind(std::function<void()> l, std::function<void()> r) override {
            m_left = l;
            m_right = r;
        }

        void triggerLeft() override {
            if (m_left) m_left();
            if (m_barSize > 0) m_barSize -= m_barStep;
            m_bar.w = m_barSize * m_scale;
        }

        void triggerRight() override {
            if (m_right) m_right();
            if (m_barSize < m_barSizeMax) m_barSize += m_barStep;
            m_bar.w = m_barSize * m_scale;
        }

        MenuItemType type() override {
            return MenuItemType::M_BAR;
        }

    private:
        // TODO: 2D menus would break the functionality of moving a bar right / left with the keyboard.
        // Could implement this so that you have to 'select' the bar before changing its values?
        // Reply: this should probably be fine with the new implementation of menus since 
        // they set up and down values for each MenuItem.
        SDL_Color m_highlighted;
        SDL_Renderer *m_renderer;
        SDL_Rect m_bar;

        int m_barSize;
        int m_barSizeMax;
        int m_barStep;
        float m_scale;

        std::shared_ptr<GText> m_text;
        std::function<void()> m_left;
        std::function<void()> m_right;
};

class GMenu : public Observer {

    public:
        GMenu(Vector2 pos) : m_pos(pos) {}

         void setCurrent(std::shared_ptr<GMenuItem> item) { m_current = item; }

        void moveUp() { if (m_current->up)    m_current = m_current->up; }
        void moveDown() { if (m_current->down)  m_current = m_current->down; }
        void moveLeft() { 
            if (m_current->type() == MenuItemType::M_BAR) m_current->triggerLeft();
            else if (m_current->left) m_current = m_current->left; 
        }
        void moveRight() { 
            if (m_current->type() == MenuItemType::M_BAR) m_current->triggerRight();
            else if (m_current->right) m_current = m_current->right; 
        }

        void trigger() { m_current->trigger(); }

        void render() {
            for (auto &mi : m_menuItems) {
                mi->render();
            }

            if (m_current) m_current->renderHighlighted();
        }

        void addItem(std::shared_ptr<GMenuItem> item) {
            m_menuItems.push_back(item);
        }

        void onEvent(const SDL_Event& event) override {
            const Uint8 *key_state = SDL_GetKeyboardState(NULL);

            if (key_state[SDL_SCANCODE_S] || key_state[SDL_SCANCODE_DOWN]) {
                moveDown();
            }

            if (key_state[SDL_SCANCODE_W] || key_state[SDL_SCANCODE_UP]) {
                moveUp();
            }

            if (key_state[SDL_SCANCODE_D] || key_state[SDL_SCANCODE_RIGHT]) {
                moveRight();
            }

            if (key_state[SDL_SCANCODE_A] || key_state[SDL_SCANCODE_LEFT]) {
                moveLeft();
            }

            if (key_state[SDL_SCANCODE_RETURN]) {
                trigger();
            }
        }

        int getX() {
            return m_pos.x;
        }

        int getY() {
            return m_pos.y;
        }

    private:
        Vector2 m_pos;
        std::vector<std::shared_ptr<GMenuItem>> m_menuItems;
        std::shared_ptr<GMenuItem> m_current;
};
