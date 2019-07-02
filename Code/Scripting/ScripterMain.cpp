#include "ScripterMain.h"

#include <Camera2D.h>

Scripter::Scripter(GameplayScreen* gameplayScreen) {
    init(gameplayScreen);
}

void Scripter::init(GameplayScreen* gameplayScreen) {

    m_gameplayScreen = gameplayScreen;

    for(int i = 0; i < WORLD_SIZE; i++) {
        m_chunks[i] = m_gameplayScreen->m_WorldIOManager->getWorld()->chunks[i];
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

unsigned int Scripter::addEntity(Entity* newEntity) {
    m_entities.push_back(newEntity);
    return m_entities.size();
}

void Scripter::removeEntity(unsigned int index) {
    for(unsigned int i = index; i < m_entities.size()-1; i++) {
        m_entities[i] = m_entities[i+1];
    }
    m_entities.pop_back();
}

void Scripter::showEntity(unsigned int index) {
    m_entities[index]->m_transparent = false;
}

void Scripter::hideEntity(unsigned int index) {
    m_entities[index]->m_transparent = true;
}

void Scripter::update() {
    // Get active script from ScriptQueue
    // Execute script
    // Take script off active list
    // Repeat until no active scripts exist

    std::vector<Script> scripts;

    scripts.clear();

    for(int i = 0; i < m_gameplayScreen->m_WorldIOManager->getScriptQueue()->m_activeScripts.size(); i++) {\
        scripts.push_back(Script());
        scripts[i] = (m_gameplayScreen->m_WorldIOManager->getScriptQueue()->m_scriptCache[m_gameplayScreen->m_WorldIOManager->getScriptQueue()->m_activeScripts[i]]);
    }

    for(unsigned int i = 0; i < scripts.size(); i++) {
        executeScript(&(scripts[i]));
    }
    m_gameplayScreen->m_WorldIOManager->getScriptQueue()->deactivateScripts();
}

/// PRIVATE FUNCTIONS

std::string Scripter::executeScript(Script* script) {
    std::string returnMessage;

    for(unsigned int i = 0; i < script->commands.size(); i++) {
        returnMessage = executeCommand(script->commands[i]);
    }

    return returnMessage;
}

std::string Scripter::executeCommand(std::string& command) {
    std::string returnMessage;
    std::vector<std::string> parameters;

    std::string temp;
    for(unsigned int i = 0; i < command.length(); i++) {
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
    // setFlag [integer(id)] [integer[1 or 0](value)]                       Sets the id'th flag's value to value
    // getFlag [string(name)]                                               Returns an integer, which coincides with the id of the flag which has the same name (enum) as name
    // echo [string]                                                        Logs a string

    if(parameters[0] == "time") {
        if(parameters[1] == "set") {
            logger->log("Setting time to " + parameters[2]);
            returnMessage += "Setting time to " + parameters[2] + "\n";

            m_gameplayScreen->m_WorldIOManager->getWorld()->time = std::stoi(parameters[2]);
        }
    } else if(parameters[0] == "removeBlock") {
        unsigned int keywordIndex = 1;
        std::vector<glm::vec2> positions = positionTarget(parameters, keywordIndex);
        for(unsigned int i = 0; i < positions.size(); i++) {
            int x = positions[i].x;
            int y = positions[i].y;

            logger->log("Removing block at: X=" + std::to_string(x) + ", Y=" + std::to_string(y));
            returnMessage += "Removing block at: X=" + std::to_string(x) + ", Y=" + std::to_string(y) + "\n";

            removeBlock(x, y);
        }
    } else if(parameters[0] == "changeBlock") {
        unsigned int keywordIndex = 1;
        std::vector<glm::vec2> positions = positionTarget(parameters, keywordIndex);
        for(unsigned int i = 0; i < positions.size(); i++) {
            int x = positions[i].x;
            int y = positions[i].y;

            logger->log("Changing block at: X=" + std::to_string(x) + ", Y=" + std::to_string(y) + " to one with the id of: " + parameters[keywordIndex]);
            returnMessage += "Changing block at: X=" + std::to_string(x) + ", Y=" + std::to_string(y) + " to one with the id of: " + parameters[keywordIndex] + "\n";

            int chunkIndex = std::floor(x / CHUNK_SIZE);
            Chunk* parent = m_chunks[chunkIndex];

            Block* block = createBlock(std::stoi(parameters[keywordIndex]), glm::vec2(x, y), parent);

            changeBlock(block);
        }
    } else if(parameters[0] == "teleport") {
        unsigned int keywordIndex = 1;
        std::vector<Entity*> entities = entityTarget(parameters, keywordIndex);
        std::vector<glm::vec2> positions = positionTarget(parameters, keywordIndex);

        for(unsigned int i = 0; i < entities.size(); i++) {
            for(unsigned int j = 0; j < positions.size(); j++) {
                logger->log("Teleporting entity to: X=" + std::to_string(positions[j].x * TILE_SIZE) + ", Y=" + std::to_string(positions[j].y * TILE_SIZE));
                returnMessage += "Teleporting entity to: X=" + std::to_string(positions[j].x * TILE_SIZE) + ", Y=" + std::to_string(positions[j].y * TILE_SIZE) + "\n";

                entities[i]->setPosition(positions[j] * glm::vec2(TILE_SIZE));
                entities[i]->setParentChunk(m_chunks);
            }
        }
    } else if(parameters[0] == "give") {
        unsigned int keywordIndex = 1;
        std::vector<Entity*> entities = entityTarget(parameters, keywordIndex);
        for(unsigned int i = 0; i < entities.size(); i++) {
            //entities[i].getInventory().add(X);
            /// TODO:
        }
    } else if(parameters[0] == "setCanInteract") {
        m_gameplayScreen->m_player->setCanInteract((bool)std::stoi(parameters[1]));
        returnMessage = "Set player ";
        if((bool)std::stoi(parameters[1])) {
            logger->log("Set player able to interact");
            returnMessage += "able to interact\n";
        } else {
            logger->log("Set player unable to interact");
            returnMessage += "unable to interact\n";
        }

    } else if(parameters[0] == "createEntity") { // position, id, parameters
        unsigned int keywordIndex = 1;
        std::vector<glm::vec2> positions = positionTarget(parameters, keywordIndex);
        Categories::EntityIDs entityId = (Categories::EntityIDs)(std::stoi(parameters[keywordIndex]));
        keywordIndex++;

        std::vector<Parameter> p;
        for(int i = keywordIndex; i < parameters.size(); i++) {
            p.push_back(interpretParameter(parameters, keywordIndex));
        }

        for(int i = 0; i < positions.size(); i++) {
            m_chunks[(unsigned int)(positions[i].x / TILE_SIZE / CHUNK_SIZE)]->addEntity(createEntity((unsigned int)entityId, positions[i] * glm::vec2(TILE_SIZE), nullptr, p));

            std::string logString = "Added Entity " + std::to_string(i+1) + " of " + std::to_string(positions.size()) + " at X=" + std::to_string(positions[i].x) + ", Y=" + std::to_string(positions[i].y) + ", with and ID of " + std::to_string((unsigned int)entityId) + " (Chunk " + std::to_string((unsigned int)(positions[i].x / TILE_SIZE / CHUNK_SIZE)) + ")" ;
            logger->log(logString);
            returnMessage += logString + "\n";
        }

        /// TODO: Have a function in Entities.h return a vector of parameter types that we will need to fill out, based on a given ID.
    } else if(parameters[0] == "setFlag") { // getFlag is in interpretParameter
        unsigned int keywordIndex = 1;
        int id = static_cast<int>(interpretParameter(parameters, keywordIndex).t);
        bool val = static_cast<bool>(interpretParameter(parameters, keywordIndex).t);

        m_gameplayScreen->m_questManager->setFlag(id, val);
        logger->log("Setting flag with id: " +
         std::to_string(id) +
        " to " +
        (val ? "true" : "false"));
    } else if(parameters[0] == "echo") {
        std::string ret = "ECHO: ";
        for(int i = 1; i < parameters.size(); i++) {
            ret += parameters[i];
        }
        logger->log(ret, true);
    } else {
        logger->log("Invalid command: " + command, true);
        returnMessage += "Invalid command: " + command + "\n";
    }

    return returnMessage;
}

/// PRIVATE, DON'T TOUCH!!!!!!!

std::vector<Entity*> Scripter::entityTarget(std::vector<std::string> parameters, unsigned int& keywordIndex) { // gets entities
    if(parameters[keywordIndex] == "near") {
        glm::vec2 position;
        keywordIndex += 1;
        position = positionTarget(parameters, keywordIndex)[0]; // for this specific command, we will never need to use multiple points (in a reasonable world)

        unsigned int chunkIndex = std::floor(position.x / TILE_SIZE / CHUNK_SIZE);

        float nearestDistance;
        unsigned int nearestId;
        unsigned int nearestChunkId;

        bool found = false;

        for(unsigned int i = 0; i < m_chunks[chunkIndex]->getEntities().size(); i++) {
            float dist = std::sqrt(std::abs(position.x - m_chunks[chunkIndex]->getEntities()[i]->getPosition().x) + std::abs(position.y - m_chunks[chunkIndex]->getEntities()[i]->getPosition().y));
            if(dist < nearestDistance || !found) {
                nearestDistance = dist;
                nearestId = i;
                nearestChunkId = chunkIndex;
                found = true;
            }
        }

        while(!found) {
            for(unsigned int j = 0; j < WORLD_SIZE / 2 - 1; j++) {
                for(unsigned int i = 0; i < m_chunks[(chunkIndex-i + WORLD_SIZE) % WORLD_SIZE]->getEntities().size(); i++) {
                    float dist = std::sqrt(std::abs(position.x - m_chunks[chunkIndex]->getEntities()[i]->getPosition().x) + std::abs(position.y - m_chunks[chunkIndex]->getEntities()[i]->getPosition().y));
                    if(dist < nearestDistance || !found) {
                        nearestDistance = dist;
                        nearestId = i;
                        nearestChunkId = chunkIndex;
                        found = true;
                    }
                }
                for(unsigned int i = 0; i < m_chunks[(chunkIndex+i + WORLD_SIZE) % WORLD_SIZE]->getEntities().size(); i++) {
                    float dist = std::sqrt(std::abs(position.x - m_chunks[chunkIndex]->getEntities()[i]->getPosition().x) + std::abs(position.y - m_chunks[chunkIndex]->getEntities()[i]->getPosition().y));
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
        ret.push_back(m_chunks[nearestChunkId]->getEntities()[nearestId]);

        return ret;
    } else if(parameters[keywordIndex] == "area") { /// Not tested
        keywordIndex += 1;
        glm::vec2 position1 = positionTarget(parameters, keywordIndex)[0]; // Again, same as near, we'll only ever need the first value (I hope)
        glm::vec2 position2 = positionTarget(parameters, keywordIndex)[0];

        unsigned int chunk1 = std::floor(position1.x / CHUNK_SIZE) + WORLD_SIZE;
        unsigned int chunk2 = std::floor(position2.x / CHUNK_SIZE) + WORLD_SIZE;

        std::vector<Entity*> ret;

        for(unsigned int i = 0; i <= chunk1-chunk2; i++) {
            for(unsigned int j = 0; j < m_chunks[(chunk1+i)%WORLD_SIZE]->getEntities().size(); j++) {
                if(m_chunks[(chunk1+i)%WORLD_SIZE]->getEntities()[j]->getPosition().x >= position1.x && m_chunks[(chunk1+i)%WORLD_SIZE]->getEntities()[j]->getPosition().x <= position2.x) {
                    if(m_chunks[(chunk1+i)%WORLD_SIZE]->getEntities()[j]->getPosition().y >= position1.y && m_chunks[(chunk1+i)%WORLD_SIZE]->getEntities()[j]->getPosition().y <= position2.y) {
                        ret.push_back(m_chunks[(chunk1+i)%WORLD_SIZE]->getEntities()[j]);
                    }
                }
            }
        }

        return ret;
    } else if(parameters[keywordIndex] == "player") {
        keywordIndex += 1;
        std::vector<Entity*> ret;
        ret.push_back(m_gameplayScreen->m_player);

        return ret;
    } else if(parameters[keywordIndex] == "speaker") {
        keywordIndex += 1;
        std::vector<Entity*> ret;
        if(m_gameplayScreen->m_player->getSelectedEntity()) ret.push_back(m_gameplayScreen->m_player->getSelectedEntity());

        return ret;
    }

    std::string line;
    for(unsigned int i = 0; i < parameters.size(); i++) {
        line += parameters[i];
    }

    logger->log("Script failed on entityTarget with line: " + line + ". Quitting...", true);
    GLEngine::fatalError("Script failed on entityTarget with line: " + line);

    std::vector<Entity*> ret;

    return ret;
}

std::vector<glm::vec2> Scripter::positionTarget(std::vector<std::string> parameters, unsigned int& keywordIndex) { // gets position
    if(parameters[keywordIndex] == "relative") {
        std::vector<glm::vec2> ret;
        keywordIndex += 1;
        std::vector<Entity*> entities = entityTarget(parameters, keywordIndex);
        std::vector<glm::vec2> positions = positionTarget(parameters, keywordIndex);
        for(unsigned int i = 0; i < entities.size(); i++) {
            for(unsigned int j = 0; j < positions.size(); j++) {
                ret.push_back(entities[i]->getPosition() / glm::vec2(TILE_SIZE) + positions[j]);
            }
        }

        return ret;
    } else if(parameters[keywordIndex] == "area") {
        keywordIndex += 1;
        glm::vec2 pos1 = positionTarget(parameters, keywordIndex)[0];
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

Parameter Scripter::interpretParameter(std::vector<std::string> parameters, unsigned int& keywordIndex) {
    Parameter p;
    try {
        float f = std::stof(parameters[keywordIndex]);
        p.setFloat(f);
    } catch(std::exception& e) {
        try {
            int i = std::stoi(parameters[keywordIndex]);
            p.setInt(i);
        } catch(std::exception& e) {
            if(parameters[keywordIndex] == "inputManager") {
                p.setPointer(&m_gameplayScreen->m_game->inputManager);
            } else if(parameters[keywordIndex] == "scriptQueue") {
                p.setPointer(m_gameplayScreen->m_player->m_sq);
            } else if(parameters[keywordIndex] == "questManager") {
                p.setPointer(m_gameplayScreen->m_questManager);
            } else if(parameters[keywordIndex] == "getFlag") {
                keywordIndex++;
                p.setInt(stringToFlagId(parameters[keywordIndex]));
            }
        }
    }
    keywordIndex++;
    return p;
}

unsigned int Scripter::stringToFlagId(std::string str) {
    if(str == "FLAG_ZERO") {
        return 0;
    } else if(str == "FLAG_ONE") {
        return 1;
    } else {
        logger->log("Tried to change value of non-existent flag with name: " + str, true);
    }
}
