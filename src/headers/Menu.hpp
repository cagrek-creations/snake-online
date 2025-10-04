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
#include "Soundmanager.hpp"
#include "Vector2.hpp"

enum class MenuItemType {
    M_BAR,
    M_BUTTON,
};

class GMenuItem {
    public:
        GMenuItem(Vector2 pos, SDL_Color color, MenuItemType type) : m_pos(pos), m_color(color), m_type(type) {}

        virtual void render() = 0;
        virtual void renderHighlighted() = 0;
        virtual void bind(std::function<void()> f) {};
        virtual void bind(std::function<void()> l, std::function<void()> r) {};
        virtual void trigger() {}
        virtual void triggerLeft() {}
        virtual void triggerRight() {}
        // TODO: Move the triggering to the menu items and let them choose based on events?
        virtual void update() {}
        MenuItemType type() const { return m_type; }

        std::shared_ptr<GMenuItem> up;
        std::shared_ptr<GMenuItem> down;
        std::shared_ptr<GMenuItem> left;
        std::shared_ptr<GMenuItem> right;

    protected:
        Vector2 m_pos;
        SDL_Color m_color;
        MenuItemType m_type;
};

class GMenuItemButton : public GMenuItem {
public:
    GMenuItemButton(GUI *gui, Vector2 pos, std::string content, std::string font,
                    SDL_Color color, SDL_Color highlighted);

    void render() override;
    void renderHighlighted() override;
    void bind(std::function<void()> f) override;
    void trigger() override;

private:
    std::shared_ptr<GText> m_text;
    std::function<void()> m_select;
    SDL_Color m_highlighted;
};

// TODO: Move the creation of text to outside the menuitem?
class GMenuItemBar : public GMenuItem {
    public:
        GMenuItemBar(GUI* gui, Vector2 pos, Vector2 dim, int step, float scale,
                    std::string content, std::string font,
                    SDL_Color color, SDL_Color highlighted);

        void render() override;
        void renderHighlighted() override;
        void bind(std::function<void()> l, std::function<void()> r) override;
        void triggerLeft() override;
        void triggerRight() override;

    private:
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
        GMenu(Vector2 pos, SoundManager *sm);

        void setCurrent(std::shared_ptr<GMenuItem> item);

        void moveUp();
        void moveDown();
        void moveLeft();
        void moveRight();

        void moveSound();

        void trigger();

        void render();
        void addItem(std::shared_ptr<GMenuItem> item);

        void onEvent(const SDL_Event& event) override;

        int getX() const;
        int getY() const;

    private:
        Vector2 m_pos;
        std::vector<std::shared_ptr<GMenuItem>> m_menuItems;
        std::shared_ptr<GMenuItem> m_current;

        SoundManager *m_sm;
};
