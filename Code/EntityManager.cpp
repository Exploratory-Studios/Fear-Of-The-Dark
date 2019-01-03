#include "EntityManager.h"

EntityManager::EntityManager()
{
    //ctor
}

EntityManager::~EntityManager()
{
    //dtor
}

void EntityManager::init(Player* player, std::vector<Entity*> entities) {
    m_player = player;
    m_entities = entities;

    m_entities.push_back(m_player);
}

void EntityManager::update(std::vector<int>& activatedChunks, Chunk* chunks[WORLD_SIZE], GLEngine::Camera2D* worldCamera, float timeStepVariable) {
    for(auto e : m_entities) {
        e->update(chunks, timeStepVariable);
    }
    m_player->updateMouse(chunks, worldCamera);
    moveEntities(timeStepVariable);
    for(int i = 0; i < timeStepVariable; i++) {
        collideEntities(activatedChunks, chunks);
    }
}

void EntityManager::tick(float dayCycleTime, Chunk* chunks[WORLD_SIZE]) { // Spawn if nighttime, each tick update ai
    spawnEntities();
    targetEntities(chunks);

    long int closestId = -1;
    float minDist = TILE_SIZE * 4.0f;

    if(m_speakingNpc) {
        m_speakingNpc->setDialogueActive(m_dialogueActive);
    }

    m_dialogueStarted = false;

    for(int i = 0; i < m_talkingNpcs.size(); i++) {
        if(m_talkingNpcs[i]->isDialogueStarted()) {
            m_speakingNpc = m_talkingNpcs[i];
            m_dialogueStarted = true;
            m_speakingNpc->m_dialogueStarted = false;
            break;
        }
        float dist = glm::distance(m_talkingNpcs[i]->getPosition(), m_player->getPosition());
        if(dist < minDist) {
            closestId = i;
            minDist = dist;
        }
    }

    if(closestId != -1) {
        m_player->setSpeakingEntity(m_talkingNpcs[closestId]);
    }
}

void EntityManager::draw(GLEngine::SpriteBatch& sb, GLEngine::DebugRenderer& dr, int tickTime, GLEngine::GLSLProgram* program) {
    for(auto e : m_entities) {
        e->draw(sb, tickTime, program);
    }
}

void EntityManager::addTalkingNpc(glm::vec2 position, unsigned int id) {
    TalkingNPC* npc = new TalkingNPC(position, id);

    m_talkingNpcs.push_back(npc);
    m_entities.push_back(npc);
}

/// PRIVATE FUNCTIONS
void EntityManager::collideEntities(std::vector<int>& activatedChunks, Chunk* chunks[WORLD_SIZE]) {
    for(auto e : m_entities) {
        for(unsigned int i = 0; i < activatedChunks.size(); i++) {
            e->collide(m_entities, activatedChunks[i]);
        }
    }
}

void EntityManager::moveEntities(float timeStepVariable) {
    for(auto e : m_entities) {
        e->move(timeStepVariable);
    }
}

void EntityManager::spawnEntities() {

}

void EntityManager::targetEntities(Chunk* chunks[WORLD_SIZE]) {
    for(unsigned int i = 0; i < m_entities.size(); i++) {
        for(unsigned int j = 0; j < m_entities.size(); j++) {
            if((int)m_entities[i]->getFaction() > (int)Categories::Faction::NEUTRAL &&
               (int)m_entities[j]->getFaction() <= (int)Categories::Faction::NEUTRAL) {
                m_entities[i]->setTargets(pathfindToTarget(m_entities[i]->getJumpHeight(), m_entities[i]->getPosition(), m_entities[j]->getPosition(), chunks));
            } else if((int)m_entities[i]->getFaction() < (int)Categories::Faction::NEUTRAL &&
               (int)m_entities[j]->getFaction() > (int)Categories::Faction::NEUTRAL) {
                m_entities[i]->setTargets(pathfindToTarget(m_entities[i]->getJumpHeight(), m_entities[i]->getPosition(), m_entities[j]->getPosition(), chunks));
            }
        }
    }
}

