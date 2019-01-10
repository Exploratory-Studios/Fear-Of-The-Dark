#include "EntityManager.h"

//#include "Chunk.h"

EntityManager::EntityManager()
{
    //ctor
}

EntityManager::~EntityManager()
{
    //dtor
}

void EntityManager::update(float timeStep, Chunk* chunks[WORLD_SIZE]) {
    for(int i = 0; i < m_entities.size(); i++) {
        m_entities[i]->update(timeStep, chunks);
        m_entities[i]->collide();
    }
    for(int i = 0; i < m_talkingEntities.size(); i++) {
        m_talkingEntities[i]->update(timeStep, chunks);
    }
}

void EntityManager::draw(GLEngine::SpriteBatch& sb, float time) {
    for(int i = 0; i < m_entities.size(); i++) {
        m_entities[i]->draw(sb, time);
    }
    for(int i = 0; i < m_talkingEntities.size(); i++) {
        m_talkingEntities[i]->draw(sb, time);
    }
}

void EntityManager::tick() {

}
