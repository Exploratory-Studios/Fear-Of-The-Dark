#pragma once

#include <string>
#include <vector>
#include <InputManager.h>
#include <boost/thread.hpp>
#include <stdarg.h>

#include <Window.h>

#include <mutex>

#include "PerlinNoise/PerlinNoise.h"

#include "Chunk.h"
#include "Player.h"
#include "Block.h"
#include "Blocks.h"
#include "Entities.h"

#include "PresetValues.h"
#include "Logging.h"

#include "SaveDataTypes.h"

class World {
public:
    World(ScriptQueue* sq) {
        chunks = new Chunk*[WORLD_SIZE]();

        for(int i = 0; i < WORLD_SIZE; i++) {
            chunks[i] = new Chunk(sq);
            chunks[i]->setIndex(i);
        }
    }
    ~World() {
        for(int i = 0; i < WORLD_SIZE; i++) {
            delete chunks[i];
        }
        delete[] chunks;
    }
    Chunk** chunks = nullptr;
    Player* player = nullptr;
    float time = 0.0f;
    WorldEra worldEra = WorldEra::NEOLITHIC_ERA;
    std::string name;
};

class WorldIOManager
{
    public:
        WorldIOManager(World* world, GLEngine::InputManager* input, GLEngine::Window* window) : m_world(world), m_input(input), m_window(window) { init(); }
        WorldIOManager(GLEngine::InputManager* input, GLEngine::Window* window) : m_input(input), m_window(window) { init(); }

        void init() {
            m_progress = new float(0.0f);
            m_saveLoadMessage = new std::string("");
            m_sq = new ScriptQueue();

            if(!m_world) {
                m_world = new World(m_sq);
            }
            m_audioManager = new AudioManager();
            logger = Logger::getInstance();
        }

        ~WorldIOManager() {
            delete m_world;
            delete m_sq;
            delete m_audioManager;
            delete m_progress;
            delete m_saveLoadMessage;
        }

        void loadWorld(std::string worldName); // These public versions of the functions are multi-threading versions for anybody to use
        void saveWorld(std::string worldName);
        void createWorld(unsigned int seed, std::string worldName, bool isFlat);

        World* getWorld() { return m_world; } /// Singleton of world, essentially
        AudioManager* getAudioManager() { return m_audioManager; }
        void setWorldTime(float newTime) { m_world->time = newTime; }
        void setPlayer(Player* player) { m_world->player = player; }
        ScriptQueue* getScriptQueue() { return m_sq; }

        void setWorldEra(WorldEra newEra);

        StructureData loadStructureFromFile(std::string& filepath);
        void placeStructure(StructureData& structure, glm::vec2 position);

        #ifdef DEV_CONTROLS
        void saveStructureToFile(std::string& filepath, StructureData& structureData);
        void saveStructureToFile(std::string& filepath, glm::vec4 destRect);
        #endif // DEV_CONTROLS

        float getProgress() const { return *m_progress; }
        void setProgress(float p) { mtx_prog.lock(); *m_progress = p; mtx_prog.unlock(); }
        std::string getMessage() const { return *m_saveLoadMessage; }
        void setMessage(std::string msg) { mtx_msg.lock(); *m_saveLoadMessage = msg; mtx_msg.unlock(); }

    private:
        void P_loadWorld(std::string worldName); /// TODO: Make multi-threaded so that we can view progress and load at the same time
        void P_saveWorld(std::string worldName); /// TODO: Make multi-threaded so that we can view progress and save at the same time
        void P_createWorld(unsigned int seed, std::string worldName, bool isFlat);

        void clearWorld(); // Destroys all chunks and everything in m_world.
        /*
            Seed will govern where the biomes are (random engine #1) and it will also seed the random engine for
            all of the world (random engine #2)
        */

        std::string m_saveVersion = "1.2.1";

        Logger* logger = nullptr;

        World* m_world = nullptr;
        GLEngine::InputManager* m_input = nullptr;
        ScriptQueue* m_sq = nullptr;
        AudioManager* m_audioManager = nullptr;

        GLEngine::Window* m_window = nullptr;

        float* m_progress = nullptr;
        std::string* m_saveLoadMessage = nullptr;

        std::mutex mtx_msg;
        std::mutex mtx_prog;
};