std::vector<glm::vec2> EntityManager::pathfindToTarget(float jumpHeight, glm::vec2 originalPosition, glm::vec2 targetPosition, Chunk* chunks[WORLD_SIZE]) {
    std::vector<glm::vec2> targets;

    int offsetY = 0;
    int lastOffsetY = 0;

    int maxJumpHeight = (jumpHeight * jumpHeight * 5.88f - 6.0f) / 8.0f + 1.0f;

    // Find grid-space coords:
    int xGridspace = std::floor(originalPosition.x / TILE_SIZE + 0.5f);
    int yGridspace = std::floor(originalPosition.y / TILE_SIZE + offsetY + 0.5f);
    int targetXGridspace = std::floor(targetPosition.x / TILE_SIZE + 0.5f);
    //int targetYGridspace = std::floor(targetPosition.y / TILE_SIZE + offsetY + 0.5f); **UNUSED**

    // Find chunk index
    int chunkIndex = std::floor(xGridspace / CHUNK_SIZE);

    // Traverse surface and find travel targets
    if(targetXGridspace > xGridspace) {
        for(int i = xGridspace+1; i < targetXGridspace; i++) {
            // Update coords
            if(i >= CHUNK_SIZE * (chunkIndex+1)) chunkIndex++;
            if(i < CHUNK_SIZE * chunkIndex) chunkIndex--;
            if(chunkIndex < 0) chunkIndex = WORLD_SIZE-1;
            if(chunkIndex >= WORLD_SIZE) chunkIndex = 0;

            // Make sure there's room to walk
            bool targetFound = false;
            lastOffsetY = offsetY;
            for(int yOff = offsetY; yOff < maxJumpHeight; yOff++) {
                if(!targetFound) {
                    if(yGridspace+yOff-1 >= 0 && yGridspace+yOff+1 < WORLD_HEIGHT) {
                        if(!chunks[chunkIndex]->tiles[yGridspace+yOff][i - CHUNK_SIZE * chunkIndex]->isSolid() &&
                           !chunks[chunkIndex]->tiles[yGridspace+yOff+1][i - CHUNK_SIZE * chunkIndex]->isSolid()) {
                            if(chunks[chunkIndex]->tiles[yGridspace+yOff-1][i - CHUNK_SIZE * chunkIndex]->isSolid()) {
                                if(offsetY != yOff || i == targetXGridspace-1) // Is it just a straight line?
                                    targets.push_back(glm::vec2(i*TILE_SIZE, (yGridspace+yOff)*TILE_SIZE));
                                offsetY = yOff;
                                targetFound = true;
                                break;
                            }
                        }
                    }
                }
            }
            if(!targetFound) {
                offsetY = lastOffsetY;
                for(int yOff = offsetY; yOff > -maxJumpHeight; yOff--) {
                    if(yGridspace+yOff-1 >= 0 && yGridspace+yOff+1 < WORLD_HEIGHT) {
                        if(!chunks[chunkIndex]->tiles[yGridspace+yOff][i - CHUNK_SIZE * chunkIndex]->isSolid() &&
                           !chunks[chunkIndex]->tiles[yGridspace+yOff+1][i - CHUNK_SIZE * chunkIndex]->isSolid()) {
                            if(chunks[chunkIndex]->tiles[yGridspace+yOff-1][i - CHUNK_SIZE * chunkIndex]->isSolid()) {
                                if(offsetY != yOff || i == targetXGridspace-1) // Is it a straight line?
                                    targets.push_back(glm::vec2(i*TILE_SIZE, (yGridspace+yOff)*TILE_SIZE));
                                offsetY = yOff;
                                targetFound = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
    } else {
        for(int i = xGridspace-1; i > targetXGridspace; i--) {
            // Update coords
            if(i >= CHUNK_SIZE * (chunkIndex+1)) chunkIndex++;
            if(i < CHUNK_SIZE * chunkIndex) chunkIndex--;
            if(chunkIndex < 0) chunkIndex = WORLD_SIZE-1;
            if(chunkIndex >= WORLD_SIZE) chunkIndex = 0;

            // Make sure there's room to walk
            bool targetFound = false;
            lastOffsetY = offsetY;
            for(int yOff = offsetY; yOff < maxJumpHeight; yOff++) {
                if(!targetFound) {
                    if(yGridspace+yOff-1 >= 0 && yGridspace+yOff+1 < WORLD_HEIGHT) {
                        if(!chunks[chunkIndex]->tiles[yGridspace+yOff][i - CHUNK_SIZE * chunkIndex]->isSolid() &&
                           !chunks[chunkIndex]->tiles[yGridspace+yOff+1][i - CHUNK_SIZE * chunkIndex]->isSolid()) {
                            if(chunks[chunkIndex]->tiles[yGridspace+yOff-1][i - CHUNK_SIZE * chunkIndex]->isSolid()) {
                                if(offsetY != yOff || i == targetXGridspace+1) // Is it a straight line?
                                    targets.push_back(glm::vec2(i*TILE_SIZE, (yGridspace+yOff)*TILE_SIZE));
                                offsetY = yOff;
                                targetFound = true;
                                break;
                            }
                        }
                    }
                }
            }
            if(!targetFound) {
                offsetY = lastOffsetY;
                for(int yOff = offsetY; yOff > -maxJumpHeight; yOff--) {
                    if(yGridspace+yOff-1 >= 0 && yGridspace+yOff+1 < WORLD_HEIGHT) {
                        if(!chunks[chunkIndex]->tiles[yGridspace+yOff][i - CHUNK_SIZE * chunkIndex]->isSolid() &&
                           !chunks[chunkIndex]->tiles[yGridspace+yOff+1][i - CHUNK_SIZE * chunkIndex]->isSolid()) {
                            if(chunks[chunkIndex]->tiles[yGridspace+yOff-1][i - CHUNK_SIZE * chunkIndex]->isSolid()) {
                                if(offsetY != yOff || i == targetXGridspace+1) // Is it a straight line?
                                    targets.push_back(glm::vec2(i*TILE_SIZE, (yGridspace+yOff)*TILE_SIZE));
                                offsetY = yOff;
                                targetFound = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    /*
    for(int i = 0; i < targets.size(); i++) {
        std::cout << "X: " << targets[i].x << " Y: " << targets[i].y << std::endl;
    }
    std::cout << "Original X: " << xGridspace << " Original Y: " << yGridspace << std::endl;
    */
    return targets;
}

void EntityManager::initResponses() {
    //std::ifstream responseFile("NPCResponses.txt");

    /*std::vector<std::vector<Dialogue>> responsesFull;
    std::vector<Dialogue> responsesTemp;

    int lineNum = 0;
    int dialogueAnswers = 0;
    int dialogues = 0;
    int lastDialogueEnd = 0;

    std::string line = "";

    while(std::getline(responseFile, line)) {

        Answers a;

        if(line == "END") {
            responseFull.push_back(responsesTemp);

            for(int i = 0; i < responsesTemp.size(); i++) {
                responsesTemp.pop_back();
            }

            lastDialogueEnd = lineNum;

            lineNum = 0;
            dialogueAnswers = 0;
            dialogues = 0;

        } else if(line[0] != '#') {
            if(lineNum == 0) {
                dialogues = std::stoi(line);
            } else if(lineNum == 1) {
                dialogueAnswers = std::stoi(line);
            } else {

            }

            lineNum++;
        }
    }*/
}
