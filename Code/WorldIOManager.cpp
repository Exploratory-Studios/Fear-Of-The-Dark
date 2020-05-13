#include "WorldIOManager.h"

#include <time.h>
#include <random>
#include <ctime>

#include "EntityPlayer.h"
#include "World.h"

void WorldIOManager::loadWorld(std::string worldName, World* world) {
    setProgress(0.0f);
    boost::thread t( [=]() { P_loadWorld(worldName, world); } );
    t.detach();
    //P_loadWorld(worldName);
    //P_createWorld(1, worldName, false);
}

void WorldIOManager::saveWorld(World* world) {
    setProgress(0.0f);
    boost::thread t( [=]() { P_saveWorld(world); } );
    t.detach();
    //P_saveWorld(worldName);
}

void WorldIOManager::createWorld(unsigned int seed, std::string worldName, bool isFlat, World* world) {
    setProgress(0.0f);
    boost::thread t( [=]() { P_createWorld(seed, worldName, isFlat, world); } );
    t.detach();
    //P_createWorld(seed, worldName, isFlat);
}

void WorldIOManager::P_loadWorld(std::string worldName, World* world) {

    if(world) {
        delete world;
    }

    world = new World();


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
        file.read(reinterpret_cast<char*>(&world->m_time), sizeof(float));
    }

    setProgress(0.1f);

    { // PLAYER

        world->m_player = new EntityPlayer(glm::vec2(0.0f), 0, MetaData(), false);

        file.read(reinterpret_cast<char*>(&world->m_player->m_canInteract), sizeof(bool));
        file.read(reinterpret_cast<char*>(&world->m_player->m_sanity), sizeof(float));
        file.read(reinterpret_cast<char*>(&world->m_player->m_health), sizeof(float));
        file.read(reinterpret_cast<char*>(&world->m_player->m_thirst), sizeof(float));
        file.read(reinterpret_cast<char*>(&world->m_player->m_hunger), sizeof(float));
        file.read(reinterpret_cast<char*>(&world->m_player->m_exhaustion), sizeof(float));
        file.read(reinterpret_cast<char*>(&world->m_player->m_stamina), sizeof(float));
        file.read(reinterpret_cast<char*>(&world->m_player->m_position), sizeof(glm::vec2));

        unsigned int favouriteIndices[10];
        file.read(reinterpret_cast<char*>(&favouriteIndices), sizeof(unsigned int) * 10); // Get indices (in the inventory) that the hotbar pointed to

        logger->log("LOAD: Loaded Player POD");


        // INVENTORY
        unsigned int items = 0;
        file.read(reinterpret_cast<char*>(&items), sizeof(unsigned int));
        world->m_player->m_inventory->m_items.clear();
        world->m_player->m_inventory->m_items.reserve(items);
        for(unsigned int i = 0; i < items; i++) {
            // ITEMS
            ItemData newItem;
            file.read(reinterpret_cast<char*>(&newItem.id), sizeof(unsigned int));
            file.read(reinterpret_cast<char*>(&newItem.quantity), sizeof(unsigned int));
            //newItem.metaData->read(file);

            world->getPlayer()->m_inventory->addItem(new Item(newItem.quantity, newItem.id, false));

            logger->log("LOAD: Loaded " + std::to_string(i+1) + " of " + std::to_string(items) + " Items");
        }

        for(int i = 0; i < 10; i++) {
            world->getPlayer()->m_favouriteItems[i] = world->getPlayer()->m_inventory->getItem(favouriteIndices[i]); // Set up the pointers based on the indices we loaded earlier
        }

        file.read(reinterpret_cast<char*>(&world->getPlayer()->m_inventory->m_absMaxWeight), sizeof(float));
        logger->log("LOAD: Loaded Player Inventory");
    }

    setProgress(0.2f);

    { // WORLD

        ChunkData* chunkData = new ChunkData[WORLD_SIZE];
        //file.read(reinterpret_cast<char*>(&chunkData[0]), sizeof(ChunkData) * WORLD_SIZE);

        for(int i = 0; i < WORLD_SIZE; i++) {
            chunkData[i].read(file);
            world->m_placesMap[i] = ((Categories::Places)chunkData[i].place);
            setProgress(getProgress() + 1.0f / (WORLD_SIZE) * 0.1f); // 0.3
        }

        for(int i = 0; i < WORLD_SIZE; i++) {
            for(unsigned int y = 0; y < WORLD_HEIGHT; y++) {
                for(unsigned int x = 0; x < CHUNK_SIZE; x++) {
                    for(unsigned int k = 0; k < WORLD_DEPTH; k++) {
                        TileData* temp = &chunkData[i].tiles[y][x][k];
                        Tile* tile = new Tile(temp->pos, k, temp->id, *temp->metaData, false);
                        world->setTile(tile);
                        setProgress(getProgress() + 1.0f / (CHUNK_SIZE * WORLD_HEIGHT * WORLD_SIZE * WORLD_DEPTH) * 0.2f); // 0.3
                    }
                }
            }
        }

        logger->log("LOAD: Loaded World Chunks");

        //world->player->setParentChunk(world->chunks);
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

void WorldIOManager::P_saveWorld(World* world) {
    logger->log("SAVE: Starting World Save to File: " + world->getName() + ".bin");
    logger->log("SAVE: Starting Save Preparations");

    PlayerData p;

    // POD
    p.canInteract = world->getPlayer()->m_canInteract;
    p.m_sanity = world->getPlayer()->m_sanity;
    p.m_health = world->getPlayer()->m_health;
    p.m_thirst = world->getPlayer()->m_thirst;
    p.m_hunger = world->getPlayer()->m_hunger;
    p.m_exhaustion = world->getPlayer()->m_exhaustion;
    p.m_stamina = world->getPlayer()->m_stamina;
    p.position = world->getPlayer()->getPosition();
    for(unsigned int i = 0; i < 10; i++) {
        p.favouriteItemIndices[i] = world->getPlayer()->m_inventory->getItemIndex(world->getPlayer()->m_favouriteItems[i]);
    }

    // INVENTORY
    PlayerInventoryData pInventory;
    pInventory.items = world->getPlayer()->m_inventory->getItems().size();
    for(unsigned int i = 0; i < pInventory.items; i++) {
        ItemData item;
        item.id = world->getPlayer()->m_inventory->getItem(i)->getID();
        item.quantity = world->getPlayer()->m_inventory->getItem(i)->getQuantity();
        //*item.metaData = *world->player->m_inventory->getItem(i)->getMetaData();
        pInventory.itemData.push_back(item);
    }
    pInventory.absMaxWeight = world->getPlayer()->m_inventory->m_absMaxWeight;

    p.inventory = pInventory;

    logger->log("SAVE: Player Data Prepared");

    // CHUNK
    ChunkData* chunkData = new ChunkData[WORLD_SIZE];
    for(unsigned int i = 0; i < WORLD_SIZE; i++) {
        for(int y = 0; y < WORLD_HEIGHT; y++) {
            for(int x = 0; x < WORLD_SIZE; x++) {
                for(int k = 0; k < WORLD_DEPTH; k++) {
                    chunkData[i].tiles[y][x][k] = world->getTile(x, y, k)->getSaveData(); // This gives data for ALL layers (recursive)
                }
            }
        }
        chunkData[i].place = world->m_placesMap[i];
    }

    logger->log("SAVE: Chunk Data Prepared");

    float time = world->getTime();

    logger->log("SAVE: ALL DATA PREPARED, STARTING SAVE", true);

    // INIT
    std::ofstream file(SAVES_PATH + world->getName() + ".bin", std::ios::binary);
    if(file.fail()) {
        GLEngine::fatalError("Error saving to file: " + SAVES_PATH + world->getName() + ".bin");
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
void WorldIOManager::P_createWorld(unsigned int seed, std::string worldName, bool isFlat, World* world) {

    World* w = new World();

    float startTime = (float)(std::clock()) / (float)(CLOCKS_PER_SEC / 1000);
    logger->log("CREATE: Starting world creation at time: " + std::to_string(startTime));

    setProgress(0.0f);

    w->m_name = worldName;

    // Set the real-world models of each chunk (randomly)
    PerlinNoise placeNoise(seed);

    float places[(WORLD_SIZE / CHUNK_SIZE)];
    float highestPlace, lowestPlace;

    for(int i = 0; i < (WORLD_SIZE / CHUNK_SIZE); i++) {

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

        //setProgress(getProgress() + 1.0f / WORLD_SIZE * 0.1f);
        setMessage("Now generating biomes... \n(" + std::to_string(i) + "/" + std::to_string((WORLD_SIZE / CHUNK_SIZE)) + ")");

    }

    // m_progress at 0.1f

    setMessage("Finishing chunk initialization...");

    for(int i = 0; i < (WORLD_SIZE / CHUNK_SIZE); i++) {
        float placeMapped = (places[i] - lowestPlace) / (highestPlace - lowestPlace);

        w->m_placesMap[i] = ((Categories::Places)std::ceil(placeMapped * (Category_Data::TOTAL_PLACES - 1)));

        //setProgress(getProgress() + 1.0f / (WORLD_SIZE / CHUNK_SIZE) * 0.1f);
    }

    // m_progress at 0.2f

    std::vector<int> blockHeights;
    blockHeights.resize(WORLD_SIZE * WORLD_DEPTH);

    std::vector<int> tempHeights;
    tempHeights.resize(WORLD_SIZE * WORLD_DEPTH);

    if(!isFlat) {
        // Set the block heights in each chunk
        setMessage("Setting block heights...");

        for(int layer = 0; layer < WORLD_DEPTH; layer++) {
            for(int i = 0; i < (WORLD_SIZE / CHUNK_SIZE); i++) {

                PerlinNoise heightNoise(seed * (i + 1) * 783);

                for(int j = 0; j < CHUNK_SIZE; j++) {
                    float extra = heightNoise.noise((j + layer * 0.5f) * Category_Data::placeData[(int)w->m_placesMap[i]].flatness / CHUNK_SIZE, 8.5, 3.7);
                    //extra += heightNoise.noise(i * j * i, i, 5.8);

                    extra -= 0.5f;
                    extra *= Category_Data::placeData[(int)w->m_placesMap[i]].maxHeightDiff;
                    //extra /= Category_Data::placeData[(int)w->chunks[i]->getPlace()].flatness;

                    float height = std::floor(extra);

                    tempHeights[layer * WORLD_SIZE + i * CHUNK_SIZE + j] = height;

                    setMessage("Setting block heights... \n(" + std::to_string(layer * WORLD_SIZE + i * CHUNK_SIZE + j) + "/" + std::to_string(WORLD_SIZE * WORLD_DEPTH) + ")");
                }
            }
        }

        // m_progress at 0.3f
        setMessage("Smoothing terrain... ");

        /*{
            const float SMOOTHED_PORTION_D = 3; // 2/3
            const float SMOOTHED_PORTION = (SMOOTHED_PORTION_D - 1.0f) / SMOOTHED_PORTION_D;

            for(int layer = 0; layer < WORLD_DEPTH; layer++) {
                for(int i = 0; i < (WORLD_SIZE / CHUNK_SIZE)+1; i++) {

                    PerlinNoise smootherExtraNoise(seed * (i + 1) * 539); // Biome-to-biome basis

                    for(int j = 0; j < CHUNK_SIZE; j++) {

                        if(i+1 < WORLD_SIZE && j > CHUNK_SIZE * SMOOTHED_PORTION) {
                            float smoother = 0.0f;
                            float a = blockHeights[layer * WORLD_SIZE + i * CHUNK_SIZE + CHUNK_SIZE * SMOOTHED_PORTION];
                            float b = blockHeights[layer * WORLD_SIZE + (i + 1) * CHUNK_SIZE + 1];

                            smoother = a - b;

                            float multiplier = ((float)j - (float)CHUNK_SIZE * SMOOTHED_PORTION) / ((float)CHUNK_SIZE / SMOOTHED_PORTION_D);

                            smoother *= -multiplier;
                            blockHeights[layer * WORLD_SIZE + i * CHUNK_SIZE + j] = smoother + blockHeights[i * CHUNK_SIZE + CHUNK_SIZE * SMOOTHED_PORTION] + smootherExtraNoise.noise(i / 10, 10, 84) * 7 - 3.5;

                        } else if(i+1 == WORLD_SIZE && j > CHUNK_SIZE * SMOOTHED_PORTION) {
                            float smoother = 0.0f;
                            float a = blockHeights[layer * WORLD_SIZE + i * CHUNK_SIZE + CHUNK_SIZE * SMOOTHED_PORTION];
                            float b = blockHeights[0];

                            smoother = a - b;

                            float multiplier = ((float)j - (float)CHUNK_SIZE * SMOOTHED_PORTION) / ((float)CHUNK_SIZE / SMOOTHED_PORTION_D);

                            smoother *= -multiplier;
                            blockHeights[layer * WORLD_SIZE + i * CHUNK_SIZE + j] = smoother + blockHeights[layer * WORLD_SIZE + i * CHUNK_SIZE + CHUNK_SIZE * SMOOTHED_PORTION] + smootherExtraNoise.noise(i / 10, 10, 84) * 7 - 3.5;

                        }
                        setMessage("Smoothing terrain... \n(" + std::to_string(layer * WORLD_SIZE + i * CHUNK_SIZE + j) + "/" + std::to_string(WORLD_SIZE * WORLD_DEPTH) + ")");
                    }
                }
            }
        }*/

        {
            setMessage("Smoothing terrain... ");
            for(int layer = 0; layer < WORLD_DEPTH; layer++) {
                for(int x = 0; x < WORLD_SIZE; x++) {
                    // blockHeights[layer * WORLD_SIZE + x]
                    /// 1. Figure out how 'deep' this x value is in its biome
                    int centreIndex = (int)(x / CHUNK_SIZE) * CHUNK_SIZE + (CHUNK_SIZE / 2); // Find the centre x value of this biome
                    unsigned int chunkDepthInv = std::abs(x - centreIndex); // How far it is from the centre. Centre is 0, very outsides are CHUNK_SIZE/2.
                    unsigned int chunkDepth = (CHUNK_SIZE / 2) - chunkDepthInv; // How far in it is. Centre is CHUNK_SIZE/2, very outsides are 0.

                    unsigned int chunkIndex = ((x / CHUNK_SIZE) + (WORLD_SIZE / CHUNK_SIZE)) % (WORLD_SIZE / CHUNK_SIZE); // Chunk index of current x value
                    unsigned int previousBase = Category_Data::placeData[(int)w->m_placesMap[(chunkIndex-1 + WORLD_SIZE/CHUNK_SIZE) % (WORLD_SIZE/CHUNK_SIZE)]].baseHeight;
                    unsigned int nextBase = Category_Data::placeData[(int)w->m_placesMap[(chunkIndex+1 + WORLD_SIZE/CHUNK_SIZE) % (WORLD_SIZE/CHUNK_SIZE)]].baseHeight;
                    unsigned int baseHeight = Category_Data::placeData[(int)w->m_placesMap[chunkIndex]].baseHeight; // Base height values of current, next, and previous chunks

                    blockHeights[layer * WORLD_SIZE + x] = tempHeights[layer * WORLD_SIZE + x];

                    if(chunkDepth / (float)CHUNK_SIZE <= 2.0f/3.0f) {
                        blockHeights[layer * WORLD_SIZE + x] *= (chunkDepth / (float)CHUNK_SIZE) * 3.0f; // I promise this makes sense. *3.0f to account for the 1/3
                    } // Each third of a chunk should be flattened

                    if(x > centreIndex) { // current and next chunks smoothen
                        blockHeights[layer * WORLD_SIZE + x] += nextBase * (chunkDepthInv / (float)CHUNK_SIZE); // ChunkDepthInv/CHUNK_SIZE -> edges=1/2, centre=0
                        blockHeights[layer * WORLD_SIZE + x] += baseHeight * ((chunkDepth / (float)CHUNK_SIZE) + 0.5f); // ChunkDepth/CHUNK_SIZE -> edges=0, centre=1/2
                    } else if(x < centreIndex) {
                        blockHeights[layer * WORLD_SIZE + x] += previousBase * (chunkDepthInv / (float)CHUNK_SIZE); // ChunkDepthInv/CHUNK_SIZE -> edges=1/2, centre=0
                        blockHeights[layer * WORLD_SIZE + x] += baseHeight * ((chunkDepth / (float)CHUNK_SIZE) + 0.5f); // ChunkDepth/CHUNK_SIZE -> edges=0, centre=1/2
                    } else {
                        blockHeights[layer * WORLD_SIZE + x] += baseHeight;
                    }
                }
            }
        }

        tempHeights.clear();

        {

            // m_progress at 0.4f

            setMessage("Placing blocks...");

            for(int layer = 0; layer < WORLD_DEPTH; layer++) {/// TODO: re-enable blocks
                for(int x = 0; x < WORLD_SIZE; x++) {
                    for(int y = 0; y < blockHeights[layer * WORLD_SIZE + x]; y++) {
                        if(y < blockHeights[layer * WORLD_SIZE + x] - 1 - 5 || y <= WATER_LEVEL) {
                            Tile* tile = new Tile(glm::vec2(x, y), layer, TileIDs::STONE, MetaData(), false);
                            w->setTile(tile);
                        } else if(y < blockHeights[layer * WORLD_SIZE + x] - 1 && y > WATER_LEVEL) {
                            Tile* tile = new Tile(glm::vec2(x, y), layer, TileIDs::DIRT, MetaData(), false);
                            w->setTile(tile);
                        } else if(y < blockHeights[layer * WORLD_SIZE + x] && y > WATER_LEVEL) {
                            Tile* tile = new Tile(glm::vec2(x, y), layer, TileIDs::GRASS, MetaData(), false);
                            w->setTile(tile);
                            int r = std::rand(); /// TODO: Re-enable flowers
                            if(r % 2 == 0) {
                                //BlockBush* flower = new BlockBush(glm::vec2(x, y + 1), layer, MetaData(), false);
                                //w->setTile(flower);
                            }
                        }
                    }
                    for(int y = blockHeights[layer * WORLD_SIZE + x]; y < WORLD_HEIGHT; y++) {
                        if(y <= WATER_LEVEL) {
                            Tile* tile = new Tile(glm::vec2(x, y), layer, TileIDs::WATER, MetaData(), false);
                            w->setTile(tile);
                        } else {
                            Tile* tile = new Tile(glm::vec2(x, y), layer, TileIDs::AIR, MetaData(), false);
                            w->setTile(tile);
                        }
                    }
                }
            }
        }
    } else {
        for(int layer = 0; layer < WORLD_DEPTH; layer++) {
            for(int k = 0; k < WORLD_SIZE; k++) {
                blockHeights[k + layer * WORLD_SIZE] = 10;
                for(int j = blockHeights[k + layer * WORLD_SIZE]; j < WORLD_HEIGHT; j++) {
                    Tile* tile = new Tile(glm::vec2(k, j), layer, TileIDs::AIR, MetaData(), false);
                    w->setTile(tile);
                }
                for(int j = 0; j < blockHeights[k + layer * WORLD_SIZE]; j++) {
                    if(j < blockHeights[k + layer * WORLD_SIZE] - 1 - 5) {
                        Tile* tile = new Tile(glm::vec2(k, j), layer, TileIDs::STONE, MetaData(), false);
                        w->setTile(tile);
                    } else if(j < blockHeights[k + layer * WORLD_SIZE] - 1) {
                        Tile* tile = new Tile(glm::vec2(k, j), layer, TileIDs::DIRT, MetaData(), false);
                        w->setTile(tile);
                    } else {
                        Tile* tile = new Tile(glm::vec2(k, j), layer, TileIDs::GRASS, MetaData(), false);
                        w->setTile(tile);
                        int r = std::rand() % 2;
                        if(r == 0) { /// TODO: Re-enable flowers
                            //BlockBush* flower = new BlockBush(glm::vec2(k, j + 1), layer, MetaData(), false);
                            //w->setTile(flower); /// TODO: Cross-layer worldgen
                        }
                    }
                    setMessage("Placing blocks... \n(" + std::to_string(k * WORLD_HEIGHT + layer * WORLD_SIZE * WORLD_HEIGHT) + "/" + std::to_string(WORLD_SIZE * WORLD_HEIGHT * WORLD_DEPTH) + ")");
                }
            }
            // m_progress at 0.7f
        }
    }

    setMessage("Making sure everything looks great!");

    for(int i = 0; i < (WORLD_SIZE / CHUNK_SIZE); i++) {
        //world->chunks[i]->update(0, 1, world->chunks, nullptr, false);
        //w->tickTiles(glm::vec4(i * CHUNK_SIZE, 0.0f, CHUNK_SIZE-1, WORLD_HEIGHT));
        setMessage("Making sure everything looks great! (" + std::to_string(i * CHUNK_SIZE) + "/" + std::to_string(WORLD_SIZE) + ")");
    }
    // m_progress should be at 1.0

    *world = *w;

    setProgress(1.0f);

    float endTime = (float)(std::clock()) / (float)(CLOCKS_PER_SEC / 1000);
    logger->log("CREATE: Finished world creation at time: " + std::to_string(endTime) + " (Elapsed: " + std::to_string(endTime - startTime) + "ms)");

    return;

    //world->player = new Player(glm::vec2(5.0f, (blockHeights[5] + 5)), m_input, m_sq);
}

void WorldIOManager::setWorldEra(World* world, WorldEra newEra) {
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
    std::string worldName = "." + world->m_name + "_" + std::to_string((unsigned int)world->m_worldEra);
    P_saveWorld(world);

    int difference = (int)newEra - (int)world->m_worldEra;

    world->m_worldEra = newEra;

    std::ifstream file("." + world->m_name + "_" + std::to_string((unsigned int)newEra) + ".bin");

    if(file.fail()) {
        logger->log("No existing save for time period: " + std::to_string((unsigned int)newEra) + ", starting actual generation.");
    } else {
        P_loadWorld("." + world->m_name + "_" + std::to_string((unsigned int)newEra), world);
        return;
    }

    if(difference > 0) { // Moving forward through time
        { // Gravity time, bay-bee
            for(int y = 1; y < WORLD_HEIGHT; y++) {
                for(int x = 0; x < WORLD_SIZE; x++) {
                    for(int chunkX = 0; chunkX < CHUNK_SIZE; chunkX++) { /// TODO: Cross-layer stuff
                        if(!world->getTile(chunkX, y, 0)->isSolid() && world->getTile(chunkX, y, 0)->getID() != (unsigned int)TileIDs::AIR) {
                            //*world->tiles[y][chunkX] = BlockAir(glm::vec2(chunkX + CHUNK_SIZE * x, y), world);
                            world->setTile(new Tile(glm::vec2(chunkX + CHUNK_SIZE * x, y), 0, TileIDs::AIR, MetaData(), false)); /// Implement cross-layer stuff
                        }
                        if(world->getTile(chunkX, y, 0)->getID() != (unsigned int)TileIDs::AIR) {
                            int yOffset = -1;

                            Tile* up = world->getTile(chunkX, y+1, 0); /// TODO: Cross-layer
                            Tile* down = world->getTile(chunkX, y-1, 0); /// TODO: Cross-layer
                            Tile* left = world->getTile(chunkX-1, y, 0); /// TODO: Cross-layer
                            Tile* right = world->getTile(chunkX+1, y, 0); /// TODO: Cross-layer

                            if(down) {
                                if(!down->isSolid()) {
                                    unsigned int support = 0;

                                    if(up) if(up->isSolid()) support++;
                                    if(right) if(right->isSolid()) support++;
                                    if(left) if(left->isSolid()) support++;

                                    float chance = (float)(std::rand() % 100) * (float)(support+1); // 400 is the highest you can get with maximum support, 0 the highest with no support, 200 the highest with 1 support

                                    if(chance < 175) { // 43.75% chance to fall with maximum support
                                        while(y + yOffset >= 0) {
                                            if(world->getTile(chunkX, y+yOffset, 0)->isSolid()) {/// TODO: Cross-layer

                                                /*
                                                    1. Delete A (world->tiles[y+yOffset+1][chunkX])
                                                    2. Move pointer to W (world->tiles[y][chunkX]) to A
                                                    3. Move W's actual position down
                                                    4. DON'T DELETE W! THERE WAS *NO* COPYING DONE
                                                    5. Make a new Air block, point W's pointer to it.

                                                // 1.
                                                delete world->tiles[y+yOffset+1][chunkX];

                                                // 2.
                                                world->tiles[y+yOffset+1][chunkX] = world->tiles[y][chunkX];

                                                // 3.
                                                world->tiles[y+yOffset+1][chunkX]->setPosition(world->tiles[y+yOffset+1][chunkX]->getPosition() + glm::vec2(0.0f, yOffset+1));
                                                world->tiles[y+yOffset+1][chunkX]->setNeedsSunCheck();

                                                // 4. Okay, I heard you!
                                                // 5.
                                                world->tiles[y][chunkX] = new BlockAir(glm::vec2(chunkX, y), world);
                                                break;*/

                                                world->setTile(new Tile(glm::vec2(chunkX, y), 0, TileIDs::AIR, MetaData(), false)); /// TODO: Implement cross-layer structure stuff

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
                    world->extraTiles[y][0] = world->chunks[(x-1 + WORLD_SIZE) % WORLD_SIZE]->tiles[y][CHUNK_SIZE-1];
                    world->extraTiles[y][1] = world->chunks[(x+1 + WORLD_SIZE) % WORLD_SIZE]->tiles[y][0];
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
                        if(world->getTile(chunkX, y, 0)->isNatural() == true) {/// TODO: Cross-layer
                            //*world->tiles[y][chunkX] = BlockStone(glm::vec2(chunkX + x * CHUNK_SIZE, y), world);
                            world->setTile(new Tile(glm::vec2(chunkX + x * CHUNK_SIZE, y), 0, TileIDs::STONE, MetaData(), false));/// TODO: Cross-layer
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
                        if(world->getTile(chunkX, y+blockHeights[chunkX + x*CHUNK_SIZE], 0)->getID() == (unsigned int)TileIDs::AIR || rand() % 100 > 90) {/// TODO: Cross-layer
                            //*world->tiles[y+blockHeights[chunkX + x * CHUNK_SIZE]][chunkX] = BlockDirt(glm::vec2(chunkX + x * CHUNK_SIZE, y+blockHeights[chunkX + x * CHUNK_SIZE]), world);
                            world->setTile(new Tile(glm::vec2(chunkX + x * CHUNK_SIZE, y+blockHeights[chunkX + x * CHUNK_SIZE]), 0, TileIDs::DIRT, MetaData(), false));/// TODO: Cross-layer

                        }
                    }
                }
            }

            /*for(int y = 0; y < WORLD_HEIGHT; y++) {
                for(int x = 0; x < WORLD_SIZE; x++) {
                    world->extraTiles[y][0] = world->chunks[(x-1 + WORLD_SIZE) % WORLD_SIZE]->tiles[y][CHUNK_SIZE-1];
                    world->extraTiles[y][1] = world->chunks[(x+1 + WORLD_SIZE) % WORLD_SIZE]->tiles[y][0];
                }
            }*/
        } // Layering's done!
    } else if(difference < 0) { // Moving backwards through time, simply load an earlier save
        std::string oldWorldName = "." + world->m_name + "_" + std::to_string((unsigned int)newEra);
        P_loadWorld(oldWorldName, world);
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

void WorldIOManager::placeStructure(World* world, StructureData& structure, glm::vec2 position) {
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

                world->setTile(new Tile(glm::vec2(tileX, tileY), 0, structure.tiles[tileIndex].id, MetaData(), false));/// TODO: Cross-layer
            }
        }
    }

    /// TODO: Special info, presetVals
}

#ifdef DEV_CONTROLS
#include <iostream>
void WorldIOManager::saveStructureToFile(World* world, std::string& filepath, StructureData& structureData) {
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

void WorldIOManager::saveStructureToFile(World* world, std::string& filepath, glm::vec4 destRect) {
    StructureData data;

    data.tiles.resize((destRect.w + 1) * (destRect.z + 1)); // y*x

    data.width = (int)destRect.z + 1;
    for(int y = (int)destRect.y; y < (int)destRect.w + 1 + (int)destRect.y; y++) {
        for(int x = (int)destRect.x; x < (int)destRect.z + 1 + (int)destRect.x; x++) {
            unsigned int realX = (x + WORLD_SIZE * CHUNK_SIZE) % (WORLD_SIZE * CHUNK_SIZE);
            unsigned int realY = y;
            unsigned int chunkX = (int)(realX / CHUNK_SIZE);

            data.tiles[(y - destRect.y) * data.width + (x - destRect.x)].id = world->getTile(realX, realY, 0)->getID();/// TODO: Cross-layer
            data.tiles[(y - destRect.y) * data.width + (x - destRect.x)].pos = glm::vec2(x - (destRect.x), y - (destRect.y));
        }
    }

    saveStructureToFile(world, filepath, data);
}

#endif // DEV_CONTROLS
