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

        m_entityManager.init(m_player, entities);
    }

    m_questManager = new QuestManager("../Assets/Questing/DialogueList.txt", "../Assets/Questing/FlagList.txt");

    m_entityManager.addTalkingNpc(glm::vec2(5.0f, 90.0f) * glm::vec2(TILE_SIZE), 0);
}
#include <iostream>
void WorldManager::update(GLEngine::Camera2D* worldCamera, float timeStepVariable, float time) {
    for(unsigned int i = 0; i < m_activatedChunks.size(); i++) {
        int xOffset = std::abs(m_activatedChunks[i] + WORLD_SIZE) % WORLD_SIZE;
        m_worldIOManager->getWorld()->chunks[xOffset]->update(time);
    }

    bool active = m_entityManager.getDialogueActive();

    m_entityManager.setDialogueActive(m_questManager->isDialogueActive());
    m_entityManager.update(m_activatedChunks, m_worldIOManager->getWorld()->chunks, worldCamera, timeStepVariable);

    if(active != m_entityManager.getDialogueActive()) {
        m_player->setCanInteract(active);
    }

    m_dialogueStarted = m_entityManager.isDialogueStarted();

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

void WorldManager::startDialogue(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, GLEngine::InputManager& input, GLEngine::GUI& gui) {
    m_questManager->m_dialogueManager->startConversation(m_entityManager.getSpeakingNpc()->getQuestionId(), sb, sf, input, gui);
    m_entityManager.setDialogueActive(true);
    m_entityManager.getPlayer()->setCanInteract(false);
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
