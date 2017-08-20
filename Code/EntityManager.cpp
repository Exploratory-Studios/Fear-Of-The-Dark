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

void EntityManager::update(std::vector<Chunk>& activatedChunks) {
    for(auto e : m_entities) {
        e->update();
    }
    moveEntities();
    spawnEntities();
    collideEntities(activatedChunks);
}

void EntityManager::draw(GLEngine::SpriteBatch& sb) {
    for(auto e : m_entities) {
        e->draw(sb);
    }
}

/// PRIVATE FUNCTIONS

void EntityManager::collideEntities(std::vector<Chunk>& activatedChunks) {
    for(unsigned int i = 0; i < activatedChunks.size(); i++) {
        for(auto e : m_entities) {
            e->collide(m_entities, activatedChunks[i].tiles);
        }
    }
}

void EntityManager::moveEntities() {
    for(auto e : m_entities) {
        e->setPosition(e->getPosition() + glm::vec2(0.0f, -0.1f));
    }
}

void EntityManager::spawnEntities() {

}
