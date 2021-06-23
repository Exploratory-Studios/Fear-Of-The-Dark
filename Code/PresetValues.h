#pragma once

#include <string>

/// World size variables
extern const unsigned int CHUNK_SIZE;
extern const unsigned int WORLD_DEPTH;
extern const unsigned int WORLD_HEIGHT;

// How high does water spawn naturally?
extern const unsigned int WATER_LEVEL;

// How many chunks can you see? (Odd) (TOTAL CHUNKS DRAWN AT ONCE)
extern const unsigned int VIEW_DIST;

// How many ticks per second?
extern const unsigned int TICK_RATE;
// How many frames per second?
extern const unsigned int FRAME_RATE;

// initial camera scale
extern const float INITIAL_ZOOM;

// Min/max camera scales
extern const float MIN_ZOOM;
extern const float MAX_ZOOM;

// How fast is TOO fast? (2.0 tiles/update, 120.0 tiles/sec.)
extern const float MAX_SPEED;

// How much can the player carry MAX
extern const float MAX_WEIGHT;

// How long a day is (frames at the moment)
extern const unsigned int DAY_LENGTH;

// How many entities are we going to attempt to spawn per tick?
extern const unsigned int ENTITY_SPAWN_ROLLS;
// How far away from the player should entities spawn?
extern const unsigned int ENTITY_SPAWN_RANGE;

extern const std::string HOME_PATH;

extern const std::string SAVES_PATH;
extern const std::string ASSETS_FOLDER_PATH;

// How long it takes (in ms) to fade in and out music
extern const unsigned int FADE_TIME;

//How far we can fall (in tiles) until we take damage (base)
extern const float SAFE_FALL_DIST;

// How far away can you hear the loudest sound? (Tiles, using pythagorean theorem for dist)
extern const float MAX_DIST_HEARD;

// How many entities can there be within a single chunk?
extern const unsigned int MAX_CHUNK_ENTITIES;

// How often should entities spawn in each chunk? x/1000 ticks
extern const unsigned int SPAWN_RATE;

// Max lights is an array size, therefore #define.
#define MAX_LIGHTS_RENDERED 30
extern const unsigned int MAX_TYPE_PARTICLES; // Max # of particles in each batch
extern const std::string UNDEFINED_ASSET_PATH;

extern const unsigned int FLUID_PARTITION_SIZE;
