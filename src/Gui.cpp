#include "Gui.hpp"
#include <SDL2/SDL_render.h>

GUI::GUI(const char *title, int windowWidth, int windowHeight, bool fullscreen) {
    int flags = 0;
    this->m_windowWidth = windowWidth;
    this->m_windowHeight = windowHeight;
    this->m_windowClose = false;

    if (fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }

    if (SDL_Init(SDL_INIT_AUDIO || SDL_INIT_VIDEO) == 0) {
        m_window = SDL_CreateWindow(
                                    title,
                                    SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED,
                                    this->m_windowWidth,
                                    this->m_windowHeight,
                                    flags | SDL_WINDOW_RESIZABLE  
        );

        m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_PRESENTVSYNC);

        if(m_renderer) {
            SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
        }
    }

    m_font = TTF_OpenFont("./font.ttf", 28);

    if (!m_font) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }

    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
        printf("SDL_image could not initialize PNG support! SDL_image Error: %s\n", IMG_GetError());
    }

}

SDL_Texture *GUI::loadTexture(TextureID name, const std::string &filePath) {
    std::filesystem::path basePathGfx = getExecutableDir() / "gfx";
    SDL_Surface *surface = IMG_Load((basePathGfx / filePath).string().c_str());
    if (!surface) {
        std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    SDL_FreeSurface(surface);

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    if (!texture) {
        std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
    }

    m_textureMap[name] = texture;

    return texture;
}

SpriteSheet *GUI::loadAtlas(TextureID name, const std::string &filePath, int frameWidth, int frameHeight, int numFrames) {
    std::filesystem::path basePathGfx = getExecutableDir() / "gfx";
    auto sheet = std::make_unique<SpriteSheet>(
        m_renderer,
        (basePathGfx / filePath).string(),
        frameWidth,
        frameHeight,
        numFrames
    );

    SpriteSheet *sheetPtr = sheet.get();
    m_atlasMap[name] = std::move(sheet);
    return sheetPtr;
}

SpriteSheet *GUI::getAtlas(TextureID id) {
    auto it = m_atlasMap.find(id);
    if (it != m_atlasMap.end()) {
        return it->second.get();
    }
    return nullptr;
}

SDL_Texture *GUI::loadTextureAlpha(TextureID name, const std::string &filePath, int alpha, bool cache) {
    std::filesystem::path basePathGfx = getExecutableDir() / "gfx";
    SDL_Surface *surface = IMG_Load((basePathGfx / filePath).string().c_str());
    if (!surface) {
        std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    SDL_FreeSurface(surface);

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, alpha);

    if (!texture) {
        std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
    }
    if (cache) {
        m_textureMap[name] = texture;
    }

    return texture;
}

SDL_Rect GUI::createRect(int x, int y, int w, int h)  {
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    return rect;
}

SDL_Texture *GUI::copyTexture(TextureID key) {
    int w, h;
    Uint32 format;
    int access;

    SDL_Texture *source = getTexture(key);
    if (source == nullptr) {
        return nullptr;
    }

    // Get texture info
    if (SDL_QueryTexture(source, &format, &access, &w, &h) != 0) {
        SDL_Log("SDL_QueryTexture failed: %s", SDL_GetError());
        return nullptr;
    }

    // Create new texture as a render target
    SDL_Texture* copy = SDL_CreateTexture(m_renderer, format, SDL_TEXTUREACCESS_TARGET, w, h);
    if (!copy) {
        SDL_Log("SDL_CreateTexture failed: %s", SDL_GetError());
        return nullptr;
    }

    SDL_SetTextureBlendMode(copy, SDL_BLENDMODE_BLEND);

    SDL_Texture* oldTarget = SDL_GetRenderTarget(m_renderer);

    SDL_SetRenderTarget(m_renderer, copy);

    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
    SDL_RenderClear(m_renderer);

    SDL_RenderCopy(m_renderer, source, nullptr, nullptr);

    SDL_SetRenderTarget(m_renderer, oldTarget);

    return copy;
}

SDL_Texture *GUI::getTexture(TextureID key) {
    // Retrieve the texture associated with the key
    auto it = m_textureMap.find(key);
    if (it != m_textureMap.end()) {
        return it->second;
    }

    return nullptr;  // Texture not found
}

void GUI::unloadTexture(TextureID key) {
    // Unload the texture associated with the key
    auto it = m_textureMap.find(key);
    if (it != m_textureMap.end()) {
        SDL_DestroyTexture(it->second);
        m_textureMap.erase(it);
    }
}

void GUI::onEvent(const SDL_Event& event) {
    if(event.type == SDL_QUIT) {
        m_windowClose = true;
    }
}

int GUI::getWindowWidth() {
    return m_windowWidth;
}

int GUI::getWindowHeight() {
    return m_windowHeight;
}

int GUI::getWindowCenterX() {
    return m_windowWidth / 2;
}

int GUI::getWindowCenterY() {
    return m_windowHeight / 2;
}

SDL_Color GUI::getColor(std::string colorName) {
    if(colorName == "green") {
        return color::GREEN;
    } else if (colorName == "white") {
        return color::WHITE;
    } else if (colorName == "red" ) {
        return color::RED;
    }

    return color::DEFAULT;
}


void GUI::update() {

}

void GUI::render() {
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderPresent(m_renderer);
}


void GUI::clearRenderer() {
    SDL_RenderClear(m_renderer);
}

bool GUI::getWindowClose() {
    return !m_windowClose;
}

SDL_Renderer *GUI::getRenderer() {
    return m_renderer;
}

TTF_Font *GUI::getFont() {
    return m_font;
}

GUI::~GUI() {
    TTF_CloseFont(m_font);
    TTF_Quit();
    Mix_Quit();
    SDL_Quit();
}