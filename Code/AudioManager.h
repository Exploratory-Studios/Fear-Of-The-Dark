#pragma once

#include <AudioEngine.h>

#include <vector>
#include <string>

#include "PresetValues.h"

enum class SoundEffectIDs {
    WALK_DIRT,
    JUMP_DIRT,
    LAND_DIRT
};

#define SOUND_EFFECT_LENGTH 0 // How many sound effect files there are
#define MORNING_MUSIC_LENGTH 1 // How many music files there are for 'morning'
#define DAY_MUSIC_LENGTH 0 // How many music files there are for 'day'
#define NIGHT_MUSIC_LENGTH 0 // How many music files there are for 'night'

class AudioManager
{
    public:
        AudioManager();
        ~AudioManager();

        void init();

        bool isMusicPlaying();

        void playMorningSong(int id);
        void playDaySong(int id);
        void playNightSong(int id);
        void playSoundEffect(int id);

    private:
        GLEngine::AudioEngine* m_audio = new GLEngine::AudioEngine();

        bool m_playingMusic = false;

        std::vector<GLEngine::Music> m_music;
        std::vector<GLEngine::SoundEffect> m_soundEffects;
        std::string m_soundEffectFiles[SOUND_EFFECT_LENGTH];/* =
        {
            "WalkDirt.wav",
            "JumpDirt.wav",
            "LandDirt.wav"
        };*/

};
