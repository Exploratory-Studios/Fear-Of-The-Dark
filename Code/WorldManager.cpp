#include "WorldManager.h"

WorldManager::WorldManager()
{

}

WorldManager::~WorldManager()
{
    //dtor
}

void WorldManager::init() {
    World* world = nullptr;
    world = m_worldIOManager->getWorld();

    if(world) {
        m_entityManager.init(world->player, world->entities);
    }
}

void WorldManager::update() {
    activateChunks();
    for(int i = 0; i < m_activatedChunks.size(); i++) {
        m_activatedChunks[i].update();
    }
}

void WorldManager::draw(GLEngine::SpriteBatch& sb) {
    for(int i = 0; i < m_activatedChunks.size(); i++) {
        m_activatedChunks[i].draw(sb);
    }
}

/// Private Functions

void WorldManager::activateChunks() {

    int chunkIndex = std::floor(m_player->getPosition().x);

    if(chunkIndex != m_lastActivated) { // Make sure that we changed chunks
        m_activatedChunks.clear(); // I forgot I had this at my disposal :)
        m_activatedChunks.push_back(m_chunks[chunkIndex]);
    }
}
