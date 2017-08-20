#include "WorldIOManager.h"

void WorldIOManager::loadWorld(std::string worldName) {

}

void WorldIOManager::saveWorld(const World& world, std::string worldName, const std::vector<Entity>& entities) {

}

void WorldIOManager::createWorld(unsigned int seed, std::string worldName) {

    // Set the real-world models of each chunk (randomly)
    PerlinNoise placeNoise(seed);
    for(int i = 0; i < WORLD_SIZE; i++) {

        float place = placeNoise.noise(i, 0.8, 0.3);

        place *= 2.0;
        place -= 0.5;

        if(place > 1.0f) place = 1.0f;
        if(place < 0.0f) place = 0.0f;

        m_world->chunks[i].setPlace((Categories::Places)std::ceil(place * Category_Data::TOTAL_PLACES));

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

            float height = std::floor(Category_Data::placeData[(int)m_world->chunks[i].getPlace()].baseHeight + extra);

            blockHeights[i * CHUNK_SIZE + j] = height;
        }
    }

    {
        const float SMOOTHED_PORTION_D = 3; // 2/3
        const float SMOOTHED_PORTION = (SMOOTHED_PORTION_D - 1.0f) / SMOOTHED_PORTION_D;

        for(int i = 0; i < WORLD_SIZE; i++) {

            PerlinNoise smootherExtraNoise(seed * (i + 1) * 539);

            for(int j = 0; j < CHUNK_SIZE; j++) {

                if(i+1 < WORLD_SIZE && j > CHUNK_SIZE * SMOOTHED_PORTION) {
                    float smoother = 0.0f;
                    float a = blockHeights[i * CHUNK_SIZE + CHUNK_SIZE * SMOOTHED_PORTION];
                    float b = blockHeights[(i + 1) * CHUNK_SIZE + 1];

                    smoother = a - b;

                    float multiplier = ((float)j - (float)CHUNK_SIZE * SMOOTHED_PORTION) / ((float)CHUNK_SIZE / SMOOTHED_PORTION_D);

                    smoother *= -multiplier;
                    blockHeights[i * CHUNK_SIZE + j] = smoother + blockHeights[i * CHUNK_SIZE + CHUNK_SIZE * SMOOTHED_PORTION] + smootherExtraNoise.noise(i / 10, 10, 84) * 7 - 3.5;

                }
            }

            for(int x = 0; x < CHUNK_SIZE; x++) {
                for(int y = 0; y < blockHeights[i * CHUNK_SIZE + x]; y++) {
                    m_world->chunks[i].tiles[y][x] = (Block(glm::vec2((x + (i * CHUNK_SIZE)), y), (unsigned int)Categories::BlockIDs::DIRT));

                }
            }

            Player player(glm::vec2(10 * TILE_SIZE, 10 * TILE_SIZE), Categories::Entity_Type::MOB, 0); //////////////
        }
    }

    /// Finished for now. Next, make sure to change screens from load to gameplay(!) and have the gameplay screen
    /// start a loop to display and update every block. Spatial partitioning (chunks) is a maybe tonight.
}
