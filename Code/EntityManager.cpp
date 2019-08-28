#include "EntityManager.h"

#include "Chunk.h"
#include "Entities.h"

EntityManager::EntityManager(Chunk* parent, AudioManager* audio)
{
    m_parentChunk = parent;
    m_audioManager = audio;
}

EntityManager::~EntityManager()
{
    for(int i = 0; i < m_entities.size(); i++) {
        removeEntity(m_entities.size()-1);
    }
}

void EntityManager::update(float timeStep, Chunk* chunks[WORLD_SIZE], Player* p) {
    for(unsigned int i = 0; i < m_entities.size(); i++) {
        m_entities[i]->update(timeStep, chunks);
        int newChunk = m_entities[i]->setParentChunk(chunks);
        if(newChunk == -1) {
            m_parentChunk->getSurroundingChunks()[0]->addEntity(m_entities[i]);
            removeEntity(i);
        } else if(newChunk == 1) {
            m_parentChunk->getSurroundingChunks()[1]->addEntity(m_entities[i]);
            removeEntity(i);
        }
        m_entities[i]->collide();

        if(m_entities[i]->isDead()) {
            delete m_entities[i];
            removeEntity(i);
        }
        targetEntities(p);
    }
}

void EntityManager::draw(GLEngine::SpriteBatch& sb, float time, float xOffset) {
    for(unsigned int i = 0; i < m_entities.size(); i++) {
        m_entities[i]->draw(sb, time, xOffset);
    }
}

void EntityManager::tick(Player* p, float tickTime, WorldEra& era) {
    for(unsigned int i = 0; i < m_entities.size(); i++) {
        m_entities[i]->tick(p);
    }
    spawnEntities(tickTime, era);
}

#ifdef DEBUG
void EntityManager::drawDebug(GLEngine::DebugRenderer& dr, int xOffset) {
    for(int i = 0; i < m_entities.size(); i++) {
        if(m_entities[i]->getType() != Categories::Entity_Type::PLAYER) {
            for(int j = 0; j < m_entities[i]->getTargets().size(); j++) {
                dr.drawBox(glm::vec4(m_entities[i]->getTargets()[j].x + (xOffset * CHUNK_SIZE), m_entities[i]->getTargets()[j].y, 1.0f, 1.0f), GLEngine::ColourRGBA8(255, 255, 255, 255), 0.0f);
                if(j < m_entities[i]->getTargets().size()-1) {
                    dr.drawLine(m_entities[i]->getTargets()[j], m_entities[i]->getTargets()[j+1], GLEngine::ColourRGBA8(0, 255, 0, 255));
                }
            }
        }
    }
}
#endif // DEBUG

void EntityManager::removeEntity(int index) {
    for(unsigned int i = index; i < m_entities.size()-1; i++) {
        m_entities[i] = m_entities[i+1];
    }
    m_entities.pop_back();
}

