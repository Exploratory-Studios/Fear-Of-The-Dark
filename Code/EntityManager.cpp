#include "EntityManager.h"

#include "Chunk.h"
#include "Entities.h"

EntityManager::EntityManager(Chunk* parent, AudioManager* audio)
{
    m_parentChunk = parent;
    m_audioManager = audio;

    m_entities = new std::vector<Entity>();
}

EntityManager::~EntityManager()
{
    //dtor
}

void EntityManager::update(float timeStep, Chunk* chunks[WORLD_SIZE]) {
    for(unsigned int i = 0; i < getNumEntities(); i++) {
        getEntity(i)->update(timeStep, chunks);
        int newChunk = getEntity(i)->setParentChunk(chunks);
        if(newChunk == -1) {
            m_parentChunk->getSurroundingChunks()[0]->addEntity(*getEntity(i));
            removeEntity(i);
        } else if(newChunk == 1) {
            m_parentChunk->getSurroundingChunks()[1]->addEntity(*getEntity(i));
            removeEntity(i);
        }
        getEntity(i)->collide();
    }
}

void EntityManager::draw(GLEngine::SpriteBatch& sb, float time, float xOffset) {
    for(unsigned int i = 0; i < getNumEntities(); i++) {
        getEntity(i)->draw(sb, time, xOffset);
    }
}

void EntityManager::tick(Player* p, float tickTime, WorldEra& era) {
    for(unsigned int i = 0; i < getNumEntities(); i++) {
        getEntity(i)->tick(p);
    }
    spawnEntities(tickTime, era);
    targetEntities(p);
}

void EntityManager::removeEntity(int index) {
    for(unsigned int i = index; i < getNumEntities()-1; i++) {
        *getEntity(i) = *getEntity(i+1);
    }
    m_entities->pop_back();
}

void EntityManager::targetEntities(Player* p) {
    m_entities->push_back(*p);

    for(unsigned int i = 0; i < getNumEntities(); i++) {
        for(unsigned int j = 0; j < getNumEntities(); j++) {
            if(i != j) {
                if((int)getEntity(i)->getFaction() > (int)Categories::Faction::NEUTRAL &&
                   (int)getEntity(j)->getFaction() <= (int)Categories::Faction::NEUTRAL) {
                    getEntity(i)->setTargets(pathfindToTarget(getEntity(i)->getJumpHeight(), getEntity(i)->getPosition(), getEntity(j)->getPosition()));
                } else if((int)getEntity(i)->getFaction() < (int)Categories::Faction::NEUTRAL &&
                   (int)getEntity(j)->getFaction() > (int)Categories::Faction::NEUTRAL) {
                    getEntity(i)->setTargets(pathfindToTarget(getEntity(i)->getJumpHeight(), getEntity(i)->getPosition(), getEntity(j)->getPosition()));
                }
            }
        }
        for(unsigned int j = 0; j < m_parentChunk->getSurroundingChunks()[0]->getEntities()->size(); j++) {
            if(i != j) {
                if((int) getEntity(i)->getFaction() > (int)Categories::Faction::NEUTRAL &&
                   (int) m_parentChunk->getSurroundingChunks()[0]->getEntity(j)->getFaction() <= (int)Categories::Faction::NEUTRAL) {
                     getEntity(i)->setTargets(pathfindToTarget(getEntity(i)->getJumpHeight(),  getEntity(i)->getPosition(),  m_parentChunk->getSurroundingChunks()[0]->getEntity(j)->getPosition()));
                } else if((int) getEntity(i)->getFaction() < (int)Categories::Faction::NEUTRAL &&
                   (int) m_parentChunk->getSurroundingChunks()[0]->getEntity(j)->getFaction() > (int)Categories::Faction::NEUTRAL) {
                     getEntity(i)->setTargets(pathfindToTarget(getEntity(i)->getJumpHeight(),  getEntity(i)->getPosition(),  m_parentChunk->getSurroundingChunks()[0]->getEntity(j)->getPosition()));
                }
            }
        }
        for(unsigned int j = 0; j < m_parentChunk->getSurroundingChunks()[1]->getEntities()->size(); j++) {
            if(i != j) {
                if((int) getEntity(i)->getFaction() > (int)Categories::Faction::NEUTRAL &&
                   (int) m_parentChunk->getSurroundingChunks()[1]->getEntity(j)->getFaction() <= (int)Categories::Faction::NEUTRAL) {
                     getEntity(i)->setTargets(pathfindToTarget(getEntity(i)->getJumpHeight(), getEntity(i)->getPosition(), m_parentChunk->getSurroundingChunks()[1]->getEntity(j)->getPosition()));
                } else if((int) getEntity(i)->getFaction() < (int)Categories::Faction::NEUTRAL &&
                   (int) m_parentChunk->getSurroundingChunks()[1]->getEntity(j)->getFaction() > (int)Categories::Faction::NEUTRAL) {
                     getEntity(i)->setTargets(pathfindToTarget(getEntity(i)->getJumpHeight(), getEntity(i)->getPosition(), m_parentChunk->getSurroundingChunks()[1]->getEntity(j)->getPosition()));
                }
            }
        }
    }

    m_entities->pop_back();
}

