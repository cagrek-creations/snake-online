#include "Soundmanager.hpp"

SoundManager::SoundManager(int &volume, int &playSound) {
    m_volume = &volume;
    m_playSound = &playSound;
    m_loops = -1;
    // Initialize SDL audio subsystem
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    }
}

SoundManager::~SoundManager() {
    for (auto& pair : m_soundMap) {
        Mix_FreeChunk(pair.second);
    }
    m_soundMap.clear();

    Mix_Quit();
    SDL_Quit();
}

void SoundManager::onEvent(const SDL_Event& event) {
    if(event.type == SDL_USEREVENT) {
        if(event.user.code == 5) {
            setVolumeAll();
            std::cout << *m_playSound << std::endl;
            if(*m_playSound) {
                playSoundAll();
            } else {
                stopSoundAll();
            }
        }
    }
}

bool SoundManager::loadSound(const char* filePath, const char* soundKey) {
    Mix_Chunk* chunk = loadChunk(filePath);
    if (chunk == nullptr) {
        return false;
    }

    m_soundMap[soundKey] = chunk;
    return true;
}

void SoundManager::playSound(const char* soundKey, int loops) {
    auto it = m_soundMap.find(soundKey);
    if (it != m_soundMap.end()) {
        Mix_PlayChannel(-1, it->second, loops);
    }
}

void SoundManager::stopSound(const char* soundKey) {
    auto it = m_soundMap.find(soundKey);
    if (it != m_soundMap.end()) {
        Mix_HaltChannel(-1);
    }
}

void SoundManager::playSoundAll() {
    Mix_Resume(-1);  // The -1 means resume all channels
}

void SoundManager::stopSoundAll() {
    Mix_Pause(-1);
}

void SoundManager::setVolumeAll() {
    for (auto& sound : m_soundMap) {
        Mix_VolumeChunk(sound.second, *m_volume);
    }
}

void SoundManager::decreaseVolume() {
    *m_volume -= 8;
    setVolumeAll();
}

void SoundManager::increaseVolume() {
    *m_volume += 8;
    setVolumeAll();
}

void SoundManager::setVolume(const char* soundKey, int volume) {
    auto it = m_soundMap.find(soundKey);
    *m_volume = volume;
    if (it != m_soundMap.end()) {
        Mix_VolumeChunk(it->second, volume);
    }
}

Mix_Chunk* SoundManager::loadChunk(const char* filePath) {
    Mix_Chunk* chunk = Mix_LoadWAV(filePath);
    if (chunk == nullptr) {
        printf("Failed to load sound effect! SDL_mixer Error: %s\n", Mix_GetError());
    }
    return chunk;
}
