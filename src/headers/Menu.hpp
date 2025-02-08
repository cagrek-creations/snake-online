#pragma once

#include <iostream>
#include <memory>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <vector>
#include <thread>
#include <functional>

#include "Observer.hpp"

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

struct Text {
    int width;
    int height;
    int xPos;
    int yPos;
    std::string name;
    SDL_Texture *texture;

    void updateX(int newX) {
        xPos = newX;
    }

    void updateY(int newY) {
        yPos = newY;
    }
};


class MenuItem;

class Menu : public Observer {

    public:
        Menu(SDL_Renderer *renderer, int menuid, int xPos, int yPos, 
                int width, int height, TTF_Font *font, int &state, int previousState, int menuOwnState);
        ~Menu();

        

        int addItem(); // Can be removed?

        template <typename T>
        void addItemT(std::unique_ptr<T> mi) {
            if(m_items.size() < 1) mi->update();
            m_items.push_back(std::move(mi));
        }

        int addItemState(const std::string &name, int nextState);

        int addItemBar(std::string name, std::function<void()> refFuncL, std::function<void()> refFuncR);

        void render();
        int update(double deltaTime, bool gameRunning);
        int getMenuIndex();
        int getMenuWidth();
        int getMenuXpos();
        int *getMenuState();

    private:


        int m_width;
        int m_height;
        int m_xPos;
        int m_yPos;
        int m_menuIndex;
        int m_limit;
        int m_menuid;

        int *m_state;
        int m_menuOwnState;
        int m_previousState;
        
        bool m_activeMenu;
        // bool m_running;
        bool m_updateMenu;
        

        SDL_Event m_event;

        void updateMenu();
        void onEvent(const SDL_Event& event) override;
        Text createText(const std::string &name, int xPos, int yPos, SDL_Color textColor);
        bool updateText(Text &t, SDL_Color textColor);
        bool updateTextValue(Text &t, const std::string newText, MenuItem &mi);

        std::vector<std::shared_ptr<MenuItem>> m_items;

        TTF_Font *m_font;
        SDL_Renderer *m_renderer;

};

class MenuItem {

    public:
        MenuItem(SDL_Renderer *renderer, std::string name, int xPos, int yPos, TTF_Font *font, Menu &mi) : m_mi(mi) {
            m_renderer      = renderer;
            m_xPos          = xPos;
            m_yPos          = yPos;
            m_name          = name;
            m_font          = font;


            m_menuText = createText(name, m_xPos, m_yPos, menuc::WHITE);
            m_menuText.updateX(m_xPos + (m_mi.getMenuWidth() - m_menuText.width) / 2 );

        }

        std::string getTextString() {
            return m_textString;
        }

        SDL_Color getColor() {
            return m_color;
        }
        
        virtual void trigger(int key) {
            std::cout << "Base trigger" << std::endl;
        }

        virtual void update() {
            std::cout << "Base update" << std::endl;
        }

        virtual void reset() {
            std::cout << "Base reset" << std::endl;
        }

        virtual void render() {
            std::cout << "Base render" << std::endl;
        }

    protected:
    
        int m_xPos;
        int m_yPos;

        Menu &m_mi;

        std::string m_name;

        Text m_menuText;

        TTF_Font *m_font;
        SDL_Color m_color;
        SDL_Renderer *m_renderer;

        bool updateTextColor(Text &txt, SDL_Color textColor) {
            SDL_Surface *textSurface = TTF_RenderText_Solid(m_font, txt.name.c_str(), textColor);
            if (!textSurface) {
                std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
            }

            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
            if (!textTexture) {
                std::cerr << "Unable to create texture from rendered text! SDL_Error: " << SDL_GetError() << std::endl;
                SDL_FreeSurface(textSurface);
            }
            SDL_FreeSurface(textSurface);

            txt.texture = textTexture;
            return true;
        }

    private:
        std::string m_textString;

        Text createText(const std::string &name, int xPos, int yPos, SDL_Color textColor) {
            SDL_Surface *textSurface = TTF_RenderText_Solid(m_font, name.c_str(), textColor);

            if (!textSurface) {
                std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
            }

            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
            if (!textTexture) {
                std::cerr << "Unable to create texture from rendered text! SDL_Error: " << SDL_GetError() << std::endl;
                SDL_FreeSurface(textSurface);
            }

            Text textInfo;
            textInfo.width      = textSurface->w;
            textInfo.height     = textSurface->h;
            textInfo.texture    = textTexture;
            textInfo.xPos       = xPos;
            textInfo.yPos       = yPos;
            textInfo.name       = name;

            SDL_FreeSurface(textSurface);
            return textInfo;
        }





};

class MenuState : public MenuItem {
    public:
        MenuState(SDL_Renderer *renderer, std::string name, int nextState/*std::function<void()> refFunc*/, 
                    int xPos, int yPos, TTF_Font *font, Menu &mi) 
        : MenuItem(renderer, name, xPos, yPos, font, mi) {
            // m_referenceFunction = refFunc;
            m_nextState = nextState;
        }

        void update() override {
            updateTextColor(m_menuText, menuc::RED);
        }

        void reset() override {
            updateTextColor(m_menuText, menuc::WHITE);
        }

