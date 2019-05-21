#include "EntityManager.h"

#include "Chunk.h"

EntityManager::EntityManager(Chunk* parent)
{
    m_parentChunk = parent;
}

EntityManager::~EntityManager()
{
    //dtor
}

void EntityManager::update(float timeStep, Chunk* chunks[WORLD_SIZE]) {
    for(unsigned int i = 0; i < m_entities.size(); i++) {
        m_entities[i]->update(timeStep, chunks);
        int newChunk = m_entities[i]->setParentChunk(chunks);
        if(newChunk != -1) {
            m_parentChunk->getSurroundingChunks()[newChunk]->addEntity(m_entities[i]);
            removeEntity(i);
        }
        m_entities[i]->collide();
    }
}

void EntityManager::draw(GLEngine::SpriteBatch& sb, float time, float xOffset) {
    for(unsigned int i = 0; i < m_entities.size(); i++) {
        m_entities[i]->draw(sb, time, xOffset);
    }
}

void EntityManager::tick(Player* p) {
    targetEntities(p);
}

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
                    m_entities[i]->setTargets(pathfindToTarget(m_entities[i]->getJumpHeight(), m_entities[i]->getPosition(), m_entities[j]->getPosition()));
                }
            }
        }
        for(unsigned int j = 0; j < m_parentChunk->getSurroundingChunks()[0]->getEntities().size(); j++) {
            if(i != j) {
                if((int) m_parentChunk->getSurroundingChunks()[0]->getEntities()[i]->getFaction() > (int)Categories::Faction::NEUTRAL &&
                   (int) m_parentChunk->getSurroundingChunks()[0]->getEntities()[j]->getFaction() <= (int)Categories::Faction::NEUTRAL) {
                     m_parentChunk->getSurroundingChunks()[0]->getEntities()[i]->setTargets(pathfindToTarget(m_parentChunk->getSurroundingChunks()[0]->getEntities()[i]->getJumpHeight(),  m_parentChunk->getSurroundingChunks()[0]->getEntities()[i]->getPosition(),  m_parentChunk->getSurroundingChunks()[0]->getEntities()[j]->getPosition()));
                } else if((int) m_parentChunk->getSurroundingChunks()[0]->getEntities()[i]->getFaction() < (int)Categories::Faction::NEUTRAL &&
                   (int) m_parentChunk->getSurroundingChunks()[0]->getEntities()[j]->getFaction() > (int)Categories::Faction::NEUTRAL) {
                     m_parentChunk->getSurroundingChunks()[0]->getEntities()[i]->setTargets(pathfindToTarget(m_parentChunk->getSurroundingChunks()[0]->getEntities()[i]->getJumpHeight(),  m_parentChunk->getSurroundingChunks()[0]->getEntities()[i]->getPosition(),  m_parentChunk->getSurroundingChunks()[0]->getEntities()[j]->getPosition()));
                }
            }
        }
        for(unsigned int j = 0; j < m_parentChunk->getSurroundingChunks()[1]->getEntities().size(); j++) {
            if(i != j) {
                if((int) m_parentChunk->getSurroundingChunks()[1]->getEntities()[i]->getFaction() > (int)Categories::Faction::NEUTRAL &&
                   (int) m_parentChunk->getSurroundingChunks()[1]->getEntities()[j]->getFaction() <= (int)Categories::Faction::NEUTRAL) {
                     m_parentChunk->getSurroundingChunks()[1]->getEntities()[i]->setTargets(pathfindToTarget(m_parentChunk->getSurroundingChunks()[1]->getEntities()[i]->getJumpHeight(), m_parentChunk->getSurroundingChunks()[1]->getEntities()[i]->getPosition(), m_parentChunk->getSurroundingChunks()[1]->getEntities()[j]->getPosition()));
                } else if((int) m_parentChunk->getSurroundingChunks()[1]->getEntities()[i]->getFaction() < (int)Categories::Faction::NEUTRAL &&
                   (int) m_parentChunk->getSurroundingChunks()[1]->getEntities()[j]->getFaction() > (int)Categories::Faction::NEUTRAL) {
                     m_parentChunk->getSurroundingChunks()[1]->getEntities()[i]->setTargets(pathfindToTarget(m_parentChunk->getSurroundingChunks()[1]->getEntities()[i]->getJumpHeight(), m_parentChunk->getSurroundingChunks()[1]->getEntities()[i]->getPosition(), m_parentChunk->getSurroundingChunks()[1]->getEntities()[j]->getPosition()));
                }
            }
        }
    }

    m_entities.pop_back();
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
                if(currentChunk->tiles[yGridspace+yOff-1][xGridspace % CHUNK_SIZE]->isSolid()) { // If the block is solid
                    if(!currentChunk->tiles[yGridspace+yOff][xGridspace % CHUNK_SIZE]->isSolid() && !currentChunk->tiles[yGridspace+yOff+1][xGridspace % CHUNK_SIZE]->isSolid()) { // And the block on top isn't
                        //if(yOff != 0 || targets.size() == 0) { // And we're not just going in a straight line
                            targetNavigated = true;
                            targets.push_back(glm::vec2(xGridspace * TILE_SIZE, (yGridspace+yOff) * TILE_SIZE));
                            yGridspace += yOff;
                        //}
                    }
                }
            }
        }
        for(int yOff = 0; yOff >= -SAFE_FALL_DIST && yGridspace+yOff+2 < WORLD_HEIGHT; yOff--) {
            if(!targetNavigated) {
                if(currentChunk->tiles[yGridspace+yOff][xGridspace % CHUNK_SIZE]->isSolid()) { // If the block is solid
                    if(!currentChunk->tiles[yGridspace+yOff+1][xGridspace % CHUNK_SIZE]->isSolid() && !currentChunk->tiles[yGridspace+yOff+2][xGridspace % CHUNK_SIZE]->isSolid()) { // And the block on top isn't
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
                            if(!chunks[chunkIndex]->tiles[yGridspace+yOff][i - CHUNK_SIZE * chunkIndex]->isSolid() &&
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
