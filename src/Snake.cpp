#include "headers/Snake.hpp"
#include "headers/SnakeEffects.hpp"

void Snake::createEffectUi() {
    int rows = 2;
    int NUMBER_OF_EFFECTS = m_snakeEffects.size();

    int columns = (NUMBER_OF_EFFECTS + rows - 1) / rows;
    int barHeight = WINDOW_HEIGHT / 15;
    int barWidth = WINDOW_WIDTH / (columns * 3);

    for (int i = 0; i < NUMBER_OF_EFFECTS; i++) {
        int row = i / columns;
        int col = i % columns;

        int padding = 2;
        int barsInRow = std::min(columns, NUMBER_OF_EFFECTS - row * columns);
        int rowWidth = barsInRow * barWidth + (barsInRow - 1) * padding;
        int startX = WINDOW_MIDDLE_X - rowWidth / 2;

        m_effectUIs[m_snakeEffects[i]->getType()] = (std::make_unique<UIElementSnakeEffect>(UIElementSnakeEffect(
            m_gui->getRenderer(), 
            m_gui->getTexture(m_snakeEffects[i]->getType()), 
            m_snakeEffects[i]->getColor(), 
            Vector2(startX + col * (barWidth + padding), WINDOW_HEIGHT - 200 + row * (barHeight + 5)), 
            barWidth,
            barHeight,
            0.35)
        ));

    }
}

Snake::Snake(GUI *gui, Vector2 pos, Grid *grid, int snakeSize, SDL_Color color, int pid, int speed) {

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

    SpriteSheet *atlas = m_gui->getAtlas(TextureID::A_YELLOW_SNAKE);
    m_spriteSnakeHead = std::make_shared<Sprite>(atlas->getTexture(), atlas->getClip(0));
    m_spriteSnakeBody = std::make_shared<Sprite>(atlas->getTexture(), atlas->getClip(1));
    m_spriteSnakeCurve = std::make_shared<Sprite>(atlas->getTexture(), atlas->getClip(2));
    m_spriteSnakeTail = std::make_shared<Sprite>(atlas->getTexture(), atlas->getClip(3));
    

    m_snakeDirection = DIR_RIGHT;
    m_newSnakeDirection = m_snakeDirection;
    m_newDegrees = 0;

    m_snakeWidth = m_grid->getGridPointWidth(); // Retrieve from grid
    m_snakeHeight = m_grid->getGridPointHeight(); // Retrieve from grid 

    // SDL_FreeSurface(snakeHead);
    Gridpoint *gp = m_grid->getPoint(pos.x, pos.y);
    if(gp == nullptr) std::cerr << "Failed finding gridpoint";
    Vector2 gridPos = Vector2(pos.x, pos.y);
    if(gp != nullptr) {
        std::cout << "Found gridpos";
        gridPos = gp->getGridPointPos();
    }
    std::cout << gridPos.x;
    if(gp == nullptr) std::cout << "WHAT";
    for (int i = 0; i < snakeSize; i++) {
        snakeBlocks.push_back(Snakeblock(m_gui, gridPos.x, gridPos.y, m_snakeWidth, m_snakeHeight, m_spriteSnakeBody, m_degrees, m_color, m_snakeDirection));
    }

    snakeBlocks.back().setSprite(m_spriteSnakeTail);
    snakeBlocks.begin()->setSprite(m_spriteSnakeHead);

    m_snakeEffects.push_back(std::make_unique<InvertControlsEffect>(*this, 0));
    m_snakeEffects.push_back(std::make_unique<SpeedBoostEffect>(*this, 0));
    m_snakeEffects.push_back(std::make_unique<SlowBoostEffect>(*this, 0));
    m_snakeEffects.push_back(std::make_unique<GhostEffect>(*this, 0));
    m_snakeEffects.push_back(std::make_unique<FreezeEffect>(*this, 0));

    createEffectUi();

}

Snake::~Snake() {
    // SDL_DestroyTexture(m_textureSnakeHead);
}

