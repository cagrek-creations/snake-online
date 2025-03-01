#include "Game.hpp"

void Game::handleEvents(std::vector<std::string> serverEvents) {
    for (auto se : serverEvents) {
        std::vector<std::string> event = splitString(se, ';');
        std::cout << "Handling: " << event[0] << std::endl;
        handleEvent(event);
    }
}

void Game::handleEvent(std::vector<std::string> &event) {
    std::string command = event[0];

    // TODO: I think this could segfault in the future if not handled properly
    if (command == "NEW_PLAYER_RESPONSE") {
        setupFromServer(event);
        m_serverSetupIsComplete = true;
    }

    if (!m_serverSetupIsComplete) return;

    if (command == "PLAYING_FIELD") {
        
    }

    if (command == "ADD_SCORE") {
        int xPos = stoi(event[3]);
        int yPos = stoi(event[4]);
        addScore(xPos, yPos);
    }

    if (command == "BERRY_POSITION") {
        int xPos = stoi(event[1]);
        int yPos = stoi(event[2]);
        addScore(xPos, yPos);
    }

    // if (command == "PLAYER_SCORE_COLLECTED") {
    //     int xPos = stoi(event[1]);
    //     int yPos = stoi(event[2]);
    //     std::string command = "SCORE_COLLECTED;" + std::to_string(m_myPid) + ";0;0;" + std::to_string(xPos) + ";" + std::to_string(yPos); 
    //     m_gameController->sendMessage(command);
    //     removeScore(xPos, yPos);
    //     playerGrow();
    // }

    // SCORE_COLLECTED;0;berry;1;22;26
    if (command == "SCORE_COLLECTED") {
        int pid = stoi(event[1]);
        int xPos = stoi(event[4]) * m_grid->getGridPointWidth();
        int yPos = stoi(event[5]) * m_grid->getGridPointHeight();

        removeScore(xPos, yPos);
        m_players[pid]->grow();
    }

    if (command == "NEW_PLAYER") {
        addNewPlayer(event);
    }

    if (command == "PLAYER_INFO") {
        addNewPlayer(event);
    }

    if (command == "NEW_PLAYER_JOINED") {

    }

    if (command == "PLAYER_NEW_POS") {
        updatePlayerPosition(event);
    }

    // PLAYER_UPDATE_POSITION;pid;xPos;yPos
    if (command == "PLAYER_UPDATE_POSITION") {
        updatePlayerPosition(event);
    }
}

void Game::updatePlayerPosition(std::vector<std::string> event) {
    int pid = stoi(event[1]);
    int xPos = stoi(event[2]);
    int yPos = stoi(event[3]);
    if (m_players[pid]) {
        m_players[pid]->updatePos(xPos * m_grid->getGridPointGranularityX(), yPos * m_grid->getGridPointGranularityY());
    }
}

void Game::addNewPlayer(std::vector<std::string> event) {
    int eventsize = event.size();
    int pid = stoi(event[1]);
    std::string color = event[3];
    int snakeSize = stoi(event[4]);
    int xPos = stoi(event[5]) * m_grid->getGridPointGranularityX();
    int yPos = stoi(event[6]) * m_grid->getGridPointGranularityY();

    std::shared_ptr<Snake> newPlayer = std::make_shared<Snake>(m_gui.get(), xPos, yPos, m_grid.get(), 40, 40, snakeSize, m_gui->getColor(color), m_players.size());
    m_players[pid] = std::move(newPlayer);

    // Should always be true but just in case.
    for (int i = 7; i <= eventsize - 1; i += 2) {
        int _xPos = stoi(event[i]) * m_grid->getGridPointWidth();
        int _yPos = stoi(event[i + 1]) * m_grid->getGridPointHeight();
        std::cout << _xPos << ", " << _yPos << std::endl;
        m_players[pid]->grow(_xPos, _yPos);
    }
    
}

void Game::setupFromServer(std::vector<std::string> event) {
    // Set pid
    m_myPid = stoi(event[1]);
    
    // Set grid size
    createGrid(stoi(event[4]), stoi(event[5]));

    // Set snake
    createPlayer(6, stoi(event[2]), stoi(event[3]));
}

void Game::removeScore(int xPos, int yPos) {
    Gridpoint *gp = m_grid->getPoint(xPos + 1, yPos + 1);
    if(!(gp == nullptr)) {
        gp->removeScore();
        std::string key = std::to_string(gp->getGridPointX()) + "," + std::to_string(gp->getGridPointY());
        std::cout << "key remove: " << key << std::endl;
        m_scores.erase(key);
    }
}

void Game::playerGrow() {
    m_players[m_myPid]->grow();
}

void Game::addScore(int xPos, int yPos) {
    std::shared_ptr<Score> score = std::make_shared<Score>(m_gui->getRenderer(), m_gui.get(), m_grid->getGridPointWidth(), m_grid->getGridPointWidth());
    xPos = ((xPos) * (m_grid->getGridPointWidth())) + 1;
    yPos = ((yPos) * (m_grid->getGridPointHeight())) + 1;
    Gridpoint *gp = m_grid->getPoint(xPos, yPos);
    if(!(gp == nullptr)) {
        gp->setScore();
        score->move(gp->getGridPointX(), gp->getGridPointY());
        std::string key = std::to_string(gp->getGridPointX()) + "," + std::to_string(gp->getGridPointY());
        std::cout << "key: " << key << std::endl;
        m_scores[key] = (score);
    }
}