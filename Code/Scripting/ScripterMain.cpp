#include "ScripterMain.h"

#include <Camera2D.h>

Scripter::Scripter(WorldManager* worldManager) {
    init(worldManager);
}

void Scripter::init(WorldManager* worldManager) {

    m_worldManager = worldManager;

    for(int i = 0; i < WORLD_SIZE; i++) {
        m_chunks[i] = m_worldManager->m_worldIOManager->getWorld()->chunks[i];
    }

    for(unsigned int i = 0; i < WORLD_SIZE; i++) {
        for(unsigned int j = 0; j < m_chunks[i]->getEntities().size(); j++) {
            m_entities.push_back(m_chunks[i]->getEntities()[j]);
        }
    }

}

void Scripter::changeBlock(Block* newBlock) {
    int chunk = std::floor(newBlock->getPosition().x / CHUNK_SIZE); // What chunk index it belongs to
    int chunkX = (int)newBlock->getPosition().x % CHUNK_SIZE;            // What x in the chunk is

    if(chunkX == 0) {
        m_chunks[((chunk-1 + WORLD_SIZE) % WORLD_SIZE)]->extraTiles[(int)newBlock->getPosition().y][1] = newBlock;
    } else if(chunkX == CHUNK_SIZE-2) {
        m_chunks[((chunk+1 + WORLD_SIZE) % WORLD_SIZE)]->extraTiles[(int)newBlock->getPosition().y][0] = newBlock;
    }

    m_chunks[chunk]->setTile(newBlock, chunkX, newBlock->getPosition().y); // Set the block, of course
    /// TODO: compile array of chunks in init()
}

void Scripter::removeBlock(int x, int y) {
    int chunk = std::floor(x / CHUNK_SIZE); // What chunk index it belongs to
    int chunkX = (int)x % CHUNK_SIZE;            // What x in the chunk is

    m_chunks[chunk]->tiles[y][chunkX] = new BlockAir(m_chunks[chunk]->tiles[y][chunkX]->getPosition(), m_chunks[chunk]->tiles[y][chunkX]->getParentChunk());
}

void Scripter::showBlock(int x, int y) {
    int chunk = std::floor(x / CHUNK_SIZE); // What chunk index it belongs to
    int chunkX = (int)x % CHUNK_SIZE;            // What x in the chunk is

    m_chunks[chunk]->tiles[y][chunkX]->m_draw = true; // Make it not-transparent
}

void Scripter::hideBlock(int x, int y) {
    int chunk = std::floor(x / CHUNK_SIZE); // What chunk index it belongs to
    int chunkX = (int)x % CHUNK_SIZE;            // What x in the chunk is

    m_chunks[chunk]->tiles[y][chunkX]->m_draw = false; // Make it transparent
}

unsigned int Scripter::addEntity(Entity newEntity) {
    m_entities.push_back(&newEntity);
    return m_entities.size();
}

void Scripter::removeEntity(unsigned int index) {
    m_entities[index] = nullptr;
}

void Scripter::showEntity(unsigned int index) {
    m_entities[index]->m_transparent = false;
}

void Scripter::hideEntity(unsigned int index) {
    m_entities[index]->m_transparent = true;
}
#include <iostream>
void Scripter::update() {
    // Get active script from ScriptQueue
    // Execute script
    // Take script off active list
    // Repeat until no active scripts exist

    std::vector<Script*> scripts = m_worldManager->m_worldIOManager->getScriptQueue()->m_activeScripts;

    for(int i = 0; i < scripts.size(); i++) {
        executeScript(*scripts[i]);
    }
    m_worldManager->m_worldIOManager->getScriptQueue()->deactivateScripts();
}

/// PRIVATE FUNCTIONS

void Scripter::executeScript(Script& script) {
    for(int i = 0; i < script.commands.size(); i++) {
        executeCommand(script.commands[i]);
    }
}

