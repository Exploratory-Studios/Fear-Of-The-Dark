#pragma once

#include <string>
#include <vector>
#include <InputManager.h>

#include "PerlinNoise/PerlinNoise.h"

#include "Chunk.h"
#include "Player.h"
#include "Block.h"
#include "Blocks.h"

#include "PresetValues.h"
#include "Logging.h"

struct ItemData {
    unsigned int id;
    unsigned int quantity;
};

struct PlayerInventoryData {
    unsigned int items; // number of items to load
    std::vector<ItemData> itemData;
    float absMaxWeight;
};

struct PlayerData {
    bool canInteract;
    float m_sanity, m_health, m_thirst, m_hunger, m_exhaustion, m_stamina;
    glm::vec2 position;
    unsigned int favouriteItemIndices[10];
    PlayerInventoryData inventory;
};

struct TileData {
    glm::vec2 pos;
    unsigned int id;
    float ambientLight;
};

struct ChunkData {
    //std::vector<EntityData> entities;
    /// TODO: create entities the same way blocks and items are created (Blocks.h & Items.h)
    TileData tiles[WORLD_HEIGHT][CHUNK_SIZE];
};

struct World {
    Chunk* chunks[WORLD_SIZE] = { nullptr };
    Player player;
};

class WorldIOManager
{
    public:
        WorldIOManager() { }
        WorldIOManager(World* world, GLEngine::InputManager* input) : m_world(world), m_input(input) { }
        WorldIOManager(GLEngine::InputManager* input) : m_input(input) { }

        void loadWorld(std::string worldName, float* progress); /// TODO: Make multi-threaded so that we can view progress and load at the same time
        void saveWorld(World& world, std::string worldName, float* progress); /// TODO: Make multi-threaded so that we can view progress and save at the same time

        void createWorld(unsigned int seed, std::string worldName, bool isFlat = false);
        /*
            Seed will govern where the biomes are (random engine #1) and it will also seed the random engine for
            all of the world (random engine #2)
        */

        World* getWorld() { return m_world; } /// I'm using pointers so that the program can always
                                              /// access the location of the same World, and the same
                                              /// WorldIOManager. That way, I will never(!) get
                                              /// duplicates of worlds that don't change at the same time.
        ScriptQueue* getScriptQueue() { return m_sq; }

    private:
        std::string m_saveVersion = "1.0.0";

        Logger* logger = Logger::getInstance();

        World* m_world = new World();
        GLEngine::InputManager* m_input = nullptr;
        ScriptQueue* m_sq = new ScriptQueue();
};
