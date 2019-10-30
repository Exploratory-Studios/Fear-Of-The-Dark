#include "WorldIOManager.h"

#include <time.h>
#include <random>
#include <ctime>

void WorldIOManager::loadWorld(std::string worldName) {
    setProgress(0.0f);
    boost::thread t( [=]() { P_loadWorld(worldName); } );
    t.detach();
    //P_loadWorld(worldName);
    //P_createWorld(1, worldName, false);
}

void WorldIOManager::saveWorld(std::string worldName) {
    setProgress(0.0f);
    boost::thread t( [=]() { P_saveWorld(worldName); } );
    t.detach();
    //P_saveWorld(worldName);
}

void WorldIOManager::createWorld(unsigned int seed, std::string worldName, bool isFlat) {
    setProgress(0.0f);
    boost::thread t( [=]() { P_createWorld(seed, worldName, isFlat); } );
    t.detach();
    //P_createWorld(seed, worldName, isFlat);
}

void WorldIOManager::P_loadWorld(std::string worldName) {
    if(m_world) {
        clearWorld();
    }

    float startTime = (float)(std::clock()) / (float)(CLOCKS_PER_SEC / 1000);

    setProgress(0.0f);

    logger->log("LOAD: STARTING LOAD AT " + std::to_string(startTime), true);

    // INIT
    std::ifstream file(SAVES_PATH + worldName + ".bin", std::ios::binary);
    if(file.fail()) {
        GLEngine::fatalError("Error loading from file: " + SAVES_PATH + worldName + ".bin");
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

    setProgress(0.1f);

    { // PLAYER

        m_world->player = new Player(glm::vec2(0.0f, 0.0f), m_world->chunks[0], m_input, m_sq, m_audioManager, false);

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
            file.read(reinterpret_cast<char*>(&newItem.id), sizeof(unsigned int));
            file.read(reinterpret_cast<char*>(&newItem.quantity), sizeof(unsigned int));
            //newItem.metaData->read(file);

            m_world->player->m_inventory->addItem(createItem(newItem.id, newItem.quantity));

            logger->log("LOAD: Loaded " + std::to_string(i+1) + " of " + std::to_string(items) + " Items");
        }

        for(int i = 0; i < 10; i++) {
            m_world->player->m_favouriteItems[i] = m_world->player->m_inventory->getItem(favouriteIndices[i]); // Set up the pointers based on the indices we loaded earlier
        }

        file.read(reinterpret_cast<char*>(&m_world->player->m_inventory->m_absMaxWeight), sizeof(float));
        logger->log("LOAD: Loaded Player Inventory");
    }

    setProgress(0.2f);

    { // WORLD

        ChunkData* chunkData = new ChunkData[WORLD_SIZE];
        //file.read(reinterpret_cast<char*>(&chunkData[0]), sizeof(ChunkData) * WORLD_SIZE);

        for(int i = 0; i < WORLD_SIZE; i++) {
            chunkData[i].read(file);
            setProgress(getProgress() + 1.0f / (WORLD_SIZE) * 0.1f); // 0.3
        }

        for(int i = 0; i < WORLD_SIZE; i++) {
            for(unsigned int y = 0; y < WORLD_HEIGHT; y++) {
                for(unsigned int x = 0; x < CHUNK_SIZE; x++) {
                    for(unsigned int k = 0; k < WORLD_DEPTH; k++) {
                        TileData* temp = &chunkData[i].tiles[y][x][k];
                        Tile* block = createBlock(temp->id, temp->pos, k, m_world->chunks[i], *temp->metaData, false);
                        m_world->chunks[i]->setTile(block);
                        setProgress(getProgress() + 1.0f / (CHUNK_SIZE * WORLD_HEIGHT * WORLD_SIZE * WORLD_DEPTH) * 0.2f); // 0.3
                    }
                }
            }
        }

        // progress at 0.8

        for(int i = 0; i < WORLD_SIZE; i++) {
            /*for(int y = 0; y < WORLD_HEIGHT; y++) { // For the extra over-lapping side bits on each chunk.
                int index = i;
                int indexP = (i + 1 + WORLD_SIZE) % WORLD_SIZE;
                int indexM = (i - 1 + WORLD_SIZE) % WORLD_SIZE;
                m_world->chunks[index]->extraTiles[y][0] = m_world->chunks[indexM]->tiles[y][CHUNK_SIZE-1];
                m_world->chunks[index]->extraTiles[y][1] = m_world->chunks[indexP]->tiles[y][0];
            }*/ // Deprecated?

            m_world->chunks[i]->setSurroundingChunk(m_world->chunks[((i-1+WORLD_SIZE) % WORLD_SIZE)], 0);
            m_world->chunks[i]->setSurroundingChunk(m_world->chunks[((i+1+WORLD_SIZE) % WORLD_SIZE)], 1);
            m_world->chunks[i]->m_index = i;

            m_world->chunks[i]->update(0, 1, m_world->chunks, nullptr, false);
            logger->log("LOAD: Loaded " + std::to_string(i+1) + " of " + std::to_string(WORLD_SIZE) + " Chunks");
            setProgress(getProgress() + 1.0f / (WORLD_SIZE) * 0.2f);
        }

        logger->log("LOAD: Loaded World Chunks");

        //m_world->player->setParentChunk(m_world->chunks);
    }

    float finishTime = (float)(std::clock()) / (float)(CLOCKS_PER_SEC / 1000);

    logger->log("LOAD: LOAD COMPLETED. FINISHED AT " + std::to_string(finishTime) + " (" + std::to_string(finishTime-startTime) + " milliseconds)", true);

    setProgress(1.0f);

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
*/ // stacksize is 8192 kbytes

void WorldIOManager::P_saveWorld(std::string worldName) {
    logger->log("SAVE: Starting World Save to File: " + worldName + ".bin");
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
        //*item.metaData = *m_world->player->m_inventory->getItem(i)->getMetaData();
        pInventory.itemData.push_back(item);
    }
    pInventory.absMaxWeight = m_world->player->m_inventory->m_absMaxWeight;

    p.inventory = pInventory;

    logger->log("SAVE: Player Data Prepared");

    // CHUNK
    ChunkData* chunkData = new ChunkData[WORLD_SIZE];
    for(unsigned int i = 0; i < WORLD_SIZE; i++) {
        for(int y = 0; y < WORLD_HEIGHT; y++) {
            for(int x = 0; x < CHUNK_SIZE; x++) {
                for(int k = 0; k < WORLD_DEPTH; k++) {
                    chunkData[i].tiles[y][x][k] = m_world->chunks[i]->getTile(x + (CHUNK_SIZE * i), y, k)->getSaveData(); // This gives data for ALL layers (recursive)
                }
            }
        }
    }

    logger->log("SAVE: Chunk Data Prepared");

    float time = m_world->time;

    logger->log("SAVE: ALL DATA PREPARED, STARTING SAVE", true);

    // INIT
    std::ofstream file(SAVES_PATH + worldName + ".bin", std::ios::binary);
    if(file.fail()) {
        GLEngine::fatalError("Error saving to file: " + SAVES_PATH + worldName + ".bin");
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
            file.write(reinterpret_cast<char*>(&p.inventory.itemData[i].id), sizeof(unsigned int));
            file.write(reinterpret_cast<char*>(&p.inventory.itemData[i].quantity), sizeof(unsigned int));
            //p.inventory.itemData[i].metaData->save(file);
            logger->log("SAVE: Wrote " + std::to_string(i+1) + " of " + std::to_string(p.inventory.items) + " Items");
        }
        file.write(reinterpret_cast<char*>(&p.inventory.absMaxWeight), sizeof(float));
        logger->log("SAVE: Wrote Player Inventory");

    }

    { // WORLD
        for(int i = 0; i < WORLD_SIZE; i++) {
        //file.write(reinterpret_cast<char*>(chunkData), sizeof(ChunkData) * WORLD_SIZE);
            chunkData[i].save(file);
        }
        delete[] chunkData;
        logger->log("SAVE: Wrote World Chunks");
    }

    logger->log("SAVE: SAVE COMPLETED", true);;

    file.close();
}
#include <iostream>
void WorldIOManager::P_createWorld(unsigned int seed, std::string worldName, bool isFlat) {

    clearWorld();

    float startTime = (float)(std::clock()) / (float)(CLOCKS_PER_SEC / 1000);
    logger->log("CREATE: Starting world creation at time: " + std::to_string(startTime));

    setProgress(0.0f);

    m_world->name = worldName;

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

        setProgress(getProgress() + 1.0f / WORLD_SIZE * 0.1f);
        setMessage("Now generating biomes... \n(" + std::to_string(i) + "/" + std::to_string(WORLD_SIZE) + ")");

    }

    // m_progress at 0.1f

    setMessage("Finishing chunk initialization...");

    for(int i = 0; i < WORLD_SIZE; i++) {
        float placeMapped = (places[i] - lowestPlace) / (highestPlace - lowestPlace);

        m_world->chunks[i]->setPlace((Categories::Places)std::ceil(placeMapped * (Category_Data::TOTAL_PLACES - 1)));
        m_world->chunks[i]->setIndex(i);

        setProgress(getProgress() + 1.0f / WORLD_SIZE * 0.1f);

    }

    // m_progress at 0.2f

    std::vector<int> blockHeights;
    blockHeights.resize(WORLD_SIZE * CHUNK_SIZE);

    if(!isFlat) {
        // Set the block heights in each chunk
        setMessage("Setting block heights...");

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

                setProgress(getProgress() + 1.0f / (CHUNK_SIZE * WORLD_SIZE) * 0.1f);
                setMessage("Setting block heights... \n(" + std::to_string(i * CHUNK_SIZE + j) + "/" + std::to_string(CHUNK_SIZE * WORLD_SIZE) + ")");
            }
        }

        // m_progress at 0.3f
        setMessage("Smoothing terrain... ");

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
                    setMessage("Smoothing terrain... \n(" + std::to_string(i * CHUNK_SIZE + j) + "/" + std::to_string((WORLD_SIZE+1) * CHUNK_SIZE) + ")");
                    setProgress(getProgress() + 1.0f / (float)(CHUNK_SIZE * (WORLD_SIZE+1)) * 0.1f);
                }
            }

            // m_progress at 0.4f

            setMessage("Placing blocks...");

            for(int layer = 0; layer < WORLD_DEPTH; layer++) {
                for(int i = 0; i < WORLD_SIZE; i++) {
                    for(int x = 0; x < CHUNK_SIZE; x++) {
                        for(int y = 0; y < blockHeights[i * CHUNK_SIZE + x]; y++) {
                            if(y < blockHeights[i * CHUNK_SIZE + x] - 1 - 5 || y <= WATER_LEVEL) {
                                BlockStone* block = new BlockStone(glm::vec2((i * CHUNK_SIZE) + x, y), layer, m_world->chunks[i], false);
                                m_world->chunks[i]->setTile(block);/// TODO: Cross-layer worldgen
                            } else if(y < blockHeights[i * CHUNK_SIZE + x] - 1 && y > WATER_LEVEL) {
                                BlockDirt* block = new BlockDirt(glm::vec2((i * CHUNK_SIZE) + x, y), layer, m_world->chunks[i], false);
                                m_world->chunks[i]->setTile(block);/// TODO: Cross-layer worldgen
                            } else if(y < blockHeights[i * CHUNK_SIZE + x] && y > WATER_LEVEL) {
                                BlockGrass* block = new BlockGrass(glm::vec2((i * CHUNK_SIZE) + x, y), layer, m_world->chunks[i], false);
                                m_world->chunks[i]->setTile(block);/// TODO: Cross-layer worldgen
                                int r = std::rand();
                                if(r % 2 == 0) {
                                    BlockBush* flower = new BlockBush(glm::vec2((i * CHUNK_SIZE) + x, y + 1), layer, m_world->chunks[i], false);
                                    m_world->chunks[i]->setTile(flower);
                                }
                            }
                            setProgress(getProgress() + 1.0f / (float)(CHUNK_SIZE * WORLD_SIZE * WORLD_DEPTH * WORLD_HEIGHT) * 0.1f); // 0.3
                            setMessage("Placing blocks... \n(" + std::to_string((layer * CHUNK_SIZE * WORLD_HEIGHT * WORLD_SIZE) + (i * CHUNK_SIZE * WORLD_HEIGHT) + (x * WORLD_HEIGHT) + y) + "/" + std::to_string(CHUNK_SIZE * WORLD_SIZE * WORLD_HEIGHT * WORLD_DEPTH) + ")");
                        }
                        for(int y = blockHeights[i * CHUNK_SIZE + x]; y < WORLD_HEIGHT; y++) {
                            if(y <= WATER_LEVEL) {
                                BlockWater* block = new BlockWater(glm::vec2((i * CHUNK_SIZE) + x, y), layer, m_world->chunks[i], 1.0f, MetaData(), false);
                                m_world->chunks[i]->setTile(block); /// TODO: Cross-layer worldgen
                            } else {
                                BlockAir* block = new BlockAir(glm::vec2((i * CHUNK_SIZE) + x, y), layer, m_world->chunks[i], false);
                                m_world->chunks[i]->setTile(block); /// TODO: Cross-layer worldgen
                            }
                            setProgress(getProgress() + 1.0f / (float)(CHUNK_SIZE * WORLD_SIZE * WORLD_DEPTH * WORLD_HEIGHT) * 0.1f);
                            setMessage("Placing blocks... \n(" + std::to_string((layer * CHUNK_SIZE * WORLD_HEIGHT * WORLD_SIZE) + (i * CHUNK_SIZE * WORLD_HEIGHT) + (x * WORLD_HEIGHT) + y) + "/" + std::to_string(CHUNK_SIZE * WORLD_SIZE * WORLD_HEIGHT * WORLD_DEPTH) + ")");
                        }
                    }
                }
            }

            // m_progress at 0.7f
        }
    } else {
        // Progress is at 0.2, ends at 0.7
        for(int layer = 0; layer < WORLD_DEPTH; layer++) {
            for(int k = 0; k < WORLD_SIZE; k++) {
                for(int i = 0; i < CHUNK_SIZE; i++) {
                    blockHeights[k * CHUNK_SIZE + i] = 10;
                    for(int j = blockHeights[k * CHUNK_SIZE + i]; j < WORLD_HEIGHT; j++) {
                        BlockAir* block = new BlockAir(glm::vec2((k * CHUNK_SIZE) + i, j), layer, m_world->chunks[k], false);
                        m_world->chunks[k]->setTile(block); /// TODO: Cross-layer worldgen
                    }
                    for(int j = 0; j < blockHeights[k * CHUNK_SIZE + i]; j++) {
                        if(j < blockHeights[k * CHUNK_SIZE + i] - 1 - 5) {
                            BlockStone* block = new BlockStone(glm::vec2((k * CHUNK_SIZE) + i, j), layer, m_world->chunks[k], false);
                            m_world->chunks[k]->setTile(block); /// TODO: Cross-layer worldgen
                        } else if(j < blockHeights[k * CHUNK_SIZE + i] - 1) {
                            BlockDirt* block = new BlockDirt(glm::vec2((k * CHUNK_SIZE) + i, j), layer, m_world->chunks[k], false);
                            m_world->chunks[k]->setTile(block); /// TODO: Cross-layer worldgen
                        } else {
                            BlockGrass* block = new BlockGrass(glm::vec2((k * CHUNK_SIZE) + i, j), layer, m_world->chunks[k], false);
                            m_world->chunks[k]->setTile(block); /// TODO: Cross-layer worldgen
                            int r = std::rand() % 2;
                            if(r == 0) {
                                BlockBush* flower = new BlockBush(glm::vec2((k * CHUNK_SIZE) + i, j + 1), layer, m_world->chunks[k], false);
                                m_world->chunks[k]->setTile(flower); /// TODO: Cross-layer worldgen
                            }
                        }
                        setMessage("Placing blocks... \n(" + std::to_string((layer * CHUNK_SIZE * WORLD_HEIGHT * WORLD_DEPTH * WORLD_SIZE) + (k * CHUNK_SIZE * WORLD_HEIGHT) + (i * WORLD_HEIGHT) + j) + "/" + std::to_string(CHUNK_SIZE * WORLD_SIZE * WORLD_HEIGHT * WORLD_DEPTH) + ")");
                    }
                    setProgress(getProgress() + 1.0f / (float)(CHUNK_SIZE * WORLD_SIZE * WORLD_DEPTH) * 0.5f);
                }
            }
            // m_progress at 0.7f
        }
    }
    for(int i = 0; i < WORLD_SIZE; i++) {
        /*for(int y = 0; y < WORLD_HEIGHT; y++) { // For the extra over-lapping side bits on each chunk.
            int index = i;
            int indexP = (i + 1 + WORLD_SIZE) % WORLD_SIZE;
            int indexM = (i - 1 + WORLD_SIZE) % WORLD_SIZE;
            m_world->chunks[index]->extraTiles[y][0] = m_world->chunks[indexM]->tiles[y][CHUNK_SIZE-1];
            m_world->chunks[index]->extraTiles[y][1] = m_world->chunks[indexP]->tiles[y][0];
        }*/

        m_world->chunks[i]->setSurroundingChunk(m_world->chunks[((i-1+WORLD_SIZE) % WORLD_SIZE)], 0);
        m_world->chunks[i]->setSurroundingChunk(m_world->chunks[((i+1+WORLD_SIZE) % WORLD_SIZE)], 1);
    }
    // m_progress is at 0.7f

    setMessage("Making sure everything looks great!");

    for(int j = 0; j < 1; j++) {
        for(int i = 0; i < WORLD_SIZE; i++) {
            //m_world->chunks[i]->update(0, 1, m_world->chunks, nullptr, false);
            if(j == 0) m_world->chunks[i]->tick(0, nullptr, m_world->worldEra, false);
            setMessage("Making sure everything looks great! (" + std::to_string(i + j * WORLD_SIZE) + "/" + std::to_string(WORLD_SIZE * 5) + ")");
            setProgress(getProgress() + 1.0f / (WORLD_SIZE * 5) * 0.25f);
        }
    }
    // m_progress should be at 1.0

    setProgress(1.0f);

    float endTime = (float)(std::clock()) / (float)(CLOCKS_PER_SEC / 1000);
    logger->log("CREATE: Finished world creation at time: " + std::to_string(endTime) + " (Elapsed: " + std::to_string(endTime - startTime) + "ms)");

    return;

    //m_world->player = new Player(glm::vec2(5.0f, (blockHeights[5] + 5)), m_input, m_sq);
}