void Scripter::executeCommand(std::string& command) {
    std::vector<std::string> parameters;

    std::string temp;
    for(int i = 0; i < command.length(); i++) {
        if(command[i] == ' ') {
            parameters.push_back(temp);
            temp = "";
        } else {
            temp += command[i];
        }
    }
    parameters.push_back(temp);

    //actual command is parameters[0], the rest are parameters

    // COMMANDS:
    // Format -> command [setChoice1][setChoice2][] (typeOfParameter)       Documentation
    // time [set] (integer)                                                 Sets time of day to 2nd parameter
    // removeBlock [relative][] (integer) (integer)                         Removes block at x=1st parameter, y=2nd parameter. "relative" makes coordinates relative to player
    // changeBlock [relative][] (integer) (integer) (integer)               Changes block at x=1st parameter, y=2nd parameter to one with id of 3rd parameter. "relative" makes coordinates relative to player
    // teleport [player] [relative][] (integer) (integer)                       Teleports the player to x=1st parameter, y=2nd parameter. "relative" makes coordinates relative to player
    // teleport [entity] [relative][] (integer) (integer)                   Teleports an entity to x=1st parameter, y=2nd parameter. "relative" makes coordinates relative to entity

    if(parameters[0] == "time") {
        if(parameters[1] == "set") {
            std::cout << "Setting time to " + parameters[2] << std::endl;
            *(m_worldManager->m_tickTime) = std::stoi(parameters[2]);
        }
    } else if(parameters[0] == "removeBlock") {
        unsigned int keywordIndex = 1;
        std::vector<glm::vec2> positions = positionTarget(parameters, keywordIndex);
        for(int i = 0; i < positions.size(); i++) {
            int x = positions[i].x;
            int y = positions[i].y;

            std::cout << "Removing block at: X=" + std::to_string(x) + ", Y=" + std::to_string(y) << std::endl;

            removeBlock(x, y);
        }
    } else if(parameters[0] == "changeBlock") {
        unsigned int keywordIndex = 1;
        std::vector<glm::vec2> positions = positionTarget(parameters, keywordIndex);
        for(int i = 0; i < positions.size(); i++) {
            int x = positions[i].x;
            int y = positions[i].y;

            std::cout << "Changing block at: X=" + std::to_string(x) + ", Y=" + std::to_string(y) << " to one with the id of: " << parameters[keywordIndex] << std::endl;

            int chunkIndex = std::floor(x / CHUNK_SIZE);
            Chunk* parent = m_chunks[chunkIndex];

            Block* block = createBlock(std::stoi(parameters[keywordIndex]), glm::vec2(x, y), parent);

            changeBlock(block);
        }
    } else if(parameters[0] == "teleport") {
        unsigned int keywordIndex = 1;
        std::vector<Entity*> entities = entityTarget(parameters, keywordIndex);
        std::vector<glm::vec2> positions = positionTarget(parameters, keywordIndex);

        for(int i = 0; i < entities.size(); i++) {
            for(int j = 0; j < positions.size(); j++) {
                entities[i]->setPosition(positions[j] * glm::vec2(TILE_SIZE));
                entities[i]->setParentChunk(m_chunks);
            }
        }
    }
}

/// PRIVATE, DON'T TOUCH!!!!!!!

