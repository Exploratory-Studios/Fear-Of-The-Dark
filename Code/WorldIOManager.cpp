#include "WorldIOManager.h"

#include <time.h>
#include <random>
#include <ctime>

#include "EntityPlayer.h"
#include "World.h"
#include "InventoryBase.h"
#include "Tile.h"

#include "XMLData.h"
#include "Factory.h"
#include "Singletons.h"

void WorldIOManager::loadWorld(std::string worldName) {
	setProgress(0.0f);
	boost::thread t([ = ]() {
		P_loadWorld(worldName, Singletons::getWorld());
	});
	t.detach();
	//P_loadWorld(worldName);
	//P_createWorld(1, worldName, false);
}

void WorldIOManager::saveWorld() {
	setProgress(0.0f);
	boost::thread t([ = ]() {
		P_saveWorld(Singletons::getWorld());
	});
	t.detach();
	//P_saveWorld(worldName);
}

void WorldIOManager::createWorld(unsigned int seed, std::string worldName, bool isFlat, unsigned int width) {
	setProgress(0.0f);
	boost::thread t([ = ]() {
		P_createWorld(seed, worldName, isFlat, width);
	});
	t.detach();
	//P_createWorld(seed, worldName, isFlat);
}

void WorldIOManager::P_loadWorld(std::string worldName, World* world) {

	if(world) {
		delete world;
	}

	world = new World(0, 0, 0); /// TODO: Read world size and all that before creation.


	float startTime = (float)(std::clock()) / (float)(CLOCKS_PER_SEC / 1000);

	setProgress(0.0f);

	logger->log("LOAD: STARTING LOAD AT " + std::to_string(startTime), true);

	// INIT
	std::ifstream file(SAVES_PATH + worldName + ".bin", std::ios::binary);
	if(file.fail()) {
		GLEngine::fatalError("Error loading from file: " + SAVES_PATH + worldName + ".bin");
	}

	{
		// VERSION
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

	{
		// MISCELLANEOUS WORLD DATA
		file.read(reinterpret_cast<char*>(&world->m_time), sizeof(float));
	}

	setProgress(0.1f);

	{
		// PLAYER

		EntityPlayer* newP = new EntityPlayer(glm::vec2(0.0f), 0, SaveDataTypes::MetaData(), false);
		Singletons::getEntityManager()->setPlayer(newP);

		SaveDataTypes::EntityPlayerData pod;
		pod.read(file);

		/// TODO: Set actual player data from pod

		logger->log("LOAD: Loaded Player POD");
	}

	setProgress(0.2f);

	{
		// WORLD

		const unsigned int chunks = world->getSize() / CHUNK_SIZE;
		SaveDataTypes::ChunkData* chunkData = new SaveDataTypes::ChunkData[chunks];
		//file.read(reinterpret_cast<char*>(&chunkData[0]), sizeof(ChunkData) * WORLD_SIZE);

		for(int i = 0; i < world->getSize(); i++) {
			chunkData[i].read(file);
			world->m_biomesMap[i] = chunkData[i].biomeID;
			setProgress(getProgress() + 1.0f / (world->getSize()) * 0.1f); // 0.3
		}

		for(int i = 0; i < world->getSize(); i++) {
			for(unsigned int y = 0; y < WORLD_HEIGHT; y++) {
				for(unsigned int x = 0; x < CHUNK_SIZE; x++) {
					for(unsigned int k = 0; k < WORLD_DEPTH; k++) {
						SaveDataTypes::TileData* temp = &chunkData[i].tiles[y][x][k];
						Tile* tile = Factory::createTile(temp->id, temp->pos, k, temp->metaData);
						world->setTile_noEvent(tile);
						setProgress(getProgress() + 1.0f / (CHUNK_SIZE * WORLD_HEIGHT * world->getSize() * WORLD_DEPTH) * 0.2f); // 0.3
					}
				}
			}
		}

		logger->log("LOAD: Loaded World Chunks");

		//world->player->setParentChunk(world->chunks);
	}

	float finishTime = (float)(std::clock()) / (float)(CLOCKS_PER_SEC / 1000);

	logger->log("LOAD: LOAD COMPLETED. FINISHED AT " + std::to_string(finishTime) + " (" + std::to_string(finishTime - startTime) + " milliseconds)", true);

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
	logger->log("SAVE: Starting World Save to File: " + world->getName() + ".bin (Full path: " + SAVES_PATH + world->getName() + ".bin)");
	logger->log("SAVE: Starting Save Preparations");

	SaveDataTypes::EntityPlayerData p = Singletons::getEntityManager()->getPlayer()->getPlayerSaveData();

	// POD
	/*EntityPlayer* playerPtr = world->getPlayer();
	p.sanity = playerPtr->m_sanity;
	p.health = playerPtr->m_health;
	p.thirst = playerPtr->m_thirst;
	p.hunger = playerPtr->m_hunger;
	p.exhaustion = playerPtr->m_exhaustion;
	p.stamina = playerPtr->m_stamina;
	p.position = playerPtr->getPosition();
	/*for(unsigned int i = 0; i < 10; i++) {
	    p.favouriteItemIndices[i] = world->getPlayer()->m_inventory->getItemIndex(world->getPlayer()->m_favouriteItems[i]);
	}*/ /// TODO: Re-enable

	/*// INVENTORY
	InventoryData pInventory;
	for(unsigned int i = 0; i < pInventory.itemData.size(); i++) {
	    ItemData item;
	    Item* tempI = world->getPlayer()->m_inventory->getItem(i);
	    item.id = tempI->getID();
	    item.quantity = tempI->getQuantity();
	    item.metaData = tempI->getMetaData();
	    pInventory.itemData.push_back(item);
	}

	p.inventory = pInventory;*/

	logger->log("SAVE: Player Data Prepared");

	// CHUNK
	const unsigned int chunks = world->getSize() / CHUNK_SIZE;

	SaveDataTypes::ChunkData* chunkData = new SaveDataTypes::ChunkData[chunks];
	for(unsigned int i = 0; i < chunks; i++) {
		for(int y = 0; y < WORLD_HEIGHT; y++) {
			for(int x = 0; x < CHUNK_SIZE; x++) {
				for(int k = 0; k < WORLD_DEPTH; k++) {
					chunkData[i].tiles[x][y][k] = world->getTile(x + i * CHUNK_SIZE, y, k)->getSaveData(); // This gives data for ALL layers (recursive)
				}
			}
		}
		chunkData[i].biomeID = world->m_biomesMap[i];
	}

	logger->log("SAVE: Chunk Data Prepared");

	unsigned long int time = world->getTime();

	logger->log("SAVE: ALL DATA PREPARED, STARTING SAVE", true);

	// INIT
	std::ofstream file(SAVES_PATH + world->getName() + ".bin", std::ios::binary);
	if(file.fail()) {
		GLEngine::fatalError("Error saving to file: " + SAVES_PATH + world->getName() + ".bin");
	}

	{
		// VERSION
		unsigned int saveLen = m_saveVersion.size();

		file.write(reinterpret_cast<char*>(&saveLen), sizeof(unsigned int));
		file.write(m_saveVersion.c_str(), m_saveVersion.size());

		logger->log("SAVE: Started Saving with version: " + m_saveVersion);
	}

	{
		// MISCELLANEOUS WORLD DATA
		file.write(reinterpret_cast<char*>(&time), sizeof(float));
	}

	{
		// PLAYER
		p.save(file);
		logger->log("SAVE: Wrote Player POD");

	}

	{
		// WORLD
		for(int i = 0; i < chunks; i++) {
			//file.write(reinterpret_cast<char*>(chunkData), sizeof(ChunkData) * WORLD_SIZE);
			chunkData[i].save(file);
		}
		delete[] chunkData;
		logger->log("SAVE: Wrote World Chunks");
	}

	logger->log("SAVE: SAVE COMPLETED", true);;

	file.close();
}

void WorldIOManager::P_createWorld(unsigned int seed, std::string worldName, bool isFlat, unsigned int width) {

	World* w = new World(width, WORLD_HEIGHT, WORLD_DEPTH);

	float startTime = (float)(std::clock()) / (float)(CLOCKS_PER_SEC / 1000);
	logger->log("CREATE: Starting world creation at time: " + std::to_string(startTime));

	setProgress(0.0f);

	w->m_name = worldName;

	PerlinNoise placeNoise(seed);

	const unsigned int chunks = w->getSize() / CHUNK_SIZE;
	float places[chunks];
	float highestPlace, lowestPlace;

	for(int i = 0; i < chunks; i++) { // 0.05 progress

		float place = placeNoise.noise(i / 2.0f, 0.8, 0.3);

		places[i] = place;

		bool lowest = true;
		bool highest = true;
		for(int j = 0; j < i; j++) {
			if(place > places[j]) lowest = false;
			if(place < places[j]) highest = false;
		}
		if(lowest) lowestPlace = place;
		if(highest) highestPlace = place;

		setMessage("Now generating biomes... \n(" + std::to_string(i) + "/" + std::to_string((w->getSize() / CHUNK_SIZE)) + ")");

		setProgress(0.05f / (w->getSize() / CHUNK_SIZE) * (i + 1));

	}

	setMessage("Finishing chunk initialization...");

	for(int i = 0; i < chunks; i++) {// 0.05 progress
		float placeMapped = (places[i] - lowestPlace) / (highestPlace - lowestPlace);

		w->m_biomesMap[i] = std::ceil(placeMapped * (XMLModule::XMLData::getTotalBiomes() - 1));

		setProgress(0.05f + 0.05f / (chunks) * (i + 1));
	}

	std::vector<int> blockHeights;
	blockHeights.resize(w->getSize() * WORLD_DEPTH);

	std::vector<int> tempHeights;
	tempHeights.resize(w->getSize() * WORLD_DEPTH);

	// progress is at 0.1

	if(!isFlat) {
		// Set the block heights in each chunk
		setMessage("Setting block heights...");

		for(int i = 0; i < chunks; i++) {
			for(int layer = 0; layer < WORLD_DEPTH; layer++) {

				XMLModule::BiomeData biome = XMLModule::XMLData::getBiomeData(w->m_biomesMap[i]);

				PerlinNoise heightNoise(seed * (i + 1) * 783);

				for(int j = 0; j < CHUNK_SIZE; j++) {
					float flatness = biome.flatness;

					float extra = heightNoise.noise((j) * flatness / CHUNK_SIZE, 8.5, 3.7);
					//extra += heightNoise.noise(i * j * i, i, 5.8);

					unsigned int maxHeightDiff = biome.maxHeightDiff;

					extra -= 0.5f;
					extra *= maxHeightDiff;
					//extra /= Category_Data::placeData[(int)w->chunks[i]->getPlace()].flatness;

					float height = std::floor(extra);

					tempHeights[layer * CHUNK_SIZE + j + i * CHUNK_SIZE * WORLD_DEPTH] = height;

					setMessage("Setting block heights... \n(" + std::to_string(i * CHUNK_SIZE * WORLD_DEPTH + j + layer * CHUNK_SIZE) + "/" + std::to_string(w->getSize()  * WORLD_DEPTH) + ")");
					setProgress(0.1f + 0.069f / (WORLD_DEPTH * w->getSize()) * ((w->getSize()  * layer) + (i * CHUNK_SIZE) + (j + 1)));  // Ends at 0.169f;
				}
			}
		}

		{
			setMessage("Smoothing terrain... ");
			for(int chunk = 0; chunk < chunks; chunk++) {

				// Get base height values of current, next, and previous chunks
				unsigned int previousBase = XMLModule::XMLData::getBiomeData(w->m_biomesMap[((chunk - 1) + (chunks)) % (chunks)]).baseHeight;
				unsigned int nextBase = XMLModule::XMLData::getBiomeData(w->m_biomesMap[chunk + 1] % (chunks)).baseHeight;
				unsigned int baseHeight = XMLModule::XMLData::getBiomeData(w->m_biomesMap[chunk]).baseHeight;

				for(int layer = 0; layer < WORLD_DEPTH; layer++) {
					for(int x = 0; x < CHUNK_SIZE; x++) {
						/// 1. Figure out how 'deep' this x value is in its biome
						short int centreDepth = (CHUNK_SIZE / 2); // Short ints for speed?

						// How far in it is (to the right, not back). Centre is CHUNK_SIZE/2, very outsides are 0.
						short int chunkDepthInv = std::abs(x - centreDepth);
						short int chunkDepth = centreDepth - chunkDepthInv;

						/// 2. Interpolate
						unsigned int blockIndex = layer * CHUNK_SIZE + x + chunk * CHUNK_SIZE * WORLD_DEPTH;

						blockHeights[blockIndex] = tempHeights[blockIndex];

						if(chunkDepth / (float)CHUNK_SIZE <= 2.0f / 3.0f) {
							blockHeights[blockIndex] *= (chunkDepth / (float)CHUNK_SIZE) * 3.0f; // I promise this makes sense. *3.0f to account for the 1/3
						} // Each third of a chunk should be flattened

						if(x > centreDepth) { // current and next chunks smoothen
							blockHeights[blockIndex] += nextBase * (chunkDepthInv / (float)CHUNK_SIZE); // ChunkDepthInv/CHUNK_SIZE -> edges=1/2, centre=0
							blockHeights[blockIndex] += baseHeight * ((chunkDepth / (float)CHUNK_SIZE) + 0.5f); // ChunkDepth/CHUNK_SIZE -> edges=0, centre=1/2
						} else if(x < centreDepth) {
							blockHeights[blockIndex] += previousBase * (chunkDepthInv / (float)CHUNK_SIZE); // ChunkDepthInv/CHUNK_SIZE -> edges=1/2, centre=0
							blockHeights[blockIndex] += baseHeight * ((chunkDepth / (float)CHUNK_SIZE) + 0.5f); // ChunkDepth/CHUNK_SIZE -> edges=0, centre=1/2
						} else {
							blockHeights[blockIndex] += baseHeight;
						}

						setProgress(0.169f + 0.069f / (WORLD_DEPTH * w->getSize()) * ((w->getSize()  * layer) + ((x + CHUNK_SIZE * chunk) + 1)));  // Ends at 0.238;
					}
				}
			}
		}

		tempHeights.clear();

		{

			// m_progress at 0.4f

			setMessage("Placing blocks...");

			for(int chunk = 0; chunk < (chunks); chunk++) {
				for(int layer = 0; layer < WORLD_DEPTH; layer++) {
					for(int x = 0; x < CHUNK_SIZE; x++) {
						unsigned int blockIndex = layer * CHUNK_SIZE + x + chunk * CHUNK_SIZE * WORLD_DEPTH;
						for(int y = 0; y < blockHeights[blockIndex]; y++) {
							glm::vec2 pos = glm::vec2(x + chunk * CHUNK_SIZE, y);
							if(y < blockHeights[blockIndex] - 1 - 5 || y <= WATER_LEVEL) {
								Tile* tile = Factory::createTile((unsigned int)TileIDs::STONE, pos, layer);
								w->setTile_noEvent(tile);
							} else if(y < blockHeights[blockIndex] - 1 && y > WATER_LEVEL) {
								Tile* tile = Factory::createTile((unsigned int)TileIDs::DIRT, pos, layer);
								w->setTile_noEvent(tile);
							} else if(y < blockHeights[blockIndex] && y > WATER_LEVEL) {
								Tile* tile = Factory::createTile((unsigned int)TileIDs::GRASS, pos, layer);
								w->setTile_noEvent(tile);
								int r = std::rand(); /// TODO: Re-enable flowers
								if(r % 2 == 0) {
									//BlockBush* flower = new BlockBush(glm::vec2(x, y + 1), layer, MetaData(), false);
									//w->setTile_noEvent(flower);
								}
							}
							//setMessage("Placing blocks... (" + std::to_string(layer * WORLD_SIZE * WORLD_HEIGHT + x * WORLD_HEIGHT + y) + "/" + std::to_string((WORLD_DEPTH * WORLD_SIZE * WORLD_HEIGHT)) + ")");
							//setProgress(0.238f + 0.752f / (WORLD_DEPTH * WORLD_SIZE * WORLD_HEIGHT) * ((WORLD_SIZE * WORLD_HEIGHT * layer) + (WORLD_HEIGHT * x) + (y + 1))); // Ends at 0.99
						}
						for(int y = blockHeights[blockIndex]; y < WORLD_HEIGHT; y++) {
							glm::vec2 pos = glm::vec2(x + chunk * CHUNK_SIZE, y);
							if(y <= WATER_LEVEL) {
								Tile* tile = Factory::createTile((unsigned int)TileIDs::WATER, pos, layer);
								w->setTile_noEvent(tile);
							} else {
								Tile* tile = Factory::createTile((unsigned int)TileIDs::AIR, pos, layer);
								w->setTile_noEvent(tile);
							}
							//setMessage("Placing blocks... (" + std::to_string(layer * WORLD_SIZE * WORLD_HEIGHT + x * WORLD_HEIGHT + y) + "/" + std::to_string((WORLD_DEPTH * WORLD_SIZE * WORLD_HEIGHT)) + ")");
							//setProgress(0.238f + 0.752f / (WORLD_DEPTH * WORLD_SIZE * WORLD_HEIGHT) * ((WORLD_SIZE * WORLD_HEIGHT * layer) + (WORLD_HEIGHT * x) + (y + 1))); // Ends at 0.99
						}
					}
				}
			}
		}
	} else {
		for(int layer = 0; layer < WORLD_DEPTH; layer++) {
			for(int k = 0; k < w->getSize(); k++) {
				blockHeights[k + layer * w->getSize()] = 10;
				for(int j = blockHeights[k + layer * w->getSize() ]; j < WORLD_HEIGHT; j++) {
					Tile* tile = Factory::createTile((unsigned int)TileIDs::AIR, glm::vec2(k, j), layer);
					w->setTile_noEvent(tile);
				}
				for(int j = 0; j < blockHeights[k + layer * w->getSize()]; j++) {
					if(j < blockHeights[k + layer * w->getSize()] - 1 - 5) {
						Tile* tile = Factory::createTile((unsigned int)TileIDs::STONE, glm::vec2(k, j), layer);
						w->setTile_noEvent(tile);
					} else if(j < blockHeights[k + layer * w->getSize()] - 1) {
						Tile* tile = Factory::createTile((unsigned int)TileIDs::DIRT, glm::vec2(k, j), layer);
						w->setTile_noEvent(tile);
					} else {
						Tile* tile = Factory::createTile((unsigned int)TileIDs::GRASS, glm::vec2(k, j), layer);
						w->setTile_noEvent(tile);
						int r = std::rand() % 2;
						if(r == 0) { /// TODO: Re-enable flowers
							//BlockBush* flower = new BlockBush(glm::vec2(k, j + 1), layer, MetaData(), false);
							//w->setTile_noEvent(flower); /// TODO: Cross-layer worldgen
						}
					}
					setMessage("Placing blocks... \n(" + std::to_string(k * WORLD_HEIGHT + layer * w->getSize() * WORLD_HEIGHT) + "/" + std::to_string(w->getSize() * WORLD_HEIGHT * WORLD_DEPTH) + ")");
				}
			}
		}
	}

	setProgress(1.0f);

	float endTime = (float)(std::clock()) / (float)(CLOCKS_PER_SEC / 1000);
	logger->log("CREATE: Finished world creation at time: " + std::to_string(endTime) + " (Elapsed: " + std::to_string(endTime - startTime) + "ms)");

	return;

	//world->player = new Player(glm::vec2(5.0f, (blockHeights[5] + 5)), m_input, m_sq);
}

void WorldIOManager::setWorldEra(unsigned int newEraID) {
	/**
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
	**/

	// Save under a different filename (.name_numberoferaenum)
	/*std::string worldName = "." + world->m_name + "_" + std::to_string((unsigned int)world->m_worldEra);
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
	                        world->setTile_noEvent(new Tile(glm::vec2(chunkX + CHUNK_SIZE * x, y), 0, TileIDs::AIR, MetaData(), false)); /// Implement cross-layer stuff
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

	                                            world->setTile_noEvent(new Tile(glm::vec2(chunkX, y), 0, TileIDs::AIR, MetaData(), false)); /// TODO: Implement cross-layer structure stuff

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
	                        world->setTile_noEvent(new Tile(glm::vec2(chunkX + x * CHUNK_SIZE, y), 0, TileIDs::STONE, MetaData(), false));/// TODO: Cross-layer
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
	                        world->setTile_noEvent(new Tile(glm::vec2(chunkX + x * CHUNK_SIZE, y+blockHeights[chunkX + x * CHUNK_SIZE]), 0, TileIDs::DIRT, MetaData(), false));/// TODO: Cross-layer

	                    }
	                }
	            }
	        }
	    } // Layering's done!
	} else if(difference < 0) { // Moving backwards through time, simply load an earlier save
	    std::string oldWorldName = "." + world->m_name + "_" + std::to_string((unsigned int)newEra);
	    P_loadWorld(oldWorldName, world);
	}*/
}

/*StructureData WorldIOManager::loadStructureFromFile(std::string& filepath) {
    /**
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

                world->setTile_noEvent(new Tile(glm::vec2(tileX, tileY), 0, structure.tiles[tileIndex].id, MetaData(), false));/// TODO: Cross-layer
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

#endif // DEV_CONTROLS*/
