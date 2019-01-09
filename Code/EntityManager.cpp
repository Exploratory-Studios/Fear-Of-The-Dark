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

void EntityManager::update(float timeStep) {
    for(int i = 0; i < m_entities.size(); i++) {
        m_entities[i]->update(timeStep);
    }
}

void EntityManager::draw(GLEngine::SpriteBatch& sb, float time) {
    for(int i = 0; i < m_entities.size(); i++) {
        m_entities[i]->draw(sb, time);
    }
}

void EntityManager::tick() {

}

bool EntityManager::isDialogueStarted() {
    for(int i = 0; i < m_talkingEntities.size(); i++) {
        if(m_talkingEntities[i]->isDialogueStarted()) {
            return true;
        }
    }
    return false;
}
