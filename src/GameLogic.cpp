#include "headers/Game.hpp"

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

    if (command == "WAITING_FOR_PLAYERS") {

    }

    if (command == "ADD_SCORE") {
        std::string type = event[1];
        int xPos = stoi(event[3]);
        int yPos = stoi(event[4]);
        addScore(Vector2(xPos, yPos), type);
    }

    if (command == "BERRY_POSITION") {
        int xPos = stoi(event[1]);
        int yPos = stoi(event[2]);
        // TODO: update
        addScore(Vector2(xPos, yPos), "berry");
    }

    // if (command == "PLAYER_SCORE_COLLECTED") {
    //     int xPos = stoi(event[1]);
    //     int yPos = stoi(event[2]);
    //     std::string command = "SCORE_COLLECTED;" + std::to_string(m_myPid) + ";0;0;" + std::to_string(xPos) + ";" + std::to_string(yPos); 
    //     m_gameController->sendMessage(command);
    //     removeScore(xPos, yPos);
    //     playerGrow();
    // }

    // SCORE_COLLECTED;0;berry;1;12;22
    if (command == "SCORE_COLLECTED") {
        int pid = stoi(event[1]);
        std::string type = event[2];
        int xPos = stoi(event[4]);
        int yPos = stoi(event[5]);

        removeScore(Vector2(xPos, yPos));
        m_players[pid]->grow();

        handleEffects(type, pid);
    }

    if (command == "NEW_PLAYER") {
        addNewPlayer(event);
    }

    if (command == "PLAYER_INFO") {
        addPlayer(event);
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
        // TODO: Fix positioning. Currently it is not aligned with server or game logic
        m_players[pid]->updatePos(xPos * m_grid->getGridPointWidth(), yPos * m_grid->getGridPointHeight());
    }
}

void Game::addNewPlayer(std::vector<std::string> event) {
    int eventsize = event.size();
    int pid = stoi(event[1]);
    std::string color = event[3];
    int snakeSize = stoi(event[4]);
    int xPos = stoi(event[5]) * m_grid->getGridPointWidth();
    int yPos = stoi(event[6]) * m_grid->getGridPointHeight();
    Vector2 pos = Vector2(xPos, yPos);

    std::shared_ptr<Snake> newPlayer = std::make_shared<Snake>(m_gui.get(), pos, m_grid.get(), snakeSize, m_gui->getColor(color), m_players.size(), 1);
    m_players[pid] = std::move(newPlayer);
}

void Game::addPlayer(std::vector<std::string> event) {
    int eventsize = event.size();
    int pid = stoi(event[1]);
    std::string color = event[3];
    int xPos = stoi(event[4]) * m_grid->getGridPointWidth();
    int yPos = stoi(event[5]) * m_grid->getGridPointHeight();
    Vector2 pos = Vector2(xPos, yPos);

    std::shared_ptr<Snake> newPlayer = std::make_shared<Snake>(m_gui.get(), pos, m_grid.get(), 1, m_gui->getColor(color), m_players.size(), 1);
    m_players[pid] = std::move(newPlayer);

    // Should always be true but just in case.
    for (int i = 6; i <= eventsize - 1; i += 2) {
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

void Game::addScore(Vector2 pos, const std::string &type) {
    std::shared_ptr<Score> score = std::make_shared<Score>(m_gui->getRenderer(), m_gui.get(), m_grid->getGridPointWidth(), m_grid->getGridPointWidth(), TextureID::ERR);
    if (type == "berry") {
        score = std::make_shared<Score>(m_gui->getRenderer(), m_gui.get(), m_grid->getGridPointWidth(), m_grid->getGridPointWidth(), TextureID::BERRY);
    } else if (type == "inverse_self") {
        score = std::make_shared<Score>(m_gui->getRenderer(), m_gui.get(), m_grid->getGridPointWidth(), m_grid->getGridPointWidth(), TextureID::SWAPAROO);
    } else if (type == "inverse_other") {
        score = std::make_shared<Score>(m_gui->getRenderer(), m_gui.get(), m_grid->getGridPointWidth(), m_grid->getGridPointWidth(), TextureID::SWAPAROO_O);
    } else if (type == "speed_self") {
        score = std::make_shared<Score>(m_gui->getRenderer(), m_gui.get(), m_grid->getGridPointWidth(), m_grid->getGridPointWidth(), TextureID::SPEED);
    } else if (type == "speed_other") {
        score = std::make_shared<Score>(m_gui->getRenderer(), m_gui.get(), m_grid->getGridPointWidth(), m_grid->getGridPointWidth(), TextureID::SPEED_O);
    } else if (type == "golden_berry") {
        score = std::make_shared<Score>(m_gui->getRenderer(), m_gui.get(), m_grid->getGridPointWidth(), m_grid->getGridPointWidth(), TextureID::GOLDEN_BERRY);
        SDL_Rect target{-99, -99, m_grid->getGridPointWidth(), m_grid->getGridPointWidth()};
        score->addEffect(std::make_unique<GlowEffect>(-1, m_gui->getTexture(TextureID::BERRY_GLOW), m_gui->getRenderer(), target));
    }

    Gridpoint *gp = calcScorePoint(pos);
    if(!(gp == nullptr)) {
        gp->setScore();
        score->move(gp->getGridPointX(), gp->getGridPointY());
        std::string key = std::to_string(gp->getGridPointX()) + "," + std::to_string(gp->getGridPointY());
        std::cout << "key: " << key << std::endl;
        m_scores[key] = (score);
    }
}

void Game::removeScore(Vector2 pos) {
    Gridpoint *gp = calcScorePoint(pos);
    if(!(gp == nullptr)) {
        gp->removeScore();
        std::string key = std::to_string(gp->getGridPointX()) + "," + std::to_string(gp->getGridPointY());
        std::cout << "key remove: " << key << std::endl;
        m_scores.erase(key);
    }
}

Gridpoint *Game::calcScorePoint(Vector2 pos) {
    pos.x = ((pos.x + 1) * (m_grid->getGridPointWidth())) + 1;
    pos.y = ((pos.y + 1) * (m_grid->getGridPointHeight())) + 1;
    return m_grid->getPoint(pos.x, pos.y);
}

void Game::handleEffects(const std::string &type, int pid) {
    std::cout << m_myPid;
    if (type == "inverse_self" && pid == m_myPid) {
        m_players[m_myPid]->addEffect(std::make_unique<InvertControlsEffect>(*m_players[m_myPid], 5000.0f));
    }
    if (type == "speed_self" && pid == m_myPid) {
        m_players[m_myPid]->addEffect(std::make_unique<SpeedBoostEffect>(*m_players[m_myPid], 5000.0f));
    }
}

void Game::playerGrow() {
    m_players[m_myPid]->grow();
}
