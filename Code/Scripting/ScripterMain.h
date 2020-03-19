#pragma once

#include "ScriptQueue.h"
#include "../GameplayScreen.h"
#include "../Entity.h"

#include "../Logging.h"

#include "../ExtraFunctions.h"

#include <boost/thread.hpp>
#include <boost/chrono.hpp>

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
void removeEntity(World* world, unsigned int index);
void showEntity(World* world, unsigned int index);
void hideEntity(World* world, unsigned int index);

void setTime(World* world, unsigned int time);
void teleport(World* world, std::vector<unsigned int> entityIndex, glm::vec2 pos);
void giveItem(World* world, std::vector<unsigned int> entityIndex, unsigned int id, unsigned int quantity);

void setPlayerCanInteract(World* world, bool canInteract);
void setPlayerGodMode(World* world, bool godMode);
void setPlayerStat_sanity(World* world, float sanity);

void setFlag(QuestManager* qm, unsigned int id, bool val);

void setEra(World* world, std::string era);

void camera_setLocked(GameplayScreen* gs, bool locked);
void camera_setPosition(GameplayScreen* gs, glm::vec2 pos);
void camera_move(GameplayScreen* gs, glm::vec2 relPos);
void camera_smoothMove(GameplayScreen* gs, glm::vec2 relPos, float speed);

void delay(unsigned int delayMS);
void pause(GameplayScreen* gs);
void play(GameplayScreen* gs);

void startTrade(QuestManager* qm, unsigned int tableID);
void startDialogue(QuestManager* qm, unsigned int questionID);

std::vector<unsigned int> nearEntityTarget(World* world, glm::vec2 nearTo);
std::vector<unsigned int> areaEntityTarget(World* world, glm::vec2 pos1, glm::vec2 pos2);
std::vector<unsigned int> playerEntityTarget(World* world);
std::vector<unsigned int> speakerEntityTarget(World* world);

std::vector<glm::vec2> relativePositionTarget(World* world, Entity* relativeTo, glm::vec2 relativePos);
std::vector<glm::vec2> areaPositionTarget(World* world, glm::vec2 pos1, glm::vec2 pos2);

class Scripter {
    public:
        Scripter();
        void init();

        /*void showAlert(std::string& title, std::string& text); // Shows an alert window, with custom text, courtesy of CEGUI
        void showPlayerInventory(bool show); // Opens/closes player inventory on screen.
        void showBlockInventory(bool show, Block* block); // Shows/hides a block's inventory on screen in same style as player's.
        */
        void update(World* world, ScriptQueue* sq, QuestManager* qm, GameplayScreen* gs);

        std::string executeScript(World* world, QuestManager* qm, GameplayScreen* gs, Script* script); /// TODO: For the love of all things unholy, do NOT pass scripts by reference. They do not like that.
        std::string executeCommand(World* world, QuestManager* qm, GameplayScreen* gs, std::string& command, Script* script = nullptr);
    private:
        Logger* logger = Logger::getInstance();
};

#define ALL_DEPS { lua_pushlightuserdata(L, static_cast<void*>(world)); lua_pushlightuserdata(L, static_cast<void*>(qm)); lua_pushlightuserdata(L, static_cast<void*>(gs)); }
#define addFunction(func, name) { ALL_DEPS lua_pushcclosure(L, func, 3); lua_setglobal(L, name); }

class LuaScript {
public:
    LuaScript(World* world, QuestManager* qm, GameplayScreen* gs) {
        L = luaL_newstate();

        addFunction(l_setBlock, "setBlock");
        addFunction(l_removeBlock, "removeBlock");
        addFunction(l_showBlock, "showBlock");
        addFunction(l_hideBlock, "hideBlock");

        addFunction(l_addEntity, "addEntity");
        addFunction(l_removeEntity, "removeEntity");
        addFunction(l_showEntity, "showEntity");
        addFunction(l_hideEntity, "hideEntity");

        addFunction(l_setTime, "setTime");
        addFunction(l_teleport, "teleport");
        addFunction(l_giveItem, "giveItem");

        addFunction(l_setPlayerCanInteract, "setPlayerCanInteract");
        addFunction(l_setPlayerStat_sanity, "setPlayerStat_sanity");
        addFunction(l_setPlayerGodMode, "setPlayerGodMode");

        addFunction(l_setFlag, "setFlag");

        addFunction(l_setEra, "setEra");

        addFunction(l_camera_setLocked, "camera_setLocked");
        addFunction(l_camera_setPosition, "camera_setPosition");
        addFunction(l_camera_move, "camera_move");
        addFunction(l_camera_smoothMove, "camera_smoothMove");

        addFunction(l_delay, "delay");
        addFunction(l_pause, "pause");
        addFunction(l_play, "unpause");

        addFunction(l_startTrade, "startTrade");
        addFunction(l_startDialogue, "startDialogue");
    }
    ~LuaScript() {
        L = 0;
    }

    int runScriptFile(std::string filepath) {
        if(luaL_loadfile(L, filepath.c_str())) {
            std::cout << "LUA ERROR ON LOADING FILE: " << filepath;
            L = 0;
            return 1;
        }

        boost::thread t = boost::thread([=]{run();});
        t.detach();

        return 0;
    }
    int runScriptString(std::string script) {
        if(luaL_loadstring(L, script.c_str())) {
            std::cout << "LUA ERROR ON LOADING SCRIPT: \"" << script << "\"";
            L = 0;
            return 1;
        }

        boost::thread t = boost::thread([=]{run();});
        t.detach();

        return 0;
    }

private:
    void run() {
        lua_pcall(L, 0, 0, 0); // run

        if(L) {
            lua_close(L);
        }
    }

    static int l_setBlock(lua_State* L); // Wrappers. A lot of em
    static int l_removeBlock(lua_State* L);
    static int l_showBlock(lua_State* L);
    static int l_hideBlock(lua_State* L);

    static int l_addEntity(lua_State* L);
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

    lua_State* L = nullptr;
};
