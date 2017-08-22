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

void EntityManager::update(std::vector<int>& activatedChunks, Chunk* chunks[WORLD_SIZE]) {
    for(auto e : m_entities) {
        e->update(chunks);
    }
    moveEntities();
    spawnEntities();
    collideEntities(activatedChunks, chunks);
}

void EntityManager::draw(GLEngine::SpriteBatch& sb, GLEngine::DebugRenderer& dr) {
    for(auto e : m_entities) {
        e->draw(sb, dr);
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

void EntityManager::moveEntities() {
    for(auto e : m_entities) {
        //e->setPosition(e->getPosition() + glm::vec2(0.0f, -0.6f));
    }
}

void EntityManager::spawnEntities() {

}