        void trigger(int key) override {
            if(key == KEY_ENTER) {
                *(m_mi.getMenuState()) = m_nextState;
            }
        }

        void render() override {
            SDL_Rect renderQuad = {m_menuText.xPos, m_menuText.yPos, m_menuText.width, m_menuText.height};
            SDL_RenderCopy(m_renderer, m_menuText.texture, nullptr, &renderQuad);
        }

    private:
        int m_nextState;
        std::function<void()> m_referenceFunction;
};

class MenuBar : public MenuItem {
    
    public:
        MenuBar(SDL_Renderer *renderer, std::string name, int xPos, int yPos, 
        TTF_Font *font, std::function<void()> refFuncL, std::function<void()> refFuncR, Menu &mi) 
        : MenuItem(renderer, name, xPos, yPos, font, mi) {
            m_referenceFunctionLeft     = refFuncL;
            m_referenceFunctionRight    = refFuncR;
            m_max           = 10;
            m_min           = 0;
            m_barWidth      = m_mi.getMenuWidth() - (m_mi.getMenuWidth() / 5);
            m_barHeight     = m_menuText.height / 2;
            m_step          = (m_mi.getMenuWidth() - (m_mi.getMenuWidth() / 5)) / (m_max - m_min);
            m_progress      = m_step * ((m_max - m_min) / 2);
            m_highlighted   = false;
            // std::cout << m_barWidth << std::endl;
            // std::cout << m_step << std::endl;

            m_rectA = {m_mi.getMenuXpos() + (m_mi.getMenuWidth() / 10), 
                                m_menuText.yPos + m_barHeight*2, m_barWidth, m_barHeight};

            m_rectB = {m_mi.getMenuXpos() + (m_mi.getMenuWidth() / 10), 
                                m_menuText.yPos + m_barHeight*2, m_progress, m_barHeight};

            m_rectC = {m_mi.getMenuXpos() + (m_mi.getMenuWidth() / 10) - 2, 
                                m_menuText.yPos + m_barHeight*2 - 2, m_barWidth + 4, m_barHeight + 4};

        }

        void trigger(int key) override {
            if (key == KEY_RIGHT){
                progressIncrease();
            } else if (key == KEY_LEFT) {
                progressDecrease();
            }
            
            // Update progress
            m_rectB = {m_mi.getMenuXpos() + (m_mi.getMenuWidth() / 10), 
                                m_menuText.yPos + m_barHeight*2, m_progress, m_barHeight};
        }

        void update() {
            updateTextColor(m_menuText, menuc::RED);
            m_highlighted = true;
        }

        void reset() {
            updateTextColor(m_menuText, menuc::WHITE);
            m_highlighted = false;
        }

        void render() override {
            SDL_Rect renderQuad = {m_menuText.xPos, m_menuText.yPos, m_menuText.width, m_menuText.height};
            SDL_RenderCopy(m_renderer, m_menuText.texture, nullptr, &renderQuad);

            // If highlighted
            if(m_highlighted) {
                SDL_SetRenderDrawColor(m_renderer, 128, 128, 128, 255);
                SDL_RenderFillRect(m_renderer, &m_rectC);
            }

            // White background for bar
            SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(m_renderer, &m_rectA);
            
            // Progress of bar
            SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(m_renderer, &m_rectB);
        }


    private:

        void progressIncrease() {
            if(m_progress < m_max * m_step) {
                m_progress += m_step;
                m_referenceFunctionRight();
            }
        }

        void progressDecrease() {
            if(m_progress > 0) {
                m_progress -= m_step;
                m_referenceFunctionLeft();
            }
        }

        bool within(int progress) {
            if(m_progress < m_max || m_progress > m_min) return true;
            return false;
        }

        int m_progress;
        int m_max;
        int m_min;
        int m_step;
        bool m_highlighted;

        int m_barWidth;
        int m_barHeight;

        SDL_Rect m_rectA; // Background for bar
        SDL_Rect m_rectB; // Bar progress
        SDL_Rect m_rectC; // Highlighted bar

        std::function<void()> m_referenceFunctionLeft;
        std::function<void()> m_referenceFunctionRight;

};

// struct MenuItem {

//     Text menuText;
//     std::string textString;
//     int &referenceValue;
//     int menuWidth;
//     int menuHeight;
//     int menuXpos;
//     int menuYpos;
//     int previousState;
//     int nextState;
//     int type;
//     bool onoff;
//     std::function<void()> refFunc;


//     MenuItem(int &refValue) : referenceValue(refValue) {}

//     void render() {
//         if(type == MENU_BAR) {
//             SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
//             SDL_Rect rectA = {menuXpos + (menuWidth / 10), menuText.yPos, menuWidth - (menuWidth / 5), menuText.height};
//             SDL_RenderFillRect(m_renderer, &rectA);
//             SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
//             SDL_Rect rectB = {menuXpos + (menuWidth / 10), menuText.yPos, int(5*referenceValue), menuText.height};
//             SDL_RenderFillRect(m_renderer, &rectB);

//         } else {
//             SDL_Rect renderQuad = {menuText.xPos, menuText.yPos, menuText.width, menuText.height};
//             SDL_RenderCopy(m_renderer, menuText.texture, nullptr, &renderQuad);
//         }

//     }
// };