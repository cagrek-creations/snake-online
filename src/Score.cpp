#include "Score.hpp"

Score::Score(SDL_Renderer *renderer, GUI *gui, int width, int height, int type) {
    m_renderer = renderer;
    m_width = width;
    m_xPos = 99999;
    m_yPos = 99999;
    m_height = height;
    m_gui = gui;
    m_type = type;
    m_textureScore = m_gui->getTexture(m_type);
}

void Score::move(int xPos, int yPos) {
    m_xPos = xPos;
    m_yPos = yPos;
}

void Score::render() {
    SDL_Rect destinationRect = {m_xPos, m_yPos,     // x, y
                                m_width, m_height}; // width, height
    SDL_RenderCopy(m_renderer, m_textureScore, NULL, &destinationRect);
}

Score::~Score() {
    // TODO: Why???
    // SDL_DestroyTexture(m_textureScore);
}