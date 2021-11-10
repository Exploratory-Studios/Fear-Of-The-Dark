#pragma once

#include <string>
#include <vector>
#include <boost/thread.hpp>
#include <stdarg.h>

#include <Window.h>

#include "PerlinNoise/PerlinNoise.h"

#include "PresetValues.h"
#include "Logging.h"

#include "SaveDataTypes.h"

class World;

class WorldIOManager {
  public:
	WorldIOManager() {
		initLoadSaveStuff();
	}

	void initLoadSaveStuff();

	~WorldIOManager() {
		delete m_progress;
		delete m_saveLoadMessage;
	}

	void loadWorld(
		std::string worldName); // These public versions of the functions are multi-threading versions for anybody to use
	void saveWorld();
	void createWorld(unsigned int seed, std::string worldName, bool isFlat, unsigned int width);

	void setWorldEra(unsigned int newEraID);

	//StructureData loadStructureFromFile(std::string& filepath);
	//void placeStructure(World* world, StructureData& structure, glm::vec2 position); /// TODO: Make structures "individuals" so that they don't need the WorldIOManager to function.

#ifdef DEV_CONTROLS
	//void saveStructureToFile(World* world, std::string& filepath, StructureData& structureData);/// TODO: Make structures "individuals" so that they don't need the WorldIOManager to function.
	//void saveStructureToFile(World* world, std::string& filepath, glm::vec4 destRect);
#endif // DEV_CONTROLS

	float getProgress() const {
		return *m_progress;
	}
	void setProgress(float p) {
		mtx_prog.lock();
		*m_progress = p;
		mtx_prog.unlock();
	}
	std::string getMessage() const {
		return *m_saveLoadMessage;
	}
	void setMessage(std::string msg) {
		mtx_msg.lock();
		*m_saveLoadMessage = msg;
		mtx_msg.unlock();
	}

  private:
	void P_loadWorld(std::string worldName,
					 World* world); /// TODO: Make multi-threaded so that we can view progress and load at the same time
	void P_saveWorld(World* world); /// TODO: Make multi-threaded so that we can view progress and save at the same time
	void P_createWorld(unsigned int seed, std::string worldName, bool isFlat, unsigned int width);
	/*
		    Seed will govern where the biomes are (random engine #1) and it will also seed the random engine for
		    all of the world (random engine #2)
		*/

	unsigned int m_saveVersion = 1;

	Logger* logger = nullptr;

	float*		 m_progress		   = nullptr;
	std::string* m_saveLoadMessage = nullptr;

	boost::mutex mtx_msg;
	boost::mutex mtx_prog;
};