void Snake::render() {

    for (const auto& pair : m_effectUIs) {
         pair.second->render();
    }

    for (int i = 0; i < snakeBlocks.size(); i++) {
        if (m_isGhost > 0) {
            snakeBlocks[i].renderWithAlpha(64);
        } else {
            snakeBlocks[i].render();
        }
    }

    if (m_pid == 0) {
        renderBoostBar();
        // renderEffectBars();
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

        m_effectUIs[(*effect)->getType()]->update(
            (*effect)->getElapsed(), 
            (*effect)->getDuration()
        );

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
            // TODO: Handle freeze
            std::cout << "GAME OVER!" << std::endl;
        }

        if(newPoint->hasScore()) {
            // snakeBlocks.push_back(Snakeblock(m_renderer, (snakeBlocks.size()-1)*m_snakeWidth, 1, m_snakeWidth, m_snakeHeight, m_textureSnakeHead, m_degrees, m_color));
            // newPoint->removeScore();
            std::string command = "PLAYER_SCORE_COLLECTED;" + std::to_string(newPoint->getGridPointX()) + ";" + std::to_string(newPoint->getGridPointY());
            signalController(command);
        }
        newPoint->setNotEmpty();
        if (!m_freeze) {
            updateSnakePos(newPoint);
        }
    } else {
        return;
    }
}

// TODO: Update how directions are calculated and make a better solution with vectors?
int Snake::calculateBodyOffset(direction dir1, direction dir2) {
    // Return offset for body texture
    if ((dir1 == DIR_UP) && (dir2 == DIR_LEFT)) return 180;
    if ((dir1 == DIR_DOWN) && (dir2 == DIR_LEFT)) return -90;
    if ((dir1 == DIR_UP) && (dir2 == DIR_RIGHT)) return 90;
    if ((dir1 == DIR_DOWN) && (dir2 == DIR_RIGHT)) return 0;
    if ((dir1 == DIR_LEFT) && (dir2 == DIR_DOWN)) return 90;
    if ((dir1 == DIR_RIGHT) && (dir2 == DIR_DOWN)) return 180;
    if ((dir1 == DIR_LEFT) && (dir2 == DIR_UP)) return 0;
    if ((dir1 == DIR_RIGHT) && (dir2 == DIR_UP)) return 270;

    return 0;
}

void Snake::updateSnakePos(Gridpoint *gp) {
    snakeBlocks.pop_back();
    Vector2 newPos = gp->getGridPointPos(); //+ Vector2(2, 2);
    Snakeblock newSnakeBlock = Snakeblock(m_gui, newPos.x, newPos.y, m_snakeWidth, m_snakeHeight, m_spriteSnakeHead, m_degrees, m_color, m_snakeDirection);
    snakeBlocks.insert(snakeBlocks.begin(), newSnakeBlock);

    snakeBlocks.back().setSprite(m_spriteSnakeTail);
    auto head = &snakeBlocks[0];
    auto neck = &snakeBlocks[1];
    auto tail = &snakeBlocks[snakeBlocks.size() - 1];
    auto tailneck = &snakeBlocks[snakeBlocks.size() - 2];
    neck->setSprite(m_spriteSnakeBody);
    tail->setDegrees(tailneck->getDegrees());
    tail->rotateTexture(180);

    // Logic for calculating neck.
    if (head->getDirection() != neck->getDirection()) {
        // Direction has changed, set texture to curve and calculate offset
        neck->setSprite(m_spriteSnakeCurve);
        neck->rotateTexture(calculateBodyOffset(head->getDirection(), neck->getDirection()) - neck->getDegrees());
    }

}

void Snake::grow() {
    snakeBlocks.push_back(Snakeblock(m_gui, -99, -99, m_snakeWidth, m_snakeHeight, m_spriteSnakeHead, m_degrees, m_color, m_snakeDirection));
}

void Snake::grow(int xPos, int yPos) {
    snakeBlocks.push_back(Snakeblock(m_gui, xPos, yPos, m_snakeWidth, m_snakeHeight, m_spriteSnakeHead, m_degrees, m_color, m_snakeDirection));
}

// TODO: These could be moved to a separate src-file as well (SnakeEffects.cpp).
void Snake::invertControls() {
    m_invertControls = !m_invertControls;
}

