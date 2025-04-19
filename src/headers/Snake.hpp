#pragma once

#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <functional>

#include "Grid.hpp"
#include "Gui.hpp"
#include "Observer.hpp"
#include "Effect.hpp"
#include "Vector2.hpp"

struct direction {
    int x;
    int y;
};

#define DIR_UP      direction{ 0, -1}
#define DIR_DOWN    direction{ 0,  1}
#define DIR_RIGHT   direction{ 1,  0}
#define DIR_LEFT    direction{-1,  0}

class Snakeblock {

    public: 
        Snakeblock(SDL_Renderer *renderer, int snakeBlockXpos, int snakeBlockYpos, int snakeBlockWidth, int snakeBlockHeight, SDL_Texture *textureSnakeHead, int degrees, SDL_Color color);
        ~Snakeblock();

        void render(); 
        void renderHead();

        Vector2 getPos();

        int getPosX();
        int getPosY();

    private:
        int m_snakeBlockWidth;
        int m_snakeBlockheight;

        Vector2 m_snakeBlockPos;
        int m_snakeBlockXpos;
        int m_snakeBlockYpos;
        int m_degrees;
        SDL_Color m_color;

        SDL_Rect m_snakeblock;
        SDL_Rect m_snakeblockOverlay;
        SDL_Renderer *m_renderer;

        SDL_Texture *m_textureSnakeHead;

};

class Snake : public Observer {

    public:
        Snake(GUI *gui, int xPos, int yPos, Grid *grid, int snakeWidth, int snakeHeight, int snakeSize, SDL_Color color, int pid);
        Snake() {
            
        }
        ~Snake();

        void render();
        void update(double deltaTime, float limit);

        void updatePos(int xPos, int yPos);

        void grow();
        void grow(int xPos, int yPos);

        Vector2 getPos() {
            return snakeBlocks[0].getPos();
        }

        // int getPosX() {
        //     return snakeBlocks[0].getPosX();
        // }

        // int getPosY() {
        //     return snakeBlocks[0].getPosY();
        // }

        int getSize() {
            return snakeBlocks.size();
        }

        bool isHead(int xPos, int yPos) {
            return (snakeBlocks[0].getPosX() == xPos) && (snakeBlocks[0].getPosY() == yPos);
        }

        void getPositions() {
            for (auto &sb : snakeBlocks) {
                std::cout << sb.getPos().x << ", " << sb.getPos().y << std::endl; 
            }
        }

        void onEvent(const SDL_Event& event) override;

        void setSignalCallback(std::function<void(const std::string&)> callback) override {
            signalCallback = std::move(callback);
        } 

        void signalController(const std::string& message) {
            if (signalCallback) {
                signalCallback(message);
            }
        }

        // Effects
        void updateEffects(float deltaTime);
        void addEffect(std::unique_ptr<Effect> effect);

        void invertControls();


    private:
        int m_snakeSize;
        int m_snakeWidth;
        int m_snakeHeight;
        int m_degrees;
        int m_newDegrees;
        int m_pid;

        double m_limit;

        bool m_speedBoost = false;
        float m_speedBoostTime;
        float m_speedBoostTimeout;
        float m_speedBoostTimeLimit;
        
        direction m_snakeDirection;
        direction m_newSnakeDirection;

        Grid *m_grid;
        GUI *m_gui;

        SDL_Renderer *m_renderer;
        SDL_Texture *m_textureSnakeHead;
        SDL_Color m_color;
        SDL_Rect m_speedBoostRect = {0,0,0,0};

        std::vector<Snakeblock> snakeBlocks;

        void renderBoostBar();

        std::function<void(const std::string&)> signalCallback;

        // Effects
        bool m_invertControls = false;

        std::vector<std::unique_ptr<Effect>> m_effects;

};

