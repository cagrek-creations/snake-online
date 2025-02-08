#include "comm_util.hpp"

// Initialize Winsock
bool initWinsock() {
#ifdef _WIN32
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
#else
    return true;
#endif
}

// Cleanup Winsock
void cleanupWinsock() {
#ifdef _WIN32
    WSACleanup();
#endif
}

// Create a socket
int createSocket() {
#ifdef _WIN32
    return socket(AF_INET, SOCK_STREAM, 0);
#else
    return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#endif
}

// Close a socket
void closeSocket(int socket) {
#ifdef _WIN32
    closesocket(socket);
#else
    close(socket);
#endif
}

// Set up a sockaddr_in structure
void setupSockAddr(sockaddr_in& addr, const char* ipAddress, int port) {
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ipAddress);
}

std::unordered_map<std::string, std::string> parseInput(const std::string& input) {
    std::unordered_map<std::string, std::string> keyValueMap;

    std::istringstream iss(input);
    std::string pair;

    while (std::getline(iss, pair, ';')) {
        std::istringstream pairStream(pair);
        std::string key, value;

        if (std::getline(pairStream, key, ':') && std::getline(pairStream, value)) {
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);

            keyValueMap[key] = value;
        }
    }

    return keyValueMap;
}


std::unordered_map<std::string, std::string> getConfiguration(const std::string& filename) {
    std::unordered_map<std::string, std::string> key_value_pairs;

    std::ifstream input_file(filename);

    if (!input_file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return key_value_pairs;
    }

    // Read each line from the file
    std::string line;
    while (std::getline(input_file, line)) {
        // Ignore everything after '#'
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }

        // Use stringstream to split the line into key and value
        std::istringstream line_stream(line);
        std::string key, value;

        // Read key and value separated by ':'
        if (std::getline(line_stream, key, ':') && std::getline(line_stream, value)) {
            // Trim leading and trailing whitespaces
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);

            // Insert key-value pair into the map
            key_value_pairs[key] = value;
        }
    }

    input_file.close();

    return key_value_pairs;
}

std::vector<std::string> splitString(const std::string& input, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream stream(input);
    std::string token;

    while (std::getline(stream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}
