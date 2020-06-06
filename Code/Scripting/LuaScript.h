#pragma once

#include <glm/glm.hpp>

#include <functional>
#include <ParticleEngine2D.h>

#include "ScriptQueue.h"

#include "../LuaHeaders.h"

class ParticleUpdate {
    public:
        ParticleUpdate(std::string scriptPathP, lua_State** Tp) : scriptPath(scriptPathP), T(Tp) {}

        std::function<void(GLEngine::Particle2D&, float)> getBoundFunctor() {
            return std::bind(updateFunction, std::placeholders::_1, std::placeholders::_2, scriptPath, T);
        }

    private:
        static void updateFunction(GLEngine::Particle2D& particle, float deltaTime, std::string& filepath, lua_State** T);

        std::string scriptPath;
        lua_State** T = nullptr;
};

#ifdef DEBUG

#include <iostream>

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

      case LUA_TTABLE: /* tables */
        lua_pushnil(L);
        std::cout << "table: {\n";
        while (lua_next(L, i) != 0) {
           /* uses 'key' (at index -2) and 'value' (at index -1) */
           std::cout << "\"" << lua_tostring(L, -2) << "\"" << " = " << lua_typename(L, lua_type(L, -1)) << "\n";
           /* removes 'value'; keeps 'key' for next iteration */
           lua_pop(L, 1);
         }
         std::cout << "\n}";
         break;

      default:  /* other values */
        std::cout << lua_typename(L, t);
        break;

    }
    std::cout << "\n";
  }
  std::cout << std::endl;
}

#endif

class LuaScript {
public:
    LuaScript(lua_State* state) {
        T = lua_newthread(state);
        m_threadReference = luaL_ref(state, LUA_REGISTRYINDEX);
        if(m_threadReference == LUA_REFNIL) {
            T = 0;
            Logger::getInstance()->log("LUA ERROR: REFNIL on new script creation!", true);
        }
        m_parent = state;
    }

    ~LuaScript() {
        destroy();
    }

    void destroy() {
        m_finished = true;
        lua_settop(T, 0);
        T = 0;
    }

    int loadScriptFile(std::string& filepath);
    int loadScriptString(std::string script);

    void run(unsigned int chunkIndex, std::vector<Argument>& args);
    void update(lua_State* state);

    lua_State* getThread() { return T; }
    void setThread(lua_State* s) { T = s; }

    unsigned int getThreadReference() { return m_threadReference; }

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
        static int l_setBlockSize(lua_State* L); // Sets the UV of a block's texture

        static int l_getBlockData(lua_State* L); // returns a table of block data to the stack
        static int l_setBlockMetaData(lua_State* L); // returns a table of block data to the stack

        static int l_addEntity(lua_State* L); // requires UUID of entity
        static int l_removeEntity(lua_State* L);
        static int l_showEntity(lua_State* L);
        static int l_hideEntity(lua_State* L);
        static int l_setEntityMetaData(lua_State* L); // requires UUID and a table of metadata

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

    lua_State* T = nullptr; // Main thread. Used for delaying the script
    int m_threadReference = 0; // LUA_REFNIL is -1, so we need to be able to accept that.
    bool m_finished = false;
    static lua_State* m_parent;
};