void Snake::becomeGhost() {
    m_isGhost++;
}

void Snake::removeGhost() {
    if (m_isGhost > 0) m_isGhost--;
}

void Snake::freeze() {
    m_freeze++;
}

void Snake::unfreeze() {
    if (m_freeze > 0) m_freeze--;
}

void Snake::applySpeedBoost() {
    m_speedLimit *= 0.5;
}

void Snake::removeSpeedBoost() {
    m_speedLimit *= 2;
}

void Snake::applySlowBoost() {
    m_speedLimit *= 2;
}

void Snake::removeSlowBoost() {
    m_speedLimit *= 0.5;
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
        //     snakeBlocks.push_back(Snakeblock(m_renderer, (snakeBlocks.size()-1)*m_snakeWidth, 1, m_snakeWidth, m_snakeHeight, m_textureSnakeHead, m_degrees, m_color));
        //     newPoint->removeScore();
        // }
        
        newPoint->setNotEmpty();

        // TODO: Replace with: updateSnakePos(newPoint);
        snakeBlocks.pop_back();
        Vector2 newPos = newPoint->getGridPointPos() + Vector2(2, 2);
        Snakeblock newSnakeBlock = Snakeblock(m_gui, newPos.x, newPos.y, m_snakeWidth, m_snakeHeight, m_spriteSnakeHead, m_degrees, m_color, m_snakeDirection);
        snakeBlocks.insert(snakeBlocks.begin(), newSnakeBlock);
    } else {
        return;
    }
}

Snakeblock::Snakeblock(GUI *gui, int snakeBlockXpos, int snakeBlockYpos, int snakeBlockWidth, int snakeBlockHeight, SDL_Texture *texture, int degrees, SDL_Color color, direction dir) {

    m_snakeBlockPos = Vector2(snakeBlockXpos, snakeBlockYpos);
    this->m_snakeBlockWidth = snakeBlockWidth;
    this->m_snakeBlockheight = snakeBlockHeight;
    // this->m_renderer = renderer;
    m_gui = gui;
    this->m_texture = texture;
    this->m_degrees = degrees;
    m_snakeBlockDirection = dir;
    m_color = color;

    m_snakeblock.w = m_snakeBlockWidth;
    m_snakeblock.h = m_snakeBlockheight;
    m_snakeblock.x = m_snakeBlockPos.x;
    m_snakeblock.y = m_snakeBlockPos.y;

    m_snakeblockOverlay.w = m_snakeBlockWidth /* -4 */;
    m_snakeblockOverlay.h = m_snakeBlockWidth /* -4 */;
    m_snakeblockOverlay.x = m_snakeBlockPos.x;
    m_snakeblockOverlay.y = m_snakeBlockPos.y;
    m_textureDegreeOffset = 180;
}

Snakeblock::Snakeblock(GUI *gui, int snakeBlockXpos, int snakeBlockYpos, int snakeBlockWidth, int snakeBlockHeight, std::shared_ptr<Sprite> sprite, int degrees, SDL_Color color, direction dir) 
    : m_sprite(sprite) {

    m_snakeBlockPos = Vector2(snakeBlockXpos, snakeBlockYpos);
    this->m_snakeBlockWidth = snakeBlockWidth;
    this->m_snakeBlockheight = snakeBlockHeight;
    // this->m_renderer = renderer;
    m_gui = gui;
    this->m_degrees = degrees;
    m_snakeBlockDirection = dir;
    m_color = color;

    m_snakeblock.w = m_snakeBlockWidth;
    m_snakeblock.h = m_snakeBlockheight;
    m_snakeblock.x = m_snakeBlockPos.x;
    m_snakeblock.y = m_snakeBlockPos.y;

    m_snakeblockOverlay.w = m_snakeBlockWidth /* -4 */;
    m_snakeblockOverlay.h = m_snakeBlockWidth /* -4 */;
    m_snakeblockOverlay.x = m_snakeBlockPos.x;
    m_snakeblockOverlay.y = m_snakeBlockPos.y;
    m_textureDegreeOffset = 180;
}

