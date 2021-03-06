#include "AudioManager.h"

#include <AudioEngine.h>

AudioManager::AudioManager() {
	m_audio = new GLEngine::AudioEngine();
}

AudioManager::~AudioManager() {
	m_audio->destroy();
}

void AudioManager::init() {
	if(!m_audio->hasBeenInited()) {
		m_audio->init();

		for(int i = 0; i < MORNING_MUSIC_LENGTH; i++) {
			m_music.push_back(m_audio->loadMusic(ASSETS_FOLDER_PATH + "Music/MorningMus" + std::to_string(i) + ".mus"));
		}
		for(int i = 0; i < DAY_MUSIC_LENGTH; i++) {
			m_music.push_back(m_audio->loadMusic(ASSETS_FOLDER_PATH + "Music/DayMus" + std::to_string(i) + ".mus"));
		}
		for(int i = 0; i < AFTERNOON_MUSIC_LENGTH; i++) {
			m_music.push_back(
				m_audio->loadMusic(ASSETS_FOLDER_PATH + "Music/AfternoonMus" + std::to_string(i) + ".mus"));
		}
		for(int i = 0; i < NIGHT_MUSIC_LENGTH; i++) {
			m_music.push_back(m_audio->loadMusic(ASSETS_FOLDER_PATH + "Music/NightMus" + std::to_string(i) + ".mus"));
		}
		for(int i = 0; i < NIGHTMARE_MUSIC_LENGTH; i++) {
			m_music.push_back(
				m_audio->loadMusic(ASSETS_FOLDER_PATH + "Music/NightmareMus" + std::to_string(i) + ".mus"));
		} /// TODO: Interpret a file that tells what each file's music is
		/* Ex: Music.txt
		Music1_ThisIsAName:MORNING
		Music2_ThisIsACreativeName:AFTERNOON
		.
		.
		.
		*/
		logger->log(
			"Loaded all " +
			std::to_string(MORNING_MUSIC_LENGTH + DAY_MUSIC_LENGTH + AFTERNOON_MUSIC_LENGTH + NIGHT_MUSIC_LENGTH) +
			" music files.");

		for(int i = 0; i < SOUND_EFFECT_LENGTH; i++) {
			m_soundEffects.push_back(m_audio->loadSoundEffect(ASSETS_FOLDER_PATH + "Sounds/" + m_soundEffectFiles[i]));
		}
		logger->log("Loaded all " + std::to_string(SOUND_EFFECT_LENGTH) + " sound files.");

	} else {
		logger->log("Already inited audio manager. Skipping...");
	}
}

bool AudioManager::isMusicPlaying() {
	for(unsigned int i = 0; i < m_music.size(); i++) {
		if(m_music[i].isPlaying())
			return true;
	}
	return false;
}

float AudioManager::setMusicVolume(float vol) {
	m_musicVolume = vol;
}

float AudioManager::setSoundsVolume(float vol) {
	m_soundsVolume = vol;
}

void AudioManager::playMorningSong(int id) {
	m_music[id].play();
	m_music[id].setVolume(m_musicVolume * 128);
	m_playingMusic = true;
}

void AudioManager::playDaySong(int id) {
	m_music[id + MORNING_MUSIC_LENGTH].play();
	m_music[id + MORNING_MUSIC_LENGTH].setVolume(m_musicVolume * 128);
	m_playingMusic = true;
}

void AudioManager::playAfternoonSong(int id) {
	m_music[id + MORNING_MUSIC_LENGTH + DAY_MUSIC_LENGTH].play();
	m_music[id + MORNING_MUSIC_LENGTH + DAY_MUSIC_LENGTH].setVolume(m_musicVolume * 128);
	m_playingMusic = true;
}

void AudioManager::playNightSong(int id) {
	m_music[id + MORNING_MUSIC_LENGTH + DAY_MUSIC_LENGTH + AFTERNOON_MUSIC_LENGTH].play();
	m_music[id + MORNING_MUSIC_LENGTH + DAY_MUSIC_LENGTH + AFTERNOON_MUSIC_LENGTH].setVolume(m_musicVolume * 128);
	m_playingMusic = true;
}

void AudioManager::playNightmareSong(int id) {
	m_music[id + MORNING_MUSIC_LENGTH + DAY_MUSIC_LENGTH + AFTERNOON_MUSIC_LENGTH + NIGHT_MUSIC_LENGTH].play();
	m_music[id + MORNING_MUSIC_LENGTH + DAY_MUSIC_LENGTH + AFTERNOON_MUSIC_LENGTH].setVolume(m_musicVolume * 128);
	m_playingMusic = true;
}

void AudioManager::playSoundEffect(int id, unsigned int volume) {
	m_soundEffects[id].play();
	m_soundEffects[id].setVolume(volume * m_soundsVolume);
}

void AudioManager::updateSoundEffect(int id, unsigned int volume) {
	m_soundEffects[id].setVolume(volume * m_soundsVolume);
}
