#include "Game.hpp"
#include "GuiElements.hpp"
#include "Snake.hpp"
#include "SnakeEffects.hpp"
#include "Vector2.hpp"
#include "headers/Gui.hpp"
#include "headers/Menu.hpp"
#include <memory>

int WINDOW_FULLSCREEN = 0;

int WINDOW_WIDTH = 1600;
int WINDOW_HEIGHT = 1024;

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

    renderState();
    
    if (m_fpsCounter > 500.f) {
        m_fpsCounter = 0;
        m_fpsString = "fps: " + std::to_string((int)m_fps);
    }
    
    m_gui->renderText(WINDOW_WIDTH - 200, 100, m_fpsString);

    // TODO: Update this to only render over the grid instead of the screen.
    SDL_Rect dstRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderCopy(m_gui->getRenderer(), m_gui->getTexture(TextureID::VINJETTE), NULL, &dstRect);

    m_gui->render();
}

void Game::onEventState(const SDL_Event &event) {
    if (m_state == START_MENU) {
        m_startMenu->onEvent(event);
    } else if (m_state == OPTIONS) {
        m_optionsMenu->onEvent(event);
    } else if (m_state == GAME_PLAY) {

    } else if (m_state == CREDITS) {
        m_creditsMenu->onEvent(event);
    }
}

void Game::onEvent(const SDL_Event& event) {
    if (m_myPid != -1) m_players[m_myPid]->onEvent(event);

    onEventState(event);
}

void Game::createGrid() {
    m_grid = std::make_unique<Grid>(m_gui.get(), WINDOW_WIDTH, WINDOW_HEIGHT, 64, 64, 80, 66, Vector2(0, 0));
}

void Game::createGrid(int width, int height) {
    m_grid = std::make_unique<Grid>(m_gui.get(), WINDOW_WIDTH, WINDOW_HEIGHT, 32, 32, width, height, Vector2(0,0));
}

void Game::createPlayer() {
    Vector2 initialPos = Vector2(400, 20);
    std::shared_ptr<Snake> snake = std::make_shared<Snake>(m_gui.get(), initialPos, m_grid.get(), 6, color::GREEN, m_players.size(), 1);
    m_gameController->attachObserver(snake.get());
    m_players[m_myPid] = std::move(snake);
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
        // t->render();
        m_startMenu->render();
        m_gui->renderTexture(TextureID::MAINMENU, Vector2(0,0), Vector2(WINDOW_WIDTH, WINDOW_HEIGHT));
    } else if (m_state == OPTIONS) {
        // m_optionsMenu->render();
        m_optionsMenu->render();
    } else if (m_state == GAME_PLAY) {
        m_grid->render();

        for (auto &p : m_players) {
            p.second->render();
        }

        for (auto &s : m_scores) {
            s.second->render();
        }
    } else if (m_state == GAME_QUIT) {
        m_isRunning = false;
    } else if (m_state == CREDITS) {
        m_creditsMenu->render();
        // TODO: Now these texts are created and destroyed in each loop. Load them once and display them instead?
        m_gui->createText(Vector2(WINDOW_MIDDLE_X, 200), "Game programming & logic", "pixeloidm_32", color::GREEN_7EAD63, TEXT_CENTRALIZED)->render();
        m_gui->createText(Vector2(WINDOW_MIDDLE_X, 250), "Darclander & Kumole", "pixeloidm_32", color::GREEN_7EAD63, TEXT_CENTRALIZED)->render();
        m_gui->createText(Vector2(WINDOW_MIDDLE_X, 400), "Music & Graphical Design", "pixeloidm_32", color::GREEN_7EAD63, TEXT_CENTRALIZED)->render();
        m_gui->createText(Vector2(WINDOW_MIDDLE_X, 450), "Gammelen", "pixeloidm_32", color::GREEN_7EAD63, TEXT_CENTRALIZED)->render();
    }
}

void Game::changeState(gameState gis) {
    std::cout << m_state << "->" << gis << std::endl;
    m_state = gis;
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
    m_gui = std::make_unique<GUI>("Snake", WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_FULLSCREEN);

    m_state = START_MENU;

    loadTextures();
    loadFonts();

    setupStartMenu();
    setupOptionsMenu();
    setupCreditsMenu();
}

void Game::setupSound() {
    m_volume = 64;
    m_playSound = 1;
    m_sound = std::make_unique<SoundManager>(m_volume, m_playSound);

    loadSounds();
}

void Game::setupController() {
    m_gameController = std::make_unique<Controller>();
    m_gameController->attachObserver(m_gui.get());
    m_gameController->attachObserver(m_sound.get());
    m_gameController->attachObserver(this);
}

bool Game::isRunning() {
    return m_isRunning && m_gui->getWindowClose();
}

