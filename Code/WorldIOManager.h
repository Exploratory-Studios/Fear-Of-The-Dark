#pragma once

#include <string>
#include <vector>

#include "PerlinNoise/PerlinNoise.h"

#include "Chunk.h"
#include "Entity.h"
#include "Player.h"
#include "Block.h"

#include "PresetValues.h"

struct World {
    Chunk chunks[WORLD_SIZE] = { }; // Chunk will hold a vector of tiles and positions.
    std::vector<Entity> entities;
    Player player;
};

class WorldIOManager
{
    public:
        WorldIOManager() { }
        WorldIOManager(World* world) : m_world(world) { }

        void loadWorld(std::string worldName);
        void saveWorld(const World& world, std::string worldName, const std::vector<Entity>& entities);

        void createWorld(unsigned int seed, std::string worldName);
        /*
            Seed will govern where the biomes are (random engine #1) and it will also seed the random engine for
            all of the world (random engine #2)
        */

        World* getWorld() { return m_world; } /// I'm using pointers so that the program can always
                                              /// access the location of the same World, and the same
                                              /// WorldIOManager. That way, I will never(!) get
                                              /// duplicates of worlds that don't change at the same time.

    private:
        World* m_world = new World();
};
