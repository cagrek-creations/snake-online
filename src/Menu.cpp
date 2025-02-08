#include "Menu.hpp"

Menu::Menu(SDL_Renderer *renderer, int menuid, int xPos, int yPos, 
            int width, int height, TTF_Font *font, int &state, int previousState, int menuOwnState) {

    m_renderer = renderer;

    m_xPos          = xPos;
    m_yPos          = yPos;
    m_width         = width;
    m_height        = height;
    m_menuid        = menuid;
    // m_running       = true;
    m_updateMenu    = false;
    m_activeMenu    = false;
    m_limit         = 0;
    m_menuIndex     = 0; // Starting at first value of a menu.

    m_state         = &state;
    m_menuOwnState  = menuOwnState;
    m_previousState = previousState;

    m_font = font;
    // m_menuThread = std::thread(&Menu::updateMenu, this);
    // std::cout << &m_items << std::endl;


}

Text Menu::createText(const std::string &name, int xPos, int yPos, SDL_Color textColor) {
    
    SDL_Surface *textSurface = TTF_RenderText_Solid(m_font, name.c_str(), textColor);

    if (!textSurface) {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
    if (!textTexture) {
        std::cerr << "Unable to create texture from rendered text! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
    }

    Text textInfo;
    textInfo.width      = textSurface->w;
    textInfo.height     = textSurface->h;
    textInfo.texture    = textTexture;
    textInfo.xPos       = xPos;
    textInfo.yPos       = yPos;
    textInfo.name       = name;

    SDL_FreeSurface(textSurface);
    return textInfo;
}

bool Menu::updateText(Text &txt, SDL_Color textColor) {
    SDL_Surface *textSurface = TTF_RenderText_Solid(m_font, txt.name.c_str(), textColor);
    if (!textSurface) {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
    if (!textTexture) {
        std::cerr << "Unable to create texture from rendered text! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
    }
    SDL_FreeSurface(textSurface);

    txt.texture = textTexture;
    return true;
}

bool Menu::updateTextValue(Text &txt, const std::string newText, MenuItem &mi) {
    std::string newValue = mi.getTextString() + newText;
    SDL_Surface *textSurface = TTF_RenderText_Solid(m_font, newValue.c_str(), mi.getColor());
    if (!textSurface) {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
    if (!textTexture) {
        std::cerr << "Unable to create texture from rendered text! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
    }
    SDL_FreeSurface(textSurface);

    txt.texture = textTexture;
    return true;
}




// template int Menu::addItem(std::string name, int type, float *reference_value);

int Menu::addItem() {
    return m_items.size();
}




int Menu::addItemState(const std::string &name, int nextState) {

    // std::unique_ptr<MenuState> mi = std::make_unique<MenuState>(m_renderer, refFunc);
    // m_items.push_back(std::move(mi));

    std::unique_ptr<MenuState> mi = std::make_unique<MenuState>(m_renderer, name, nextState, m_xPos, m_yPos + m_items.size() * 50, m_font, *this);
    // m_items.push_back(std::move(mi));
    addItemT(std::move(mi));
    // Text textInfo = createText(name, m_xPos, m_yPos + m_items.size() * 50, menuc::WHITE);
    // textInfo.updateX(m_xPos + (m_width - textInfo.width) / 2);

    // if(m_items.size() == 0) {
    //     updateText(textInfo, menuc::RED);
    // }
    // int x = 3;
    // // MenuItem mi(x);
    // // mi.menuText = textInfo;
    // // mi.nextState = newValue;
    // // mi.type = MENU_STATE;
    // // mi.m_renderer = m_renderer;
    // auto myLambda = []() {
        
    // };

    // // std::function using the lambda
    // std::function<void()> myFunction = myLambda;
    // MenuState mi = MenuState(m_renderer, myFunction);

    // m_items.push_back(&mi);

    return m_items.size(); 
}

int Menu::addItemBar(std::string name, std::function<void()> refFuncL, std::function<void()> refFuncR) {
    std::unique_ptr<MenuBar> mi = std::make_unique<MenuBar>(m_renderer, name, m_xPos, m_yPos + m_items.size() * 50, m_font, refFuncL, refFuncR, *this);
    // m_items.push_back(std::move(mi));
    addItemT(std::move(mi));
    return 0;
}



void Menu::updateMenu() {
    
}

void Menu::onEvent(const SDL_Event& event) {
    
    // Replace with game menu.
    if(event.type == SDL_USEREVENT) {
        if(event.user.code == 3) {
            const Uint8 *key_state = SDL_GetKeyboardState(NULL);
            if(key_state[SDL_SCANCODE_ESCAPE]) {
                *m_state = 0x0;
            }
        }
    }

    if(*m_state == m_menuOwnState) {
        if(event.type == SDL_USEREVENT) {
            if(event.user.code == m_menuid) {
                m_activeMenu = true;
            } else {
                m_activeMenu = false;
            }
        }

        if (event.type == SDL_KEYDOWN) {
          const Uint8 *key_state = SDL_GetKeyboardState(NULL);
          if (m_items.size() > 0) {
            if (key_state[SDL_SCANCODE_DOWN]) {
              m_items[m_menuIndex]->reset();
              if (m_menuIndex < m_items.size() - 1)
                m_menuIndex++;
              m_items[m_menuIndex]->update();
            } else if (key_state[SDL_SCANCODE_UP]) {
              m_items[m_menuIndex]->reset();
              if (m_menuIndex > 0)
                m_menuIndex--;
              m_items[m_menuIndex]->update();
            }
            // if(key_state[SDL_SCANCODE_DOWN]) {
            //     updateText(m_items[m_menuIndex]->getColor(), menuc::WHITE);
            //     if(m_menuIndex < m_items.size()-1) m_menuIndex++;
            //     updateText(m_items[m_menuIndex].menuText, menuc::RED);
            // } else if (key_state[SDL_SCANCODE_UP]) {
            //     updateText(m_items[m_menuIndex].menuText, menuc::WHITE);
            //     if(m_menuIndex > 0) m_menuIndex--;
            //     updateText(m_items[m_menuIndex].menuText, menuc::RED);
            // }

            else if (key_state[SDL_SCANCODE_RIGHT]) {
              m_items[m_menuIndex]->trigger(KEY_RIGHT);
            }

            else if (key_state[SDL_SCANCODE_LEFT]) {
              m_items[m_menuIndex]->trigger(KEY_LEFT);
            }

            else if (key_state[SDL_SCANCODE_RETURN]) {
              m_items[m_menuIndex]->trigger(-1);
            }
          }

          // if(key_state[SDL_SCANCODE_RIGHT] && m_items[m_menuIndex].type ==
          // MENU_BAR) {
          //     (m_items[m_menuIndex].referenceValue) += 128 / 10;
          //     m_controller->broadcastEvent(5); // Sound change
          // }

          // if(key_state[SDL_SCANCODE_LEFT] && m_items[m_menuIndex].type ==
          // MENU_BAR) {
          //     (m_items[m_menuIndex].referenceValue) -= 128 / 10;
          //     m_controller->broadcastEvent(5);
          // }

          // if (key_state[SDL_SCANCODE_ESCAPE]) {
          //     std::cout << "ESCAPE" << std::endl;
          //     if(m_items.size() > 0)
          //     updateText(m_items[m_menuIndex].menuText, menuc::WHITE);
          //     m_menuIndex = -2;
          //     m_updateMenu = true;
          // }

          if (key_state[SDL_SCANCODE_ESCAPE]) {
            std::cout << "PREVIOUS:" << m_previousState << std::endl;
            *m_state = m_previousState;
          }

          if (key_state[SDL_SCANCODE_RETURN]) {
            if (m_items.size() > 0) {
              // m_updateMenu = true;
              // m_items[m_menuIndex].menuText
              // if(m_items[m_menuIndex].type == MENU_STATE) {
              //     updateText(m_items[m_menuIndex].menuText, menuc::WHITE);
              //     *m_state = m_items[m_menuIndex].nextState;
              // } else if (m_items[m_menuIndex].type == MENU_ON_OFF) {
              //     if (m_items[m_menuIndex].onoff) {
              //         updateTextValue(m_items[m_menuIndex].menuText, "off",
              //         m_items[m_menuIndex]); m_items[m_menuIndex].onoff =
              //         false; m_items[m_menuIndex].referenceValue = 0;
              //     } else {
              //         updateTextValue(m_items[m_menuIndex].menuText, "on",
              //         m_items[m_menuIndex]); m_items[m_menuIndex].onoff =
              //         true; m_items[m_menuIndex].referenceValue = 1;
              //     }
              //     m_controller->broadcastEvent(5);
              // }
            }
          }
        }
    }
}


int Menu::update(double deltaTime, bool gameRunning) {
    m_limit += deltaTime;  
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    // m_running = gameRunning;


    // SDL_Event event;
    // SDL_PollEvent( &event );
    //     if (event.type == SDL_KEYDOWN) {
    //         std::cout << "KEYDOWN";
    //         if(key_state[SDL_SCANCODE_DOWN]) {
    //             updateText(m_items[m_menuIndex].menuText, menuc::WHITE);
    //             if(m_menuIndex < m_items.size()-1) m_menuIndex++;
    //             updateText(m_items[m_menuIndex].menuText, menuc::RED);
    //         } else if (key_state[SDL_SCANCODE_UP]) {
    //             updateText(m_items[m_menuIndex].menuText, menuc::WHITE);
    //             if(m_menuIndex > 0) m_menuIndex--;
    //             updateText(m_items[m_menuIndex].menuText, menuc::RED);
    //         }
    //     }
    
    // if(m_limit > 100.f) {
    //     m_limit = 0;
    //     if(key_state[SDL_SCANCODE_DOWN]) {
    //         updateText(m_items[m_menuIndex].menuText, menuc::WHITE);
    //         if(m_menuIndex < m_items.size()-1) m_menuIndex++;
    //         updateText(m_items[m_menuIndex].menuText, menuc::RED);
    //     } else if (key_state[SDL_SCANCODE_UP]) {
    //         updateText(m_items[m_menuIndex].menuText, menuc::WHITE);
    //         if(m_menuIndex > 0) m_menuIndex--;
    //         updateText(m_items[m_menuIndex].menuText, menuc::RED);
    //     }
    // }

    if(key_state[SDL_SCANCODE_RETURN]) {
        return m_menuIndex + 1;
    }

    return 0;
}

int Menu::getMenuIndex() {
    if(m_updateMenu) {
        m_updateMenu = false;
        int newIndex = m_menuIndex + 1; 
        return newIndex;
    } else {
        return -2;
    }
}

int *Menu::getMenuState() {
    return m_state;
}

int Menu::getMenuXpos() {
    return m_xPos;
}

int Menu::getMenuWidth() {
    return m_width;
}

void Menu::render() {

    SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);

    SDL_Rect rectL = {m_xPos, m_yPos, 1, m_height};
    SDL_Rect rectR = {m_xPos + m_width, m_yPos, 1, m_height};

    SDL_RenderFillRect(m_renderer, &rectL);
    SDL_RenderFillRect(m_renderer, &rectR);

    for (auto &m : m_items) {
        // SDL_Rect renderQuad = {m.menuText.xPos, m.menuText.yPos, m.menuText.width, m.menuText.height};
        // SDL_RenderCopy(m_renderer, m.menuText.texture, nullptr, &renderQuad); 
        m->render();
    }

}

Menu::~Menu() {
    // for (auto &m : m_items) {
    //     SDL_DestroyTexture(m.menuText.texture);
    // }
}