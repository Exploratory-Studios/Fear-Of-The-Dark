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

        m_player = &m_worldIOManager->getWorld()->player;
        m_entityManager.init(m_player, entities);
    }
}

void WorldManager::update() {
    activateChunks();

    for(int i = 0; i < m_activatedChunks.size(); i++) {
        m_worldIOManager->getWorld()->chunks[m_activatedChunks[i]]->update();
    }

    m_entityManager.update(m_activatedChunks, m_worldIOManager->getWorld()->chunks);
}

void WorldManager::draw(GLEngine::SpriteBatch& sb, GLEngine::DebugRenderer& dr) {
    for(int i = 0; i < m_activatedChunks.size(); i++) {
        m_worldIOManager->getWorld()->chunks[m_activatedChunks[i]]->draw(sb);
    }
    m_entityManager.draw(sb, dr);
}

/// Private Functions
void WorldManager::activateChunks() {

    int chunkIndex = std::floor(m_player->getPosition().x / TILE_SIZE / CHUNK_SIZE);

    if(chunkIndex != m_lastActivated && chunkIndex >= 0) { // Make sure that we changed chunks
        m_activatedChunks.clear(); // I forgot I had this at my disposal :)
        const int viewDist = 3; // Must be odd

        const int each = (viewDist - 1) / 2;

        for(int i = -each; i <= each; i++) {
            if(chunkIndex + i >= 0 && chunkIndex + i < WORLD_SIZE) {
                m_activatedChunks.push_back(chunkIndex + i);
            }
        }

        m_lastActivated = chunkIndex;
    }
}
