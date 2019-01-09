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

    m_audioManager.init();

    if(m_worldIOManager->getWorld()) {
        std::vector<Entity*> entities;

        for(unsigned int i = 0; i < m_worldIOManager->getWorld()->entities.size(); i++) {
            entities.push_back(&m_worldIOManager->getWorld()->entities[i]);
        }

        m_player = &m_worldIOManager->getWorld()->player;
    }

    m_questManager = new QuestManager(ASSETS_FOLDER_PATH + "Questing/DialogueList.txt", ASSETS_FOLDER_PATH + "Questing/FlagList.txt");
}
#include <iostream>
void WorldManager::update(GLEngine::Camera2D* worldCamera, float timeStepVariable, float time, GLEngine::InputManager& input, GLEngine::GUI& gui) {
    activateChunks();

    for(unsigned int i = 0; i < m_activatedChunks.size(); i++) {
        int xOffset = std::abs(m_activatedChunks[i] + WORLD_SIZE) % WORLD_SIZE;
        m_worldIOManager->getWorld()->chunks[xOffset]->update(time, timeStepVariable);
    }

    // Set player caninteract

    if(m_player->getTalkingEntity()->isDialogueStarted())) {
        startDialogue();
        m_player->getTalkingEntity()->setDialogueActive(true);
    }

    m_questManager->update(input, gui);


}

void WorldManager::tick(float* tickTime) {
    for(unsigned int i = 0; i < m_activatedChunks.size(); i++) {
        int xOffset = std::abs(m_activatedChunks[i] + WORLD_SIZE) % WORLD_SIZE;
        m_worldIOManager->getWorld()->chunks[xOffset]->tick(tickTime);
    }
    //m_entityManager.tick(tickTime, m_worldIOManager->getWorld()->chunks);

    if(!m_audioManager.isMusicPlaying()) {
        m_audioManager.playMorningSong(0);
    }
}
#include <iostream>
void WorldManager::draw(GLEngine::SpriteBatch& sb, GLEngine::DebugRenderer& dr, int tickTime, GLEngine::GLSLProgram* program) {
    sb.begin();

    for(unsigned int i = 0; i < m_activatedChunks.size(); i++) {
        int xOffset = std::abs(m_activatedChunks[i] + WORLD_SIZE) % WORLD_SIZE;

        m_worldIOManager->getWorld()->chunks[xOffset]->draw(sb, (m_activatedChunks[i] - xOffset), tickTime);
    }

    sb.end();
    sb.renderBatch();
}

void WorldManager::drawGUI(GLEngine::GUI& gui) {
    m_questManager->draw(gui);
}

void WorldManager::startDialogue() {
    m_questManager->setQuestionId(m_player->getTalkingNpc()->getQuestionId());
    m_questManager->setDialogueStarted(true);
}

/// Private Functions

#include <iostream>
void WorldManager::activateChunks() {

    int chunkIndex = m_player->getChunkIndex();

    if(chunkIndex != m_lastActivated && chunkIndex >= 0) { // Make sure that we changed chunks
        m_activatedChunks.clear(); // I forgot I had this at my disposal :)

        const signed int each = std::ceil((VIEW_DIST - 1) / 2); // How many chunks on each side of the centre of the selection

        for(signed int i = -each; i <= each; i++) {
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
