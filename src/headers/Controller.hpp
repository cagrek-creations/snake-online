#pragma once

#include <atomic>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm> 

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

        // TODO: Update to pass a vector as a reference / parameter here to avoid the copy.
        std::vector<std::string> getServerEvents() {
            std::vector<std::string> m_serverEventsCopy;
            {
                std::lock_guard<std::mutex> lock(m_mutexServerEvents);

                m_serverEventsCopy = std::move(m_serverEvents);
                m_serverEvents.clear();
            }
            for (auto e : m_serverEvents) {
                std::cout << "Event: " << e << std::endl;
            }
            return m_serverEventsCopy;
        }

        std::vector<std::string> getLocalEvents() {
            std::vector<std::string> m_localEventsCopy = m_localEvents;
            m_localEvents.clear();
            return m_localEventsCopy;
        }

        void connect(std::string m_ip, int m_port) {
            m_client = std::make_unique<TcpCommunication>(m_ip.c_str(), m_port);

            if (m_client->isConnected() && !m_running) {
                // Allow for client to communicate
                m_running = true;
                m_receiveThread = std::thread(&Controller::comm_receive, this);
            }
        }

        void disconnect() {
            std::cout << "disconnecting" << std::endl;
            m_client->closeConnection();
            m_running = false;
            if (m_receiveThread.joinable()) {
                m_receiveThread.join();
            }
            std::cout << "disconnecting" << std::endl;
            m_client.reset();
        }

        // Attach an observer to the controller
        void attachObserver(Observer* observer) {
            observers.push_back(observer);

            observer->setSignalCallback([this](const std::string &message) {
                this->onObserverSignal(message);
            });
        }
    
        void detachObserver(Observer* observer) {
            observers.erase(
                std::remove(observers.begin(), observers.end(), observer),
                observers.end()
            );
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
            // if(m_client->isConnected()) {
            //     m_client->send(message.c_str(), message.size());
            // }

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
            while(m_running && m_client && m_client->isConnected()) {
                std::string input = "";
                m_client->receive(input);
                std::cout << "Receiving: " << input << std::endl;
                // std::vector<std::string> parsedInput = splitString(input, ';');

                // In cases of multiple messages being received in the same buffer at once.
                std::vector<std::string> messages = splitString(input, '\n');

                for (auto m : messages) {
                    handleInput(m);
                }
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
        std::atomic<bool> m_running{false};
        std::thread m_receiveThread;

};
