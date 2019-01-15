#include "WorldManager.h"

#include <random>

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
        std::vector<TalkingNPC*> talkingEntities;

        talkingEntities.emplace_back(new TalkingNPC(glm::vec2(10 * TILE_SIZE, 15 * TILE_SIZE), 0, &m_audioManager));

        for(unsigned int i = 0; i < m_worldIOManager->getWorld()->entities.size(); i++) {
            entities.push_back(&m_worldIOManager->getWorld()->entities[i]);
        }

        for(unsigned int i = 0; i < m_worldIOManager->getWorld()->talkingEntities.size(); i++) {
            talkingEntities.push_back(&m_worldIOManager->getWorld()->talkingEntities[i]);
        }

        m_player = &m_worldIOManager->getWorld()->player;

        {
            int index = (m_player->getPosition().x / TILE_SIZE) / CHUNK_SIZE / WORLD_SIZE;
            m_player->setParentChunk(m_worldIOManager->getWorld()->chunks[index%WORLD_SIZE]);
        }

        {
            for(int i = 0; i < entities.size(); i++) {
                int index = (entities[i]->getPosition().x / TILE_SIZE) / CHUNK_SIZE;
                entities[i]->setParentChunk(m_worldIOManager->getWorld()->chunks[index]);
                m_worldIOManager->getWorld()->chunks[index]->addEntity(entities[i]);
            }
        }

        {
            for(int i = 0; i < talkingEntities.size(); i++) {
                int index = (talkingEntities[i]->getPosition().x / TILE_SIZE) / CHUNK_SIZE;
                talkingEntities[i]->setParentChunk(m_worldIOManager->getWorld()->chunks[index]);
                m_worldIOManager->getWorld()->chunks[index]->addTalkingEntity(talkingEntities[i]);
            }
        }
    }

    m_questManager = new QuestManager(ASSETS_FOLDER_PATH + "Questing/DialogueList.txt", ASSETS_FOLDER_PATH + "Questing/FlagList.txt");
}
#include <iostream>
void WorldManager::update(GLEngine::Camera2D* worldCamera, float timeStepVariable, float time, GLEngine::InputManager& input, GLEngine::GUI& gui) {
    for(unsigned int i = 0; i < m_activatedChunks.size(); i++) {
        int xOffset = std::abs(m_activatedChunks[i] + WORLD_SIZE) % WORLD_SIZE;
        m_worldIOManager->getWorld()->chunks[xOffset]->update(time, timeStepVariable, m_worldIOManager->getWorld()->chunks);
    }

    // Set player caninteract

    if(m_player) {
        m_player->update(timeStepVariable, m_worldIOManager->getWorld()->chunks);
        m_player->updateMouse(worldCamera);
        m_player->collide();
        if(m_player->getTalkingEntity() && m_questManager) {
            m_player->getTalkingEntity()->setDialogueStarted(m_questManager->isDialogueStarted());

            if(m_player->getTalkingEntity()->isDialogueStarted()) {
                startDialogue();
            }
        }
        activateChunks();
    }

    m_questManager->update(input, gui);


}

void WorldManager::tick(float* tickTime) {
    for(unsigned int i = 0; i < m_activatedChunks.size(); i++) {
        int xOffset = std::abs(m_activatedChunks[i] + WORLD_SIZE) % WORLD_SIZE;
        m_worldIOManager->getWorld()->chunks[xOffset]->tick(tickTime, m_player);
    }
    //m_entityManager.tick(tickTime, m_worldIOManager->getWorld()->chunks);

    if(!m_audioManager.isMusicPlaying()) {
        //m_audioManager.playMorningSong(0);
    }
}
#include <iostream>
void WorldManager::draw(GLEngine::SpriteBatch& sb, GLEngine::DebugRenderer& dr, int tickTime, GLEngine::GLSLProgram* program) {
    sb.begin();

    for(unsigned int i = 0; i < m_activatedChunks.size(); i++) {
        int xOffset = std::abs(m_activatedChunks[i] + WORLD_SIZE) % WORLD_SIZE;
        m_worldIOManager->getWorld()->chunks[xOffset]->draw(sb, m_activatedChunks[i] - xOffset, tickTime);
    }

    m_player->draw(sb, tickTime, 0);

    sb.end();
    sb.renderBatch();
}

void WorldManager::drawGUI(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, GLEngine::GUI& gui) {
    m_questManager->draw(gui);
    m_player->drawGUI(sb, sf);
}

void WorldManager::startDialogue() {
    m_questManager->setQuestionId(m_player->getTalkingEntity()->getQuestionId());
    m_questManager->setDialogueStarted(true);
}

/// Private Functions

#include <iostream>
void WorldManager::activateChunks() {
    const int chunkIndex = m_player->getChunkIndex();

    if(chunkIndex != m_lastActivated && chunkIndex >= 0) { // Make sure that we changed chunks
        m_activatedChunks.clear(); // I forgot I had this at my disposal :)

        const signed int each = std::ceil((VIEW_DIST - 1) / 2); // How many chunks on each side of the centre of the selection

        for(signed int i = -each; i <= each; i++) {
            //if(chunkIndex + i >= 0) {
                m_activatedChunks.push_back(chunkIndex + i);

                int realIndex = (chunkIndex + i + WORLD_SIZE) % WORLD_SIZE;

                m_worldIOManager->getWorld()->chunks[realIndex]->activateChunk();
            //} else if(chunkIndex + i < 0) {
            //    m_activatedChunks.push_back(((chunkIndex + i) % WORLD_SIZE) + WORLD_SIZE);
            //}
        }

        m_lastActivated = chunkIndex;
        //std::cout << chunkIndex << std::endl;
    }
}
