#include "Game.hpp"
#include "Snake.hpp"
#include "SnakeEffects.hpp"
#include "Vector2.hpp"
#include "headers/Gui.hpp"

int WINDOW_FULLSCREEN = 0;

int WINDOW_WIDTH = 1024;
int WINDOW_HEIGHT = 800;

int WINDOW_MIDDLE_X (WINDOW_WIDTH / 2);
int WINDOW_MIDDLE_Y (WINDOW_HEIGHT / 2);

Game::Game() {
    setupSound();

    setupGui();

    setupController();

    setupGame();
}

void Game::update(double deltaTime) {
    m_deltaTime = deltaTime;
    m_fps = 1.0f / (m_deltaTime / 1000.f);
    m_fpsCounter += deltaTime;

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

        for (auto &s : m_scores) {
            s.second->update(deltaTime);
        }

        auto player = m_players[m_myPid];
        player->update(m_deltaTime);
        Vector2 pos = player->getPos();
        Gridpoint *gp = m_grid->getPoint(pos.x, pos.y);

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
    
    if (m_fpsCounter > 500.f) {
        m_fpsCounter = 0;
        m_fpsString = "fps: " + std::to_string((int)m_fps);
    }
    
    m_gui->renderText(WINDOW_WIDTH - 100, 100, m_fpsString);

    m_grid->render();

    // TODO: Update this to only render over the grid instead of the screen.
    SDL_Rect dstRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderCopy(m_gui->getRenderer(), m_gui->getTexture(TextureID::VINJETTE), NULL, &dstRect);

    renderState();

    m_gui->render();
}

void Game::onEvent(const SDL_Event& event) {
    if (m_myPid != -1) m_players[m_myPid]->onEvent(event);
}

void Game::createGrid() {
    m_grid = std::make_unique<Grid>(m_gui.get(), WINDOW_WIDTH, WINDOW_HEIGHT, 32, 32, 25, 15, Vector2(100, 100));
}

void Game::createGrid(int width, int height) {
    m_grid = std::make_unique<Grid>(m_gui.get(), WINDOW_WIDTH, WINDOW_HEIGHT, 32, 32, width, height, Vector2(0,0));
}

void Game::createPlayer() {
    Vector2 initialPos = Vector2(WINDOW_MIDDLE_X, WINDOW_MIDDLE_Y);
    std::shared_ptr<Snake> snake = std::make_shared<Snake>(m_gui.get(), initialPos, m_grid.get(), 6, color::GREEN, m_players.size(), 1);
    m_gameController->attachObserver(snake.get());
    m_players[m_myPid] = std::move(snake);
    // m_players[m_myPid]->addEffect(std::make_unique<FreezeEffect>(*m_players[m_myPid], 50000.0f));
    // m_players[m_myPid]->addEffect(std::make_unique<InvertControlsEffect>(*m_players[m_myPid], 15000.0f));
}

void Game::createPlayer(int size, int xPos, int yPos) {
    int xPosGrid = ((xPos) * (m_grid->getGridPointWidth()));
    int yPosGrid = ((yPos) * (m_grid->getGridPointHeight()));
    Vector2 initialPos = Vector2(xPosGrid, yPosGrid);
    std::shared_ptr<Snake> snake = std::make_shared<Snake>(m_gui.get(), initialPos, m_grid.get(), size, m_gui->getColor(m_playerColor), m_players.size(), 1);
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
    int timeout = 0;
    while (!m_serverSetupIsComplete) {
        std::vector<std::string> events = m_gameController->getServerEvents();

        handleEvents(events);
        ++timeout;
        if (timeout > 1000000) {
            std::cout << "Reached timeout connecting to server, continuing..." << std::endl;
            createGrid();
            createPlayer();
            break;
        }
    }
}

void Game::setupGui() {

    m_state = START_MENU;

    std::filesystem::path basePathGfx = getExecutableDir() / "gfx";

    m_gui = std::make_unique<GUI>("Snake", WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_FULLSCREEN);
    m_gui->loadTexture(TextureID::ERR, "err.png");
    m_gui->loadTexture(TextureID::BERRY, "berry.png");
    m_gui->loadTexture(TextureID::SPEED, "speed.png");
    m_gui->loadTexture(TextureID::SPEED_O, "speed.png");
    m_gui->loadTexture(TextureID::SWAPAROO, "swaparoo.png");
    m_gui->loadTexture(TextureID::SWAPAROO_O, "swaparoo.png");
    m_gui->loadTexture(TextureID::GRIDTILE, "gridtile.png");
    m_gui->loadTexture(TextureID::GHOST, "scores/SnakePowerGhost.png");
    m_gui->loadTexture(TextureID::SLOW, "scores/SnakePowerSlow.png");
    m_gui->loadTexture(TextureID::FREEZE, "scores/SnakePowerFreeze.png");
    m_gui->loadTexture(TextureID::RAGE, "scores/SnakePowerRage.png");
    m_gui->loadTextureAlpha(TextureID::BERRY_GLOW, "shiny.png", 255, true);
    m_gui->loadTextureAlpha(TextureID::VINJETTE, "vinjette.png", 64, true);
    m_gui->loadAtlas(TextureID::A_YELLOW_SNAKE, "snakes/y_s.png", 16, 16, 4);
    m_gui->loadAtlas(TextureID::A_PURPLE_SNAKE, "snakes/p_s.png", 16, 16, 4);
    m_gui->loadAtlas(TextureID::A_GREEN_SNAKE, "snakes/g_s.png", 16, 16, 4);
    m_gui->loadAtlas(TextureID::A_RED_SNAKE, "snakes/r_s.png", 16, 16, 4);

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
    
    m_sound->loadSound("MegaSnake.mp3", "MegaSnake");
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
