#include "headers/Gui.hpp"

SDL_Surface *m_loadSurface(const std::string &path) {
    SDL_Surface *surface = IMG_Load(path.c_str());

    if (!surface) {
        std::cout << "Failed to load image: " << IMG_GetError() << std::endl;
    }

    return surface;
}

SDL_Surface *m_createTTFSurface(TTF_Font *font, std::string content, SDL_Color color) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, content.c_str(), color);

    if (!surface) {
        std::cout << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }

    return surface;
}

// TODO: Make this part of the GUI and remove renderer paremeter.
SDL_Texture *m_createTextureFromSurface(SDL_Renderer *renderer, SDL_Surface *surface) {
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (!texture) {
        std::cout << "Failed to create texture: " << SDL_GetError() << std::endl;
    }

    return texture;
}

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

    std::filesystem::path basePathFonts = getExecutableDir() / "fonts";
    m_font = TTF_OpenFont((basePathFonts / "PixeloidMono.ttf").string().c_str(), 28);

    if (!m_font) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }

    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
        printf("SDL_image could not initialize PNG support! SDL_image Error: %s\n", IMG_GetError());
    }

}

SDL_Texture *GUI::loadTexture(TextureID name, const std::string &filePath) {
    std::filesystem::path basePathGfx = getExecutableDir() / "gfx";

    SDL_Surface *surface = m_loadSurface((basePathGfx / filePath).string());

    SDL_Texture *texture = m_createTextureFromSurface(m_renderer, surface);

    SDL_FreeSurface(surface);

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

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
    SDL_Surface *surface = m_loadSurface((basePathGfx / filePath).string());

    SDL_Texture *texture = m_createTextureFromSurface(m_renderer, surface);
    SDL_FreeSurface(surface);

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, alpha);

    if (cache) {
        m_textureMap[name] = texture;
    }

    return texture;
}

// TODO: Remove???
SDL_Rect GUI::createRect(int x, int y, int w, int h)  {
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    return rect;
}

void GUI::renderTexture(TextureID key, Vector2 pos, Vector2 dim) {
    SDL_Rect dst;

    dst.x = pos.x;
    dst.y = pos.y;
    dst.w = dim.x;
    dst.h = dim.y;

    SDL_RenderCopy(m_renderer, getTexture(key), nullptr, &dst);
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

void GUI::renderText(int x, int y, const std::string &text) {
    // Create surface from text
    SDL_Surface* surface = TTF_RenderText_Blended(m_font, text.c_str(), color::WHITE);
    if (!surface) {
        SDL_Log("Failed to create text surface: %s", TTF_GetError());
        return;
    }

    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    if (!texture) {
        SDL_Log("Failed to create text texture: %s", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect destRect = { x, y, surface->w, surface->h };

    SDL_FreeSurface(surface); // surface no longer needed

    // Render the texture to screen
    SDL_RenderCopy(m_renderer, texture, nullptr, &destRect);

    SDL_DestroyTexture(texture); // Clean up texture
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

// TODO: Update to take a font from the m_fonts vector.
TTF_Font *GUI::getFont() {
    return m_font;
}

bool GUI::loadFont(std::string name, std::string path, int f_size) {
    // TODO: Make constants in GUI?
    std::filesystem::path basePathFonts = getExecutableDir() / "fonts";

    // See if font with same name already exists.
    if (m_fonts.find(name) != m_fonts.end()) {
        return false;
    }

    TTF_Font *font = TTF_OpenFont((basePathFonts / path).string().c_str(), f_size);
    if (!font) {
        std::cout << "Failed to load font" << std::endl;
        return false;
    }

    m_fonts[name] = font;

    return true;
}

GUI::~GUI() {
    TTF_CloseFont(m_font);
    TTF_Quit();
    Mix_Quit();
    SDL_Quit();
}

void GUIElement::updatePos(Vector2 newPos) {
    m_pos = newPos;
}


// Text
GText::GText(SDL_Renderer *renderer, Vector2 pos, Vector2 dim, std::string content, TTF_Font *font, SDL_Texture *texture, SDL_Color color) : GUIElement(renderer, pos, texture, color) {
    m_font = font;
    m_content = content;

    m_dest.x = pos.x;
    m_dest.y = pos.y;
    m_dest.w = dim.x;
    m_dest.h = dim.y;
}

void GText::render() {
    SDL_SetTextureColorMod(m_texture, m_color.r, m_color.g, m_color.b);
    SDL_RenderCopy(m_renderer, m_texture, nullptr, &m_dest);
}

void GText::renderColor(SDL_Color c) {
    SDL_SetTextureColorMod(m_texture, c.r, c.g, c.b);
    SDL_RenderCopy(m_renderer, m_texture, nullptr, &m_dest);
}

int GText::getWidth() {
    return m_dest.w;
}

int GText::getHeight() {
    return m_dest.h;
}

GText::~GText() {
    // if (m_font) TTF_CloseFont(m_font);
}

std::shared_ptr<GText> GUI::createText(Vector2 pos, const std::string &content, std::string font, SDL_Color color, int flags) {

    auto it = m_fonts.find(font);
    if (it == m_fonts.end()) {
        // fallback
        it = m_fonts.find("default");
        if (it == m_fonts.end()) return nullptr;
    }
    TTF_Font *_font = it->second;
    if (!_font) {
        std::cerr << "Error: font pointer is null for " << font << std::endl;
        return nullptr;
    }

    SDL_Surface *textSurface = m_createTTFSurface(_font, content, color::WHITE);
    SDL_Texture *textTexture = m_createTextureFromSurface(m_renderer, textSurface);

    if (flags & TEXT_CENTRALIZED) {
        pos.x = pos.x - textSurface->w / 2;
    }

    std::shared_ptr<GText> _text = std::make_shared<GText>(m_renderer, pos, Vector2(textSurface->w, textSurface->h), content, _font, textTexture, color);
    SDL_FreeSurface(textSurface);
    
    return _text;
}
