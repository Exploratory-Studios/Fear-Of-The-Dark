#include "WorldIOManager.h"

#include <time.h>
#include <random>

#include <iostream>

void WorldIOManager::loadWorld(std::string worldName) {
    std::ifstream file("test.bin", std::ios::binary);
    if(file.fail()) {
        GLEngine::fatalError("Error loading from file: " + worldName + ".bin");
    }

    file.read(reinterpret_cast<char*>(&m_world->player), sizeof(Player));
    ///m_world->player.m_inventory = nullptr;
    ///std::cout << "\nLoaded Player POD";

    Inventory newInventory;
    file.read(reinterpret_cast<char*>(&newInventory), sizeof(float) * 2);
    ///m_world->player.m_inventory = &newInventory;
    ///std::cout << "\nLoaded Player Inventory";

    unsigned int items = -1;
    file.read(reinterpret_cast<char*>(&items), sizeof(unsigned int));

    for(int i = 0; i < items; i++) {
        Item* loadedItem = new Item();
        file.read(reinterpret_cast<char*>(loadedItem), sizeof(Item));
        m_world->player.m_inventory->getItems().push_back(loadedItem);
        std::cout << "\nLoaded Player Item " << std::to_string(i+1) << " of " << std::to_string(items);
    }

    for(int i = 0; i < WORLD_SIZE; i++) {
        for(int j = 0; j < WORLD_HEIGHT; j++) {
            for(int k = 0; k < CHUNK_SIZE; k++) { /// Fuck it. Store the tiles as ids, positions, and metadata and recover the other data like textures.
                Tile newTile = Tile();

                unsigned int texturePathSize;
                std::string texturePath;
                file.read(reinterpret_cast<char*>(&texturePathSize), sizeof(texturePathSize));

                texturePath.resize(texturePathSize);
                file.read(&texturePath[0], texturePathSize);
                file.read(reinterpret_cast<char*>(&newTile), sizeof(Tile));

                GLEngine::GLTexture blank;
                blank.height = newTile.m_texture.height;
                blank.width = newTile.m_texture.width;
                blank.id = newTile.m_texture.id;
                newTile.m_texture = blank;

                newTile.m_texture.filePath = texturePath;
            }
        }
        for(int j = 0; j < m_world->chunks[i]->getEntities()->size(); j++) {
            //file.read(reinterpret_cast<char*>(m_world->chunks[i]->getEntities()[j]), sizeof(Entity));
        }
        for(int j = 0; j < m_world->chunks[i]->getTalkingEntities()->size(); j++) {
            //file.read(reinterpret_cast<char*>(m_world->chunks[i]->getTalkingEntities()[j]), sizeof(TalkingNPC));
        }
    }
    std::cout << "\nLoaded World";

    file.close();
}

/*
SAVING ORDER:
Player POD (x, y, stats, etc.)
Player Inventory
    Inventory POD
    Inventory Items -> need to init properly on load
Chunks
    Tiles -> need to init properly on load
    Entities -> need to init properly on load
        Talking -> need to init properly on load
        Otherwise -> need to init properly on load
*/

