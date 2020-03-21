#pragma once

#include "ScriptQueue.h"
#include "../GameplayScreen.h"
#include "../Entity.h"

#include "../Logging.h"

#include "../ExtraFunctions.h"

extern "C" {
    #include <lua5.3/lua.h>
    #include <lua5.3/lauxlib.h>
    #include <lua5.3/luaconf.h>
}

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
class ScriptQueue;

class LuaScript;

void setBlock(World* world, unsigned int id, glm::vec2 pos, int layer, MetaData metaData);
void removeBlock(World* world, int x, int y, unsigned int layer);
void showBlock(World* world, int x, int y, unsigned int layer);
void hideBlock(World* world, int x, int y, unsigned int layer);

unsigned int addEntity(World* world, Entity* newEntity);
void removeEntity(World* world, std::string UUID);
void showEntity(World* world, std::string UUID);
void hideEntity(World* world, std::string UUID);

void setTime(World* world, unsigned int time);
void teleport(World* world, std::string UUID, glm::vec2 pos);
void giveItem(World* world, std::string UUID, unsigned int id, unsigned int quantity);

void setPlayerCanInteract(World* world, bool canInteract);
void setPlayerGodMode(World* world, bool godMode);
void setPlayerStat_sanity(World* world, float sanity);

void setFlag(QuestManager* qm, unsigned int id, bool val);

void setEra(World* world, std::string era);

void camera_setLocked(GameplayScreen* gs, bool locked);
void camera_setPosition(GameplayScreen* gs, glm::vec2 pos);
void camera_move(GameplayScreen* gs, glm::vec2 relPos);
void camera_smoothMove(GameplayScreen* gs, glm::vec2 relPos, float speed);

void pause(GameplayScreen* gs);
void play(GameplayScreen* gs);

void startTrade(QuestManager* qm, unsigned int tableID);
void startDialogue(QuestManager* qm, unsigned int questionID);

std::vector<Entity*> nearEntityTarget(World* world, glm::vec2 nearTo);
std::vector<Entity*> areaEntityTarget(World* world, glm::vec2 pos1, glm::vec2 pos2); // returns an array of entities' indices

void lua_pushintegertotable(lua_State* L, int index, int value);
void lua_pushnumbertotable(lua_State* L, int index, float value);
void lua_pushstringtotable(lua_State* L, int index, char* value);

class Scripter {
    public:
        Scripter();
        void init(World* world, QuestManager* qm, GameplayScreen* gs); // Initialize all lua functions

        /*void showAlert(std::string& title, std::string& text); // Shows an alert window, with custom text, courtesy of CEGUI
        void showPlayerInventory(bool show); // Opens/closes player inventory on screen.
        void showBlockInventory(bool show, Block* block); // Shows/hides a block's inventory on screen in same style as player's.
        */
        void update(World* world, ScriptQueue* sq, QuestManager* qm, GameplayScreen* gs);

        std::string executeScript(World* world, QuestManager* qm, GameplayScreen* gs, Script* script); /// TODO: For the love of all things unholy, do NOT pass scripts by reference. They do not like that.
        std::string executeCommand(World* world, QuestManager* qm, GameplayScreen* gs, std::string& command);
    private:
        Logger* logger = Logger::getInstance();
        lua_State* m_luaState = nullptr;

        std::vector<LuaScript*> m_scripts;
};

#define WORLD_KEY "world"
#define GAMEPLAYSCREEN_KEY "gameplayscreen"
#define QUESTMANAGER_KEY "questmanager"

#define addDepsToRegistry(L, world, qm, gs)                 \
    lua_pushlightuserdata(L, (void *)&WORLD_KEY);           \
    lua_pushlightuserdata(L, static_cast<void*>(world));    \
    lua_settable(L, LUA_REGISTRYINDEX);                     \
    lua_pushlightuserdata(L, (void *)&QUESTMANAGER_KEY);    \
    lua_pushlightuserdata(L, static_cast<void*>(qm));       \
    lua_settable(L, LUA_REGISTRYINDEX);                     \
    lua_pushlightuserdata(L, (void *)&GAMEPLAYSCREEN_KEY);  \
    lua_pushlightuserdata(L, static_cast<void*>(gs));       \
    lua_settable(L, LUA_REGISTRYINDEX);

#define addFunction(func, name) { lua_pushcfunction(m_luaState, func); lua_setglobal(m_luaState, name); }

class LuaScript {
public:
    LuaScript(lua_State* state) {
        T = lua_newthread(state);
    }
    ~LuaScript() {
        T = 0;
    }

    int runScriptFile(std::string filepath) {
        if(luaL_loadfile(T, filepath.c_str())) {
            std::cout << "LUA ERROR ON LOADING FILE: " << filepath;
            T = 0;
            return 1;
        }

        run();

        return 0;
    }
    int runScriptString(std::string script) {
        if(luaL_loadstring(T, script.c_str())) {
            std::cout << "LUA ERROR ON LOADING SCRIPT: \"" << script << "\"";
            T = 0;
            return 1;
        }

        run();

        return 0;
    }
    void update(lua_State* state) {
        if(lua_status(T) == LUA_YIELD) { // Is the thread currently paused?
            int delayLeft = lua_tointeger(T, -1); // Get the delay from the top of the stack
            delayLeft--; // Take one frame away
            lua_pop(T, 1); // Take the delay value off of the stack completely

            if(delayLeft <= 0) {
                lua_resume(T, state, 0); // Continue the script
            } else {
                lua_pushinteger(T, delayLeft); // Otherwise, re-add the new delay value
            }
        } else if(lua_status(T) == LUA_OK) {
            m_finished = true;
        }
    }

    bool isFinished() { return m_finished; }

    // Wrappers for Lua
    // Setters
        static int l_setBlock(lua_State* L);
        static int l_removeBlock(lua_State* L);
        static int l_showBlock(lua_State* L);
        static int l_hideBlock(lua_State* L);

        static int l_addEntity(lua_State* L); // requires UUID of entity
        static int l_removeEntity(lua_State* L);
        static int l_showEntity(lua_State* L);
        static int l_hideEntity(lua_State* L);

        static int l_setTime(lua_State* L);
        static int l_teleport(lua_State* L);
        static int l_giveItem(lua_State* L);

        static int l_setPlayerCanInteract(lua_State* L);
        static int l_setPlayerStat_sanity(lua_State* L);
        static int l_setPlayerGodMode(lua_State* L);

        static int l_setFlag(lua_State* L);

        static int l_setEra(lua_State* L);

        static int l_camera_setLocked(lua_State* L);
        static int l_camera_setPosition(lua_State* L);
        static int l_camera_move(lua_State* L);
        static int l_camera_smoothMove(lua_State* L);

        static int l_delay(lua_State* L);
        static int l_pause(lua_State* L);
        static int l_play(lua_State* L);

        static int l_startTrade(lua_State* L);
        static int l_startDialogue(lua_State* L);

    // Getters
        static int l_getEntitiesNear(lua_State* L); // returns UUIDs
        static int l_getEntitiesArea(lua_State* L);
        static int l_getPlayer(lua_State* L);
        static int l_getSpeakingEntity(lua_State* L);

        static int l_getEntityPosition(lua_State* L); // takes entity UUID, returns a 2-index array

    // Debug
        static int l_log(lua_State* L);

private:
    void run() {
        lua_resume(T, NULL, 0);
    }

    lua_State* T = nullptr; // Main thread. Used for delaying the script
    bool m_finished = false;
};
