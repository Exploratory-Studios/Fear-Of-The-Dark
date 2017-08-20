#include "WorldManager.h"

WorldManager::WorldManager()
{

}

WorldManager::~WorldManager()
{
    //dtor
}

void WorldManager::init(WorldIOManager* worldIOManager) {

    m_worldIOManager = worldIOManager;

    if(m_worldIOManager->getWorld()) {
        std::vector<Entity*> entities;

        for(int i = 0; i < m_worldIOManager->getWorld()->entities.size(); i++) {
            entities.push_back(&m_worldIOManager->getWorld()->entities[i]);
        }

        m_entityManager.init(&m_worldIOManager->getWorld()->player, entities);
        m_player = m_worldIOManager->getWorld()->player;

        for(int i = 0; i < WORLD_SIZE; i++) {
            m_chunks[i] = &m_worldIOManager->getWorld()->chunks[i];
        }
    }
}

void WorldManager::update() {
    activateChunks();

    std::vector<Chunk> activated;

    for(int i = 0; i < m_activatedChunks.size(); i++) {
        m_chunks[m_activatedChunks[i]]->update();
        activated.push_back(*m_chunks[m_activatedChunks[i]]);
    }

    m_entityManager.update(activated);
}

void WorldManager::draw(GLEngine::SpriteBatch& sb) {
    for(int i = 0; i < m_activatedChunks.size(); i++) {
        m_chunks[m_activatedChunks[i]]->draw(sb);
    }
    m_entityManager.draw(sb);
}
#include <iostream>
/// Private Functions
void WorldManager::activateChunks() {

    int chunkIndex = std::floor(m_player.getPosition().x / TILE_SIZE / CHUNK_SIZE);

    if(chunkIndex != m_lastActivated && chunkIndex >= 0) { // Make sure that we changed chunks
        m_activatedChunks.clear(); // I forgot I had this at my disposal :)
        m_activatedChunks.push_back(chunkIndex);
        m_lastActivated = chunkIndex;
        std::cout << "Added Chunk " << chunkIndex;
    }
}
