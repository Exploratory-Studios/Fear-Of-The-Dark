#include "ScripterMain.h"

#include "ScriptQueue.h"
#include "../Entity.h"
#include "../Entities.h"
#include "../QuestManager.h"

#include <Camera2D.h>

Scripter::Scripter() {
    init();
}

void Scripter::init() {

}

void Scripter::changeBlock(World* world, Block* newBlock) {
    int chunk = std::floor(newBlock->getPosition().x / CHUNK_SIZE); // What chunk index it belongs to

    world->setTile(newBlock); // Set the block, of course
    /// TODO: compile array of chunks in init()
}

void Scripter::removeBlock(World* world, int x, int y, unsigned int layer) {
    int chunk = std::floor(x / CHUNK_SIZE); // What chunk index it belongs to

    world->setTile(new BlockAir(glm::vec2(x, y), layer));
}

void Scripter::showBlock(World* world, int x, int y, unsigned int layer) {
    int chunk = std::floor(x / CHUNK_SIZE); // What chunk index it belongs to

    world->getTile(x, y, layer)->m_draw = true; // Make it not-transparent
}

void Scripter::hideBlock(World* world, int x, int y, unsigned int layer) {
    int chunk = std::floor(x / CHUNK_SIZE); // What chunk index it belongs to

    world->getTile(x, y, layer)->m_draw = false; // Make it not-transparent
}

unsigned int Scripter::addEntity(World* world, Entity* newEntity) {
    world->addEntity(newEntity);
}

void Scripter::removeEntity(World* world, unsigned int index) {
    world->removeEntity(index);
}

void Scripter::showEntity(World* world, unsigned int index) {
    world->getEntities()[index]->m_transparent = false;
}

void Scripter::hideEntity(World* world, unsigned int index) {
    world->getEntities()[index]->m_transparent = true;
}

/*void Scripter::showAlert(std::string& title, std::string& text) { // Shows an alert window, with custom text, courtesy of CEGUI
    if(!m_alertWindow) {
        m_alertWindow = static_cast<CEGUI::FrameWindow*>(m_gameplayScreen->getGUI()->createWidget("FOTDSkin/FrameWindow", glm::vec4(0.2f, 0.2f, 0.6f, 0.6f), glm::vec4(0.0f), "Scripter_AlertWindow"));
    }
    m_alertWindow->setText(title);
    m_alertWindow->setCloseButtonEnabled(true);
}

void Scripter::showPlayerInventory(bool show) { // Opens/closes player inventory on screen.
    m_gameplayScreen->m_player->showInventory(show);
}

void Scripter::showBlockInventory(bool show, Block* block) { // Shows/hides a block's inventory on screen in same style as player's.
    static_cast<InventoryBlock*>(block)->showInventory(show);
}*/

void Scripter::update(World* world, ScriptQueue* sq, QuestManager* qm, GameplayScreen* gs) {
    // Get active script from ScriptQueue
    // Execute script
    // Take script off active list
    // Repeat until no active scripts exist

    std::vector<Script*> scripts;

    scripts.clear();

    for(unsigned int i = 0; i < sq->m_activeScripts.size(); i++) {
        scripts.push_back(&(sq->m_activeScripts[i]));
    }

    if(scripts.size() > 0) {
        for(unsigned int i = scripts.size(); i > 0; i--) { // We have to go backwards through the queue so that we deactivate them in the right order (because of the whole popping back thing)
            executeScript(world, qm, gs, (scripts[i-1]));
            if(scripts[i-1]->paused == false) {
                sq->deactivateScript(i-1);
            }
        }
    }
}

/// PRIVATE FUNCTIONS

std::string Scripter::executeScript(World* world, QuestManager* qm, GameplayScreen* gs, Script* script) {
    std::string returnMessage;

    for(unsigned int i = script->place; i < script->commands.size(); i++) {
        returnMessage = executeCommand(world, qm, gs, script->commands[i], script);
        if(script->paused == false) {
            script->place++;
        } else {
            if(world->getTime() - script->startTime >= script->timerTime) {
                script->paused = false;
                script->startTime = 0;
                script->timerTime = 0;
                script->place++;
            } else {
                break;
            }
        }
    }

    return returnMessage;
}

