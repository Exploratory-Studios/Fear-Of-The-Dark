#pragma once

#include "ScriptQueue.h"
#include "../GameplayScreen.h"

#include "../Logging.h"

#include "../ExtraFunctions.h"

#include <thread>

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

class LuaScript {
public:
    LuaScript(World* world, QuestManager* qm, GameplayScreen* gs) {
        L = luaL_newstate();

        ALL_DEPS
        lua_pushcclosure(L, l_setBlock, 3); // Push function, referring to the world pointer
        lua_setglobal(L, "setBlock"); // Name function

        ALL_DEPS
        lua_pushcclosure(L, l_removeBlock, 3); // Push function, referring to the world pointer
        lua_setglobal(L, "removeBlock"); // Name function

        ALL_DEPS
        lua_pushcclosure(L, l_showBlock, 3); // Push function, referring to the world pointer
        lua_setglobal(L, "showBlock"); // Name function

        ALL_DEPS
        lua_pushcclosure(L, l_hideBlock, 3); // Push function, referring to the world pointer
        lua_setglobal(L, "hideBlock"); // Name function


        ALL_DEPS
        lua_pushcclosure(L, l_addEntity, 3); // Push function, referring to the world pointer
        lua_setglobal(L, "addEntity"); // Name function

        ALL_DEPS
        lua_pushcclosure(L, l_removeEntity, 3); // Push function, referring to the world pointer
        lua_setglobal(L, "removeEntity"); // Name function

        ALL_DEPS
        lua_pushcclosure(L, l_showEntity, 3); // Push function, referring to the world pointer
        lua_setglobal(L, "showEntity"); // Name function

        ALL_DEPS
        lua_pushcclosure(L, l_hideEntity, 3); // Push function, referring to the world pointer
        lua_setglobal(L, "hideEntity"); // Name function


        ALL_DEPS
        lua_pushcclosure(L, l_setTime, 3); // Push function, referring to the world pointer
        lua_setglobal(L, "setTime"); // Name function

        ALL_DEPS
        lua_pushcclosure(L, l_teleport, 3); // Push function, referring to the world pointer
        lua_setglobal(L, "teleport"); // Name function

        ALL_DEPS
        lua_pushcclosure(L, l_giveItem, 3); // Push function, referring to the world pointer
        lua_setglobal(L, "giveItem"); // Name function


        ALL_DEPS
        lua_pushcclosure(L, l_setPlayerCanInteract, 3); // Push function, referring to the world pointer
        lua_setglobal(L, "setPlayerCanInteract"); // Name function

        ALL_DEPS
        lua_pushcclosure(L, l_setPlayerStat_sanity, 3); // Push function, referring to the world pointer
        lua_setglobal(L, "setPlayerStat_sanity"); // Name function

        ALL_DEPS
        lua_pushcclosure(L, l_setPlayerGodMode, 3); // Push function, referring to the world pointer
        lua_setglobal(L, "setPlayerGodMode"); // Name function


        ALL_DEPS
        lua_pushcclosure(L, l_setFlag, 3); // Push function, referring to the world pointer
        lua_setglobal(L, "setFlag"); // Name function


        ALL_DEPS
        lua_pushcclosure(L, l_setEra, 3); // Push function, referring to the world pointer
        lua_setglobal(L, "setEra"); // Name function


        ALL_DEPS
        lua_pushcclosure(L, l_camera_setLocked, 3); // Push function, referring to the world pointer
        lua_setglobal(L, "camera_setLocked"); // Name function

        ALL_DEPS
        lua_pushcclosure(L, l_camera_setPosition, 3); // Push function, referring to the world pointer
        lua_setglobal(L, "camera_setPosition"); // Name function

        ALL_DEPS
        lua_pushcclosure(L, l_camera_move, 3); // Push function, referring to the world pointer
        lua_setglobal(L, "camera_move"); // Name function

        ALL_DEPS
        lua_pushcclosure(L, l_camera_smoothMove, 3); // Push function, referring to the world pointer
        lua_setglobal(L, "camera_smoothMove"); // Name function


        ALL_DEPS
        lua_pushcclosure(L, l_delay, 3); // Push function, referring to the world pointer
        lua_setglobal(L, "delay"); // Name function

        ALL_DEPS
        lua_pushcclosure(L, l_pause, 3); // Push function, referring to the world pointer
        lua_setglobal(L, "pause"); // Name function

        ALL_DEPS
        lua_pushcclosure(L, l_play, 3); // Push function, referring to the world pointer
        lua_setglobal(L, "unpause"); // Name function


        ALL_DEPS
        lua_pushcclosure(L, l_startTrade, 3); // Push function, referring to the world pointer
        lua_setglobal(L, "startTrade"); // Name function

        ALL_DEPS
        lua_pushcclosure(L, l_startDialogue, 3); // Push function, referring to the world pointer
        lua_setglobal(L, "startDialogue"); // Name function

        /*
            static int l_setBlock(lua_State* L); // Wrappers. A lot of em
            static int l_removeBlock(lua_State* L);
            static int l_showBlock(lua_State* L);
            static int l_hideBlock(lua_State* L);

            static int l_addEntity(lua_State* L);
            static int l_removeEntity(lua_State* L);
            static int l_showEntity(lua_State* L);
            static int l_hideEntity(lua_State* L);

            static int l_setTime(lua_State* L)
            static int l_teleport(lua_State* L)
            static int l_giveItem(lua_State* L)

            static int l_setPlayerCanInteract(lua_State* L)
            static int l_setPlayerStat_sanity(lua_State* L)
            static int l_setPlayerGodMode(lua_State* L)

            static int l_setFlag(lua_State* L)

            static int l_setEra(lua_State* L)

            static int l_camera_setLocked(lua_State* L)
            static int l_camera_setPosition(lua_State* L)
            static int l_camera_move(lua_State* L)
            static int l_camera_smoothMove(lua_State* L)

            static int l_delay(lua_State* L)
            static int l_pause(lua_State* L)
            static int l_play(lua_State* L)

            static int l_startTrade(lua_State* L)
            static int l_startDialogue(lua_State* L)

            std::vector<glm::vec2> relativePositionTarget(World* world, Entity* relativeTo, glm::vec2 relativePos);
            std::vector<glm::vec2> areaPositionTarget(World* world, glm::vec2 pos1, glm::vec2 pos2);
        */
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

        //std::thread t([=]{run();});
        //t.detach();

        run();

        return 0;
    }
    int runScriptString(std::string script) {
        if(luaL_loadstring(L, script.c_str())) {
            std::cout << "LUA ERROR ON LOADING SCRIPT: \"" << script << "\"";
            L = 0;
            return 1;
        }

        std::thread t([=]{run();});
        t.detach();

        return 0;
    }

private:
    void run() {
        lua_pcall(L, 0, 0, 0); // run

        lua_close(L);
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
