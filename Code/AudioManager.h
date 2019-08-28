#pragma once

#include <AudioEngine.h>

#include <vector>
#include <string>

#include "PresetValues.h"
#include "Logging.h"

enum class SoundEffectIDs {
    WALK_DIRT, // = 0
    HUSK_SOUND // = 1
};

enum class ParticleIDs {
    DIRT_PARTICLE,
    WATER_PARTICLE,
    FIRE_PARTICLE
};

#define SOUND_EFFECT_LENGTH 2 // How many sound effect files there are
#define MORNING_MUSIC_LENGTH 1 // How many music files there are for 'morning'
#define DAY_MUSIC_LENGTH 0 // How many music files there are for 'day'
#define AFTERNOON_MUSIC_LENGTH 2 // How many music files there are for 'afternoon'
#define NIGHT_MUSIC_LENGTH 0 // How many music files there are for 'night'
#define NIGHTMARE_MUSIC_LENGTH 0 // How many music files there are for 'nightmare hour'

class AudioManager
{
    public:
        AudioManager();
        ~AudioManager();

        void init();

        bool isMusicPlaying();

        void playMorningSong(int id);
        void playDaySong(int id);
        void playAfternoonSong(int id);
        void playNightSong(int id);
        void playNightmareSong(int id);
        void playSoundEffect(int id, unsigned int volume);

        void updateSoundEffect(int id, unsigned int volume);

    private:
        Logger* logger = Logger::getInstance();

        GLEngine::AudioEngine* m_audio = new GLEngine::AudioEngine();

        bool m_playingMusic = false;

        std::vector<GLEngine::Music> m_music;
        std::vector<GLEngine::SoundEffect> m_soundEffects;
        std::string m_soundEffectFiles[SOUND_EFFECT_LENGTH] =
        {
            "WalkDirt.wav",
            "HuskNoise.wav"
        };

};
