#include "headers/Score.hpp"

Score::Score(SDL_Renderer *renderer, GUI *gui, int width, int height, TextureID type) {
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

    for (auto effect = m_effects.begin(); effect != m_effects.end(); ) {
        (*effect)->move(xPos, yPos);
        ++effect;
    }
}

void Score::render() {
    SDL_Rect destinationRect = {m_xPos, m_yPos,     // x, y
                                m_width, m_height}; // width, height
    SDL_RenderCopy(m_renderer, m_textureScore, NULL, &destinationRect);

    for (auto effect = m_effects.begin(); effect != m_effects.end(); ) {
        if (*effect) {
            (*effect)->render();
        }
        ++effect;
    }
}

void Score::update(float deltaTime) {
    for (auto effect = m_effects.begin(); effect != m_effects.end(); ) {
        (*effect)->update(deltaTime);
        ++effect;
    }
}

Score::~Score() {
    // TODO: Why???
    // SDL_DestroyTexture(m_textureScore);
}

void Score::addEffect(std::unique_ptr<Effect> effect) {
    m_effects.emplace_back(std::move(effect));
}


