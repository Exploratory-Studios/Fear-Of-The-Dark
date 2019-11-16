#pragma once

#include "../EntityManager.h"
#include "ScriptQueue.h"
#include "../GameplayScreen.h"

#include "../Logging.h"

#include "../ExtraFunctions.h"

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
        void removeBlock(int x, int y, unsigned int layer); // doesn't actually 'remove' per say, but changes it to air.
        void showBlock(int x, int y, unsigned int layer);
        void hideBlock(int x, int y, unsigned int layer);

        unsigned int addEntity(Entity* newEntity); // Returns new entity's index
        void removeEntity(unsigned int index); // This doesn't change the size of the vector, just sets the index to NULL values
        void showEntity(unsigned int index); // Index is reference to place in vector
        void hideEntity(unsigned int index);

        void showAlert(std::string& title, std::string& text); // Shows an alert window, with custom text, courtesy of CEGUI
        void showPlayerInventory(bool show); // Opens/closes player inventory on screen.
        void showBlockInventory(bool show, Block* block); // Shows/hides a block's inventory on screen in same style as player's.

        void update();

        std::string executeScript(Script* script); /// TODO: For the love of all things unholy, do NOT pass scripts by reference. They do not like that very much
        std::string executeCommand(std::string& command, Script* script = nullptr);

    private:

        Logger* logger = Logger::getInstance();

        std::vector<Entity*> entityTarget(std::vector<std::string> parameters, unsigned int& keywordIndex);
        std::vector<glm::vec2> positionTarget(std::vector<std::string> parameters, unsigned int& keywordIndex);
        void* pntr_interpretParameter(std::vector<std::string> parameters, unsigned int& keywordIndex);
        int int_interpretParameter(std::vector<std::string> parameters, unsigned int& keywordIndex);
        float float_interpretParameter(std::vector<std::string> parameters, unsigned int& keywordIndex);

        unsigned int stringToFlagId(std::string str);

        GameplayScreen* m_gameplayScreen = nullptr;

        std::vector<Entity*> m_entities;
        Chunk* m_chunks[WORLD_SIZE] = {};

        CEGUI::FrameWindow* m_alertWindow = nullptr;

        /*GLEngine::Camera2D* m_camera = nullptr;*/
};
