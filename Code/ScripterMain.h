#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "../Logging.h"

class World;
namespace QuestModule { class QuestManager; }
class GameplayScreen;
class AudioManager;
#include <ParticleEngine2D.h>
namespace ScriptingModule {
    class LuaScript;
    class Script;
}
class lua_State;

namespace ScriptingModule {

    class Scripter {
        public:
            Scripter();
            void init(World* world, QuestModule::QuestManager* qm, GameplayScreen* gs, AudioManager* audio, GLEngine::ParticleEngine2D* particleEngine); // Initialize all lua functions

            /*void showAlert(std::string& title, std::string& text); // Shows an alert window, with custom text, courtesy of CEGUI
            void showPlayerInventory(bool show); // Opens/closes player inventory on screen.
            void showBlockInventory(bool show, Block* block); // Shows/hides a block's inventory on screen in same style as player's.
            */
            void update();

            ScriptingModule::LuaScript* executeScript(ScriptingModule::Script& script); /// TODO: For the love of all things unholy, do NOT pass scripts by reference. They do not like that.
            ScriptingModule::LuaScript* executeCommand(std::string& command);
        private:
            void updateEvents(); // Goes through all yielded scripts and queued events, assigning appropriate ones to each other.

            Logger* logger = Logger::getInstance();
            lua_State* m_luaState = nullptr; // Used to load files, etc.

            std::vector<ScriptingModule::LuaScript*> m_scripts;
            std::unordered_map<std::string, unsigned int> m_chunkRefCache;
            std::multimap<std::string, ScriptingModule::LuaScript*> m_yieldedScripts; // An ordered map (we need to iterate through ranges, using upper_bound() and lower_bound()) of scripts waiting for specific events. These are copies of pointers in the main m_scripts vector.

            unsigned long int m_numOfScripts = 0;
    };

}
