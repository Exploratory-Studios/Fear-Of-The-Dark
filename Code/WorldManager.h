#pragma once

#define WORLD_SIZE 8
#define WORLD_SIZE 10

#include <string>
#include <vector>

#include "Chunk.h"
#include "Entity.h"
#include "PerlinNoise/PerlinNoise.h"
#include "Block.h"
//#include "Player.h"

struct World {
    Chunk chunks[WORLD_SIZE] = { }; // Chunk will hold a vector of tiles and positions.
    //std::vector<Entity> entities; // For saving purposes only. May add to saveWorld()'s parameters
    //Player player;
};

class WorldManager
{
    public:
        WorldManager() { }
        WorldManager(World* world) : m_world(world) { }

        void loadWorld(std::string worldName);
        void saveWorld(const World& world, std::string worldName);

        void createWorld(unsigned int seed, std::string worldName);
        /*
            Seed will govern where the biomes are (random engine #1) and it will also seed the random engine for
            all of the world (random engine #2)
        */

        World* getWorld() { return m_world; } /// I'm using pointers so that the program can always
                                               /// access the location of the same World, and the same
                                               /// WorldManager. That way, I will never(!) get
                                               /// duplicates of worlds that don't change at the same time.

    private:
        World* m_world = new World;
};
