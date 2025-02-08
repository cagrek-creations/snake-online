#pragma once

#include <iostream>
#include <functional>
#include <SDL2/SDL.h>


class Observer {
    public:
        virtual void onEvent(const SDL_Event& event) {};
        virtual void onServerMessage(const std::string message) {};
        virtual void onMessage(const std::string message) {};

        virtual void setSignalCallback(std::function<void(const std::string&)> callback) {};
};
