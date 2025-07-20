#include "Snake.hpp"

Snake::Snake(GUI *gui, Vector2 pos, Grid *grid, int snakeWidth, int snakeHeight, int snakeSize, SDL_Color color, int pid, int speed) {

    this->m_renderer = gui->getRenderer();
    // this->m_snakeWidth = snakeWidth;
    // this->m_snakeHeight = snakeHeight;
    this->m_snakeSize = snakeSize;
    this->m_grid = grid;
    this->m_gui = gui;
    m_color = color;
    m_pid = pid;
    m_speed = speed;
    m_speedLimit = 100.0f * m_speed;

    m_speedBoostRect.h = 15;
    m_speedBoostTimeLimit = 1500.0f;
    m_speedBoostTime = m_speedBoostTimeLimit;
    m_speedBoostTimeout = 0.0f;

    m_snakeDirection = DIR_RIGHT;
    m_newSnakeDirection = m_snakeDirection;

    m_snakeWidth = m_grid->getGridPointWidth(); // Retrieve from grid
    m_snakeHeight = m_grid->getGridPointHeight(); // Retrieve from grid 

    m_textureSnakeHead = m_gui->getTexture(SNAKEHEAD);

    // SDL_FreeSurface(snakeHead);
    Gridpoint *gp = m_grid->getPoint(pos.x, pos.y);
    if(gp == nullptr) std::cerr << "Failed finding gridpoint";
    Vector2 gridPos = Vector2(pos.x, pos.y);
    if(gp != nullptr) {
        Vector2 gridPos = gp->getGridPointPos();
    }
    if(gp == nullptr) std::cout << "WHAT";
    for(int i = 0; i < snakeSize; i++) {
        snakeBlocks.push_back(Snakeblock(m_renderer, gridPos.x, gridPos.y, m_snakeWidth-2, m_snakeHeight-2, m_textureSnakeHead, m_degrees, m_color));
    }
}

Snake::~Snake() {
    // SDL_DestroyTexture(m_textureSnakeHead);
}

void Snake::render() {


    // SDL_RenderPresent(m_renderer);

    for (int i = 0; i < snakeBlocks.size(); i++) {
        if(i == 0) {
            snakeBlocks[i].renderHead();
        } else {
            snakeBlocks[i].render();
        }
    }

    if (m_pid == 0) {
        // std::cout << "Drawing bar" << std::endl;
        renderBoostBar();
        renderEffectBars();
    }

}

void Snake::renderBoostBar() {
    int barWidth = m_speedBoostTime / 10;
    int xPos = m_gui->getWindowCenterX() - (barWidth / 2);
    int yPos = m_gui->getWindowHeight() - (m_speedBoostRect.h * 2);

    m_speedBoostRect.x = xPos;
    m_speedBoostRect.y = yPos;
    m_speedBoostRect.w = barWidth;

    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(m_renderer, &m_speedBoostRect);
}

SDL_Rect Snake::createEffectBar(float e, float d, int i) {
    SDL_Rect r;
    int textureYOffset = (i - 1) * 15;
    r.x = m_effectBarXdefault /* + offset for effects.size() */;
    r.y = m_effectBarYdefault /* + offset for effects.size() */ - textureYOffset;
    r.w = m_effectBarWidth - m_effectBarWidth * (e / d);
    r.h = m_effectBarHeight;
    return r;
}

SDL_Rect Snake::createEffectTextureRect(int i) {
    SDL_Rect r;
    int textureXOffset = (m_effectBarHeight + 15);
    int textureYOffset = (i - 1) * 15;
    r.x = m_effectBarXdefault /* + offset for effects.size() */ - textureXOffset;
    r.y = m_effectBarYdefault /* + offset for effects.size() */ - textureYOffset;
    r.w = m_effectBarHeight;
    r.h = m_effectBarHeight;
    return r;
}