void EntityManager::targetEntities(Player* p) {
    m_entities.push_back(p);

    for(unsigned int i = 0; i < m_entities.size(); i++) {
        for(unsigned int j = 0; j < m_entities.size(); j++) {
            if(i != j) {
                if((int)m_entities[i]->getFaction() > (int)Categories::Faction::NEUTRAL &&
                   (int)m_entities[j]->getFaction() <= (int)Categories::Faction::NEUTRAL) {
                    m_entities[i]->setTargets(pathfindToTarget(m_entities[i]->getJumpHeight(), m_entities[i]->getPosition(), m_entities[j]->getPosition()));
                } else if((int)m_entities[i]->getFaction() < (int)Categories::Faction::NEUTRAL &&
                   (int)m_entities[j]->getFaction() > (int)Categories::Faction::NEUTRAL) {
                    //m_entities[j]->setTargets(pathfindToTarget(m_entities[i]->getJumpHeight(), m_entities[i]->getPosition(), m_entities[j]->getPosition()));
                }
            }
        }
        for(unsigned int j = 0; j < m_parentChunk->getSurroundingChunks()[0]->getEntities().size(); j++) {
            if(i != j) {
                if((int) m_entities[i]->getFaction() > (int)Categories::Faction::NEUTRAL &&
                   (int) m_parentChunk->getSurroundingChunks()[0]->getEntities()[j]->getFaction() <= (int)Categories::Faction::NEUTRAL) {
                     m_entities[i]->setTargets(pathfindToTarget(m_entities[i]->getJumpHeight(),  m_entities[i]->getPosition(),  m_parentChunk->getSurroundingChunks()[0]->getEntities()[j]->getPosition()));
                } else if((int) m_entities[i]->getFaction() < (int)Categories::Faction::NEUTRAL &&
                   (int) m_parentChunk->getSurroundingChunks()[0]->getEntities()[j]->getFaction() > (int)Categories::Faction::NEUTRAL) {
                     //m_entities[j]->setTargets(pathfindToTarget(m_entities[i]->getJumpHeight(),  m_entities[i]->getPosition(),  m_parentChunk->getSurroundingChunks()[0]->getEntities()[j]->getPosition()));
                }
            }
        }
        for(unsigned int j = 0; j < m_parentChunk->getSurroundingChunks()[1]->getEntities().size(); j++) {
            if(i != j) {
                if((int) m_entities[i]->getFaction() > (int)Categories::Faction::NEUTRAL &&
                   (int) m_parentChunk->getSurroundingChunks()[1]->getEntities()[j]->getFaction() <= (int)Categories::Faction::NEUTRAL) {
                     m_entities[i]->setTargets(pathfindToTarget(m_entities[i]->getJumpHeight(), m_entities[i]->getPosition(), m_parentChunk->getSurroundingChunks()[1]->getEntities()[j]->getPosition()));
                } else if((int) m_entities[i]->getFaction() < (int)Categories::Faction::NEUTRAL &&
                   (int) m_parentChunk->getSurroundingChunks()[1]->getEntities()[j]->getFaction() > (int)Categories::Faction::NEUTRAL) {
                     //m_entities[j]->setTargets(pathfindToTarget(m_entities[i]->getJumpHeight(), m_entities[i]->getPosition(), m_parentChunk->getSurroundingChunks()[1]->getEntities()[j]->getPosition()));
                }
            }
        }
    }

    m_entities.pop_back();
}

