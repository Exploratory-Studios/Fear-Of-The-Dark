#include "EntityManager.h"

EntityManager::EntityManager()
{
    //ctor
}

EntityManager::~EntityManager()
{
    //dtor
}

void EntityManager::init(Player* player, std::vector<Entity*> entities) {
    m_player = player;
    m_entities = entities;

    m_entities.push_back(m_player);
}

void EntityManager::update(std::vector<int>& activatedChunks, Chunk* chunks[WORLD_SIZE], GLEngine::Camera2D* worldCamera, float timeStepVariable) {
    for(auto e : m_entities) {
        e->update(chunks, timeStepVariable);
    }
    m_player->updateMouse(chunks, worldCamera);
    moveEntities(timeStepVariable);
    for(int i = 0; i < timeStepVariable; i++) {
        collideEntities(activatedChunks, chunks);
    }
}

void EntityManager::tick(float dayCycleTime, Chunk* chunks[WORLD_SIZE]) { // Spawn if nighttime, each tick update ai
    spawnEntities();
    targetEntities(chunks);
}

void EntityManager::draw(GLEngine::SpriteBatch& sb, GLEngine::DebugRenderer& dr, int tickTime) {
    for(auto e : m_entities) {
        e->draw(sb, tickTime);
    }
}

/// PRIVATE FUNCTIONS
void EntityManager::collideEntities(std::vector<int>& activatedChunks, Chunk* chunks[WORLD_SIZE]) {
    for(auto e : m_entities) {
        for(int i = 0; i < activatedChunks.size(); i++) {
            e->collide(m_entities, activatedChunks[i]);
        }
    }
}

void EntityManager::moveEntities(float timeStepVariable) {
    for(auto e : m_entities) {
        e->move(timeStepVariable);
    }
}

void EntityManager::spawnEntities() {

}

void EntityManager::targetEntities(Chunk* chunks[WORLD_SIZE]) {
    for(int i = 0; i < m_entities.size(); i++) {
        for(int j = 0; j < m_entities.size(); j++) {
            if((int)m_entities[i]->getFaction() > (int)Categories::Faction::NEUTRAL &&
               (int)m_entities[j]->getFaction() <= (int)Categories::Faction::NEUTRAL) {
                m_entities[i]->setTargets(pathfindToTarget(m_entities[i]->getJumpHeight(), m_entities[i]->getPosition(), m_entities[j]->getPosition(), chunks));
            } else if((int)m_entities[i]->getFaction() < (int)Categories::Faction::NEUTRAL &&
               (int)m_entities[j]->getFaction() > (int)Categories::Faction::NEUTRAL) {
                m_entities[i]->setTargets(pathfindToTarget(m_entities[i]->getJumpHeight(), m_entities[i]->getPosition(), m_entities[j]->getPosition(), chunks));
            }
        }
    }
}
#include <iostream>
std::vector<glm::vec2> EntityManager::pathfindToTarget(float jumpHeight, glm::vec2 originalPosition, glm::vec2 targetPosition, Chunk* chunks[WORLD_SIZE]) {
    std::vector<glm::vec2> targets;

    int offsetY = 0;
    int lastOffsetY = 0;

    int maxJumpHeight = (jumpHeight * jumpHeight * 5.88f - 6.0f) / 8.0f + 1.0f;

    // Find grid-space coords:
    int xGridspace = std::floor(originalPosition.x / TILE_SIZE + 0.5f);
    int yGridspace = std::floor(originalPosition.y / TILE_SIZE + offsetY + 0.5f);
    int targetXGridspace = std::floor(targetPosition.x / TILE_SIZE + 0.5f);
    int targetYGridspace = std::floor(targetPosition.y / TILE_SIZE + offsetY + 0.5f);

    // Find chunk index
    int chunkIndex = std::floor(xGridspace / CHUNK_SIZE);

    // Traverse surface and find travel targets
    if(targetXGridspace > xGridspace) {
        for(int i = xGridspace+1; i < targetXGridspace; i++) {
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
                for(int yOff = offsetY; yOff > -maxJumpHeight; yOff--) {
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
                for(int yOff = offsetY; yOff > -maxJumpHeight; yOff--) {
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
    }
    /*
    for(int i = 0; i < targets.size(); i++) {
        std::cout << "X: " << targets[i].x << " Y: " << targets[i].y << std::endl;
    }
    std::cout << "Original X: " << xGridspace << " Original Y: " << yGridspace << std::endl;
    */
    return targets;
}