void Snakeblock::render() {
    // TODO: Optimize this.
    SDL_Rect tmp;
    tmp.w = m_snakeBlockWidth;
    tmp.h = m_snakeBlockheight;
    tmp.x = m_snakeBlockPos.x;
    tmp.y = m_snakeBlockPos.y;
    SDL_RenderCopyEx(m_gui->getRenderer(), m_sprite->getTexture(), &m_sprite->getRect(), &tmp, m_textureDegreeOffset + m_degrees, NULL, SDL_FLIP_NONE);
    // SDL_RenderCopyEx(m_gui->getRenderer(), m_texture, NULL, &tmp, m_textureDegreeOffset + m_degrees, NULL, SDL_FLIP_NONE)
}

void Snakeblock::renderWithAlpha(int alpha) {
    SDL_SetTextureAlphaMod(m_sprite->getTexture(), alpha);
    render();
    SDL_SetTextureAlphaMod(m_sprite->getTexture(), 255);
}

void Snakeblock::renderHead() {
    // this->render();
    SDL_Rect tmp;
    tmp.w = m_snakeBlockWidth;
    tmp.h = m_snakeBlockheight;
    tmp.x = m_snakeBlockPos.x;
    tmp.y = m_snakeBlockPos.y;
    SDL_SetTextureAlphaMod(m_texture, 64);
    SDL_RenderCopyEx(m_gui->getRenderer(), m_texture, NULL, &tmp, m_textureDegreeOffset + m_degrees, NULL, SDL_FLIP_NONE);
    SDL_SetTextureAlphaMod(m_texture, 255);
}

void Snakeblock::setTexture(SDL_Texture *texture) {
    m_texture = texture;
}

void Snakeblock::setSprite(std::shared_ptr<Sprite> sprite) {
    m_sprite = sprite;
}

void Snakeblock::rotateTexture(int degrees) {
    m_textureDegreeOffset = degrees;
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

int Snakeblock::getDegrees() {
    return m_degrees;
}

void Snakeblock::setDegrees(int degrees) {
    m_degrees = degrees;
}

direction Snakeblock::getDirection() {
    return m_snakeBlockDirection;
}

Snakeblock::~Snakeblock() {

}

UIElementSnakeEffect::UIElementSnakeEffect(SDL_Renderer *renderer, SDL_Texture *texture, SDL_Color color, Vector2 pos, int width, int height, float scale) {
    m_renderer = renderer;
    m_texture = texture;
    m_width = width;
    m_height = height;
    m_pos = pos;
    m_color = color;

    float m_scale = scale;

    int barWidth = 0;
    int m_barWidth = 0;
    int m_barWidthMax = width * (0.95 - scale); // 95% for padding


    int textureWidth = static_cast<int>(width * scale);
    m_textureRect = SDL_Rect {pos.x, pos.y, textureWidth, height};
    
    m_barPos = pos + Vector2(textureWidth * 1.1, 0); // 10% padding from width
    m_barBackgroundLayer = SDL_Rect {m_barPos.x, m_barPos.y + height / 2, m_barWidthMax, static_cast<int>(height * scale)};
    m_effectDurationBar = SDL_Rect {m_barPos.x, m_barPos.y + height / 2, 0, static_cast<int>(height * scale)};
}

void UIElementSnakeEffect::update(float e, float d) {
    float _width = 100.0f * (1.0f - e / d);
    m_barWidth = _width;
    m_effectDurationBar.w = m_barWidth;
}

void UIElementSnakeEffect::render() {
    SDL_RenderCopy(m_renderer, m_texture, nullptr, &m_textureRect);

    SDL_SetRenderDrawColor(m_renderer, color::DARKGRAY.r, color::DARKGRAY.g, color::DARKGRAY.b, color::DARKGRAY.a);
    SDL_RenderFillRect(m_renderer, &m_barBackgroundLayer);

    SDL_SetRenderDrawColor(m_renderer, m_color.r, m_color.g, m_color.b, m_color.a);
    SDL_RenderFillRect(m_renderer, &m_effectDurationBar);
}
