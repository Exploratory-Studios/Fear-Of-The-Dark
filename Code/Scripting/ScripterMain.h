#pragma once

#include "../EntityManager.h"
#include "ScriptQueue.h"
#include "../GameplayScreen.h"

#include "../Logging.h"

const bool
    SCRIPT_INIT_FLAG_MODIFYWORLD = 0x1,     // 0000 0001
    SCRIPT_INIT_FLAG_MODIFYENTITIES = 0x2,  // 0000 0010
    SCRIPT_INIT_FLAG_INHERITBLOCKS = 0x4,   // 0000 0100
    SCRIPT_INIT_FLAG_INHERITENTITIES = 0x8, // 0000 1000
    SCRIPT_INIT_FLAG_5 = 0x10,               // 0001 0000
    SCRIPT_INIT_FLAG_6 = 0x20,               // 0010 0000
    SCRIPT_INIT_FLAG_7 = 0x40,               // 0100 0000
    SCRIPT_INIT_FLAG_8 = 0x80,               // 1000 0000
    SCRIPT_INIT_FLAG_ALL = 0xFF;             // 1111 1111

class GameplayScreen;

class Scripter {
    public:
        Scripter(GameplayScreen* gameplayScreen);
        void init(GameplayScreen* gameplayScreen);

        void changeBlock(Block* newBlock);
        void removeBlock(int x, int y); // doesn't actually 'remove' per say, but changes it to air.
        void showBlock(int x, int y);
        void hideBlock(int x, int y);

        unsigned int addEntity(Entity* newEntity); // Returns new entity's index
        void removeEntity(unsigned int index); // This doesn't change the size of the vector, just sets the index to NULL values
        void showEntity(unsigned int index); // Index is reference to place in vector
        void hideEntity(unsigned int index);

        void update();

        std::string executeScript(Script& script);
        std::string executeCommand(std::string& command);

    private:

        Logger* logger = Logger::getInstance();

        std::vector<Entity*> entityTarget(std::vector<std::string> parameters, unsigned int& keywordIndex);
        std::vector<glm::vec2> positionTarget(std::vector<std::string> parameters, unsigned int& keywordIndex);
        Parameter interpretParameter(std::vector<std::string> parameters, unsigned int& keywordIndex);

        bool m_scriptInitFlags = 0x00;

        GameplayScreen* m_gameplayScreen = nullptr;

        std::vector<Entity*> m_entities;
        Chunk* m_chunks[WORLD_SIZE];

        /*GLEngine::Camera2D* m_camera = nullptr;*/
};
