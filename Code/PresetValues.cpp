#include "PresetValues.h"

const unsigned int CHUNK_SIZE = 32;
const unsigned int WORLD_DEPTH = 4;
const unsigned int WORLD_HEIGHT = 320;

const unsigned int WATER_LEVEL = 50;

const unsigned int VIEW_DIST = 5;

const unsigned int TICK_RATE = 15;
const unsigned int FRAME_RATE = 60;

const float INITIAL_ZOOM = 25.0f;

#ifndef DEV_CONTROLS
const float MIN_ZOOM = 20.0f;
const float MAX_ZOOM = 20.0f;
#else
const float MIN_ZOOM = 1.0f;
const float MAX_ZOOM = 100.0f;
#endif // DEV_CONTROLS

const float MAX_SPEED = 2.0f;

const float MAX_WEIGHT = 100.0f;

const unsigned int DAY_LENGTH = 2400;

const unsigned int ENTITY_SPAWN_ROLLS = 50;
const unsigned int ENTITY_SPAWN_RANGE = 50;

#ifdef DEBUG
const std::string HOME_PATH = std::string("../../.."); // For debugging purposes, so we don't have to copy/paste and deal with weird directories in git.
#elif WINDOWS
const std::string HOME_PATH = std::string((std::string)getenv("APPDATA") + "/Exploratory Studios/Fear Of The Dark");
#elif LINUX
const std::string HOME_PATH = std::string((std::string)getenv("HOME") + "/.exploratory/Fear Of The Dark");
#else
const std::string HOME_PATH = std::string("UNDEFINED_HOME_ERROR");
#endif

const std::string SAVES_PATH = HOME_PATH + std::string("/Saves");
const std::string ASSETS_FOLDER_PATH = HOME_PATH + std::string("/Assets/");

const unsigned int FADE_TIME = 30 * 1000;

const float SAFE_FALL_DIST = 10;

const float MAX_DIST_HEARD = 30;

const unsigned int MAX_CHUNK_ENTITIES = 15;
const unsigned int SPAWN_RATE = 10;

const unsigned int MAX_TYPE_PARTICLES = 300;
const std::string UNDEFINED_ASSET_PATH = std::string(ASSETS_FOLDER_PATH + "Textures/UNDEFINED.png");

const unsigned int FLUID_PARTITION_SIZE = 32;
const float FLUID_CELL_SIZE = 0.5f;
