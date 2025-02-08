#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <thread>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>

#include "Gui.hpp"
#include "Snake.hpp"
#include "Grid.hpp"
#include "Menu.hpp"
#include "Score.hpp"
#include "Game.hpp"
#include "Soundmanager.hpp"
// #include "Event.hpp"
// #include "Client.hpp"

#include <windows.h>
#include <winbase.h>

typedef std::chrono::high_resolution_clock Clock;

#define FPS 60
#define frameDelay = 1000 / FPS

#define SEND_LIMIT 100

#define WINDOW_FULLSCREEN 0
// #define WINDOW_WIDTH 800
// #define WINDOW_HEIGHT 600

#define WINDOW_MIDDLE_X (WINDOW_WIDTH / 2)
#define WINDOW_MIDDLE_Y (WINDOW_HEIGHT / 2)

void fpsCap(Uint32 starting_tick) {
    // '1000 / FPS' is meant to be replaced with 'frameDelay'
    if ((1000 / FPS) > SDL_GetTicks() - starting_tick) {
        SDL_Delay(1000 / FPS - (SDL_GetTicks() - starting_tick));
    }
}

void updateSnake(Snake &s, double deltaTime, double limit) {
    s.update(deltaTime, limit);
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

// struct Game {
//     GUI gui = ...
//     SoundManager = ..
//     Snake =
//     Grid = 
// }


int main(int argc, char **argv) {

    int pid = -1;
    // Controller controller = Controller();

    
    Game g = Game();
    // GameController c = GameController();


    // int volume = 64;
    // int playSound = 1;
    // SoundManager sound = SoundManager(volume, playSound);
    // sound.loadSound("./debug/sounds/s.wav", "song");
    // sound.setVolume("song", volume); // 50%
    // sound.playSound("song", -1);

    std::unordered_map<int, std::shared_ptr<Snake>> players{};
    std::unordered_map<std::string, std::shared_ptr<Score>> scores{};

    double deltaTime = 0;
    uint32_t startingTick = 0;
    

    // Grid grid = Grid(ui.getRenderer(), WINDOW_WIDTH, WINDOW_HEIGHT, 40, 30);



    // int state = START_MENU;
    // // gameState state         = START_MENU;
    // // gameState previousState = START_MENU;

    // Menu startMenu      = Menu(ui.getRenderer(), 0, WINDOW_MIDDLE_X - (250 / 2), 
    //                                                 WINDOW_MIDDLE_Y - (200 / 2), 
    //                                                 250, 
    //                                                 200, 
    //                                                 ui.getFont(), state, START_MENU, START_MENU);
    
    // Menu optionsMenu    = Menu(ui.getRenderer(), 1, WINDOW_MIDDLE_X - (250 / 2), 
    //                                                 WINDOW_MIDDLE_Y - (200 / 2), 
    //                                                 250, 
    //                                                 200, 
    //                                                 ui.getFont(), state, START_MENU, OPTIONS);
    // int option = 0;
    // startMenu.addItemState("START GAME", GAME_PLAY);
    // startMenu.addItemState("OPTIONS",    OPTIONS  );
    // startMenu.addItemState("QUIT",       GAME_QUIT);

    // std::function<void()> funcL = bindMemberFunction(sound, &SoundManager::decreaseVolume);
    // std::function<void()> funcR = bindMemberFunction(sound, &SoundManager::increaseVolume);
    // optionsMenu.addItemBar("sound", funcL, funcR);
    // optionsMenu.addItemState("asd", 1);

    // int soundVolume;
    // optionsMenu.addItem("sound: ", MENU_ON_OFF, playSound);
    // optionsMenu.addItem("sound", MENU_BAR, volume);

    // TODO: Move logic to GameController?
    // c.attachObserver(&startMenu);
    // c.attachObserver(&optionsMenu);
    // c.attachObserver(&ui);
    // controller.attachObserver(player);
    // controller.attachObserver(&sound);

    // bool hasScore = false;
    // Gridpoint *scorePoint;

    // Score score = Score(ui.getRenderer(), &ui, grid.getGridPointWidth(), grid.getGridPointHeight());

    

    // bool running = true;
    // while(ui.getWindowClose() && running) {
    while(g.isRunning()) {
        auto t0 = Clock::now();
        g.render();
        g.update(deltaTime);
        auto t1 = Clock::now();
        deltaTime = (double)(std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count()) / 1000000.f;
    }
        
    //     c.update();

    //     auto t1 = Clock::now();
    //     startingTick = SDL_GetTicks();

    //     ui.clearRenderer();
    //     ui.update();
        
    //     if (state == START_MENU) {
    //         startMenu.render();
    //     } else if (state == OPTIONS) {
    //         optionsMenu.render();
    //         // sound.setVolume("song", volume);
    //         // std::cout << volume << std::endl;
    //     } else if (state == GAME_PLAY) {
    //         controller.broadcastNewMenu(3);
    //         // ui.render(grid);

    //         for (const auto& pair : players) {
    //             pair.second->render();
    //         }

    //         for (const auto &score : scores) {
    //             score.second->render();
    //         }



    //         // if(!hasScore) {
    //         //     std::pair<int, int> pos = getRandomCoordinate();
    //         //     std::cout << "X: " << pos.first << " Y: " << pos.second << std::endl;
    //         //     scorePoint = grid.getPoint(pos.first, pos.second);
    //         //     if(scorePoint != nullptr && scorePoint->isEmpty()) {
    //         //         scorePoint->setScore();
    //         //         score.move(scorePoint->getGridPointX(), scorePoint->getGridPointY());
    //         //         hasScore = true;
    //         //     }
    //         // } else {
    //         //     score.render();
    //         //     if(scorePoint != nullptr) hasScore = scorePoint->hasScore();
    //         //     else hasScore = false;
    //         // }
            
    //         // ui.render(*player); // Perhaps a better solution?

    //         // snake.update(deltaTime, 100.f);
    //         // player->update(deltaTime, 100.f);
    //     } else if (state == GAME_QUIT) {
    //         running = false;
    //     }
        
    //     ui.render();

    //     auto t3 = Clock::now();
               
    //     // See method-description
    //     deltaTime = (double)(std::chrono::duration_cast<std::chrono::nanoseconds>(t3 - t1).count()) / 1000000.f;

    //     // fpsCap(startingTick);
        
    //     // int gridXPos = 0;
    //     // int gridYPos = 0;
    //     // Gridpoint *gp = grid.getPoint(player->getPosX(), player->getPosY());
    //     // gridXPos = (gp->getGridPointX() / grid.getGridPointWidth());
    //     // gridYPos = (gp->getGridPointY() / grid.getGridPointHeight());
    //     // std::string command = "PLAYER_UPDATE_POSITION;" + std::to_string(pid) + ";" + std::to_string(gridXPos) + ";" + std::to_string(gridYPos) + ";";
    //     // if(client.isConnected()) client.send(command.c_str(), command.size()); 


    // }

    // client.closeConnection();
    return 0;
}
