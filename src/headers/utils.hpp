#pragma once

#include <iostream>
#include <string>
#include <filesystem>

#if defined(_WIN32)
#include <winsock2.h>
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

inline std::string getExecutablePath() {
    char path[1024];
#if defined(_WIN32)
    GetModuleFileNameA(nullptr, path, sizeof(path));
#elif defined(__APPLE__)
    uint32_t size = sizeof(path);
    _NSGetExecutablePath(path, &size);
#elif defined(__linux__)
    ssize_t count = readlink("/proc/self/exe", path, sizeof(path));
    path[count] = '\0';
#endif
    return std::string(path);
}

inline std::filesystem::path getExecutableDir() {
    return std::filesystem::path(getExecutablePath()).parent_path();
}