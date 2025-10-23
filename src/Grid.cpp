#include "Grid.hpp"

Grid::Grid(GUI *gui, int width, int height, int granularityX, int granularityY, int rows, int columns, Vector2 pos) {
    m_gridWidth = width;
    m_gridHeight = height;
    // m_granularity = granularity;
    m_granularityX = granularityX;
    m_granularityY = granularityY;
    m_gui = gui;
    m_renderer = m_gui->getRenderer();

    // int baseSize = 16;
    // TODO: Better scaling?
    int baseSize = std::min((int)(m_gui->getWindowWidth() / rows), (int)(m_gui->getWindowHeight() - (m_gui->getWindowHeight() * 0.2)) / columns);
    std::cout << baseSize << std::endl;
    int scale = 1;
    // while ((baseSize / scale) * rows > m_gui->getWindowWidth() || (baseSize / scale) * columns > m_gui->getWindowHeight()) scale *= 2;
    // while ((baseSize / scale) * rows < m_gui->getWindowWidth() && (baseSize / scale) * columns < m_gui->getWindowHeight()) baseSize += 2;

    m_gridPointWidth = baseSize / scale;
    m_gridPointHeight = baseSize / scale;

    int offsetX = (m_gui->getWindowWidth() - baseSize * rows) / 2;
    std::cout << "offsetx" << offsetX << std::endl;
    int offsetY = 0;

    m_pos = pos;

    // Wrong order here?
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            int _x = i * m_gridPointWidth + m_pos.x + offsetX;
            int _y = j * m_gridPointHeight + m_pos.y + offsetY;
            m_gridpoints.push_back(Gridpoint(m_gui, Vector2(_x, _y), m_gridPointWidth, m_gridPointHeight));
        }
    }

    m_gridTexture = SDL_CreateTexture(
        m_renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        m_gridWidth,
        m_gridHeight
    );

    SDL_SetRenderTarget(m_renderer, m_gridTexture);
    for (auto &gp : m_gridpoints)
        gp.renderTexture();
    SDL_SetRenderTarget(m_renderer, nullptr);
}

void Grid::setSize(int width, int height) {
    int rows = width;
    int cols = height;

    m_gridpoints.clear();

    // Wrong order here?
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int _x = i * m_gridPointWidth + m_pos.x;
            int _y = j * m_gridPointHeight + m_pos.y;
            m_gridpoints.push_back(Gridpoint(m_gui, Vector2(_x, _y), m_gridPointWidth, m_gridPointHeight));
        }
    }
}

void Grid::render() {
    SDL_RenderCopy(m_renderer, m_gridTexture, nullptr, nullptr);
}

bool Grid::isPointEmpty(Gridpoint point) {
    return point.isEmpty();
}

int Grid::getGridPointWidth() {
    return m_gridPointWidth;
}

int Grid::getGridPointHeight() {
    return m_gridPointHeight;
}

int Grid::getWidth() {
    return m_gridWidth;
}

int Grid::getHeight() {
    return m_gridHeight;
}

int Grid::getGridPointGranularityX() {
    return m_granularityX;
}

int Grid::getGridPointGranularityY() {
    return m_granularityY;
}

Gridpoint *Grid::getPoint(int x, int y) {
    for (auto &gp : m_gridpoints) {
        if(gp.contains(x, y)) return &gp;
    }
    return nullptr;
}

Grid::~Grid() {}


Gridpoint::Gridpoint(GUI *gui, Vector2 pos, int width, int height) {
    this->m_gui = gui;
    this->m_gridPointX = pos.x;
    this->m_gridPointY = pos.y;
    m_pos = pos;
    this->m_gridWidth = width;
    this->m_gridHeight = height;
    m_texture = m_gui->getTexture(TextureID::GRIDTILE);

    m_destRect = {m_gridPointX, m_gridPointY, m_gridWidth, m_gridHeight};
}

void Gridpoint::render() {
    SDL_Rect gpL, gpR, gpB, gpT;
    gpL.x = m_gridPointX;
    gpL.y = m_gridPointY;
    gpL.h = m_gridHeight;
    gpL.w = 1;

    gpR.x = m_gridPointX;
    gpR.y = m_gridPointY;
    gpR.h = 1;
    gpR.w = m_gridWidth;

    if(m_gridPointIsEmpty) {
        SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);
    } else {
        SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
    }

    if(m_gridPointHasScore) {
        SDL_SetRenderDrawColor(m_renderer, 255, 255, 0, 255);
    }

    SDL_RenderDrawRect(m_renderer, &gpL);
    SDL_RenderFillRect(m_renderer, &gpL);
    
    SDL_RenderDrawRect(m_renderer, &gpR);
    SDL_RenderFillRect(m_renderer, &gpR);    
}

void Gridpoint::renderTexture() {
    SDL_RenderCopy(m_gui->getRenderer(), m_texture, NULL, &m_destRect);
}

bool Gridpoint::contains(int x, int y) {
    // std::cout << "X: " << x << std::endl;
    // std::cout << "m_gridPointX" << m_gridPointX << std::endl;
    bool withinX = (x >= m_gridPointX && x <= (m_gridPointX + m_gridWidth));
    bool withinY = (y >= m_gridPointY && y <= (m_gridPointY + m_gridHeight));
    return withinX && withinY;
}

void Gridpoint::setNotEmpty() {
    m_gridPointIsEmpty = false;
}

void Gridpoint::setScore() {
    m_gridPointHasScore = true;
}

void Gridpoint::removeScore() {
    m_gridPointHasScore = false;
}

bool Gridpoint::hasScore() {
    return m_gridPointHasScore;
}

void Gridpoint::setEmpty() {
    m_gridPointIsEmpty = true;
}

bool Gridpoint::isEmpty() {
    return m_gridPointIsEmpty;
}

Vector2 Gridpoint::getGridPointPos() {
    return m_pos;
}

int Gridpoint::getGridPointX() {
    return m_gridPointX;
}

int Gridpoint::getGridPointY() {
    return m_gridPointY;
}

int Gridpoint::getGridPointWidth() {
    return m_gridWidth;
}

int Gridpoint::getGridPointHeight() {
    return m_gridHeight;
}

Gridpoint::~Gridpoint() {}