void Game::setupStartMenu() {
    m_startMenu = std::make_unique<GMenu>(Vector2(WINDOW_MIDDLE_X, WINDOW_MIDDLE_Y), m_sound.get());

    auto s = std::make_shared<GMenuItemButton>(m_gui.get(), Vector2(m_startMenu->getX() - 275, m_startMenu->getY() - 25), "START GAME", "pixeloidm_64", color::GREEN_7EAD63, color::WHITE_CCCCCC);
    s->bind([this]() {
        this->changeState(gameState::GAME_PLAY);
        this->m_sound->playSound("MenuSelectEnter", 0);
    });

    auto o = std::make_shared<GMenuItemButton>(m_gui.get(), Vector2(m_startMenu->getX() + 275, m_startMenu->getY() - 25), "OPTIONS", "pixeloidm_32", color::GREEN_7EAD63, color::WHITE_CCCCCC);
    o->bind([this]() {
        this->changeState(gameState::OPTIONS);
    });

    auto q = std::make_shared<GMenuItemButton>(m_gui.get(), Vector2(m_startMenu->getX() + 285, m_startMenu->getY() + 10), "QUIT", "pixeloidm_32", color::GREEN_7EAD63, color::WHITE_CCCCCC);
    q->bind([this]() {
        m_isRunning = false;
    });

    auto c = std::make_shared<GMenuItemButton>(m_gui.get(), Vector2(100, WINDOW_HEIGHT - 100), "CREDITS", "pixeloidm_32", color::GREEN_7EAD63, color::WHITE_CCCCCC);
    c->bind([this]() {
        this->changeState(gameState::CREDITS);
    });

    s->right = o;
    s->left = c;
    s->down = c;
    o->down = q;
    q->up = o;
    o->left = s;
    q->left = s;
    c->right = s;
    c->up = s;

    m_startMenu->addItem(s);
    m_startMenu->addItem(o);
    m_startMenu->addItem(q);
    m_startMenu->addItem(c);

    m_startMenu->setCurrent(s);
}

void Game::setupOptionsMenu() {
    m_optionsMenu = std::make_unique<GMenu>(Vector2(WINDOW_MIDDLE_X, 200), m_sound.get());

    auto s = std::make_shared<GMenuItemBar>(m_gui.get(), Vector2(m_optionsMenu->getX(), 400), Vector2(128, 10), 8, 1, "volume", "pixeloidm_32", color::GREEN_7EAD63, color::WHITE_CCCCCC);
    s->bind([this]() {
        m_sound->decreaseVolume();
    }, [this]() {
        m_sound->increaseVolume();
    });

    auto b = std::make_shared<GMenuItemButton>(m_gui.get(), Vector2(m_optionsMenu->getX(), 500), "BACK", "pixeloidm_32", color::GREEN_7EAD63, color::WHITE_CCCCCC);
    b->bind([this]() {
        // TODO: Should back be previous state?
        this->changeState(gameState::START_MENU);
    });

    m_optionsMenu->addItem(s);
    m_optionsMenu->addItem(b);

    b->up = s;
    s->down = b;

    m_optionsMenu->setCurrent(s);
}

void Game::setupCreditsMenu() {
    m_creditsMenu = std::make_unique<GMenu>(Vector2(WINDOW_MIDDLE_X, 200), m_sound.get());

    auto b = std::make_shared<GMenuItemButton>(m_gui.get(), Vector2(m_creditsMenu->getX(), 700), "BACK", "pixeloidm_32", color::GREEN_7EAD63, color::WHITE_CCCCCC);
    b->bind([this]() {
        this->changeState(gameState::START_MENU);
    });

    m_creditsMenu->addItem(b);

    m_creditsMenu->setCurrent(b);
}

void Game::loadFonts() {
    m_gui->loadFont("default_32", "font.ttf", 32);
    m_gui->loadFont("default_64", "font.ttf", 64);

    m_gui->loadFont("pixeloidm_32", "PixeloidMono.ttf", 32);
    m_gui->loadFont("pixeloidm_64", "PixeloidMono.ttf", 64);
}

void Game::loadSounds() {
    m_sound->loadSound("MegaSnake.mp3", "MegaSnake");
    m_sound->setVolume("MegaSnake", m_volume); // 50%
    m_sound->playSound("MegaSnake", -1);
    m_sound->stopSound("MegaSnake");

    m_sound->loadSound("MenuSelectEnter.wav", "MenuSelectEnter");
    m_sound->setVolume("MenuSelectEnter", m_volume); // 50%

    m_sound->loadSound("MenuTriggerArrowKeys.wav", "MenuTriggerArrowKeys");
    m_sound->setVolume("MenuTriggerArrowKeys", m_volume); // 50%
}

void Game::loadTextures() {
    m_gui->loadTexture(TextureID::ERR, "err.png");
    m_gui->loadTexture(TextureID::BERRY, "scores/SnakePowerBerry.png");
    m_gui->loadTexture(TextureID::SPEED, "scores/SnakePowerSpeed.png");
    m_gui->loadTexture(TextureID::SPEED_O, "scores/SnakePowerSpeed.png");
    m_gui->loadTexture(TextureID::SWAPAROO, "scores/SnakePowerInvert.png");
    m_gui->loadTexture(TextureID::SWAPAROO_O, "scores/SnakePowerInvertO.png");
    m_gui->loadTexture(TextureID::GRIDTILE, "gridtile.png");
    m_gui->loadTexture(TextureID::GHOST, "scores/SnakePowerGhost.png");
    m_gui->loadTexture(TextureID::SLOW, "scores/SnakePowerSlow.png");
    m_gui->loadTexture(TextureID::FREEZE, "scores/SnakePowerFreeze.png");
    m_gui->loadTexture(TextureID::RAGE, "scores/SnakePowerRage.png");
    m_gui->loadTexture(TextureID::MAINMENU, "MainMenu.png");
    m_gui->loadTextureAlpha(TextureID::BERRY_GLOW, "shiny.png", 255, true);
    m_gui->loadTextureAlpha(TextureID::VINJETTE, "vinjette.png", 64, true);
    m_gui->loadAtlas(TextureID::A_YELLOW_SNAKE, "snakes/y_s.png", 16, 16, 4);
    m_gui->loadAtlas(TextureID::A_PURPLE_SNAKE, "snakes/p_s.png", 16, 16, 4);
    m_gui->loadAtlas(TextureID::A_GREEN_SNAKE, "snakes/g_s.png", 16, 16, 4);
    m_gui->loadAtlas(TextureID::A_RED_SNAKE, "snakes/r_s.png", 16, 16, 4);
}
