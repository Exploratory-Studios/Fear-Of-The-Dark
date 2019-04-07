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
    // teleport [player] [relative][] (float) (float)                   Teleports the player to x=1st parameter, y=2nd parameter. "relative" makes coordinates relative to player
    /// teleport [entity] [relative][] (integer) (integer) (integer)         Teleports an entity with UUID of 3rd parameter to x=1st parameter, y=2nd parameter. "relative" makes coordinates relative to entity

    if(parameters[0] == "time") {
        if(parameters[1] == "set") {
            std::cout << "Setting time to " + parameters[2] << std::endl;
            *(m_worldManager->m_tickTime) = std::stoi(parameters[2]);
        }
    } else if(parameters[0] == "removeBlock") {
        if(parameters[1] == "relative") {
            int x = std::stoi(parameters[2]) + m_worldManager->m_player->getPosition().x / TILE_SIZE;
            int y = std::stoi(parameters[3]) + m_worldManager->m_player->getPosition().y / TILE_SIZE;

            std::cout << "Removing block relative to player at: X=" + std::to_string(x) + ", Y=" + std::to_string(y) << std::endl;

            removeBlock(x, y);
        } else {
            std::cout << "Removing block at: X=" + parameters[1] + ", Y=" + parameters[2] << std::endl;
            int x = std::stoi(parameters[1]);
            int y = std::stoi(parameters[2]);

            removeBlock(x, y);
        }
    } else if(parameters[0] == "changeBlock") {
        if(parameters[1] == "relative") {
            int x = std::stoi(parameters[2]) + m_worldManager->m_player->getPosition().x / TILE_SIZE;
            int y = std::stoi(parameters[3]) + m_worldManager->m_player->getPosition().y / TILE_SIZE;

            std::cout << "Changing block relative to player at: X=" + std::to_string(x) + ", Y=" + std::to_string(y) << " to one with the id of: " << parameters[4] << std::endl;

            int chunkIndex = std::floor(x / CHUNK_SIZE);
            Chunk* parent = m_chunks[chunkIndex];

            Block* block = createBlock(std::stoi(parameters[4]), glm::vec2(x, y), parent);

            changeBlock(block);
        } else {
            int x = std::stoi(parameters[2]);
            int y = std::stoi(parameters[3]);

            std::cout << "Changing block at: X=" + std::to_string(x) + ", Y=" + std::to_string(y) << " to one with the id of: " << parameters[4] << std::endl;

            int chunkIndex = std::floor(x / CHUNK_SIZE);
            Chunk* parent = m_chunks[chunkIndex];

            changeBlock(createBlock(std::stoi(parameters[4]), glm::vec2(x, y), parent));
        }
    } else if(parameters[0] == "teleport") {
        if(parameters[1] == "player") {
            if(parameters[2] == "relative") {
                float x = std::stoi(parameters[3]) * TILE_SIZE + m_worldManager->m_player->getPosition().x;
                float y = std::stoi(parameters[4]) * TILE_SIZE + m_worldManager->m_player->getPosition().y;

                m_worldManager->getPlayer()->setPosition(glm::vec2(x, y));
                m_worldManager->getPlayer()->setParentChunk(m_chunks);
            } else {
                float x = std::stoi(parameters[3]) * TILE_SIZE;
                float y = std::stoi(parameters[4]) * TILE_SIZE;

                m_worldManager->getPlayer()->setPosition(glm::vec2(x, y));
                m_worldManager->getPlayer()->setParentChunk(m_chunks);
            }
        } else if (parameters[1] == "entity") {
            GLEngine::fatalError("Teleporting entities is not implemented yet, stop screwing around and give them UUIDs!");
        }
    }
}


