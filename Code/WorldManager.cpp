#include "WorldManager.h"

void WorldManager::loadWorld(std::string worldName) {

}

void WorldManager::saveWorld(const World& world, std::string worldName) {

}

<<<<<<< HEAD


#include <iostream>


=======
>>>>>>> 98b8385241da9920f404ce8b5264336ac8b85c16
void WorldManager::createWorld(unsigned int seed, std::string worldName) {

    // Set the real-world models of each chunk (randomly)
    PerlinNoise placeNoise(seed);
    for(int i = 0; i < WORLD_SIZE; i++) {

        float place = placeNoise.noise(i, 0.8, 0.3);

        place *= 2.0;
        place -= 0.5;

        if(place > 1.0f) place = 1.0f;
        if(place < 0.0f) place = 0.0f;

        m_world->chunks[i].setPlace((Categories::Places)std::ceil(place * Categories::TOTAL_PLACES));

    }

    std::vector<int> blockHeights;
    blockHeights.resize(WORLD_SIZE * CHUNK_SIZE);

    // Set the block heights in each chunk
    for(int i = 0; i < WORLD_SIZE; i++) {

        PerlinNoise heightNoise(seed * (i + 1) * 783);

        for(int j = 0; j < CHUNK_SIZE; j++) {
            float extra = heightNoise.noise(j * Category_Data::placeData[(int)m_world->chunks[i].getPlace()].flatness / CHUNK_SIZE, 8.5, 3.7);
            //extra += heightNoise.noise(i * j * i, i, 5.8);

            extra -= 0.5f;
            extra *= Category_Data::placeData[(int)m_world->chunks[i].getPlace()].maxHeightDiff;
            //extra /= Category_Data::placeData[(int)m_world->chunks[i].getPlace()].flatness;

<<<<<<< HEAD
            float height = std::floor(Category_Data::placeData[(int)m_world->chunks[i].getPlace()].baseHeight + extra);

            blockHeights[i * CHUNK_SIZE + j] = height;
        }
    }

    for(int i = 0; i < WORLD_SIZE; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {

            const float SMOOTHED_PORTION_D = 4;
            const float SMOOTHED_PORTION = (SMOOTHED_PORTION_D - 1.0f) / SMOOTHED_PORTION_D;

            if(i+1 < WORLD_SIZE && j > CHUNK_SIZE * SMOOTHED_PORTION) {
                float smoother = 0.0f;
                float a = blockHeights[i * CHUNK_SIZE + CHUNK_SIZE * SMOOTHED_PORTION];
                float b = blockHeights[(i + 1) * CHUNK_SIZE + 1];

                smoother = a - b;

                float multiplier = ((float)j - (float)CHUNK_SIZE * SMOOTHED_PORTION) / ((float)CHUNK_SIZE / SMOOTHED_PORTION_D);

                smoother *= -multiplier;
                blockHeights[i * CHUNK_SIZE + j] = smoother + blockHeights[i * CHUNK_SIZE + CHUNK_SIZE * SMOOTHED_PORTION];
            }
=======
            float smoother = 0;
            if(i+1 < WORLD_SIZE && j > CHUNK_SIZE +1 ) {

                float a = Category_Data::placeData[(int)m_world->chunks[i].getPlace()].baseHeight;
                float b = Category_Data::placeData[(int)m_world->chunks[i+1].getPlace()].baseHeight;
                smoother = (a + b) / 2;
                smoother *= j / CHUNK_SIZE;
            }

            extra += smoother;

            float height = std::floor(Category_Data::placeData[(int)m_world->chunks[i].getPlace()].baseHeight + extra);

            blockHeights[i * CHUNK_SIZE + j] = height;
>>>>>>> 98b8385241da9920f404ce8b5264336ac8b85c16
        }

        for(int x = 0; x < CHUNK_SIZE; x++) {
            for(int y = 0; y < blockHeights[i * CHUNK_SIZE + x]; y++) {
                m_world->chunks[i].tiles[y][x] = (Block(glm::vec2((x * TILE_SIZE + (i * CHUNK_SIZE * TILE_SIZE)), y * TILE_SIZE), (unsigned int)Categories::BlockIDs::DIRT));

            }
        }
    }

    /// Finished for now. Next, make sure to change screens from load to gameplay(!) and have the gameplay screen
    /// start a loop to display and update every block. Spatial partitioning (chunks) is a maybe tonight.
}