void Snake::renderEffectBars() {

    for (int i = 0; i < m_effects.size(); i++) {
        auto &e = m_effects[i];

        // Create new instance of effect bar - TODO: Bad implementation? Will effects even last longer than this?
        SDL_Rect er = createEffectBar(e->getElapsed(), e->getDuration(), i);
        SDL_Rect et = createEffectTextureRect(i);

        SDL_RenderCopy(m_renderer, m_gui->getTexture(e->getType()), nullptr, &et);

        SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(m_renderer, &er);
    }
}

bool operator!=(const direction& lhs, const direction& rhs) {
    return (lhs.x != rhs.x) || (lhs.y != rhs.y);
}

void Snake::onEvent(const SDL_Event& event) {
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if(key_state[SDL_SCANCODE_S] || key_state[SDL_SCANCODE_DOWN]) {
        if((m_snakeDirection != DIR_UP) && (m_snakeDirection != DIR_DOWN)) {
            m_newSnakeDirection = m_invertControls ? DIR_UP : DIR_DOWN;
            // TODO: bad 'hack' for resolving head direction being dependant on inverted controls
            // Right side is 'correct'
            m_newDegrees = m_invertControls ? -90 : 90;
        }
    }

    if(key_state[SDL_SCANCODE_W] || key_state[SDL_SCANCODE_UP]) {
        if((m_snakeDirection != DIR_DOWN) && (m_snakeDirection != DIR_UP)) {
            m_newSnakeDirection = m_invertControls ? DIR_DOWN : DIR_UP;
            m_newDegrees = m_invertControls ? 90 : -90;
        }
    }

    if(key_state[SDL_SCANCODE_D] || key_state[SDL_SCANCODE_RIGHT]) {
        if((m_snakeDirection != DIR_LEFT) && (m_snakeDirection != DIR_RIGHT)) {
            m_newSnakeDirection = m_invertControls ? DIR_LEFT : DIR_RIGHT;
            m_newDegrees = m_invertControls ? 180 : 0;
        }
    }

    if(key_state[SDL_SCANCODE_A] || key_state[SDL_SCANCODE_LEFT]) {
        if((m_snakeDirection != DIR_RIGHT) && (m_snakeDirection != DIR_LEFT)) {
            m_newSnakeDirection = m_invertControls ? DIR_RIGHT : DIR_LEFT;
            m_newDegrees = m_invertControls ? 0 : 180;
        }
    }

    if (key_state[SDL_SCANCODE_SPACE]) {
        m_speedBoost = true;
    }

    if (event.type == SDL_KEYUP && event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
        m_speedBoost = false;
        m_speedBoostTimeout = 1000.0f;
    }
}

// TODO: These could be moved to a separate src-file as well (SnakeEffects.cpp).
void Snake::addEffect(std::unique_ptr<Effect> effect) {
    m_effects.emplace_back(std::move(effect));
}

void Snake::updateEffects(float deltaTime) {
    for (auto effect = m_effects.begin(); effect != m_effects.end(); ) {
        (*effect)->update(deltaTime);

        if (!(*effect)->isActive()) {
            effect = m_effects.erase(effect);
        } else {
            ++effect;
        }
    }
}

