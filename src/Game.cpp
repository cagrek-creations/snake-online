#include "Game.hpp"

Game::Game() {
    setupSound();

    setupGui();    

    setupController();

    setupGame();
}

void Game::update(double deltaTime) {
    m_deltaTime = deltaTime;
    m_gameController->update();

    // Change name to controller events?

    // TODO: add state as an input or how to handle so that observers should act according to state?
    // Some helper function to set a state?
    // setState(GAME_PLAY) {
    //     controller->addObserver(Snake)
    //     controller->addObserver(...)
    // }
    handleEvents(m_gameController->getServerEvents());
    handleEvents(m_gameController->getLocalEvents());

    if (m_state == GAME_PLAY) {
        auto player = m_players[m_myPid];
        player->update(m_deltaTime, 100.f);
        int xPos = player->getPosX();
        int yPos = player->getPosY();
        Gridpoint *gp = m_grid->getPoint(xPos, yPos);

        std::string command = "PLAYER_UPDATE_POSITION;" + std::to_string(m_myPid) + ";" + std::to_string(gp->getGridPointX() / m_grid->getGridPointWidth()) + ";" + std::to_string(gp->getGridPointY() / m_grid->getGridPointHeight());

        if (command != m_lastPosition) {
            std::cout << command << std::endl;
            m_lastPosition = command;
            m_gameController->sendMessage(command);
        }

    }
}

void Game::render() {
    m_gui->clearRenderer();
    m_gui->update();

    renderState();

    m_gui->render();
}

void Game::onEvent(const SDL_Event& event) {
    if (m_myPid != -1) m_players[m_myPid]->onEvent(event);
}

void Game::createGrid() {
    m_grid = std::make_unique<Grid>(m_gui->getRenderer(), WINDOW_WIDTH, WINDOW_HEIGHT, 40, 30);
}

void Game::createGrid(int width, int height) {
    m_grid = std::make_unique<Grid>(m_gui->getRenderer(), WINDOW_WIDTH, WINDOW_HEIGHT, width, height);
}

void Game::createPlayer() {
    std::shared_ptr<Snake> snake = std::make_shared<Snake>(m_gui.get(), WINDOW_MIDDLE_X, WINDOW_MIDDLE_Y, m_grid.get(), 40, 40, 3, color::GREEN);
    m_gameController->attachObserver(snake.get());
    m_players[m_myPid] = std::move(snake);
}

void Game::createPlayer(int size, int xPos, int yPos) {
    int xPosGrid = ((xPos) * (m_grid->getGridPointWidth()));
    int yPosGrid = ((yPos) * (m_grid->getGridPointHeight()));
    std::shared_ptr<Snake> snake = std::make_shared<Snake>(m_gui.get(), xPosGrid, yPosGrid, m_grid.get(), 40, 40, size, m_gui->getColor(m_playerColor));
    m_gameController->attachObserver(snake.get());
    m_players[m_myPid] = std::move(snake);
}

void Game::renderState() {
    if (m_state == START_MENU) {
        m_startMenu->render();
    } else if (m_state == OPTIONS) {
        m_optionsMenu->render();
    } else if (m_state == GAME_PLAY) {
        for (auto &p : m_players) {
            p.second->render();
        }

        for (auto &s : m_scores) {
            s.second->render();
        }
    } else if (m_state == GAME_QUIT) {
        m_isRunning = false;
    }
}

void Game::setupGame() {

    getIpAdressAndPort(m_serverIp, m_serverPort);
    getName(m_playerName);
    getColor(m_playerColor);

    std::string firstCommand = "ADD_NEW_PLAYER;" + m_playerName + ";" + m_playerColor;

    m_gameController->connect(m_serverIp, m_serverPort);
    m_gameController->sendMessage(firstCommand);

    // Wait for server response
    // TODO: fix timeout
    while (!m_serverSetupIsComplete) {
        std::vector<std::string> events = m_gameController->getServerEvents();

        handleEvents(events);
    }

    // createGrid();
    // createPlayer();
}

void Game::setupGui() {

    m_state = START_MENU;

    m_gui = std::make_unique<GUI>("Snake", WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_FULLSCREEN);
    m_gui->loadTexture("berry", "./textures/berry.png");

    m_startMenu =       std::make_unique<Menu>(m_gui->getRenderer(), 0, WINDOW_MIDDLE_X - (250 / 2), 
                                                    WINDOW_MIDDLE_Y - (200 / 2), 
                                                    250, 
                                                    200, 
                                                    m_gui->getFont(), m_state, START_MENU, START_MENU);
    
    m_optionsMenu =     std::make_unique<Menu>(m_gui->getRenderer(), 1, WINDOW_MIDDLE_X - (250 / 2), 
                                                    WINDOW_MIDDLE_Y - (200 / 2), 
                                                    250, 
                                                    200, 
                                                    m_gui->getFont(), m_state, START_MENU, OPTIONS);
    int option = 0;
    m_startMenu->addItemState("START GAME", GAME_PLAY);
    m_startMenu->addItemState("OPTIONS",    OPTIONS  );
    m_startMenu->addItemState("QUIT",       GAME_QUIT);

    std::function<void()> funcL = bindMemberFunction(m_sound, &SoundManager::decreaseVolume);
    std::function<void()> funcR = bindMemberFunction(m_sound, &SoundManager::increaseVolume);
    m_optionsMenu->addItemBar("sound", funcL, funcR);
    m_optionsMenu->addItemState("asd", 1);
}

void Game::setupSound() {
    m_volume = 64;
    m_playSound = 1;
    m_sound = std::make_unique<SoundManager>(m_volume, m_playSound);
    m_sound->loadSound("./debug/sfx/MegaSnake.mp3", "MegaSnake");
    m_sound->setVolume("MegaSnake", m_volume); // 50%
    m_sound->playSound("MegaSnake", -1);
}

void Game::setupController() {
    m_gameController = std::make_unique<Controller>();
    m_gameController->attachObserver(m_gui.get());
    m_gameController->attachObserver(m_sound.get());
    m_gameController->attachObserver(m_startMenu.get());
    m_gameController->attachObserver(m_optionsMenu.get());
}

bool Game::isRunning() {
    return m_isRunning && m_gui->getWindowClose();
}