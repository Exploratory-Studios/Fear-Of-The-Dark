#include "AudioManager.h"

#include <AudioEngine.h>
#include <iostream>

AudioManager::AudioManager() {

}

AudioManager::~AudioManager() {
    m_audio->destroy();
}

void AudioManager::init() {
    m_audio->init();

    for(int i = 0; i < MORNING_MUSIC_LENGTH; i++) {
        m_music.push_back(m_audio->loadMusic(ASSETS_FOLDER_PATH + "Music/MorningMus" + std::to_string(i) + ".mus"));
    }
    for(int i = 0; i < DAY_MUSIC_LENGTH; i++) {
        m_music.push_back(m_audio->loadMusic(ASSETS_FOLDER_PATH + "Music/DayMus" + std::to_string(i) + ".mus"));
    }
    for(int i = 0; i < NIGHT_MUSIC_LENGTH; i++) {
        m_music.push_back(m_audio->loadMusic(ASSETS_FOLDER_PATH + "Music/NightMus" + std::to_string(i) + ".mus"));
    }
    std::cout << "Loaded all " << MORNING_MUSIC_LENGTH + DAY_MUSIC_LENGTH + NIGHT_MUSIC_LENGTH << " music files.\n";

    for(int i = 0; i < SOUND_EFFECT_LENGTH; i++) {
        m_soundEffects.push_back(m_audio->loadSoundEffect(ASSETS_FOLDER_PATH + "Sounds/" + m_soundEffectFiles[i]));
    }
    std::cout << "Loaded all " << SOUND_EFFECT_LENGTH << " sound files.\n";
}

bool AudioManager::isMusicPlaying() {
    for(int i = 0; i < m_music.size(); i++) {
        if(m_music[i].isPlaying()) return true;
    }
    return false;
}

void AudioManager::playMorningSong(int id) {
    m_music[id].fadeIn(FADE_TIME);
    m_playingMusic = true;
}

void AudioManager::playDaySong(int id) {
    m_music[id + MORNING_MUSIC_LENGTH].fadeIn(FADE_TIME);
    m_playingMusic = true;
}

void AudioManager::playNightSong(int id) {
    m_music[id + MORNING_MUSIC_LENGTH + DAY_MUSIC_LENGTH].fadeIn(FADE_TIME);
    m_playingMusic = true;
}

void AudioManager::playSoundEffect(int id) {
    m_soundEffects[id].play();
}