void Snake::update(double deltaTime) {
    m_limit += deltaTime;
    double _speedLimit = m_speedLimit;

    updateEffects(deltaTime);

    // TODO: move to own function
    if (m_speedBoostTimeout > 0) m_speedBoostTimeout -= deltaTime;
    if (m_speedBoostTimeout < 0) m_speedBoostTimeout = 0.0f;
    if (m_speedBoostTime < 0) {
        m_speedBoost = false;
        m_speedBoostTimeout = 1000.0f;
        m_speedBoostTime = 0;
    }

    if (m_speedBoost && m_speedBoostTimeout < 0.1f && m_speedBoostTime > 0.1f) {
        // Modifier to base speed. Effects can also modify the value pre-update.
        _speedLimit *= 0.75; 
        m_speedBoostTime -= deltaTime;
    } else if (m_speedBoostTimeout < 0.1f && m_speedBoostTime < m_speedBoostTimeLimit) {
        m_speedBoostTime += deltaTime;
    }

    if(m_limit < _speedLimit) return;
    m_limit = 0;
    m_snakeDirection = m_newSnakeDirection;
    m_degrees = m_newDegrees;
    Vector2 newPos = snakeBlocks[0].getPos() + Vector2(m_snakeDirection.x * m_snakeWidth, m_snakeDirection.y * m_snakeHeight);
    Gridpoint *newPoint = m_grid->getPoint(newPos.x + m_snakeWidth / 2, newPos.y + m_snakeHeight / 2);

    Vector2 oldPos = snakeBlocks.back().getPos();
    Gridpoint *oldPoint = m_grid->getPoint(oldPos.x + m_snakeWidth / 2, oldPos.y + m_snakeHeight / 2);

    if(oldPoint != nullptr) oldPoint->setEmpty();
    if(newPoint != nullptr) {
        // std::string command = "PLAYER_UPDATE_POSITION;0;" + std::to_string(newPoint->getGridPointX() / m_grid->getGridPointWidth()) + ";" + std::to_string(newPoint->getGridPointY() / m_grid->getGridPointHeight()) + ";";
        // TODO: This signaling is no longer used since the server does it all.
        // Should it be removed or adapted for when a server is not used?
        // signalController(command);
        if(!newPoint->isEmpty()) {
            std::cout << "GAME OVER!" << std::endl;
        }

        if(newPoint->hasScore()) {
            // snakeBlocks.push_back(Snakeblock(m_renderer, (snakeBlocks.size()-1)*m_snakeWidth, 1, m_snakeWidth-2, m_snakeHeight-2, m_textureSnakeHead, m_degrees, m_color));
            // newPoint->removeScore();
            std::string command = "PLAYER_SCORE_COLLECTED;" + std::to_string(newPoint->getGridPointX()) + ";" + std::to_string(newPoint->getGridPointY());
            signalController(command);
        }
        newPoint->setNotEmpty();

        snakeBlocks.pop_back();
        Vector2 newPos = newPoint->getGridPointPos() + Vector2(2, 2);
        Snakeblock newSnakeBlock = Snakeblock(m_renderer, newPos.x, newPos.y, m_snakeWidth - 2, m_snakeHeight - 2, m_textureSnakeHead, m_degrees, m_color);
        snakeBlocks.insert(snakeBlocks.begin(), newSnakeBlock);
    } else {
        return;
    }
}

void Snake::grow() {
    snakeBlocks.push_back(Snakeblock(m_renderer, -99, -99, m_snakeWidth-2, m_snakeHeight-2, m_textureSnakeHead, m_degrees, m_color));
}

void Snake::grow(int xPos, int yPos) {
    snakeBlocks.push_back(Snakeblock(m_renderer, xPos, yPos, m_snakeWidth-2, m_snakeHeight-2, m_textureSnakeHead, m_degrees, m_color));
}

// TODO: These could be moved to a separate src-file as well (SnakeEffects.cpp).
void Snake::invertControls() {
    m_invertControls = !m_invertControls;
}

void Snake::applySpeedBoost() {
    m_speedLimit *= 0.5;
}

void Snake::removeSpeedBoost() {
    m_speedLimit *= 2;
}

void Snake::setSpeed(int speed) {
    m_speedLimit = m_speedLimitBase / speed;
}

