#include "GameAudioManager.h"

#include <AudioManager.hpp>
#include <ResourceManager.hpp>

GameAudioManager::GameAudioManager() {
	m_audio = BARE2D::AudioManager::getInstance();
}

GameAudioManager::~GameAudioManager() {
}

void GameAudioManager::init() {
	m_audio->init();

	for(int i = 0; i < MORNING_MUSIC_LENGTH; i++) {
		std::string path = ASSETS_FOLDER_PATH + "Music/MorningMus" + std::to_string(i) + ".mus";
		m_music.push_back(BARE2D::ResourceManager::loadMusic(path));
	}
	for(int i = 0; i < DAY_MUSIC_LENGTH; i++) {
		std::string path = ASSETS_FOLDER_PATH + "Music/DayMus" + std::to_string(i) + ".mus";
		m_music.push_back(BARE2D::ResourceManager::loadMusic(path));
	}
	for(int i = 0; i < AFTERNOON_MUSIC_LENGTH; i++) {
		std::string path = ASSETS_FOLDER_PATH + "Music/AfternoonMus" + std::to_string(i) + ".mus";
		m_music.push_back(BARE2D::ResourceManager::loadMusic(path));
	}
	for(int i = 0; i < NIGHT_MUSIC_LENGTH; i++) {
		std::string path = ASSETS_FOLDER_PATH + "Music/NightMus" + std::to_string(i) + ".mus";
		m_music.push_back(BARE2D::ResourceManager::loadMusic(path));
	}
	for(int i = 0; i < NIGHTMARE_MUSIC_LENGTH; i++) {
		std::string path = ASSETS_FOLDER_PATH + "Music/NightmareMus" + std::to_string(i) + ".mus";
		m_music.push_back(BARE2D::ResourceManager::loadMusic(path));
	} /// TODO: Interpret a file that tells what each file's music is
	/* Ex: Music.txt
	Music1_ThisIsAName:MORNING
	Music2_ThisIsACreativeName:AFTERNOON
	.
	.
	.
	*/
	BARE2D::Logger::getInstance()->log(
		"Loaded all " +
		std::to_string(MORNING_MUSIC_LENGTH + DAY_MUSIC_LENGTH + AFTERNOON_MUSIC_LENGTH + NIGHT_MUSIC_LENGTH) +
		" music files.");

	for(int i = 0; i < SOUND_EFFECT_LENGTH; i++) {
		std::string path = ASSETS_FOLDER_PATH + "Sounds/" + m_soundEffectFiles[i];
		BARE2D::ResourceManager::loadSound(path);
	}
	BARE2D::Logger::getInstance()->log("Loaded all " + std::to_string(SOUND_EFFECT_LENGTH) + " sound files.");
}

void GameAudioManager::playMorningSong(int id) {
	m_audio->playMusic(m_music[id]);
}

void GameAudioManager::playDaySong(int id) {
	m_audio->playMusic(m_music[id + MORNING_MUSIC_LENGTH]);
}

void GameAudioManager::playAfternoonSong(int id) {
	m_audio->playMusic(m_music[id + MORNING_MUSIC_LENGTH + DAY_MUSIC_LENGTH]);
}

void GameAudioManager::playNightSong(int id) {
	m_audio->playMusic(m_music[id + MORNING_MUSIC_LENGTH + DAY_MUSIC_LENGTH + AFTERNOON_MUSIC_LENGTH]);
}

void GameAudioManager::playNightmareSong(int id) {
	m_audio->playMusic(
		m_music[id + MORNING_MUSIC_LENGTH + DAY_MUSIC_LENGTH + AFTERNOON_MUSIC_LENGTH + NIGHT_MUSIC_LENGTH]);
}
