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

struct World {
    Chunk* chunks[WORLD_SIZE] = { nullptr }; // Chunk will hold a vector of tiles and positions.
    std::vector<Entity> entities;
    std::vector<TalkingNPC> talkingEntities;
    Player player;
};

class WorldIOManager
{
    public:
        WorldIOManager() { }
        WorldIOManager(World* world, GLEngine::InputManager* input) : m_world(world), m_input(input) { }
        WorldIOManager(GLEngine::InputManager* input) : m_input(input) { }

        void loadWorld(std::string worldName);
        void saveWorld(const World& world, std::string worldName, const std::vector<Entity>& entities);

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
        World* m_world = new World();
        GLEngine::InputManager* m_input = nullptr;
        ScriptQueue* m_sq = new ScriptQueue();
};