std::vector<glm::vec2> EntityManager::pathfindToTarget(float jumpHeight, glm::vec2 originalPosition, glm::vec2 targetPosition) {
    /*  Algorithm Below:
        1. Construct a 2d vector of all tiles between originalPos and target
        2. Move horizontally towards target
        3. If 'cursor' is solid, move up one, add target to return vector
        4. If 'cursor' is above a tile that's not solid, move down one
        5. If 'cursor' is at target, add target to return vector, return.
        6. Else, repeat 2-6

    */

    std::vector<glm::vec2> targets;

    std::vector<std::vector<bool>> solidTiles;

    Chunk* currentChunk = m_parentChunk;

    // Step 1: build vector of tiles' solidity
    // Step 1.1: figure out which direction to go!
    float dist1 = std::abs(originalPosition.x - targetPosition.x);
    float dist2 = (originalPosition.x < targetPosition.x ? originalPosition.x : targetPosition.x) + WORLD_SIZE * CHUNK_SIZE - (originalPosition.x >= targetPosition.x ? originalPosition.x : targetPosition.x);

    float leftDist = originalPosition.x > targetPosition.x ? dist1 : dist2; // Distance if we go left
    float rightDist = originalPosition.x <= targetPosition.x ? dist1 : dist2; // Distance if we go right

    unsigned int dist = leftDist < rightDist ? leftDist : rightDist;

    if(originalPosition.y < 0 || originalPosition.y > WORLD_HEIGHT-1) {
        return targets;
    }

    for(int i = 0; i < WORLD_HEIGHT; i++) {
        std::vector<bool> solid;
        for(int j = 0; j < dist; j++) {
            int xPos = (originalPosition.x < targetPosition.x ? originalPosition.x : targetPosition.x) + j;

            int cursorChunk = (int)(std::floor((float)xPos / (float)CHUNK_SIZE) + WORLD_SIZE) % WORLD_SIZE;

            Chunk* c = nullptr;

            while(currentChunk->getIndex() != cursorChunk) {
                currentChunk = currentChunk->getSurroundingChunks()[1];
            }
            c = currentChunk;

            if(!c) {
                Logger::getInstance()->log("WARNING: Pathfinding: Could not get cursor chunk at pos: " + std::to_string(xPos) + ", " + std::to_string(i), true);
                return std::vector<glm::vec2>{};
            }

            bool isSolid = c->getTile(xPos, i, 0) ? c->getTile(xPos, i, 0)->isSolid() : false; /// TODO: Cross-layer pathfinding

            solid.push_back(isSolid);
        }
        solidTiles.push_back(solid);
    }

    if(rightDist < leftDist) {
        int yOffset = 0;
        for(int x = 0; x < dist; x++) { // Step 2: Move horizontally
            bool target = false;
            while(solidTiles[originalPosition.y + yOffset][x]) {
                yOffset++; // Move cursor up until we find a space
                if(yOffset + originalPosition.y == WORLD_HEIGHT) {
                    return targets;
                }
            }
            while(!solidTiles[originalPosition.y + yOffset - 1][x]) {
                yOffset--;
                if(yOffset + originalPosition.y - 1 < 0) {
                    return targets;
                }
            }
            if(x + std::abs(yOffset) >= 1.0f) {
                targets.push_back(glm::vec2(originalPosition.x + x + 0.5f, originalPosition.y + yOffset + 0.5f));
            }
        }
    } else {
        int yOffset = 0;
        for(int x = 1; x <= dist; x++) { // Step 2: Move horizontally
            bool target = false;
            while(solidTiles[originalPosition.y + yOffset][dist - x]) {
                yOffset++; // Move cursor up until we find a space
                if(yOffset + (int)originalPosition.y == WORLD_HEIGHT) {
                    return targets;
                }
            }
            while(!solidTiles[originalPosition.y + yOffset - 1][dist - x]) {
                yOffset--;
                if(yOffset + (int)originalPosition.y - 1 < 0) {
                    return targets;
                }
            }
            if((dist-x) + std::abs(yOffset) >= 1.0f) {
                targets.push_back(glm::vec2(originalPosition.x - x + 0.5f, originalPosition.y + yOffset + 0.5f));
            }
        }
    }

    targets.push_back(targetPosition + glm::vec2(0.5f));

    /*

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
                                targets.push_back(glm::vec2(xGridspace, (yGridspace+yOff)));
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
                                targets.push_back(glm::vec2(xGridspace, (yGridspace+yOff)));
                                yGridspace += yOff;
                            //}
                        }
                    }
                }
            }
        }
    }*/

    return targets;
}

void EntityManager::spawnEntities(float tickTime, WorldEra& era) {
    int randomNum = std::rand() % 1000;

    if(randomNum < SPAWN_RATE) {
        if(m_entities.size() < MAX_CHUNK_ENTITIES) {
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

            /*for(unsigned int i = 0; i < Category_Data::placeData[(unsigned int)m_parentChunk->getPlace()].mobSpawnIds.size(); i++) { // Go through all the criteria
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
                        if(!m_parentChunk->getTile(x, y, 0)->isSolid()) { /// TODO: Cross-layer spawning
                            bool isSafe = true;
                            if(!m_parentChunk->getTile(x, y - 1, 0)->isSolid()) { /// Here too
                                isSafe = false;
                            }
                            for(int xMod = 0; xMod < sizeX-1; xMod++) {
                                for(int yMod = 0; yMod < sizeY-1; yMod++) {
                                    if(m_parentChunk->getTile(x+xMod, y+yMod, 0)->isSolid()) { /// And here
                                        isSafe = false;
                                    }
                                }
                            }
                            if(isSafe) {
                                m_entities.push_back(createEntity(spawnables[randomNum], glm::vec2(x, y), m_parentChunk, m_audioManager)); /// TODO: Could have serious problem here
                                return;
                            }
                        }
                    }
                }
            }*/ /// TODO: Bring this back to life
        }
    }
}
