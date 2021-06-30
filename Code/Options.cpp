#include "Options.h"

#include "PresetValues.h"

#include <fstream>
#include <Logging.h>
#include <GLEngineErrors.h>

float Options::masterVolume = 1.0f;
float Options::musicVolume	= 1.0f;
float Options::soundsVolume = 1.0f;

void Options::loadFromFile(std::string fileName) {
	std::ifstream file(ASSETS_FOLDER_PATH + fileName, std::ios::binary);

	if(file.fail()) {
		Logger::getInstance()->log("Failed to load options from file: " + ASSETS_FOLDER_PATH + fileName +
								   ", creating file.");
		saveToFile(fileName);
		return;
	}

	// Save volumes
	file.read(reinterpret_cast<char*>(&masterVolume), sizeof(float));
	file.read(reinterpret_cast<char*>(&musicVolume), sizeof(float));
	file.read(reinterpret_cast<char*>(&soundsVolume), sizeof(float));
	// Volume done!
}

void Options::saveToFile(std::string fileName) {
	std::ofstream file(ASSETS_FOLDER_PATH + fileName, std::ios::binary);

	if(file.fail()) {
		GLEngine::fatalError("Failed to save options to file: " + ASSETS_FOLDER_PATH + fileName);
		return;
	}

	// Save volumes
	file.write(reinterpret_cast<char*>(&masterVolume), sizeof(float));
	file.write(reinterpret_cast<char*>(&musicVolume), sizeof(float));
	file.write(reinterpret_cast<char*>(&soundsVolume), sizeof(float));
	// Volume done!
}
