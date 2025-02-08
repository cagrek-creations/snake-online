#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

#include <SDL2/SDL.h>

#include "Client_interface.hpp"
#include "Observer.hpp"

void getIpAdressAndPort(std::string &ip, int &port);
void getName(std::string &name);
void getColor(std::string &color);

class Controller {
    public:

        Controller() {
            m_serverEvents = {};
            m_localEvents = {};
        }

        ~Controller() {
            // m_controllerThread.join();
            // delete m_client;
        }

        std::vector<std::string> getServerEvents() {
            std::lock_guard<std::mutex> lock(m_mutexServerEvents);
            for (auto e : m_serverEvents) {
                std::cout << "Event: " << e << std::endl;
            }
            std::vector<std::string> m_serverEventsCopy = m_serverEvents;
            m_serverEvents.clear();
            return m_serverEventsCopy;
        }

        std::vector<std::string> getLocalEvents() {
            std::vector<std::string> m_localEventsCopy = m_localEvents;
            m_localEvents.clear();
            return m_localEventsCopy;
        }

        void connect(std::string m_ip, int m_port) {
            m_client = std::make_unique<TcpCommunication>(m_ip.c_str(), m_port); 

            if (m_client->isConnected()) {
                // Allow for client to communicate
                std::thread receiveThread(&Controller::comm_receive, this);
                receiveThread.detach();
            }
        }

        // Attach an observer to the controller
        void attachObserver(Observer* observer) {
            observers.push_back(observer);

            observer->setSignalCallback([this](const std::string &message) {
                this->onObserverSignal(message);
            });
        }

        void onObserverSignal(const std::string &message) {
            std::cout << "Controller received notification: " << message << std::endl;
            notifyMessage(message);
        }

        // Notify all attached observers about an event
        void notifyEvent(const SDL_Event& event) {
            for (auto observer : observers) {
                observer->onEvent(event);
            }
        }

        void notifyMessage(const std::string &message) {
            if(m_client->isConnected()) {
                m_client->send(message.c_str(), message.size());
            }

            for (auto observer : observers) {
                observer->onMessage(message);
            }

            m_localEvents.push_back(message);
        }

        void sendMessage(const std::string &message) {
            if (m_client->isConnected()) {
                m_client->send(message.c_str(), message.size());
            }
        }

        // Simulate an event in the controller
        void simulateEvent() {
            SDL_Event event;
            event.type = SDL_USEREVENT;
            notifyEvent(event);
        }

        void broadcastNewMenu(int menuid) {
            SDL_Event event;
            event.type = SDL_USEREVENT;
            event.user.code = menuid;
            notifyEvent(event);
        }

        void broadcastEvent(int eventId) {
            SDL_Event event;
            event.type = SDL_USEREVENT;
            event.user.code = eventId;
            notifyEvent(event);
        }

        void update() {
            listener();
        }

    protected:
        
        std::unique_ptr<TcpCommunication> m_client; 

        std::string m_nickname;
        std::string m_ip = "127.0.0.1";
        int m_port = 12345;

        std::vector<Observer*> observers;
        // std::thread m_controllerThread;
        SDL_Event m_event;
        bool running = true;

        void handleInput(const std::string input) {
            // Default implementation: just notify observers
            for (auto observer : observers) {
                observer->onServerMessage(input);
            }

            std::lock_guard<std::mutex> lock(m_mutexServerEvents);
            m_serverEvents.push_back(input);
        }

        void comm_receive() {
            while(m_client && m_client->isConnected()) {
                std::string input = "";
                m_client->receive(input);
                std::cout << input << std::endl;
                // std::vector<std::string> parsedInput = splitString(input, ';');

                handleInput(input);
            }
        }

        void listener() {
            while(SDL_PollEvent(&m_event)) {
                notifyEvent(m_event);
            }
        }

    private:
        std::vector<std::string> m_serverEvents;
        std::vector<std::string> m_localEvents;
        std::mutex m_mutexServerEvents;

};

