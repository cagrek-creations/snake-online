#include "headers/Game.hpp"

void Game::handleEvent(std::vector<std::string> &event) {
    std::string command = event[0];

    // TODO: I think this could segfault in the future if not handled properly
    if (command == "NEW_PLAYER_RESPONSE") {
        handleNewPlayerResponse(event);
    }

    if (!m_serverSetupIsComplete) return;

    if (command == "ADD_SCORE") {
        handleAddScore(event);
    }

    if (command == "BERRY_POSITION") {
        handleBerryPosition(event);
    }

    if (command == "PLAYING_FIELD") {
        
    }

    if (command == "WAITING_FOR_PLAYERS") {

    }

    // SCORE_COLLECTED;0;berry;1;12;22
    if (command == "SCORE_COLLECTED") {
        handleScoreCollected(event);
    }

    if (command == "NEW_PLAYER") {
        addNewPlayer(event);
    }

    if (command == "NEW_PLAYER_JOINED") {

    }

    if (command == "PLAYER_INFO") {
        addPlayer(event);
    }

    if (command == "PLAYER_NEW_POS") {
        updatePlayerPosition(event);
    }

    // PLAYER_UPDATE_POSITION;pid;xPos;yPos
    if (command == "PLAYER_UPDATE_POSITION") {
        updatePlayerPosition(event);
    }
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