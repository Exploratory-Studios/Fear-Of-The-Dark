#pragma once

#include <string>

class Options {
  public:
	static void loadFromFile(std::string fileName); // Uses ASSETS_FOLDER_PATH as well.
	static void saveToFile(std::string fileName);	// Uses ASSETS_FOLDER_PATH as well.

	/// Audio
	static float masterVolume;
	static float soundsVolume;
	static float musicVolume;
	/// Audio End
};
