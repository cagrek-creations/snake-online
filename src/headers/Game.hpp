#pragma once

#include <iostream>
#include <memory>
#include <chrono>

#include "Common.hpp"
#include "Snake.hpp"
#include "Grid.hpp"
#include "Menu.hpp"
#include "Score.hpp"
#include "Soundmanager.hpp"
#include "Gui.hpp"
#include "Snake.hpp"
#include "Controller.hpp"
#include "Vector2.hpp"
#include "utils.hpp"

#include "SnakeEffects.hpp"

typedef std::chrono::high_resolution_clock Clock;

enum gameState {START_MENU  = 0, 
                GAME_PLAY   = 1,
                OPTIONS     = 2, 
                GAME_QUIT   = 3};


template <typename ClassType>
std::function<void()> bindMemberFunction(std::unique_ptr<ClassType>& object, void (ClassType::*memberFunction)()) {
    return [ptr = object.get(), memberFunction]() {
        (ptr->*memberFunction)();
    };
}


class Game : public Observer{

    public:
        Game();


        void createGrid();
        void createGrid(int width, int height); 
        void createPlayer();
        void createPlayer(int size, int xPos, int yPos);


        void update(double deltaTime);
        void render();

        void renderState();

        void onEvent(const SDL_Event& event) override;

        bool isRunning();
        
    private:
        double m_deltaTime = 0;
        int m_myPid = -1;
        int m_state = START_MENU;
        bool m_isRunning = true;
        bool m_serverSetupIsComplete = false;

        std::string m_serverIp = ""; 
        int m_serverPort = -1; 

        std::string m_playerName = "default";
        std::string m_playerColor = "green";

        std::unordered_map<int, std::shared_ptr<Snake>> m_players{};
        std::unordered_map<std::string, std::shared_ptr<Score>> m_scores{};

        std::unique_ptr<Grid> m_grid;

        std::unique_ptr<GUI> m_gui;

        std::unique_ptr<Menu> m_startMenu;
        std::unique_ptr<Menu> m_optionsMenu;

        std::unique_ptr<Controller> m_gameController;


        std::string m_lastPosition;

        // Sound
        int m_volume = 64;
        int m_playSound = 1;
        std::unique_ptr<SoundManager> m_sound;

        void handleEvents(std::vector<std::string> serverEvents);
        void handleEvent(std::vector<std::string> &event);

        void setupGui();
        void setupSound();
        void setupController();
        void setupGame();

        // Game logic
        void setupFromServer(std::vector<std::string> event);
        void addNewPlayer(std::vector<std::string> event);
        void addPlayer(std::vector<std::string> event);
        void updatePlayerPosition(std::vector<std::string> event);

        void addScore(Vector2 pos, const std::string &type);
        void removeScore(Vector2 pos);
        void playerGrow();

        void handleEffects(const std::string &type, int pid);
};
