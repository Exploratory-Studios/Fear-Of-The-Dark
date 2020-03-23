#pragma once

#include "ScriptQueue.h"
#include "../GameplayScreen.h"
#include "../Entity.h"

#include "../Logging.h"

#include "../ExtraFunctions.h"

#include <ParticleBatch2D.h>

extern "C" {
    #include <lua5.3/lua.h>
    #include <lua5.3/lauxlib.h>
    #include <lua5.3/luaconf.h>
    #include <lua5.3/lualib.h>
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
void lua_pushthreadtotable(lua_State* L, int index, lua_State* thread);

class Scripter {
    public:
        Scripter();
        void init(World* world, QuestManager* qm, GameplayScreen* gs, AudioManager* audio, GLEngine::ParticleEngine2D* particleEngine); // Initialize all lua functions

        /*void showAlert(std::string& title, std::string& text); // Shows an alert window, with custom text, courtesy of CEGUI
        void showPlayerInventory(bool show); // Opens/closes player inventory on screen.
        void showBlockInventory(bool show, Block* block); // Shows/hides a block's inventory on screen in same style as player's.
        */
        void update();

        std::string executeScript(Script* script); /// TODO: For the love of all things unholy, do NOT pass scripts by reference. They do not like that.
        std::string executeCommand(std::string& command);
    private:
        Logger* logger = Logger::getInstance();
        lua_State* m_luaState = nullptr;

        std::vector<LuaScript*> m_scripts;

        unsigned long int m_numOfScripts = 0;
};

#define WORLD_KEY "world"
#define GAMEPLAYSCREEN_KEY "gameplayscreen"
#define QUESTMANAGER_KEY "questmanager"
#define AUDIOMANAGER_KEY "audiomanager"
#define PARTICLEENGINE_KEY "particleengine"

#define addDepsToRegistry(L, world, qm, gs, audio, particle)          \
    lua_pushlightuserdata(L, (void *)&WORLD_KEY);           \
    lua_pushlightuserdata(L, static_cast<void*>(world));    \
    lua_settable(L, LUA_REGISTRYINDEX);                     \
    lua_pushlightuserdata(L, (void *)&QUESTMANAGER_KEY);    \
    lua_pushlightuserdata(L, static_cast<void*>(qm));       \
    lua_settable(L, LUA_REGISTRYINDEX);                     \
    lua_pushlightuserdata(L, (void *)&GAMEPLAYSCREEN_KEY);  \
    lua_pushlightuserdata(L, static_cast<void*>(gs));       \
    lua_settable(L, LUA_REGISTRYINDEX);                     \
    lua_pushlightuserdata(L, (void *)&AUDIOMANAGER_KEY);    \
    lua_pushlightuserdata(L, static_cast<void*>(audio));    \
    lua_settable(L, LUA_REGISTRYINDEX);                     \
    lua_pushlightuserdata(L, (void *)&PARTICLEENGINE_KEY);  \
    lua_pushlightuserdata(L, static_cast<void*>(particle)); \
    lua_settable(L, LUA_REGISTRYINDEX);                     \


#define addFunction(func, name) { lua_pushcfunction(m_luaState, func); lua_setglobal(m_luaState, name); }

class ParticleUpdate {
public:
    ParticleUpdate(std::string scriptPathP, lua_State* Tp) : scriptPath(scriptPathP), T(Tp) {}
    void operator() (GLEngine::Particle2D& particle, float deltaTime) const;

    std::string scriptPath;
    lua_State* T;
};

static void stackDump (lua_State *L) {
  int i;
  int top = lua_gettop(L);
  for (i = 1; i <= top; i++) {  /* repeat for each level */
    int t = lua_type(L, i);
    std::cout << i << ": ";
    switch (t) {

      case LUA_TSTRING:  /* strings */
        std::cout << lua_tostring(L, i);
        break;

      case LUA_TBOOLEAN:  /* booleans */
        std::cout << lua_toboolean(L, i);
        break;

      case LUA_TNUMBER:  /* numbers */
        std::cout << lua_tonumber(L, i);
        break;

      default:  /* other values */
        std::cout << lua_typename(L, t);
        break;

    }
    std::cout << "    ";
  }
  std::cout << std::endl;
}

class LuaScript {
public:
    LuaScript(lua_State* state) {
        T = lua_newthread(state);
        m_particle = nullptr;
    }
    ~LuaScript() {
        T = 0;
    }

    int runScriptFile(std::string filepath) {
        int errCode = luaL_loadfile(T, filepath.c_str());

        if(errCode) {
            printf("error code %i: %s\n", errCode, lua_tostring(T, -1));
            T = 0;
            return 1;
        }

        run();

        return 0;
    }
    int runScriptString(std::string script) {
        std::string sc = script;

        int errCode = luaL_loadstring(T, (char*)sc.c_str());

        if(errCode) {
            printf("error code %i: %s\n", errCode, lua_tostring(T, -1));
            T = 0;
            return 1;
        }

        run();

        return 0;
    }
    void update(lua_State* state) {
        if(lua_status(T) == LUA_YIELD || !m_finished) { // Is the thread currently paused?
            int delayLeft = lua_tointeger(T, -1); // Get the delay from the top of the stack
            delayLeft--; // Take one frame away
            lua_pop(T, 1); // Take the delay value off of the stack completely

            if(delayLeft <= 0) {
                lua_resume(T, state, 0); // Continue the script
            } else {
                lua_pushinteger(T, delayLeft); // Otherwise, re-add the new delay value
            }
        } else if(lua_status(T) == LUA_OK && !m_finished) {
            m_finished = true;
            //lua_close(T); Can't lua_close threads!
        }
    }

    lua_State* getThread() { return T; }

    bool isFinished() { return m_finished; }

    // Wrappers for Lua
    // Utility
        static int l_playSound(lua_State* L);
        static int l_createParticle(lua_State* L);

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

        static int l_getCurrentFrame(lua_State* L); // returns the current frame

    // Debug
        static int l_log(lua_State* L);

private:
    void run() {
        lua_resume(T, NULL, 0);
    }

    lua_State* T = nullptr; // Main thread. Used for delaying the script
    bool m_finished = false;
    static GLEngine::Particle2D* m_particle;
};
