#include "WorldIOManager.h"

void WorldIOManager::loadWorld(std::string worldName) {

}

void WorldIOManager::saveWorld(const World& world, std::string worldName, const std::vector<Entity>& entities) {

}

void WorldIOManager::createWorld(unsigned int seed, std::string worldName, bool isFlat) {

    for(int i = 0; i < WORLD_SIZE; i++) {
        m_world->chunks[i] = new Chunk();
    }

    // Set the real-world models of each chunk (randomly)
    PerlinNoise placeNoise(seed);
    for(int i = 0; i < WORLD_SIZE; i++) {

        float place = placeNoise.noise(i, 0.8, 0.3);

        place *= 2.0;
        place -= 0.5;

        if(place > 1.0f) place = 1.0f;
        if(place < 0.0f) place = 0.0f;

        m_world->chunks[i]->setPlace((Categories::Places)std::ceil(place * Category_Data::TOTAL_PLACES));
        m_world->chunks[i]->setIndex(i);

    }

    std::vector<int> blockHeights;
    blockHeights.resize(WORLD_SIZE * CHUNK_SIZE);

    if(!isFlat) {
        // Set the block heights in each chunk
        for(int i = 0; i < WORLD_SIZE; i++) {

            PerlinNoise heightNoise(seed * (i + 1) * 783);

            for(int j = 0; j < CHUNK_SIZE; j++) {
                float extra = heightNoise.noise(j * Category_Data::placeData[(int)m_world->chunks[i]->getPlace()].flatness / CHUNK_SIZE, 8.5, 3.7);
                //extra += heightNoise.noise(i * j * i, i, 5.8);

                extra -= 0.5f;
                extra *= Category_Data::placeData[(int)m_world->chunks[i]->getPlace()].maxHeightDiff;
                //extra /= Category_Data::placeData[(int)m_world->chunks[i]->getPlace()].flatness;

                float height = std::floor(Category_Data::placeData[(int)m_world->chunks[i]->getPlace()].baseHeight + extra);

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
                    for(int y = blockHeights[i * CHUNK_SIZE + x]; y < WORLD_HEIGHT; y++) {
                        Block* block = new Block(glm::vec2((i * CHUNK_SIZE) + x, y), (unsigned int)Categories::BlockIDs::AIR, m_world->chunks[i]);
                        m_world->chunks[i]->tiles[y][x] = block;
                    }
                    for(int y = 0; y < blockHeights[i * CHUNK_SIZE + x]; y++) {
                        if(y != blockHeights[i * CHUNK_SIZE + x] - 1) {
                            Block* block = new Block(glm::vec2((i * CHUNK_SIZE) + x, y), (unsigned int)Categories::BlockIDs::DIRT, m_world->chunks[i]);
                            m_world->chunks[i]->tiles[y][x] = block;
                        } else {
                            Block* block = new Block(glm::vec2((i * CHUNK_SIZE) + x, y), (unsigned int)Categories::BlockIDs::GRASS, m_world->chunks[i]);
                            m_world->chunks[i]->tiles[y][x] = block;
                        }
                    }
                }
            }
        }
    } else {
        for(int k = 0; k < WORLD_SIZE; k++) {
            for(int i = 0; i < CHUNK_SIZE; i++) {
                blockHeights[k * CHUNK_SIZE + i] = 10;
                for(int j = blockHeights[k * CHUNK_SIZE + i]; j < WORLD_HEIGHT; j++) {
                    Block* block = new Block(glm::vec2((k * CHUNK_SIZE) + i, j), (unsigned int)Categories::BlockIDs::AIR, m_world->chunks[k]);
                    m_world->chunks[k]->tiles[j][i] = block;
                }
                for(int j = 0; j < blockHeights[k * CHUNK_SIZE + i]; j++) {
                    if(j != blockHeights[k * CHUNK_SIZE + i] - 1) {
                        Block* block = new Block(glm::vec2((k * CHUNK_SIZE) + i, j), (unsigned int)Categories::BlockIDs::DIRT, m_world->chunks[k]);
                        m_world->chunks[k]->tiles[j][i] = block;
                    } else {
                        Block* block = new Block(glm::vec2((k * CHUNK_SIZE) + i, j), (unsigned int)Categories::BlockIDs::GRASS, m_world->chunks[k]);
                        m_world->chunks[k]->tiles[j][i] = block;
                    }
                }
            }
        }
    }

    Player player(glm::vec2(5.0f * TILE_SIZE, (blockHeights[5] + 5) * TILE_SIZE), m_input);
    m_world->player = player;

    //Entity ent(glm::vec2(10.5 * TILE_SIZE, 24 * TILE_SIZE), Categories::Entity_Type::MOB, 0);
    //m_world->entities.push_back(ent);
}
