#include "WorldIOManager.h"

#include <time.h>
#include <random>
#include <ctime>

void WorldIOManager::loadWorld(std::string worldName, float* progress) {
    float startTime = (float)(std::clock()) / (float)(CLOCKS_PER_SEC / 1000);

    logger->log("LOAD: STARTING LOAD AT " + std::to_string(startTime), true);

    // INIT
    std::ifstream file("testSaveFile.bin", std::ios::binary);
    if(file.fail()) {
        GLEngine::fatalError("Error loading from file: " + worldName + ".bin");
    }

    { // VERSION
        unsigned int saveLen;

        file.read(reinterpret_cast<char*>(&saveLen), sizeof(unsigned int));

        char* saveVersion = new char();
        file.read(&saveVersion[0], saveLen);

        logger->log("LOAD: Loaded Version: " + std::string(saveVersion) + ", Using Version: " + m_saveVersion, true);

        if(m_saveVersion + "\177" != saveVersion) {
            logger->log("LOAD: Loaded Version Doesn't Match Current Loader Version. Quitting...", true);
            GLEngine::fatalError("Error loading from file: " + worldName + ".bin: Save Version Mismatch");
        }
    }

    { // MISCELLANEOUS WORLD DATA
        file.read(reinterpret_cast<char*>(&m_world->time), sizeof(float));
    }

    { // PLAYER
        if(m_world->player) {
            file.read(reinterpret_cast<char*>(&m_world->player->m_canInteract), sizeof(bool));
            file.read(reinterpret_cast<char*>(&m_world->player->m_sanity), sizeof(float));
            file.read(reinterpret_cast<char*>(&m_world->player->m_health), sizeof(float));
            file.read(reinterpret_cast<char*>(&m_world->player->m_thirst), sizeof(float));
            file.read(reinterpret_cast<char*>(&m_world->player->m_hunger), sizeof(float));
            file.read(reinterpret_cast<char*>(&m_world->player->m_exhaustion), sizeof(float));
            file.read(reinterpret_cast<char*>(&m_world->player->m_stamina), sizeof(float));
            file.read(reinterpret_cast<char*>(&m_world->player->m_position), sizeof(glm::vec2));

            unsigned int favouriteIndices[10];
            file.read(reinterpret_cast<char*>(&favouriteIndices), sizeof(unsigned int) * 10); // Get indices (in the inventory) that the hotbar pointed to

            logger->log("LOAD: Loaded Player POD");


            // INVENTORY
            unsigned int items = 0;
            file.read(reinterpret_cast<char*>(&items), sizeof(unsigned int));
            m_world->player->m_inventory->m_items.clear();
            m_world->player->m_inventory->m_items.reserve(items);
            for(unsigned int i = 0; i < items; i++) {
                // ITEMS
                ItemData newItem;
                file.read(reinterpret_cast<char*>(&newItem), sizeof(ItemData));

                m_world->player->m_inventory->addItem(createItem(newItem.id, newItem.quantity));

                logger->log("LOAD: Loaded " + std::to_string(i+1) + " of " + std::to_string(items) + " Items");
            }

            for(int i = 0; i < 10; i++) {
                m_world->player->m_favouriteItems[i] = m_world->player->m_inventory->getItem(favouriteIndices[i]); // Set up the pointers based on the indices we loaded earlier
            }

            file.read(reinterpret_cast<char*>(&m_world->player->m_inventory->m_absMaxWeight), sizeof(float));
            logger->log("LOAD: Loaded Player Inventory");

        } else {
            logger->log("LOAD: Player could not be saved: null pointer", true);
        }

    }

    { // WORLD
        ChunkData chunkData[WORLD_SIZE];
        file.read(reinterpret_cast<char*>(&chunkData[0]), sizeof(ChunkData) * WORLD_SIZE);

        for(int i = 0; i < WORLD_SIZE; i++) {
            for(unsigned int y = 0; y < WORLD_HEIGHT; y++) {
                for(unsigned int x = 0; x < CHUNK_SIZE; x++) {
                    m_world->chunks[i]->tiles[y][x] = createBlock(chunkData[i].tiles[y][x].id, chunkData[i].tiles[y][x].pos, m_world->chunks[i]);
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
            logger->log("LOAD: Loaded " + std::to_string(i+1) + " of " + std::to_string(WORLD_SIZE) + " Chunks");
        }

        logger->log("LOAD: Loaded World Chunks");
    }

    float finishTime = (float)(std::clock()) / (float)(CLOCKS_PER_SEC / 1000);

    logger->log("LOAD: LOAD COMPLETED. FINISHED AT " + std::to_string(finishTime) + " (" + std::to_string(finishTime-startTime) + " milliseconds)", true);

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

void WorldIOManager::saveWorld(World& world, std::string worldName, float* progress) {
    logger->log("SAVE: Starting Save Preparations");

    PlayerData p;

    // POD
    p.canInteract = m_world->player->m_canInteract;
    p.m_sanity = m_world->player->m_sanity;
    p.m_health = m_world->player->m_health;
    p.m_thirst = m_world->player->m_thirst;
    p.m_hunger = m_world->player->m_hunger;
    p.m_exhaustion = m_world->player->m_exhaustion;
    p.m_stamina = m_world->player->m_stamina;
    p.position = m_world->player->getPosition();
    for(unsigned int i = 0; i < 10; i++) {
        p.favouriteItemIndices[i] = m_world->player->m_inventory->getItemIndex(m_world->player->m_favouriteItems[i]);
    }

    // INVENTORY
    PlayerInventoryData pInventory;
    pInventory.items = m_world->player->m_inventory->getItems().size();
    for(unsigned int i = 0; i < pInventory.items; i++) {
        ItemData item;
        item.id = m_world->player->m_inventory->getItem(i)->getID();
        item.quantity = m_world->player->m_inventory->getItem(i)->getQuantity();
        pInventory.itemData.push_back(item);
    }
    pInventory.absMaxWeight = m_world->player->m_inventory->m_absMaxWeight;

    p.inventory = pInventory;

    logger->log("SAVE: Player Data Prepared");

    // CHUNK
    ChunkData chunkData[WORLD_SIZE];
    for(unsigned int i = 0; i < WORLD_SIZE; i++) {
        for(int y = 0; y < WORLD_HEIGHT; y++) {
            for(int x = 0; x < CHUNK_SIZE; x++) {
                TileData tile;
                tile.pos = m_world->chunks[i]->tiles[y][x]->getPosition();
                tile.id = m_world->chunks[i]->tiles[y][x]->getID();
                tile.ambientLight = m_world->chunks[i]->tiles[y][x]->getAmbientLight();
                chunkData[i].tiles[y][x] = tile;
            }
        }
    }

    logger->log("SAVE: Chunk Data Prepared");

    float time = m_world->time;

    logger->log("SAVE: ALL DATA PREPARED, STARTING SAVE", true);

    // INIT
    std::ofstream file("testSaveFile.bin", std::ios::binary);
    if(file.fail()) {
        GLEngine::fatalError("Error saving to file: " + worldName + ".bin");
    }

    { // VERSION
        unsigned int saveLen = m_saveVersion.size();

        file.write(reinterpret_cast<char*>(&saveLen), sizeof(unsigned int));
        file.write(m_saveVersion.c_str(), m_saveVersion.size());

        logger->log("SAVE: Started Saving with version: " + m_saveVersion);
    }

    { // MISCELLANEOUS WORLD DATA
        file.write(reinterpret_cast<char*>(&time), sizeof(float));
    }

    { // PLAYER
        //file.write(reinterpret_cast<char*>(), sizeof());
        file.write(reinterpret_cast<char*>(&p.canInteract), sizeof(bool));
        file.write(reinterpret_cast<char*>(&p.m_sanity), sizeof(float));
        file.write(reinterpret_cast<char*>(&p.m_health), sizeof(float));
        file.write(reinterpret_cast<char*>(&p.m_thirst), sizeof(float));
        file.write(reinterpret_cast<char*>(&p.m_hunger), sizeof(float));
        file.write(reinterpret_cast<char*>(&p.m_exhaustion), sizeof(float));
        file.write(reinterpret_cast<char*>(&p.m_stamina), sizeof(float));
        file.write(reinterpret_cast<char*>(&p.position), sizeof(glm::vec2));
        file.write(reinterpret_cast<char*>(&p.favouriteItemIndices), sizeof(unsigned int) * 10);
        logger->log("SAVE: Wrote Player POD");


        // INVENTORY
        file.write(reinterpret_cast<char*>(&p.inventory.items), sizeof(unsigned int));
        for(unsigned int i = 0; i < p.inventory.items; i++) {
            // ITEMS
            file.write(reinterpret_cast<char*>(&p.inventory.itemData[i]), sizeof(ItemData));
            logger->log("SAVE: Wrote " + std::to_string(i+1) + " of " + std::to_string(p.inventory.items) + " Items");
        }
        file.write(reinterpret_cast<char*>(&p.inventory.absMaxWeight), sizeof(float));
        logger->log("SAVE: Wrote Player Inventory");

    }

    { // WORLD
        file.write(reinterpret_cast<char*>(&chunkData[0]), sizeof(ChunkData) * WORLD_SIZE);
        logger->log("SAVE: Wrote World Chunks");
    }

    logger->log("SAVE: SAVE COMPLETED", true);;

    file.close();
}

void WorldIOManager::createWorld(unsigned int seed, std::string worldName, bool isFlat) {

    for(int i = 0; i < WORLD_SIZE; i++) {
        m_world->chunks[i] = new Chunk();
    }

    // Set the real-world models of each chunk (randomly)
    PerlinNoise placeNoise(seed);

    float places[WORLD_SIZE];
    float highestPlace, lowestPlace;

    for(int i = 0; i < WORLD_SIZE; i++) {

        float place = placeNoise.noise(i, 0.8, 0.3);

        places[i] = place;

        bool lowest = true;
        bool highest = true;
        for(int j = 0; j < i; j++) {
            if(place > places[j]) lowest = false;
            if(place < places[j]) highest = false;
        }
        if(lowest) lowestPlace = place;
        if(highest) highestPlace = place;

    }

    for(int i = 0; i < WORLD_SIZE; i++) {
        float placeMapped = (places[i] - lowestPlace) / (highestPlace - lowestPlace);

        m_world->chunks[i]->setPlace((Categories::Places)std::ceil(placeMapped * (Category_Data::TOTAL_PLACES)));
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
                            if(y < blockHeights[i * CHUNK_SIZE + x] - 1 - 5) {
                                BlockStone* block = new BlockStone(glm::vec2((i * CHUNK_SIZE) + x, y), m_world->chunks[i]);
                                m_world->chunks[i]->tiles[y][x] = block;
                            } else if(y < blockHeights[i * CHUNK_SIZE + x] - 1) {
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
                    if(j < blockHeights[k * CHUNK_SIZE + i] - 1 - 5) {
                        BlockStone* block = new BlockStone(glm::vec2((k * CHUNK_SIZE) + i, j), m_world->chunks[k]);
                        m_world->chunks[k]->tiles[j][i] = block;
                    } else if(j < blockHeights[k * CHUNK_SIZE + i] - 1) {
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

    //m_world->player = new Player(glm::vec2(5.0f * TILE_SIZE, (blockHeights[5] + 5) * TILE_SIZE), m_input, m_sq);

    std::vector<Parameter> ps;
    Parameter p;
    p.setPointer(m_input);
    ps.push_back(p);
    p.setPointer(m_sq);
    ps.push_back(p);

    m_world->player = reinterpret_cast<Player*>(createEntity((unsigned int)Categories::EntityIDs::MOB_PLAYER, glm::vec2(5.0f * TILE_SIZE, (blockHeights[5] + 5) * TILE_SIZE), nullptr, ps));
}
