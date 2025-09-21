#include "Menu.hpp"



GMenuItemButton::GMenuItemButton(GUI* gui, Vector2 pos, std::string content, std::string font, SDL_Color color, SDL_Color highlighted) : GMenuItem(pos, color, MenuItemType::M_BUTTON), m_highlighted(highlighted) {
    m_text = gui->createText(m_pos, content, font, color, TEXT_CENTRALIZED);
}

void GMenuItemButton::render() {
    m_text->render();
}

void GMenuItemButton::renderHighlighted() {
    m_text->renderColor(m_highlighted);
}

void GMenuItemButton::bind(std::function<void()> f) {
    m_select = f;
}

void GMenuItemButton::trigger() {
    if (m_select) m_select();
}

GMenuItemBar::GMenuItemBar(GUI* gui, Vector2 pos, Vector2 dim, int step, float scale, std::string content, std::string font, SDL_Color color, SDL_Color highlighted) : GMenuItem(pos, color, MenuItemType::M_BAR), m_highlighted(highlighted) {
    m_text = gui->createText(m_pos, content, font, color, TEXT_CENTRALIZED);
    m_renderer = gui->getRenderer();
    m_scale = scale;
    m_barStep = step;
    m_barSizeMax = dim.x;
    m_barSize = m_barSizeMax / 2;

    m_bar.w = m_barSize;
    m_bar.h = dim.y;
    m_bar.x = pos.x - m_barSizeMax / 2;
    m_bar.y = pos.y;
}

void GMenuItemBar::render() {
    m_text->render();
    SDL_SetRenderDrawColor(m_renderer, m_color.r, m_color.g, m_color.b, 255);
    SDL_RenderFillRect(m_renderer, &m_bar);
}

void GMenuItemBar::renderHighlighted() {
    m_text->renderColor(m_highlighted);
    SDL_SetRenderDrawColor(m_renderer, m_highlighted.r, m_highlighted.g, m_highlighted.b, 255);
    SDL_RenderFillRect(m_renderer, &m_bar);
}

void GMenuItemBar::bind(std::function<void()> l, std::function<void()> r) {
    m_left = l;
    m_right = r;
}

void GMenuItemBar::triggerLeft() {
    if (m_left) m_left();
    if (m_barSize > 0) m_barSize -= m_barStep;
    m_bar.w = m_barSize * m_scale;
}

void GMenuItemBar::triggerRight() {
    if (m_right) m_right();
    if (m_barSize < m_barSizeMax) m_barSize += m_barStep;
    m_bar.w = m_barSize * m_scale;
}

GMenu::GMenu(Vector2 pos) : m_pos(pos) {}

void GMenu::setCurrent(std::shared_ptr<GMenuItem> item) {
    m_current = item;
}

void GMenu::moveUp() {
    if (m_current && m_current->up) {
        m_current = m_current->up;
    }
}

void GMenu::moveDown() {
    if (m_current && m_current->down) {
        m_current = m_current->down;
    }
}

void GMenu::moveLeft() {
    if (!m_current) return;

    if (m_current->type() == MenuItemType::M_BAR) {
        m_current->triggerLeft();
    } else if (m_current->left) {
        m_current = m_current->left;
    }
}

void GMenu::moveRight() {
    if (!m_current) return;

    if (m_current->type() == MenuItemType::M_BAR) {
        m_current->triggerRight();
    } else if (m_current->right) {
        m_current = m_current->right;
    }
}

void GMenu::trigger() {
    if (m_current) {
        m_current->trigger();
    }
}

void GMenu::render() {
    for (auto& mi : m_menuItems) {
        mi->render();
    }

    if (m_current) {
        m_current->renderHighlighted();
    }
}

void GMenu::addItem(std::shared_ptr<GMenuItem> item) {
    m_menuItems.push_back(item);
}

void GMenu::onEvent(const SDL_Event& event) {
    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_S] || key_state[SDL_SCANCODE_DOWN]) {
        moveDown();
    }

    if (key_state[SDL_SCANCODE_W] || key_state[SDL_SCANCODE_UP]) {
        moveUp();
    }

    if (key_state[SDL_SCANCODE_D] || key_state[SDL_SCANCODE_RIGHT]) {
        moveRight();
    }

    if (key_state[SDL_SCANCODE_A] || key_state[SDL_SCANCODE_LEFT]) {
        moveLeft();
    }

    if (key_state[SDL_SCANCODE_RETURN]) {
        trigger();
    }
}

int GMenu::getX() const {
    return m_pos.x;
}

int GMenu::getY() const {
    return m_pos.y;
}
