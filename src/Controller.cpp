#include "Controller.hpp"

void getIpAdressAndPort(std::string &ip, int &port) {
    std::unordered_map<std::string, std::string> config = getConfiguration("config.txt");

    auto ip_it      = config.find("ip");
    auto port_it    = config.find("port");
    
    if (ip_it   != config.end()) ip = ip_it->second;
    if (port_it != config.end()) port = std::stoi(port_it->second);
}

void getName(std::string &name) {
    std::unordered_map<std::string, std::string> config = getConfiguration("config.txt");

    auto name_it = config.find("name");

    if (name_it != config.end()) name = name_it->second;
}

void getColor(std::string &color) {
    std::unordered_map<std::string, std::string> config = getConfiguration("config.txt");

    auto color_it = config.find("color");

    if (color_it != config.end()) color = color_it->second;
}