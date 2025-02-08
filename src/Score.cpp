#include "Score.hpp"


Score::Score(SDL_Renderer *renderer, GUI *gui, int width, int height) {
    // SDL_Surface* surface = IMG_Load("./textures/berry.png");
    // if (!surface) {
    //     std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
    // }

    // SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    // SDL_FreeSurface(surface);

    // if (!texture) {
    //     std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
    // }


    m_renderer = renderer;
    m_width = width;
    m_xPos = 99999;
    m_yPos = 99999;
    m_height = height;
    m_gui = gui;
    m_textureScore = m_gui->getTexture("berry");
    // std::cout << m_textureScore << std::endl;

}

void Score::move(int xPos, int yPos) {
    m_xPos = xPos;
    m_yPos = yPos;
}

void Score::render() {
    SDL_Rect destinationRect = {m_xPos, m_yPos,  // x, y
                                m_width, m_height}; // width, height
    SDL_RenderCopy(m_renderer, m_textureScore, NULL, &destinationRect);
}

Score::~Score() {
    // SDL_DestroyTexture(m_textureScore);
}