void Snake::updatePos(int xPos, int yPos) {
    
    int newPosX = xPos;
    int newPosY = yPos;
  
    Gridpoint *newPoint = m_grid->getPoint(newPosX + m_snakeWidth / 2, newPosY + m_snakeHeight / 2);

    int oldPosX = snakeBlocks.back().getPosX();
    int oldPosY = snakeBlocks.back().getPosY();
    Vector2 oldPos = snakeBlocks.back().getPos();
    Gridpoint *oldPoint = m_grid->getPoint(oldPos.x + m_snakeWidth / 2, oldPos.y + m_snakeHeight / 2);

    if(oldPoint != nullptr) oldPoint->setEmpty();
    if(newPoint != nullptr) {
        if(!newPoint->isEmpty()) {
            std::cout << "GAME OVER!" << std::endl;
        }

        // if(newPoint->hasScore()) {
        //     snakeBlocks.push_back(Snakeblock(m_renderer, (snakeBlocks.size()-1)*m_snakeWidth, 1, m_snakeWidth-2, m_snakeHeight-2, m_textureSnakeHead, m_degrees, m_color));
        //     newPoint->removeScore();
        // }
        
        newPoint->setNotEmpty();

        snakeBlocks.pop_back();
        Vector2 newPos = newPoint->getGridPointPos() + Vector2(2, 2);
        Snakeblock newSnakeBlock = Snakeblock(m_renderer, newPos.x, newPos.y, m_snakeWidth - 2, m_snakeHeight - 2, m_textureSnakeHead, m_degrees, m_color);
        snakeBlocks.insert(snakeBlocks.begin(), newSnakeBlock);
    } else {
        return;
    }
}

Snakeblock::Snakeblock(SDL_Renderer *renderer, int snakeBlockXpos, int snakeBlockYpos, int snakeBlockWidth, int snakeBlockHeight, SDL_Texture *textureSnakeHead, int degrees, SDL_Color color) {

    m_snakeBlockPos = Vector2(snakeBlockXpos, snakeBlockYpos);
    this->m_snakeBlockWidth = snakeBlockWidth;
    this->m_snakeBlockheight = snakeBlockHeight;
    this->m_renderer = renderer;
    this->m_textureSnakeHead = textureSnakeHead;
    this->m_degrees = degrees;
    m_color = color;

    m_snakeblock.w = m_snakeBlockWidth;
    m_snakeblock.h = m_snakeBlockheight;
    m_snakeblock.x = m_snakeBlockPos.x;
    m_snakeblock.y = m_snakeBlockPos.y;

    m_snakeblockOverlay.w = m_snakeBlockWidth - 4;
    m_snakeblockOverlay.h = m_snakeBlockWidth - 4;
    m_snakeblockOverlay.x = m_snakeBlockPos.x + 2;
    m_snakeblockOverlay.y = m_snakeBlockPos.y + 2;
    m_textureSnakeHeadDegreeOffset = 180;
}

void Snakeblock::render() {
    // SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);
    SDL_SetRenderDrawColor(m_renderer, m_color.r, m_color.g, m_color.b, 255);
    SDL_RenderDrawRect(m_renderer, &m_snakeblock);
    SDL_RenderFillRect(m_renderer, &m_snakeblock);

    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(m_renderer, &m_snakeblockOverlay);
    SDL_RenderFillRect(m_renderer, &m_snakeblockOverlay);
}

void Snakeblock::renderHead() {
    // this->render();
    SDL_Rect tmp;
    tmp.w = m_snakeBlockWidth;
    tmp.h = m_snakeBlockheight;
    tmp.x = m_snakeBlockPos.x;
    tmp.y = m_snakeBlockPos.y;
    SDL_RenderCopyEx(m_renderer, m_textureSnakeHead, NULL, &tmp, m_textureSnakeHeadDegreeOffset + m_degrees, NULL, SDL_FLIP_NONE);
}

Vector2 Snakeblock::getPos() {
    return m_snakeBlockPos;
}

int Snakeblock::getPosX() {
    return m_snakeBlockXpos;
}

int Snakeblock::getPosY() {
    return m_snakeBlockYpos;
}

Snakeblock::~Snakeblock() {

}