std::string Scripter::executeCommand(World* world, QuestManager* qm, GameplayScreen* gs, std::string& command, Script* script) {
    std::string returnMessage;
    std::vector<std::string> parameters;

    std::string strippedCommand = extras::removeWhitespace(command);

    std::string temp;
    bool readingString = false;
    for(unsigned int i = 0; i < strippedCommand.length(); i++) {
        if(strippedCommand[i] == ' ' && !readingString) {
            parameters.push_back(temp);
            temp = "";
        } else {
            if(strippedCommand[i] == '\"') {
                readingString = true;
            } else {
                temp += strippedCommand[i];
            }
        }
    }
    parameters.push_back(temp);

    //actual command is parameters[0], the rest are parameters

    // COMMANDS:
    // Format -> command [setChoice1][setChoice2][] (typeOfParameter)       Documentation
    // time [set] (integer)                                                 Sets time of day to 2nd parameter
    // removeBlock [relative][] (integer) (integer)                         Removes block at x=1st parameter, y=2nd parameter. "relative" makes coordinates relative to player
    // changeBlock [relative][] (integer) (integer) (integer)               Changes block at x=1st parameter, y=2nd parameter to one with id of 3rd parameter. "relative" makes coordinates relative to player
    // teleport [player] [relative][] (integer) (integer)                   Teleports the player to x=1st parameter, y=2nd parameter. "relative" makes coordinates relative to player
    // teleport [entity] [relative][] (integer) (integer)                   Teleports an entity to x=1st parameter, y=2nd parameter. "relative" makes coordinates relative to entity
    // setFlag [integer(id)] [integer[1 or 0](value)]                       Sets the id'th flag's value to value
    // getFlag [string(name)]                                               Returns an integer, which coincides with the id of the flag which has the same name (enum) as name
    // echo [string]                                                        Logs a string

    try {
        if(parameters[0] == "time") {
            if(parameters[1] == "set") {
                logger->log("Setting time to " + parameters[2]);
                returnMessage += "Setting time to " + parameters[2] + "\n";

                world->setTime(std::stoi(parameters[2]));
            }
        } else if(parameters[0] == "removeBlock") {
            unsigned int keywordIndex = 1;
            std::vector<glm::vec2> positions = positionTarget(world, parameters, keywordIndex);
            for(unsigned int i = 0; i < positions.size(); i++) {
                int x = positions[i].x;
                int y = positions[i].y;

                logger->log("Removing block at: X=" + std::to_string(x) + ", Y=" + std::to_string(y));
                returnMessage += "Removing block at: X=" + std::to_string(x) + ", Y=" + std::to_string(y) + "\n";

                removeBlock(world, x, y, 0); /// TODO: Incorporate Layers
            }
        } else if(parameters[0] == "changeBlock") {
            unsigned int keywordIndex = 1;
            std::vector<glm::vec2> positions = positionTarget(world, parameters, keywordIndex);
            for(unsigned int i = 0; i < positions.size(); i++) {
                int x = positions[i].x;
                int y = positions[i].y;

                logger->log("Changing block at: X=" + std::to_string(x) + ", Y=" + std::to_string(y) + " to one with the id of: " + parameters[keywordIndex]);
                returnMessage += "Changing block at: X=" + std::to_string(x) + ", Y=" + std::to_string(y) + " to one with the id of: " + parameters[keywordIndex] + "\n";

                Block* block = createBlock(std::stoi(parameters[keywordIndex]), glm::vec2(x, y), 0);

                changeBlock(world, block);
            }
        } else if(parameters[0] == "teleport") {
            unsigned int keywordIndex = 1;
            std::vector<Entity*> entities = entityTarget(world, parameters, keywordIndex);
            std::vector<glm::vec2> positions = positionTarget(world, parameters, keywordIndex);

            for(unsigned int i = 0; i < entities.size(); i++) {
                for(unsigned int j = 0; j < positions.size(); j++) {
                    logger->log("Teleporting entity to: X=" + std::to_string(positions[j].x) + ", Y=" + std::to_string(positions[j].y));
                    returnMessage += "Teleporting entity to: X=" + std::to_string(positions[j].x) + ", Y=" + std::to_string(positions[j].y) + "\n";

                    world->getEntities()[i]->setPosition(positions[j]);
                }
            }
        } else if(parameters[0] == "give") {
            unsigned int keywordIndex = 1;
            std::vector<Entity*> entities = entityTarget(world, parameters, keywordIndex);

            unsigned int id = int_interpretParameter(parameters, keywordIndex);
            unsigned int quantity = int_interpretParameter(parameters, keywordIndex);

            for(unsigned int i = 0; i < entities.size(); i++) {
                world->getEntities()[i]->giveItem(createItem(id, quantity));
                /// TODO:
            }
        } else if(parameters[0] == "setCanInteract") {
            world->getPlayer()->setCanInteract((bool)std::stoi(parameters[1]));
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
            std::vector<glm::vec2> positions = positionTarget(world, parameters, keywordIndex);
            unsigned int layer = int_interpretParameter(parameters, keywordIndex);
            Categories::EntityIDs entityId = (Categories::EntityIDs)(int_interpretParameter(parameters, keywordIndex));
            keywordIndex++;

            for(unsigned int i = 0; i < positions.size(); i++) {
                world->addEntity(createEntity((unsigned int)entityId, positions[i], layer));

                std::string logString = "Added Entity " + std::to_string(i+1) + " of " + std::to_string(positions.size()) + " at X=" + std::to_string(positions[i].x) + ", Y=" + std::to_string(positions[i].y) + ", with and ID of " + std::to_string((unsigned int)entityId) + " (Chunk " + std::to_string((unsigned int)(positions[i].x / CHUNK_SIZE)) + ")" ;
                logger->log(logString);
                returnMessage += logString + "\n";
            }

            /// TODO: Have a function in Entities.h return a vector of parameter types that we will need to fill out, based on a given ID.
        } else if(parameters[0] == "setFlag") { // getFlag is in interpretParameter
            unsigned int keywordIndex = 1;
            int id = static_cast<int>(int_interpretParameter(parameters, keywordIndex));
            bool val = static_cast<bool>(int_interpretParameter(parameters, keywordIndex));

            qm->setFlag(id, val);
            std::string logString = "Setting flag with id: " + std::to_string(id) + " to " + (val ? "true" : "false");

            logger->log(logString);
            returnMessage += logString;
        } else if(parameters[0] == "echo") {
            std::string ret = "ECHO: ";
            for(unsigned int i = 1; i < parameters.size(); i++) {
                ret += parameters[i] + " ";
            }
            logger->log(ret, true);
            returnMessage += ret + "\n";
        } else if(parameters[0] == "setPeriod") {
            unsigned int keywordIndex = 1;
            std::string ret;
            if(parameters[keywordIndex] == "neothilic") {
                ret = "Setting time period to NEOLITHIC";
                world->setWorldEra(WorldEra::NEOLITHIC_ERA);
            } else if(parameters[keywordIndex] == "common_era") {
                ret = "Setting time period to COMMON ERA";
                world->setWorldEra(WorldEra::COMMON_ERA);
            } else if(parameters[keywordIndex] == "future_era") {
                ret = "Setting time period to FUTURE ERA";
                world->setWorldEra(WorldEra::FUTURE_ERA);
            } else {
                ret = "Time period \"" + parameters[keywordIndex] + "\" does not exist.";
            }

            logger->log(ret);
            returnMessage += ret + "\n";
        } else if(parameters[0] == "god") {
            world->getPlayer()->m_godMode = !world->getPlayer()->m_godMode;
        } else if(parameters[0] == "camera") {
            unsigned int keywordIndex = 1;
            if(parameters[keywordIndex] == "set") {
                gs->setCameraLocked(true);
                returnMessage += "Locked camera in place, it will no longer move with the player. (Run \"camera unlock\" to unlock)";

                keywordIndex++;
                glm::vec2 newPos = positionTarget(world, parameters, keywordIndex)[0];
                gs->getCamera()->setPosition(newPos);
                returnMessage += "Moved camera to position: X=" + std::to_string(newPos.x) + ", Y=" + std::to_string(newPos.y);

                logger->log(returnMessage);
            } else if(parameters[keywordIndex] == "lock") {
                gs->setCameraLocked(true);
                returnMessage += "Locked camera in place, it will no longer move with the player. (Run \"camera unlock\" to unlock)";
                logger->log(returnMessage);
            } else if(parameters[keywordIndex] == "unlock") {
                gs->setCameraLocked(false);
                returnMessage += "Unlocked camera, it will now follow the player.\n";
                logger->log(returnMessage);
            } else if(parameters[keywordIndex] == "move") {
                gs->setCameraLocked(true);
                returnMessage += "Locked camera in place, it will no longer move with the player. (Run \"camera unlock\" to unlock)";

                keywordIndex++;
                glm::vec2 newPos = positionTarget(world, parameters, keywordIndex)[0] + gs->getCamera()->getPosition();
                gs->getCamera()->setPosition(newPos);
                returnMessage += "\nShifted camera to position: X=" + std::to_string(newPos.x) + ", Y=" + std::to_string(newPos.y);

                logger->log(returnMessage);
            } else if(parameters[keywordIndex] == "smoothMove") {
                gs->setCameraLocked(true);
                returnMessage += "Locked camera in place, it will no longer move with the player. (Run \"camera unlock\" to unlock)";

                keywordIndex++;
                glm::vec2 newPos = positionTarget(world, parameters, keywordIndex)[0] + gs->getCamera()->getPosition();
                gs->setSmoothMoveTarget(newPos);

                float speed = float_interpretParameter(parameters, keywordIndex);
                gs->setSmoothMoveSpeed(speed);
                returnMessage += "\nStarted smoothly moving camera to position: X=" + std::to_string(newPos.x) + ", Y=" + std::to_string(newPos.y) + ", at " + std::to_string((int)(speed * 100.0f)) + "% speed";

                logger->log(returnMessage);
            }
        } else if(parameters[0] == "timer") {
            if(script) {
                if(script->timerTime == 0) {
                    unsigned int keywordIndex = 1;
                    script->paused = true;
                    script->timerTime = int_interpretParameter(parameters, keywordIndex);
                    script->startTime = world->getTime();
                    returnMessage += "Set timer for " + std::to_string(script->timerTime) + " frames.";
                    logger->log(returnMessage);
                }
            } else {
                returnMessage += "Sorry. That command can only be used in pre-written scripts.";
                logger->log(returnMessage, true);
            }
        } else if(parameters[0] == "pause") {
            gs->pauseForCutscene();
        } else if(parameters[0] == "continue") {
            gs->unpauseCutscene();
        } else if(parameters[0] == "setPlayerStat") {
            if(parameters[1] == "sanity") {
                unsigned int keywordIndex = 2;
                world->getPlayer()->m_sanity = float_interpretParameter(parameters, keywordIndex);
            }
        } else if(parameters[0] == "showAlert") {
            //showAlert(parameters[1], parameters[2]);
        } else {
            logger->log("Invalid command: " + command, true);
            returnMessage += "Invalid command: " + command + "\n";
        }
    } catch(std::exception& e) {
        returnMessage += "Error in command syntax. Please check your script/command and retry.\n";
        returnMessage += "Error: " + strippedCommand + "\n";;
        logger->log(returnMessage, true);
    }

    return returnMessage;
}

