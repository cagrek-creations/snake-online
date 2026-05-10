#include "headers/Game.hpp"

void Game::registerHandlers() {
    std::cout << "registering handlers" << std::endl;
    // TODO: I think this could segfault in the future if not handled properly
    handlers["NEW_PLAYER_RESPONSE"]     = &Game::handleNewPlayerResponse;

    handlers["ADD_SCORE"]               = &Game::handleAddScore;
    handlers["BERRY_POSITION"]          = &Game::handleBerryPosition;
    handlers["NEW_PLAYER"]              = &Game::addNewPlayer;
    // handlers["NEW_PLAYER_JOINED"]       = &Game::handleNewPlayerJoined;

    handlers["GAME_STARTED"]            = &Game::handleWaitGame;
    handlers["GAME_STARTING"]           = &Game::handleWaitGame;

    handlers["PLAYER_INFO"]             = &Game::addPlayer;
    handlers["PLAYER_NEW_POS"]          = &Game::updatePlayerPosition;
    handlers["PLAYER_UPDATE_POSITION"]  = &Game::updatePlayerPosition;
    // handlers["PLAYING_FIELD"]           = &Game::handlePlayingField;
    handlers["SCORE_COLLECTED"]         = &Game::handleScoreCollected;
    // handlers["WAITING_FOR_PLAYERS"]     = &Game::handleWaitingForPlayers;

    allowedBeforeSetup.insert("NEW_PLAYER_RESPONSE");
}

void Game::handleEvent(std::vector<std::string> &event) {
    std::string command = event[0];

    // Before game is setup, only allow specific commands.
    std::cout << *allowedBeforeSetup.begin() << std::endl;
    if (!m_serverSetupIsComplete && (allowedBeforeSetup.find(command) == allowedBeforeSetup.end())) {
        std::cout << command << " is not allowed before server setup" << std::endl;
        return;
    }
    std::cout << "cmd: " << command << std::endl;
    auto it = handlers.find(command);

    if (it != handlers.end()) {
        (this->*(it->second))(event);
    }

}

void Game::handleWaitGame(const std::vector <std::string> &event) {
    m_state = GAME_SPECTATE;
}

void Game::handleAddScore(const std::vector<std::string> &event) {
    std::string type = event[1];
    int xPos = stoi(event[3]);
    int yPos = stoi(event[4]);
    addScore(Vector2(xPos, yPos), type);
}

void Game::handleBerryPosition(const std::vector<std::string> &event) {
    int xPos = stoi(event[1]);
    int yPos = stoi(event[2]);
    addScore(Vector2(xPos, yPos), "berry");
}

void Game::handleNewPlayerResponse(const std::vector<std::string> &event) {
    setupFromServer(event);
    std::cout << "PID" << m_myPid << std::endl;
    m_serverSetupIsComplete = true;
}

void Game::handleScoreCollected(const std::vector<std::string> &event) {
    int pid = stoi(event[1]);
    std::string type = event[2];
    int xPos = stoi(event[4]);
    int yPos = stoi(event[5]);

    removeScore(Vector2(xPos, yPos));
    m_players[pid]->grow();

    handleEffects(type, pid);
}


    // if (command == "PLAYER_SCORE_COLLECTED") {
    //     int xPos = stoi(event[1]);
    //     int yPos = stoi(event[2]);
    //     std::string command = "SCORE_COLLECTED;" + std::to_string(m_myPid) + ";0;0;" + std::to_string(xPos) + ";" + std::to_string(yPos); 
    //     m_gameController->sendMessage(command);
    //     removeScore(xPos, yPos);
    //     playerGrow();
    // }