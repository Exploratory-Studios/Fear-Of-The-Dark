#pragma once

#include "../WorldManager.h"

const bool
    SCRIPT_INIT_FLAG_MODIFYWORLD = 0x01,     // 0000 0001
    SCRIPT_INIT_FLAG_MODIFYENTITIES = 0x02,  // 0000 0010
    SCRIPT_INIT_FLAG_INHERITBLOCKS = 0x04,   // 0000 0100
    SCRIPT_INIT_FLAG_INHERITENTITIES = 0x08, // 0000 1000
    SCRIPT_INIT_FLAG_5 = 0x0F,               // 0001 0000
    SCRIPT_INIT_FLAG_6 = 0x2F,               // 0010 0000
    SCRIPT_INIT_FLAG_7 = 0x4F,               // 0100 0000
    SCRIPT_INIT_FLAG_8 = 0x8F;               // 1000 0000

class Scripter { // Script class will have a virtual function that you fill in. Possibly another class that reads from a text file and executes accordingly for modding purposes? (?)Variables(?)
    public:
        void init(WorldIOManager& worldIOManager, GLEngine::Camera2D& camera,
                  bool scriptInitFlags = SCRIPT_INIT_FLAG_MODIFYWORLD |
                    SCRIPT_INIT_FLAG_MODIFYENTITIES |
                    SCRIPT_INIT_FLAG_INHERITBLOCKS);
                         // This sets the vectors and stuff, and can
                         // also disable actual world-CHANGING options (etc.), such as
                         // modify-world and modify-entities effects :)

        void addBlock(Block* newBlock);
        void removeBlock(int x, int y);
        void showBlock(int x, int y);
        void hideBlock(int x, int y);

        unsigned int addEntity(Entity newEntity); // Returns new entity's index
        void removeEntity(unsigned int index); // This doesn't change the size of the vector, just sets the index to NULL values
        void showEntity(unsigned int index); // Index is reference to place in vector
        void hideEntity(unsigned int index);

        void updateWorld();

    private:
        void draw();
        void update();

        bool m_scriptInitFlags = 0x00;

        std::vector<Entity*> m_entities;
        Chunk** m_chunks;
        GLEngine::Camera2D* m_camera = nullptr;
};
