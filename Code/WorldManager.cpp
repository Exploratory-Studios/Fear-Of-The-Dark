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

        for(unsigned int i = 0; i < m_worldIOManager->getWorld()->entities.size(); i++) {
            entities.push_back(&m_worldIOManager->getWorld()->entities[i]);
        }

        m_player = &m_worldIOManager->getWorld()->player;
        //Entity* ent = new Entity(glm::vec2(80, 800), Categories::Entity_Type::MOB, 1);
        //WalkingNPC* ent = new WalkingNPC(glm::vec2(80, 800), 1);
        //entities.push_back(ent);

        m_entityManager.init(m_player, entities);
    }

    m_questManager = new QuestManager("../Assets/Dialogue/Quests.txt");
}
#include <iostream>
void WorldManager::update(GLEngine::Camera2D* worldCamera, float timeStepVariable, float time) {
    for(unsigned int i = 0; i < m_activatedChunks.size(); i++) {
        int xOffset = std::abs(m_activatedChunks[i] + WORLD_SIZE) % WORLD_SIZE;
        m_worldIOManager->getWorld()->chunks[xOffset]->update(time);
    }

    m_entityManager.update(m_activatedChunks, m_worldIOManager->getWorld()->chunks, worldCamera, timeStepVariable);

    activateChunks();
}

void WorldManager::tick(float tickTime) {
    for(unsigned int i = 0; i < m_activatedChunks.size(); i++) {
        int xOffset = std::abs(m_activatedChunks[i] + WORLD_SIZE) % WORLD_SIZE;
        m_worldIOManager->getWorld()->chunks[xOffset]->tick(tickTime);
    }
    m_entityManager.tick(tickTime, m_worldIOManager->getWorld()->chunks);
}
#include <iostream>
void WorldManager::draw(GLEngine::SpriteBatch& sb, GLEngine::DebugRenderer& dr, int tickTime, GLEngine::GLSLProgram* program) {
    sb.begin();

    for(unsigned int i = 0; i < m_activatedChunks.size(); i++) {
        int xOffset = std::abs(m_activatedChunks[i] + WORLD_SIZE) % WORLD_SIZE;

        m_worldIOManager->getWorld()->chunks[xOffset]->draw(sb, (m_activatedChunks[i] - xOffset));
    }

    sb.end();
    sb.renderBatch();

    sb.begin();

    m_entityManager.draw(sb, dr, tickTime, program);

    sb.end();
    sb.renderBatch();
}

/// Private Functions

#include <iostream>
void WorldManager::activateChunks() {

    int chunkIndex = m_player->getChunkIndex();
    //if((int)m_player->getPosition().x == 0) chunkIndex = 0; // Just to avoid 0 / whatever. It just creates issues :(

    if(chunkIndex != m_lastActivated && chunkIndex >= 0) { // Make sure that we changed chunks
        m_activatedChunks.clear(); // I forgot I had this at my disposal :)

        const int each = (VIEW_DIST - 1) / 2; // How many chunks on each side of the centre of the selection

        for(int i = -each; i <= each; i++) {
            //if(chunkIndex + i >= 0) {
                m_activatedChunks.push_back(chunkIndex + i);
            //} else if(chunkIndex + i < 0) {
            //    m_activatedChunks.push_back(((chunkIndex + i) % WORLD_SIZE) + WORLD_SIZE);
            //}
        }

        m_lastActivated = chunkIndex;
        //std::cout << chunkIndex << std::endl;
    }
}