std::vector<glm::vec2> EntityManager::pathfindToTarget(float jumpHeight, glm::vec2 originalPosition, glm::vec2 targetPosition) {
    std::vector<glm::vec2> targets;

    Chunk* chunks[3] = { nullptr };
    Chunk* currentChunk = m_parentChunk;

    chunks[0] = m_parentChunk->getSurroundingChunks()[0];
    chunks[1] = m_parentChunk;
    chunks[2] = m_parentChunk->getSurroundingChunks()[1];

    int maxJumpHeight = 2.5*(jumpHeight*jumpHeight) / TILE_SIZE * 2;

    // Find grid-space coords:
    int xGridspace = std::floor(originalPosition.x / TILE_SIZE);
    int yGridspace = std::floor(originalPosition.y / TILE_SIZE);
    int targetXGridspace = std::floor(targetPosition.x / TILE_SIZE);

    // Find chunk index
    int chunkIndex = std::floor(xGridspace / CHUNK_SIZE);
    int chunkIndexN = std::floor(targetXGridspace / CHUNK_SIZE);

    //int currentYOffset = 0;
    while(!(abs(xGridspace - targetXGridspace) <= 0.0f)) {

        {
            int a = chunkIndex, b = chunkIndexN;

            if(abs(a-b) > 1) {
                if(a < b) {
                    a += WORLD_SIZE;
                } else {
                    b += WORLD_SIZE;
                }
            }

            if(a > b) {
                targetXGridspace = chunkIndex * CHUNK_SIZE - 2;
            } else if(b > a) {
                targetXGridspace = (chunkIndex + 1) * CHUNK_SIZE + 2;
            }

        }

        if(xGridspace < targetXGridspace) {
            xGridspace++;
        } else {
            xGridspace--;
        }

        if(std::floor(xGridspace / CHUNK_SIZE) > chunkIndex) {
            currentChunk = chunks[2];
        } else if(std::floor(xGridspace / CHUNK_SIZE) < chunkIndex) {
            currentChunk = chunks[0];
        }

        bool targetNavigated = false; // True if the path is found.
        for(int yOff = 0; yOff <= maxJumpHeight+1 && yGridspace+yOff+1 < WORLD_HEIGHT; yOff++) {
            if(!targetNavigated) {
                if(yGridspace + yOff - 1 >= 0) {
                    if(currentChunk->tiles[yGridspace+yOff-1][(xGridspace + CHUNK_SIZE) % CHUNK_SIZE]->isSolid()) { // If the block is solid
                        if(!currentChunk->tiles[yGridspace+yOff][(xGridspace + CHUNK_SIZE) % CHUNK_SIZE]->isSolid() && !currentChunk->tiles[yGridspace+yOff+1][(xGridspace + CHUNK_SIZE) % CHUNK_SIZE]->isSolid()) { // And the block on top isn't
                            //if(yOff != 0 || targets.size() == 0) { // And we're not just going in a straight line
                                targetNavigated = true;
                                targets.push_back(glm::vec2(xGridspace * TILE_SIZE, (yGridspace+yOff) * TILE_SIZE));
                                yGridspace += yOff;
                            //}
                        }
                    }
                }
            }
        }
        for(int yOff = 0; yOff >= -SAFE_FALL_DIST && yGridspace+yOff+2 < WORLD_HEIGHT; yOff--) {
            if(!targetNavigated) {
                if(yGridspace + yOff >= 0) {
                    if(currentChunk->tiles[yGridspace+yOff][(xGridspace + CHUNK_SIZE) % CHUNK_SIZE]->isSolid()) { // If the block is solid
                        if(!currentChunk->tiles[yGridspace+yOff+1][(xGridspace + CHUNK_SIZE) % CHUNK_SIZE]->isSolid() && !currentChunk->tiles[yGridspace+yOff+2][(xGridspace + CHUNK_SIZE) % CHUNK_SIZE]->isSolid()) { // And the block on top isn't
                            //if(yOff != 0 || targets.size() == 0) { // And we're not just going in a straight line
                                targetNavigated = true;
                                targets.push_back(glm::vec2(xGridspace * TILE_SIZE, (yGridspace+yOff) * TILE_SIZE));
                                yGridspace += yOff;
                            //}
                        }
                    }
                }
            }
        }
    }

        /*chunkIndex -= m_parentChunk->getIndex();
        if(targetXGridspace > xGridspace) {
            for(int i = xGridspace+1; i < targetXGridspace; i++) {
                neighbouring = false;
                if(std::abs(chunkIndex - chunkIndexN) <= 1) {
                    neighbouring = true;
                } else {
                    if(chunkIndex == 0 || chunkIndexN == WORLD_SIZE - 1) {
                        if(chunkIndexN == 0 || chunkIndexN == WORLD_SIZE - 1) {
                            neighbouring = true;
                        }
                    }
                }

                if(neighbouring == false) {
                    break;
                }

                // Update coords
                if(i >= CHUNK_SIZE * (chunkIndex+1)) chunkIndex++;
                if(i < CHUNK_SIZE * chunkIndex) chunkIndex--;
                if(chunkIndex < 0) chunkIndex = WORLD_SIZE-1;
                if(chunkIndex >= WORLD_SIZE) chunkIndex = 0;

                // Make sure there's room to walk
                bool targetFound = false;
                lastOffsetY = offsetY;
                for(int yOff = offsetY; yOff < maxJumpHeight; yOff++) {
                    if(!targetFound) {
                        if(yGridspace+yOff-1 >= 0 && yGridspace+yOff+1 < WORLD_HEIGHT) {
                            if(!chunks\[chunkIndex].tiles[yGridspace+yOff][i - CHUNK_SIZE * chunkIndex]->isSolid() &&
                               !chunks[chunkIndex]->tiles[yGridspace+yOff+1][i - CHUNK_SIZE * chunkIndex]->isSolid()) {
                                if(chunks[chunkIndex]->tiles[yGridspace+yOff-1][i - CHUNK_SIZE * chunkIndex]->isSolid()) {
                                    if(offsetY != yOff || i == targetXGridspace-1) // Is it just a straight line?
                                        targets.push_back(glm::vec2(i*TILE_SIZE, (yGridspace+yOff)*TILE_SIZE));
                                    offsetY = yOff;
                                    targetFound = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if(!targetFound) {
                    offsetY = lastOffsetY;
                    for(int yOff = offsetY; yOff + yGridspace >= 0; yOff--) {
                        if(yGridspace+yOff-1 >= 0 && yGridspace+yOff+1 < WORLD_HEIGHT) {
                            if(!chunks[chunkIndex]->tiles[yGridspace+yOff][i - CHUNK_SIZE * chunkIndex]->isSolid() &&
                               !chunks[chunkIndex]->tiles[yGridspace+yOff+1][i - CHUNK_SIZE * chunkIndex]->isSolid()) {
                                if(chunks[chunkIndex]->tiles[yGridspace+yOff-1][i - CHUNK_SIZE * chunkIndex]->isSolid()) {
                                    if(offsetY != yOff || i == targetXGridspace-1) // Is it a straight line?
                                        targets.push_back(glm::vec2(i*TILE_SIZE, (yGridspace+yOff)*TILE_SIZE));
                                    offsetY = yOff;
                                    targetFound = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        } else {
            for(int i = xGridspace-1; i > targetXGridspace; i--) {
                // Update coords
                if(i >= CHUNK_SIZE * (chunkIndex+1)) chunkIndex++;
                if(i < CHUNK_SIZE * chunkIndex) chunkIndex--;
                if(chunkIndex < 0) chunkIndex = WORLD_SIZE-1;
                if(chunkIndex >= WORLD_SIZE) chunkIndex = 0;

                // Make sure there's room to walk
                bool targetFound = false;
                lastOffsetY = offsetY;
                for(int yOff = offsetY; yOff < maxJumpHeight; yOff++) {
                    if(!targetFound) {
                        if(yGridspace+yOff-1 >= 0 && yGridspace+yOff+1 < WORLD_HEIGHT) {
                            if(!chunks[chunkIndex]->tiles[yGridspace+yOff][i - CHUNK_SIZE * chunkIndex]->isSolid() &&
                               !chunks[chunkIndex]->tiles[yGridspace+yOff+1][i - CHUNK_SIZE * chunkIndex]->isSolid()) {
                                if(chunks[chunkIndex]->tiles[yGridspace+yOff-1][i - CHUNK_SIZE * chunkIndex]->isSolid()) {
                                    if(offsetY != yOff || i == targetXGridspace+1) // Is it a straight line?
                                        targets.push_back(glm::vec2(i*TILE_SIZE, (yGridspace+yOff)*TILE_SIZE));
                                    offsetY = yOff;
                                    targetFound = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if(!targetFound) {
                    offsetY = lastOffsetY;
                    for(int yOff = offsetY; yOff + yGridspace >= 0; yOff--) {
                        if(yGridspace+yOff-1 >= 0 && yGridspace+yOff+1 < WORLD_HEIGHT) {
                            if(!chunks[chunkIndex]->tiles[yGridspace+yOff][i - CHUNK_SIZE * chunkIndex]->isSolid() &&
                               !chunks[chunkIndex]->tiles[yGridspace+yOff+1][i - CHUNK_SIZE * chunkIndex]->isSolid()) {
                                if(chunks[chunkIndex]->tiles[yGridspace+yOff-1][i - CHUNK_SIZE * chunkIndex]->isSolid()) {
                                    if(offsetY != yOff || i == targetXGridspace+1) // Is it a straight line?
                                        targets.push_back(glm::vec2(i*TILE_SIZE, (yGridspace+yOff)*TILE_SIZE));
                                    offsetY = yOff;
                                    targetFound = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }*/
    /*
    for(int i = 0; i < targets.size(); i++) {
        std::cout << "X: " << targets[i].x << " Y: " << targets[i].y << std::endl;
    }
    std::cout << "Original X: " << xGridspace << " Original Y: " << yGridspace << std::endl;
    */

    return targets;
}