void WorldIOManager::setWorldEra(WorldEra newEra) {
    /*
        This function will affect the entire world in a way that simulates lots of time passing. This may include:
            - Buildings collapsing
            - Things being buried
            - Land masses moving slightly (mountains smoothing, new mountains forming)

        It will also archive a copy of the world before time passing, so that each time period can be revisited.

        Order of operations:
        1. Save world under a hidden name (eg. TestWorld.bin -> .TestWorld_NEOLITHIC_BACKUP.bin)
        2. Find the time difference between two eras
        3. Use 'gravity' starting from the bottom of the world, and working its way down, collapsing or destroying blocks (based on chance and surrounding block support)
        4. Change existing dirt to stone, add a noise-based layer of dirt/stone/sediment
        5. Smooth mountains a little bit, choose random points to 'spike' the land.
        6. Loop 3-5 the amount of eras between
    */

    // Save under a different filename (.name_numberoferaenum)
    std::string worldName = "." + m_world->name + "_" + std::to_string((unsigned int)m_world->worldEra);
    P_saveWorld(worldName);

    int difference = (int)newEra - (int)m_world->worldEra;

    m_world->worldEra = newEra;

    std::ifstream file("." + m_world->name + "_" + std::to_string((unsigned int)newEra) + ".bin");

    if(file.fail()) {
        logger->log("No existing save for time period: " + std::to_string((unsigned int)newEra) + ", starting actual generation.");
    } else {
        P_loadWorld("." + m_world->name + "_" + std::to_string((unsigned int)newEra));
        return;
    }

    if(difference > 0) { // Moving forward through time
        { // Gravity time, bay-bee
            for(int y = 1; y < WORLD_HEIGHT; y++) {
                for(int x = 0; x < WORLD_SIZE; x++) {
                    for(int chunkX = 0; chunkX < CHUNK_SIZE; chunkX++) { /// TODO: Cross-layer stuff
                        if(!m_world->chunks[x]->getTile(chunkX, y, 0)->isSolid() && m_world->chunks[x]->getTile(chunkX, y, 0)->m_id != (unsigned int)Categories::BlockIDs::AIR) {
                            //*m_world->chunks[x]->tiles[y][chunkX] = BlockAir(glm::vec2(chunkX + CHUNK_SIZE * x, y), m_world->chunks[x]);
                            m_world->chunks[x]->setTile(new BlockAir(glm::vec2(chunkX + CHUNK_SIZE * x, y), 0, m_world->chunks[x])); /// Implement cross-layer stuff
                        }
                        if(m_world->chunks[x]->getTile(chunkX, y, 0)->m_id != (unsigned int)Categories::BlockIDs::AIR) {
                            int yOffset = -1;

                            Tile* up = m_world->chunks[x]->getTile(chunkX, y+1, 0); /// TODO: Cross-layer
                            Tile* down = m_world->chunks[x]->getTile(chunkX, y-1, 0); /// TODO: Cross-layer
                            Tile* left = m_world->chunks[x]->getTile(chunkX-1, y, 0); /// TODO: Cross-layer
                            Tile* right = m_world->chunks[x]->getTile(chunkX+1, y, 0); /// TODO: Cross-layer

                            if(down) {
                                if(!down->isSolid()) {
                                    unsigned int support = 0;

                                    if(up) if(up->isSolid()) support++;
                                    if(right) if(right->isSolid()) support++;
                                    if(left) if(left->isSolid()) support++;

                                    float chance = (float)(std::rand() % 100) * (float)(support+1); // 400 is the highest you can get with maximum support, 0 the highest with no support, 200 the highest with 1 support

                                    if(chance < 175) { // 43.75% chance to fall with maximum support
                                        while(y + yOffset >= 0) {
                                            if(m_world->chunks[x]->getTile(chunkX, y+yOffset, 0)->isSolid()) {/// TODO: Cross-layer

                                                /*
                                                    1. Delete A (m_world->chunks[x]->tiles[y+yOffset+1][chunkX])
                                                    2. Move pointer to W (m_world->chunks[x]->tiles[y][chunkX]) to A
                                                    3. Move W's actual position down
                                                    4. DON'T DELETE W! THERE WAS *NO* COPYING DONE
                                                    5. Make a new Air block, point W's pointer to it.

                                                // 1.
                                                delete m_world->chunks[x]->tiles[y+yOffset+1][chunkX];

                                                // 2.
                                                m_world->chunks[x]->tiles[y+yOffset+1][chunkX] = m_world->chunks[x]->tiles[y][chunkX];

                                                // 3.
                                                m_world->chunks[x]->tiles[y+yOffset+1][chunkX]->setPosition(m_world->chunks[x]->tiles[y+yOffset+1][chunkX]->getPosition() + glm::vec2(0.0f, yOffset+1));
                                                m_world->chunks[x]->tiles[y+yOffset+1][chunkX]->setNeedsSunCheck();

                                                // 4. Okay, I heard you!
                                                // 5.
                                                m_world->chunks[x]->tiles[y][chunkX] = new BlockAir(glm::vec2(chunkX, y), m_world->chunks[x]);
                                                break;*/

                                                m_world->chunks[x]->setTile(new BlockAir(glm::vec2(chunkX, y), 0, m_world->chunks[x])); /// TODO: Implement cross-layer structure stuff
                                            } else {
                                                yOffset--;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            /*for(int y = 0; y < WORLD_HEIGHT; y++) {
                for(int x = 0; x < WORLD_SIZE; x++) {
                    m_world->chunks[x]->extraTiles[y][0] = m_world->chunks[(x-1 + WORLD_SIZE) % WORLD_SIZE]->tiles[y][CHUNK_SIZE-1];
                    m_world->chunks[x]->extraTiles[y][1] = m_world->chunks[(x+1 + WORLD_SIZE) % WORLD_SIZE]->tiles[y][0];
                }
            }*/
        } // Gravity's done!

        { // Layer of rock and dirt time, bay-bee
            // Change everything to stone
            // Find blockHeights
            std::vector<unsigned int> blockHeights;
            blockHeights.resize(CHUNK_SIZE * WORLD_SIZE);
            for(unsigned int y = 0; y < WORLD_HEIGHT; y++) {
                for(unsigned int x = 0; x < WORLD_SIZE; x++) {
                    for(unsigned int chunkX = 0; chunkX < CHUNK_SIZE; chunkX++) {
                        if(m_world->chunks[x]->getTile(chunkX, y, 0)->isNatural() == true) {/// TODO: Cross-layer
                            //*m_world->chunks[x]->tiles[y][chunkX] = BlockStone(glm::vec2(chunkX + x * CHUNK_SIZE, y), m_world->chunks[x]);
                            m_world->chunks[x]->setTile(new BlockStone(glm::vec2(chunkX + x * CHUNK_SIZE, y), 0, m_world->chunks[x]));/// TODO: Cross-layer
                            blockHeights[chunkX + x * CHUNK_SIZE] = y;
                        }
                    }
                }
            } // Done!

            for(unsigned int x = 0; x < WORLD_SIZE; x++) {
                // Get perlin noise to make more dirt!
                PerlinNoise heightNoise(97682354 / rand() % 6 * (x + 8) * 2348);
                for(unsigned int chunkX = 0; chunkX < CHUNK_SIZE; chunkX++) {
                    for(unsigned int y = 1; y < heightNoise.noise(chunkX, x, 1.0f) * 10 + 5; y++) {
                        if(m_world->chunks[x]->getTile(chunkX, y+blockHeights[chunkX + x*CHUNK_SIZE], 0)->getID() == (unsigned int)Categories::BlockIDs::AIR || rand() % 100 > 90) {/// TODO: Cross-layer
                            //*m_world->chunks[x]->tiles[y+blockHeights[chunkX + x * CHUNK_SIZE]][chunkX] = BlockDirt(glm::vec2(chunkX + x * CHUNK_SIZE, y+blockHeights[chunkX + x * CHUNK_SIZE]), m_world->chunks[x]);
                            m_world->chunks[x]->setTile(new BlockDirt(glm::vec2(chunkX + x * CHUNK_SIZE, y+blockHeights[chunkX + x * CHUNK_SIZE]), 0, m_world->chunks[x]));/// TODO: Cross-layer

                        }
                    }
                }
            }

            /*for(int y = 0; y < WORLD_HEIGHT; y++) {
                for(int x = 0; x < WORLD_SIZE; x++) {
                    m_world->chunks[x]->extraTiles[y][0] = m_world->chunks[(x-1 + WORLD_SIZE) % WORLD_SIZE]->tiles[y][CHUNK_SIZE-1];
                    m_world->chunks[x]->extraTiles[y][1] = m_world->chunks[(x+1 + WORLD_SIZE) % WORLD_SIZE]->tiles[y][0];
                }
            }*/
        } // Layering's done!
    } else if(difference < 0) { // Moving backwards through time, simply load an earlier save
        std::string oldWorldName = "." + m_world->name + "_" + std::to_string((unsigned int)newEra);
        P_loadWorld(oldWorldName);
    }
}

StructureData WorldIOManager::loadStructureFromFile(std::string& filepath) {
    /*
        This function should error-check:
            - If the file could be opened
            - If the version is the same
            - If it's a valid structure

        Structures are stored as binary, starting with a sort of file header, giving:
        - The version it was saved in
        - The id (which corresponds to some data in PresetVals
        - The size (width and height)

        The file should also have a footer, giving:
        - Special, specific information: A possible id, based on the position of a block in the structure (id = x + width*y), along with any special information (such as contents of a chest, etc.)

        This function should:
        1. Error-check
        2. Create a structure object
        3. Populate it with data from the given file, reading the header, constructing an array of Blocks from the size, then reading the footer for any extra necessary info
    */

    // Open the file
    std::ifstream file(filepath);
    if(file.fail()) {
        logger->log("FATAL ERROR: The structure file could not be opened: " + filepath, true);
        GLEngine::fatalError("The structure file could not be opened: " + filepath + "\n");
    } else {
        // Get all the information, starting with the header
        unsigned int saveLen; // The length of the saveVersion string
        char* saveVersion; // The version that the file was saved at, in char* form because it's easier to read that way
        unsigned int id; // The ID of the structure, corresponding with PresetVals something
        unsigned int width, height; // The width and height of the structure

        { // Get header information
            { // Get the version
                file.read(reinterpret_cast<char*>(&saveLen), sizeof(unsigned int));

                char* saveVersion = new char();
                file.read(&saveVersion[0], saveLen);

                logger->log("LOAD: Loaded Version: " + std::string(saveVersion) + ", Using Version: " + m_saveVersion, true);

                if(m_saveVersion + "\177" != saveVersion) {
                    logger->log("LOAD: Loaded Version Doesn't Match Current Loader Version. Quitting...", true);
                    GLEngine::fatalError("Error loading structure from file: " + filepath + ".bin: Save Version Mismatch");
                }
            }

            { // Get other header information
                file.read(reinterpret_cast<char*>(&id), sizeof(unsigned int)); // The id of the structure

                file.read(reinterpret_cast<char*>(&width), sizeof(unsigned int)); // The width of the structure
                file.read(reinterpret_cast<char*>(&height), sizeof(unsigned int)); // The height of the structure
            }
        } // Got header information

        std::vector<TileData> tiles; // Made a vector to hold all the block information, right now it's really just the ids

        // Now to make sure we have enough room
        tiles.resize(height * width);

        { // Get blocks
            for(unsigned int y = 0; y < height; y++) { // A 3*2 would be structured like so: (x, y) tiles = {(0, 0), (1, 0), (2, 0), (0, 1), (1, 1), (2, 1)}
                for(unsigned int x = 0; x < width; x++) {
                    TileData data;

                    unsigned int tileId;
                    file.read(reinterpret_cast<char*>(&data), sizeof(TileData)); // The id of the tile
                    //data.id = tileId;

                    //data.pos = glm::vec2(x, y); // This will be summed with the structure's overall position to get it's actual position

                    tiles[y*width+x] = data;
                }
            }
        } // Got blocks, created an object out of them (std::vector<TileData> tiles)

        { // Time to get weird special things (Literally nothing to do here yet) (Hopefully never will be)

        }

        // Simply make a StructureData object to return that stores all of it
        StructureData retStructure;
        retStructure.id = id;
        retStructure.width = width;
        retStructure.tiles = tiles;

        return retStructure;
    }
}

void WorldIOManager::placeStructure(StructureData& structure, glm::vec2 position) {
    /*
        This function should simply iterate through all the tiles, special info, and PresetVals, and place a structure in the world, at a given position
    */

    unsigned int id = structure.id;
    unsigned int width = structure.width;
    unsigned int height = structure.tiles.size() / width;
    unsigned int xPos = (int)(position.x + WORLD_SIZE * CHUNK_SIZE) % (WORLD_SIZE * CHUNK_SIZE);
    unsigned int yPos = (int)(position.y);

    { // Tiles time
        for(unsigned int y = yPos; y < (WORLD_HEIGHT > yPos + height ? yPos + height : WORLD_HEIGHT-1); y++) {
            for(unsigned int x = xPos; x < xPos + width; x++) {
                unsigned int tileIndex = (y - yPos) * width + (x - xPos);

                unsigned int tileX = (int)(xPos + structure.tiles[tileIndex].pos.x) % (WORLD_SIZE * CHUNK_SIZE);
                unsigned int tileY = yPos + structure.tiles[tileIndex].pos.y;

                unsigned int chunkX = (tileX / CHUNK_SIZE + WORLD_SIZE);

                m_world->chunks[(chunkX % WORLD_SIZE)]->setTile(createBlock(structure.tiles[tileIndex].id, glm::vec2(tileX, tileY), 0, m_world->chunks[(chunkX % WORLD_SIZE)]));/// TODO: Cross-layer
            }
        }
    }

    /// TODO: Special info, presetVals
}

#ifdef DEV_CONTROLS
#include <iostream>
void WorldIOManager::saveStructureToFile(std::string& filepath, StructureData& structureData) {
    /*
        This function should error-check:
            - If the file could be opened
            - If it's a valid structure

        Structures are stored as binary, starting with a sort of file header, giving:
        - The version it was saved in
        - The id (which corresponds to some data in PresetVals
        - The size (width and height)

        The file should also have a footer, giving:
        - Special, specific information: A possible id, based on the position of a block in the structure (id = x + width*y), along with any special information (such as contents of a chest, etc.)

        This function should:
        1. Error-check
        2. Break down the structureData object into its constituent parts (width, height, id, tileData -> special data as well, etc.)
        3. Save each one (in the right order) to the save file
        4. Save extra-special information such as inventories, etc.
    */

    // Open the file
    std::ofstream file(filepath);
    if(file.fail()) {
        logger->log("FATAL ERROR: The structure file could not be opened: " + filepath, true);
        GLEngine::fatalError("The structure file could not be opened: " + filepath + "\n");
    } else {
        // Collect all the information (break down structureData)
        unsigned int id; // The ID of the structure, corresponding with PresetVals stuff

        // Since we are saving a new structure, the id will have to be set by the user. This means ONLY the programmer can make structures that link with PresetVals...
        /// TODO: ^^^^^^^^^^^^
        std::cout << "Please enter the id of the new structure... ";
        std::cin >> id;

        unsigned int width = structureData.width; // The width of the structure
        unsigned int height = structureData.tiles.size() / width; // The height of the structure
        std::vector<TileData> tiles = structureData.tiles;

        { // Save header information
            { // Save the version and its length
                unsigned int saveLen = m_saveVersion.size();

                file.write(reinterpret_cast<char*>(&saveLen), sizeof(unsigned int));
                file.write(m_saveVersion.c_str(), m_saveVersion.size());
            }

            { // Save other header information
                file.write(reinterpret_cast<char*>(&id), sizeof(unsigned int)); // The id of the structure

                file.write(reinterpret_cast<char*>(&width), sizeof(unsigned int)); // The width of the structure
                file.write(reinterpret_cast<char*>(&height), sizeof(unsigned int)); // The height of the structure
            }
        } // Saved header information

        { // Save blocks (TileData)
            for(unsigned int i = 0; i < width*height; i++) { // Should break if I did something wrong
                file.write(reinterpret_cast<char*>(&tiles[i]), sizeof(TileData));
            }
        } // Saved TileData

        { // Time to save weird special things (Literally nothing to do here yet) (Hopefully never will be)

        }

        // Say everything is all good
        logger->log("SAVE: Saved structure with width of " + std::to_string(width) + " and height of " + std::to_string(height) + " successfully under id " + std::to_string(id) + ".");
    }
}

void WorldIOManager::saveStructureToFile(std::string& filepath, glm::vec4 destRect) {
    StructureData data;

    data.tiles.resize((destRect.w + 1) * (destRect.z + 1)); // y*x

    data.width = (int)destRect.z + 1;
    for(int y = (int)destRect.y; y < (int)destRect.w + 1 + (int)destRect.y; y++) {
        for(int x = (int)destRect.x; x < (int)destRect.z + 1 + (int)destRect.x; x++) {
            unsigned int realX = (x + WORLD_SIZE * CHUNK_SIZE) % (WORLD_SIZE * CHUNK_SIZE);
            unsigned int realY = y;
            unsigned int chunkX = (int)(realX / CHUNK_SIZE);

            data.tiles[(y - destRect.y) * data.width + (x - destRect.x)].id = m_world->chunks[chunkX]->getTile(realX, realY, 0)->getID();/// TODO: Cross-layer
            data.tiles[(y - destRect.y) * data.width + (x - destRect.x)].pos = glm::vec2(x - (destRect.x), y - (destRect.y));
        }
    }

    saveStructureToFile(filepath, data);
}

#endif // DEV_CONTROLS

void WorldIOManager::clearWorld() {
    delete m_world;
    m_world = new World();
}