std::vector<Entity*> Scripter::entityTarget(std::vector<std::string> parameters, unsigned int& keywordIndex) { // gets entity
    if(parameters[keywordIndex] == "near") {
        glm::vec2 position;
        keywordIndex += 1;
        position = positionTarget(parameters, keywordIndex)[0]; // for this specific command, we will never need to use multiple points (in a reasonable world)

        unsigned int chunkIndex = std::floor(position.x / TILE_SIZE / CHUNK_SIZE);

        float nearestDistance;
        unsigned int nearestId;
        unsigned int nearestChunkId;

        bool found = false;

        for(int i = 0; i < m_chunks[chunkIndex]->getAllEntities().size(); i++) {
            float dist = std::sqrt(std::abs(position.x - m_chunks[chunkIndex]->getAllEntities()[i]->getPosition().x) + std::abs(position.y - m_chunks[chunkIndex]->getAllEntities()[i]->getPosition().y));
            if(dist < nearestDistance || !found) {
                nearestDistance = dist;
                nearestId = i;
                nearestChunkId = chunkIndex;
                found = true;
            }
        }

        while(!found) {
            for(int j = 0; j < WORLD_SIZE / 2 - 1; j++) {
                for(int i = 0; i < m_chunks[(chunkIndex-i + WORLD_SIZE) % WORLD_SIZE]->getAllEntities().size(); i++) {
                    float dist = std::sqrt(std::abs(position.x - m_chunks[chunkIndex]->getAllEntities()[i]->getPosition().x) + std::abs(position.y - m_chunks[chunkIndex]->getAllEntities()[i]->getPosition().y));
                    if(dist < nearestDistance || !found) {
                        nearestDistance = dist;
                        nearestId = i;
                        nearestChunkId = chunkIndex;
                        found = true;
                    }
                }
                for(int i = 0; i < m_chunks[(chunkIndex+i + WORLD_SIZE) % WORLD_SIZE]->getAllEntities().size(); i++) {
                    float dist = std::sqrt(std::abs(position.x - m_chunks[chunkIndex]->getAllEntities()[i]->getPosition().x) + std::abs(position.y - m_chunks[chunkIndex]->getAllEntities()[i]->getPosition().y));
                    if(dist < nearestDistance || !found) {
                        nearestDistance = dist;
                        nearestId = i;
                        nearestChunkId = chunkIndex;
                        found = true;
                    }
                }
            }
        }

        std::vector<Entity*> ret;
        ret.push_back(m_chunks[nearestChunkId]->getAllEntities()[nearestId]);

        return ret;
    } else if(parameters[keywordIndex] == "area") { /// Not tested
        keywordIndex += 1;
        glm::vec2 position1 = positionTarget(parameters, keywordIndex)[0]; // Again, same as near, we'll only ever need the first value (I hope)
        glm::vec2 position2 = positionTarget(parameters, keywordIndex)[0];

        unsigned int chunk1 = std::floor(position1.x / CHUNK_SIZE) + WORLD_SIZE;
        unsigned int chunk2 = std::floor(position2.x / CHUNK_SIZE) + WORLD_SIZE;

        std::vector<Entity*> ret;

        for(int i = 0; i <= chunk1-chunk2; i++) {
            for(int j = 0; j < m_chunks[(chunk1+i)%WORLD_SIZE]->getAllEntities().size(); j++) {
                if(m_chunks[(chunk1+i)%WORLD_SIZE]->getAllEntities()[j]->getPosition().x >= position1.x && m_chunks[(chunk1+i)%WORLD_SIZE]->getAllEntities()[j]->getPosition().x <= position2.x) {
                    if(m_chunks[(chunk1+i)%WORLD_SIZE]->getAllEntities()[j]->getPosition().y >= position1.y && m_chunks[(chunk1+i)%WORLD_SIZE]->getAllEntities()[j]->getPosition().y <= position2.y) {
                        ret.push_back(m_chunks[(chunk1+i)%WORLD_SIZE]->getAllEntities()[j]);
                    }
                }
            }
        }

        return ret;
    } else if(parameters[keywordIndex] == "player") {
        keywordIndex += 1;
        std::vector<Entity*> ret;
        ret.push_back(m_worldManager->getPlayer());

        return ret;
    }
}

std::vector<glm::vec2> Scripter::positionTarget(std::vector<std::string> parameters, unsigned int& keywordIndex) { // gets position
    if(parameters[keywordIndex] == "relative") {
        std::vector<glm::vec2> ret;
        keywordIndex += 1;
        std::vector<Entity*> entities = entityTarget(parameters, keywordIndex);
        std::vector<glm::vec2> positions = positionTarget(parameters, keywordIndex);
        for(int i = 0; i < entities.size(); i++) {
            for(int j = 0; j < positions.size(); j++) {
                ret.push_back(entities[i]->getPosition() / glm::vec2(TILE_SIZE) + positions[j]);
            }
        }

        return ret;
    } else if(parameters[keywordIndex] == "area") {
        keywordIndex += 1;
        glm::vec2 pos1 = positionTarget(parameters, keywordIndex)[0]; /// Actually implement?
        glm::vec2 pos2 = positionTarget(parameters, keywordIndex)[0];

        std::vector<glm::vec2> ret;

        for(int y = pos1.y; y <= pos2.y; y++) {
            for(int x = pos1.x; x <= pos2.x; x++) {
                ret.push_back(glm::vec2(x, y));
            }
        }

        return ret;
    } else {
        std::vector<glm::vec2> ret;
        ret.push_back(glm::vec2(std::stof(parameters[keywordIndex]), std::stof(parameters[keywordIndex+1])));
        keywordIndex += 2;
        return ret;
    }
}
