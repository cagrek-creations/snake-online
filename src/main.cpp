#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <thread>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>

#include "headers/Gui.hpp"
#include "headers/Snake.hpp"
#include "headers/Grid.hpp"
#include "headers/Menu.hpp"
#include "headers/Score.hpp"
#include "headers/Game.hpp"
#include "headers/Soundmanager.hpp"
// #include "Event.hpp"
// #include "Client.hpp"

#if defined(_WIN32)
#include <windows.h>
#include <winbase.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

typedef std::chrono::high_resolution_clock Clock;

#define FPS 60
#define frameDelay = 1000 / FPS

#define SEND_LIMIT 100

void fpsCap(Uint32 starting_tick) {
    // '1000 / FPS' is meant to be replaced with 'frameDelay'
    if ((1000 / FPS) > SDL_GetTicks() - starting_tick) {
        SDL_Delay(1000 / FPS - (SDL_GetTicks() - starting_tick));
    }
}

void updateSnake(Snake &s, double deltaTime, double limit) {
    s.update(deltaTime);
}

std::pair<int, int> getRandomCoordinate() {
    std::random_device rd;   
    std::mt19937 gen(rd());  

    // Define the range for x and y
    std::uniform_int_distribution<int> xDist(0, WINDOW_WIDTH);  
    std::uniform_int_distribution<int> yDist(0, WINDOW_HEIGHT);  

    // Generate random x and y coordinates
    int x = xDist(gen);
    int y = yDist(gen);

    return std::make_pair(x, y);
}

// enum gameState {START_MENU  = 0, 
//                 GAME_PLAY   = 1,
//                 OPTIONS     = 2, 
//                 GAME_QUIT   = 3};

std::string gameStateToString(gameState value) {
    switch (value) {
        case START_MENU:
            return "START_MENU";
        case GAME_PLAY:
            return "GAME_PLAY";
        case OPTIONS:
            return "OPTIONS";
        case GAME_QUIT:
            return "GAME_QUIT";
        default:
            return "UNKNOWN_GAME_STATE";
    }
}

template <typename ClassType>
std::function<void()> bindMemberFunction(ClassType& object, void (ClassType::*memberFunction)()) {
    return std::bind(memberFunction, std::ref(object));
}

// break out to controller.config.get("key")
void getColor(GUI &ui, SDL_Color &color, std::string &colorString) {
    std::unordered_map<std::string, std::string> config = getConfiguration("config.txt");

    auto color_it = config.find("color");

    if (color_it != config.end()) {
        color = ui.getColor(color_it->second);
        colorString = color_it->second;
    } 
}

int main(int argc, char **argv) {

    int pid = -1;
    
    auto g = std::make_shared<Game>();
    g->init();

    std::unordered_map<int, std::shared_ptr<Snake>> players{};
    std::unordered_map<std::string, std::shared_ptr<Score>> scores{};

    double deltaTime = 0;
    uint32_t startingTick = 0;

    while(g->isRunning()) {
        auto t0 = Clock::now();
        g->render();
        g->update(deltaTime);
        auto t1 = Clock::now();
        deltaTime = (double)(std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count()) / 1000000.f;
    }
    
    return 0;
}
