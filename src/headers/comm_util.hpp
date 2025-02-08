#ifndef COMM_UTIL_HPP
#define COMM_UTIL_HPP

#pragma once

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <cstring>
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <cerrno>
#endif

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <fstream>

// Initialize Winsock
bool initWinsock();

// Cleanup Winsock
void cleanupWinsock();

// Create a socket
int createSocket();

// Close a socket
void closeSocket(int socket);

// Set up a sockaddr_in structure
void setupSockAddr(sockaddr_in& addr, const char* ipAddress, int port);

std::unordered_map<std::string, std::string> parseInput(const std::string& input);

std::unordered_map<std::string, std::string> getConfiguration(const std::string& filename);

std::vector<std::string> splitString(const std::string& input, char delimiter);

#endif