void WorldIOManager::saveWorld(World& world, std::string worldName) {
    std::ofstream file("test.bin", std::ios::binary);
    if(file.fail()) {
        GLEngine::fatalError("Error saving to file: " + worldName + ".bin");
    }

    file.write(reinterpret_cast<char*>(&world.player), sizeof(Player));
    std::cout << "\nSaving " << "Player POD";

    file.write(reinterpret_cast<char*>(world.player.m_inventory), sizeof(float) * 2);
    std::cout << "\nSaving " << "Player Inventory";

    unsigned int items = world.player.m_inventory->getItems().size();
    file.write(reinterpret_cast<char*>(&items), sizeof(unsigned int));

    for(int i = 0; i < items; i++) {
        file.write(reinterpret_cast<char*>(&world.player.m_inventory->m_items[i]), sizeof(Item));
        std::cout << "\nSaving Player Item " << std::to_string(i+1) << " of " << std::to_string(items);
    }

    for(int i = 0; i < WORLD_SIZE; i++) {
        for(int j = 0; j < WORLD_HEIGHT; j++) {
            for(int k = 0; k < CHUNK_SIZE; k++) {
                unsigned int len = world.chunks[i]->tiles[k][j]->m_texture.filePath.size();
                file.write(reinterpret_cast<char*>(&len), sizeof(unsigned int)); //write the size of the texture string so we can read it later
                file.write(&world.chunks[i]->tiles[k][j]->m_texture.filePath[0], len);
                world.chunks[i]->tiles[k][j]->m_parentChunk = nullptr;
                world.chunks[i]->tiles[k][j]->m_texture.filePath = "";
                file.write(reinterpret_cast<char*>(world.chunks[i]->tiles[k][j]), sizeof(Tile));
            }
            for(int k = 0; k < 2; k++) {
                unsigned int len = world.chunks[i]->extraTiles[k][j]->m_texture.filePath.size();
                file.write(reinterpret_cast<char*>(&len), sizeof(unsigned int)); //write the size of the texture string so we can read it later
                file.write(reinterpret_cast<char*>(world.chunks[i]->extraTiles[k][j]), sizeof(Tile));
            }
        }
        for(int j = 0; j < world.chunks[i]->getEntities()->size(); j++) {
            file.write(reinterpret_cast<char*>(&(*world.chunks[i]->getEntities())[j]), sizeof(Entity));
        }
        for(int j = 0; j < world.chunks[i]->getTalkingEntities()->size(); j++) {
            file.write(reinterpret_cast<char*>(&(*world.chunks[i]->getTalkingEntities())[j]), sizeof(TalkingNPC));
        }
    }

    file.close();
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

            for(int i = 0; i < WORLD_SIZE+1; i++) {

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

                    } else if(i+1 == WORLD_SIZE && j > CHUNK_SIZE * SMOOTHED_PORTION) {
                        float smoother = 0.0f;
                        float a = blockHeights[i * CHUNK_SIZE + CHUNK_SIZE * SMOOTHED_PORTION];
                        float b = blockHeights[0];

                        smoother = a - b;

                        float multiplier = ((float)j - (float)CHUNK_SIZE * SMOOTHED_PORTION) / ((float)CHUNK_SIZE / SMOOTHED_PORTION_D);

                        smoother *= -multiplier;
                        blockHeights[i * CHUNK_SIZE + j] = smoother + blockHeights[i * CHUNK_SIZE + CHUNK_SIZE * SMOOTHED_PORTION] + smootherExtraNoise.noise(i / 10, 10, 84) * 7 - 3.5;

                    }
                }

                if(i < WORLD_SIZE) {
                    for(int x = 0; x < CHUNK_SIZE; x++) {
                        for(int y = blockHeights[i * CHUNK_SIZE + x]; y < WORLD_HEIGHT; y++) {
                            BlockAir* block = new BlockAir(glm::vec2((i * CHUNK_SIZE) + x, y), m_world->chunks[i]);
                            m_world->chunks[i]->tiles[y][x] = block;
                        }
                        for(int y = 0; y < blockHeights[i * CHUNK_SIZE + x]; y++) {
                            if(y != blockHeights[i * CHUNK_SIZE + x] - 1) {
                                BlockDirt* block = new BlockDirt(glm::vec2((i * CHUNK_SIZE) + x, y), m_world->chunks[i]);
                                m_world->chunks[i]->tiles[y][x] = block;
                            } else {
                                BlockGrass* block = new BlockGrass(glm::vec2((i * CHUNK_SIZE) + x, y), m_world->chunks[i]);
                                m_world->chunks[i]->tiles[y][x] = block;
                                int r = std::rand();
                                if(r % 2 == 0) {
                                    BlockBush* flower = new BlockBush(glm::vec2((i * CHUNK_SIZE) + x, y + 1), m_world->chunks[i]);
                                    m_world->chunks[i]->tiles[y+1][x] = flower;
                                }
                            }
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
                    BlockAir* block = new BlockAir(glm::vec2((k * CHUNK_SIZE) + i, j), m_world->chunks[k]);
                    m_world->chunks[k]->tiles[j][i] = block;
                }
                for(int j = 0; j < blockHeights[k * CHUNK_SIZE + i]; j++) {
                    if(j != blockHeights[k * CHUNK_SIZE + i] - 1) {
                        BlockDirt* block = new BlockDirt(glm::vec2((k * CHUNK_SIZE) + i, j), m_world->chunks[k]);
                        m_world->chunks[k]->tiles[j][i] = block;
                    } else {
                        BlockGrass* block = new BlockGrass(glm::vec2((k * CHUNK_SIZE) + i, j), m_world->chunks[k]);
                        m_world->chunks[k]->tiles[j][i] = block;
                        int r = std::rand() % 2;
                        if(r == 0) {
                            BlockBush* flower = new BlockBush(glm::vec2((k * CHUNK_SIZE) + i, j + 1), m_world->chunks[k]);
                            m_world->chunks[k]->tiles[j+1][i] = flower;
                        }
                    }
                }
            }
        }
    }
    for(int i = 0; i < WORLD_SIZE; i++) {
        for(int y = 0; y < WORLD_HEIGHT; y++) { // For the extra over-lapping side bits on each chunk.
            int index = i;
            int indexP = (i + 1 + WORLD_SIZE) % WORLD_SIZE;
            int indexM = (i - 1 + WORLD_SIZE) % WORLD_SIZE;
            m_world->chunks[index]->extraTiles[y][0] = m_world->chunks[indexM]->tiles[y][CHUNK_SIZE-1];
            m_world->chunks[index]->extraTiles[y][1] = m_world->chunks[indexP]->tiles[y][0];
        }

        m_world->chunks[i]->setSurroundingChunk(m_world->chunks[((i-1+WORLD_SIZE) % WORLD_SIZE)], 0);
        m_world->chunks[i]->setSurroundingChunk(m_world->chunks[((i+1+WORLD_SIZE) % WORLD_SIZE)], 1);
    }

    Player player(glm::vec2(5.0f * TILE_SIZE, (blockHeights[5] + 5) * TILE_SIZE), m_input, m_sq);
    m_world->player = player;
}