void EntityManager::spawnEntities(float tickTime, WorldEra& era) {
    int randomNum = std::rand() % 1000;

    if(randomNum < SPAWN_RATE) {
        if(m_entities->size() < MAX_CHUNK_ENTITIES) {
            std::vector<unsigned int> spawnables; // Entities that can be spawned in this era and biome, at this time.

            DayPortion currentPortion;
            { // Get the current daytime
                float hour = (float)((int)tickTime % DAY_LENGTH) / (float)DAY_LENGTH + 0.5f;

                if(hour > 4.0f/24.0f && hour < 9.0f/24.0f) { // Morning (4am-9am)
                    currentPortion = DayPortion::MORNING;
                } else if(hour > 9.0f/24.0f && hour < 16.0f/24.0f) { // Day (9am-4pm)
                    currentPortion = DayPortion::DAY;
                } else if(hour > 16.0f/24.0f && hour < 21.0f/24.0f) { // Afternoon (4pm-9pm)
                    currentPortion = DayPortion::AFTERNOON;
                } else if(hour > 21.0f/24.0f || hour < 4.0f/24.0f) { // Night (9pm-4am) (Overlap requires OR logic)
                    if(hour >= 0.0f && hour <= 1.0f/24.0f) { // Nightmare hour (Midnight-1am)
                        currentPortion = DayPortion::NIGHTMARE_HOUR;
                    } else {
                        currentPortion = DayPortion::NIGHT;
                    }
                }
            }

            for(unsigned int i = 0; i < Category_Data::placeData[(unsigned int)m_parentChunk->getPlace()].mobSpawnIds.size(); i++) { // Go through all the criteria
                unsigned int id = Category_Data::placeData[(unsigned int)m_parentChunk->getPlace()].mobSpawnIds[i];
                for(int j = 0; j < Category_Data::mobData[id].spawnEra.size(); j++) {
                    if(Category_Data::mobData[id].spawnEra[j] == era) {
                        for(int k = 0; k < Category_Data::mobData[id].spawnTime.size(); k++) {
                            if(Category_Data::mobData[id].spawnTime[k] == currentPortion) {
                                spawnables.push_back(id);
                            }
                        }
                    }
                }
            }

            if(spawnables.size() > 0) { // Is anything actually spawnable?
                // Now we have to find a suitable place for a random mob to spawn.
                // First, choose the random mob
                randomNum = std::rand() % spawnables.size();

                int sizeX = Category_Data::mobData[spawnables[randomNum]].size.x, sizeY = Category_Data::mobData[spawnables[randomNum]].size.y; // in tile size

                // Next, find a place
                for(int i = 0; i < CHUNK_SIZE; i++) { // Test a bunch of random spots
                    int x = std::rand() % CHUNK_SIZE;
                    for(int y = 0; y < WORLD_HEIGHT; y++) {
                        if(!m_parentChunk->tiles[y][x]->isSolid()) {
                            bool isSafe = true;
                            if(!m_parentChunk->tiles[y-1][x]->isSolid()) {
                                isSafe = false;
                            }
                            for(int xMod = 0; xMod < sizeX-1; xMod++) {
                                for(int yMod = 0; yMod < sizeY-1; yMod++) {
                                    if(m_parentChunk->tiles[y+yMod][x+xMod]->isSolid()) {
                                        isSafe = false;
                                    }
                                }
                            }
                            if(isSafe) {
                                m_entities->push_back(*createEntity(spawnables[randomNum], glm::vec2(x, y), m_parentChunk, m_audioManager)); /// TODO: Could have serious problem here
                                return;
                            }
                        }
                    }
                }
            }
        }
    }
}
