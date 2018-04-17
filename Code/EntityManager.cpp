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
        spawnEntities();
    }
}

void EntityManager::tick(float dayCycleTime) { // Spawn if nighttime, each tick update ai

}

void EntityManager::draw(GLEngine::SpriteBatch& sb, GLEngine::DebugRenderer& dr) {
    for(auto e : m_entities) {
        e->draw(sb);
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
