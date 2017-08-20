#include "EntityManager.h"

EntityManager::EntityManager()
{
    //ctor
}

EntityManager::~EntityManager()
{
    //dtor
}

void EntityManager::init(Player& player, std::vector<Entity>& entities) {

}

void EntityManager::update(std::vector<Chunk>& activatedChunks) {
    for(auto e : m_entities) {
        e->update();
    }
    moveEntities();
    spawnEntities();
    collideEntities(activatedChunks);
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

}

void EntityManager::spawnEntities() {

}
