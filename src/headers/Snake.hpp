#pragma once

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <iostream>
#include <memory>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <functional>
#include <map>
#include <cmath>

#include "Grid.hpp"
#include "Gui.hpp"
#include "Score.hpp"
#include "Sprite.hpp"
#include "Observer.hpp"
#include "Effect.hpp"
#include "Vector2.hpp"
#include "Common.hpp"


// TODO: Replace this with Vector2?
struct direction {
    int x;
    int y;
};

#define DIR_UP      direction{ 0, -1}
#define DIR_DOWN    direction{ 0,  1}
#define DIR_RIGHT   direction{ 1,  0}
#define DIR_LEFT    direction{-1,  0}

constexpr bool operator==(const direction& lhs, const direction& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

class UIElementSnakeEffect {
    public:
        UIElementSnakeEffect(SDL_Renderer *renderer, SDL_Texture *texture, SDL_Color color, Vector2 pos, int width, int height, float scale);

        void render();
        void update(float e, float d);

    private:
        SDL_Rect m_barBackgroundLayer;
        SDL_Rect m_effectDurationBar;
        SDL_Rect m_textureRect;
        SDL_Texture *m_texture;
        SDL_Renderer *m_renderer;

        SDL_Color m_color;
        
        Vector2 m_pos;
        Vector2 m_barPos;

        int m_width;
        int m_height;

        int barHeight;
        int m_barWidthMax;
        int m_barWidth;

        // float duration;
};

class Snakeblock {

    public: 
        Snakeblock(GUI *gui, int snakeBlockXpos, int snakeBlockYpos, int snakeBlockWidth, int snakeBlockHeight, SDL_Texture *texture, int degrees, SDL_Color color, direction dir);
        Snakeblock(GUI *gui, int snakeBlockXpos, int snakeBlockYpos, int snakeBlockWidth, int snakeBlockHeight, std::shared_ptr<Sprite> sprite, int degrees, SDL_Color color, direction dir);
        ~Snakeblock();

        void render();
        void renderWithAlpha(int alpha);
        void renderHead();

        Vector2 getPos();

        int getPosX();
        int getPosY();

        void setTexture(SDL_Texture *texture);
        void setSprite(std::shared_ptr<Sprite> sprite);
        void rotateTexture(int degrees);

        void setDegrees(int degrees);
        int getDegrees();
        direction getDirection();

    private:
        int m_snakeBlockWidth;
        int m_snakeBlockheight;

        Vector2 m_snakeBlockPos;
        direction m_snakeBlockDirection;
        int m_snakeBlockXpos;
        int m_snakeBlockYpos;
        int m_degrees;
        SDL_Color m_color;

        SDL_Rect m_snakeblock;
        SDL_Rect m_snakeblockOverlay;
        SDL_Renderer *m_renderer;
        GUI *m_gui;

        std::shared_ptr<Sprite> m_sprite;

        SDL_Texture *m_texture;
        int m_textureDegreeOffset;

};

class Snake : public Observer {

    public:
        Snake(GUI *gui, Vector2 pos, Grid *grid, int snakeSize, SDL_Color color, int pid, int speed);
        Snake() {
            
        }
        ~Snake();

        void render();
        void update(double deltaTime);

        void updatePos(int xPos, int yPos);

        void setSpeed(int speed);

        void grow();
        void grow(int xPos, int yPos);

        Vector2 getPos() {
            return snakeBlocks[0].getPos();
        }

        int getSize() {
            return snakeBlocks.size();
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

        void applySpeedBoost();
        void removeSpeedBoost();
        void invertControls();
        void becomeGhost();
        void removeGhost();
        void applySlowBoost();
        void removeSlowBoost();
        void freeze();
        void unfreeze();

    private:

        void updateSnakePos(Gridpoint *gp);
        int calculateBodyOffset(direction dir1, direction dir2);

        int m_snakeSize;
        int m_snakeWidth;
        int m_snakeHeight;
        int m_degrees;
        int m_newDegrees;
        int m_pid;
        int m_speed;

        double m_limit;
        double m_speedLimit;
        const double m_speedLimitBase = 100.0f;

        bool m_speedBoost = false;
        float m_speedBoostTime;
        float m_speedBoostTimeout;
        float m_speedBoostTimeLimit;
        
        direction m_snakeDirection;
        direction m_newSnakeDirection;

        Grid *m_grid;
        GUI *m_gui;

        SDL_Renderer *m_renderer;

        // TODO: Remove
        SDL_Texture *m_textureSnakeHead;
        SDL_Texture *m_textureSnakeBody;
        SDL_Texture *m_textureSnakeCurve;
        SDL_Texture *m_textureSnakeTail;

        // TODO: Turn into a list?
        std::shared_ptr<Sprite> m_spriteSnakeHead;
        std::shared_ptr<Sprite> m_spriteSnakeBody;
        std::shared_ptr<Sprite> m_spriteSnakeCurve;
        std::shared_ptr<Sprite> m_spriteSnakeTail;

        int m_textureSnakeHeadDegreeOffset;
        SDL_Color m_color;
        SDL_Rect m_speedBoostRect = {0,0,0,0};

        std::vector<Snakeblock> snakeBlocks;

        void renderBoostBar();
        void renderEffectBars();
        SDL_Rect createEffectBar(float e, float d, int i);
        SDL_Rect createEffectTextureRect(int i);
        void createEffectUi();

        int m_effectBarHeight = 10;
        int m_effectBarWidth = 50;
        int m_effectBarXdefault = 0 + ((WINDOW_WIDTH / 100) * 5);
        int m_effectBarYdefault = WINDOW_HEIGHT - ((WINDOW_HEIGHT / 100) * 5);

        std::function<void(const std::string&)> signalCallback;

        // Effects
        bool m_invertControls = false;
        int m_isGhost = 0;
        int m_freeze = 0;

        std::vector<std::unique_ptr<Effect>> m_effects;
        std::unordered_map<TextureID, std::unique_ptr<UIElementSnakeEffect>> m_effectUIs;
        std::vector<std::unique_ptr<Effect>> m_snakeEffects;

};