/// PRIVATE, DON'T TOUCH!!!!!!!

std::vector<Entity*> Scripter::entityTarget(World* world, std::vector<std::string> parameters, unsigned int& keywordIndex) { // gets entities
    if(parameters[keywordIndex] == "near") {
        glm::vec2 position;
        keywordIndex += 1;
        position = positionTarget(world, parameters, keywordIndex)[0]; // for this specific command, we will never need to use multiple points (in a reasonable world)

        unsigned int chunkIndex = std::floor(position.x / CHUNK_SIZE);

        float nearestDistance;
        unsigned int nearestId;
        unsigned int nearestChunkId;

        bool found = false;

        for(unsigned int i = 0; i < world->getEntities().size(); i++) { /// TODO: Optimize this
            float dist = std::sqrt(std::abs(position.x - world->getEntities()[i]->getPosition().x) + std::abs(position.y - world->getEntities()[i]->getPosition().y));
            if(dist < nearestDistance || !found) {
                nearestDistance = dist;
                nearestId = i;
                nearestChunkId = chunkIndex;
                found = true;
            }
        }

        std::vector<Entity*> ret;
        ret.push_back(world->getEntities()[nearestId]);

        return ret;
    } else if(parameters[keywordIndex] == "area") { /// Not tested
        keywordIndex += 1;
        glm::vec2 position1 = positionTarget(world, parameters, keywordIndex)[0]; // Again, same as near, we'll only ever need the first value (I sure hope)
        glm::vec2 position2 = positionTarget(world, parameters, keywordIndex)[0];

        unsigned int chunk1 = std::floor(position1.x / CHUNK_SIZE) + WORLD_SIZE;
        unsigned int chunk2 = std::floor(position2.x / CHUNK_SIZE) + WORLD_SIZE;

        std::vector<Entity*> ret;

        for(unsigned int i = 0; i <= chunk1-chunk2; i++) {
            for(unsigned int j = 0; j < world->getEntities().size(); j++) {
                if(world->getEntities()[j]->getPosition().x >= position1.x && world->getEntities()[j]->getPosition().x <= position2.x) {
                    if(world->getEntities()[j]->getPosition().y >= position1.y && world->getEntities()[j]->getPosition().y <= position2.y) {
                        ret.push_back(world->getEntities()[j]);
                    }
                }
            }
        }

        return ret;
    } else if(parameters[keywordIndex] == "player") {
        keywordIndex += 1;
        std::vector<Entity*> ret;
        ret.push_back(world->getPlayer());

        return ret;
    } else if(parameters[keywordIndex] == "speaker") {
        keywordIndex += 1;
        std::vector<Entity*> ret;
        if(world->getPlayer()->getSelectedEntity()) ret.push_back(world->getPlayer()->getSelectedEntity());

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

std::vector<glm::vec2> Scripter::positionTarget(World* world, std::vector<std::string> parameters, unsigned int& keywordIndex) { // gets position
    if(parameters[keywordIndex] == "relative") {
        std::vector<glm::vec2> ret;
        keywordIndex += 1;
        std::vector<Entity*> entities = entityTarget(world, parameters, keywordIndex);
        std::vector<glm::vec2> positions = positionTarget(world, parameters, keywordIndex);
        for(unsigned int i = 0; i < entities.size(); i++) {
            for(unsigned int j = 0; j < positions.size(); j++) {
                ret.push_back(entities[i]->getPosition() + positions[j]);
            }
        }

        return ret;
    } else if(parameters[keywordIndex] == "area") {
        keywordIndex += 1;
        glm::vec2 pos1 = positionTarget(world, parameters, keywordIndex)[0];
        glm::vec2 pos2 = positionTarget(world, parameters, keywordIndex)[0];

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

int Scripter::int_interpretParameter(std::vector<std::string> parameters, unsigned int& keywordIndex) {
    int p;
    try {
        int i = std::stoi(parameters[keywordIndex]);
        p = i;
    } catch(std::exception& e) {
        if(parameters[keywordIndex] == "getFlag") {
            keywordIndex++;
            p = stringToFlagId(parameters[keywordIndex]);
        }
    }
    keywordIndex++;
    return p;
}

float Scripter::float_interpretParameter(std::vector<std::string> parameters, unsigned int& keywordIndex) {
    float p;
    float f = std::stof(parameters[keywordIndex]);
    p = f;
    keywordIndex++;
    return p;
}

unsigned int Scripter::stringToFlagId(std::string str) {
    if(str == "FLAG_ZERO") {
        return 0;
    } else if(str == "FLAG_ONE") {
        return 1;
    }
    logger->log("Tried to get ID of non-existent flag with name: " + str, true);
    return 0;
}
