#include "ScripterMain.h"

#include "../World.h"
#include "../QuestManager.h"
#include "../GameplayScreen.h"
#include "../AudioManager.h"
#include <ParticleBatch2D.h>
#include "LuaScript.h"

#include "../EventQueue.h"
#include "ScriptQueue.h"

#include <Camera2D.h>

#include "../ExtraFunctions.h"

#include "../XMLData.h"

#include "../EntityNPC.h"
#include "../EntityPlayer.h"

#include "ScriptingHelperFunctions.h"

Scripter::Scripter() {

}

void Scripter::init(World* world, QuestManager* qm, GameplayScreen* gs, AudioManager* audio, GLEngine::ParticleEngine2D* particleEngine) {
    m_luaState = luaL_newstate();

    luaL_openlibs(m_luaState);

    pushDepsToRegistry(m_luaState, world, qm, gs, audio, particleEngine);

    // Utility
    pushFunction(m_luaState, LuaScript::l_playSound, "playSound");
    pushFunction(m_luaState, LuaScript::l_createParticle, "createParticle");

    // Setters
    pushFunction(m_luaState, LuaScript::l_setBlock, "setBlock");
    pushFunction(m_luaState, LuaScript::l_removeBlock, "removeBlock");
    pushFunction(m_luaState, LuaScript::l_showBlock, "showBlock");
    pushFunction(m_luaState, LuaScript::l_hideBlock, "hideBlock");
    pushFunction(m_luaState, LuaScript::l_setBlockSize, "setBlockSize");

    pushFunction(m_luaState, LuaScript::l_getBlockData, "getBlockData");
    pushFunction(m_luaState, LuaScript::l_setBlockMetaData, "setBlockMetaData");

    pushFunction(m_luaState, LuaScript::l_addEntity, "addEntity");
    pushFunction(m_luaState, LuaScript::l_removeEntity, "removeEntity");
    pushFunction(m_luaState, LuaScript::l_showEntity, "showEntity");
    pushFunction(m_luaState, LuaScript::l_hideEntity, "hideEntity");
    pushFunction(m_luaState, LuaScript::l_setEntityMetaData, "setEntityMetaData");

    pushFunction(m_luaState, LuaScript::l_setTime, "setTime");
    pushFunction(m_luaState, LuaScript::l_teleport, "teleport");
    pushFunction(m_luaState, LuaScript::l_giveItem, "giveItem");

    pushFunction(m_luaState, LuaScript::l_setPlayerCanInteract, "setPlayerCanInteract");
    pushFunction(m_luaState, LuaScript::l_setPlayerStat_sanity, "setPlayerStat_sanity");
    pushFunction(m_luaState, LuaScript::l_setPlayerGodMode, "setPlayerGodMode");

    pushFunction(m_luaState, LuaScript::l_setFlag, "setFlag");

    pushFunction(m_luaState, LuaScript::l_setEra, "setEra");

    pushFunction(m_luaState, LuaScript::l_camera_setLocked, "camera_setLocked");
    pushFunction(m_luaState, LuaScript::l_camera_setPosition, "camera_setPosition");
    pushFunction(m_luaState, LuaScript::l_camera_move, "camera_move");
    pushFunction(m_luaState, LuaScript::l_camera_smoothMove, "camera_smoothMove");

    pushFunction(m_luaState, LuaScript::l_delay, "delay");
    pushFunction(m_luaState, LuaScript::l_waitForEvent, "waitForEvent");
    pushFunction(m_luaState, LuaScript::l_pause, "pause");
    pushFunction(m_luaState, LuaScript::l_play, "unpause");

    pushFunction(m_luaState, LuaScript::l_startTrade, "startTrade");
    pushFunction(m_luaState, LuaScript::l_startDialogue, "startDialogue");

    pushFunction(m_luaState, LuaScript::l_getEntitiesNear, "getEntitiesNear");
    pushFunction(m_luaState, LuaScript::l_getEntitiesArea, "getEntitiesNear");

    pushFunction(m_luaState, LuaScript::l_getPlayer, "getPlayer");
    pushFunction(m_luaState, LuaScript::l_getSpeakingEntity, "getSelectedEntity");

    pushFunction(m_luaState, LuaScript::l_getEntityPosition, "getEntityPosition");

    pushFunction(m_luaState, LuaScript::l_getCurrentFrame, "getFrame");

    pushFunction(m_luaState, LuaScript::l_log, "log");
}

void Scripter::update() {
    // Get active script from ScriptQueue
    // Execute script
    // Take script off active list
    // Repeat until no active scripts exist

    for(unsigned int i = 0; i < ScriptQueue::m_activeScripts.size(); i++) {
        executeScript(ScriptQueue::m_activeScripts[i]);
    }

    ScriptQueue::deactivateScripts();

    std::vector<LuaScript*> liveScripts; // Scripts that aren't done
    for(unsigned int i = 0; i < m_scripts.size(); i++) {
        if(m_scripts[i]->isFinished()) {
            luaL_unref(m_luaState, LUA_REGISTRYINDEX, m_scripts[i]->getThreadReference());
            delete m_scripts[i];
            continue;
        } else {
            liveScripts.push_back(m_scripts[i]);
            m_scripts[i]->update(m_luaState);
        }
    }

    m_scripts.clear();
    m_scripts = liveScripts;
}

void Scripter::updateEvents() {
    /**
        Uses the static EventQueue to loop through all triggered events, applying them to whichever yielded scripts need them.
    **/
                                                                                                                                /// NOTE: The event system is only to be used for quests! Not regular scripts!
                                                                                                                                /// ONLY ONE TIME, LONG-RUNNING SCRIPTS!!!!!
    // First, we need to get all the triggered events.
    std::vector<EventModule::Event> evnts = EventModule::EventQueue::getEvents();

    // Then we loop through all of them.
    for(auto evnt : evnts) {
        // Find the lower and upper bounds of all yielded scripts with the evnt's type
        std::string type = evnt.getType();
        auto lb = m_yieldedScripts.lower_bound(type); // The first element that matches
        auto ub = m_yieldedScripts.upper_bound(type); // One past the last element that matches

        /// IMPORTANT NOTE: If there are events of duplicate type, then only the first is used.

        while(lb != ub) { // This also handles the case where either one is std::end()
            lb->second->resume(m_luaState, evnt.getArguments());
            m_yieldedScripts.erase(lb);
            lb++;
        }
    }

    EventModule::EventQueue::clearQueue();
}

void Scripter::executeScript(Script& script) {
    std::string returnMessage;

    lua_checkstack(m_luaState, 10);

    LuaScript* s = new LuaScript(m_luaState);

    int chunkRef = -1;

    std::string fn = script.getFileName();
    auto index = m_chunkRefCache.find(fn);

    if(index != m_chunkRefCache.end()) {
        chunkRef = m_chunkRefCache.at(fn);
    } else {
        int newChunkRef = s->loadScriptFile(fn);
        if(newChunkRef == -1) {
            Logger::getInstance()->log("LUA ERROR: BAD FILE LOAD", true);
            return;
        }
        m_chunkRefCache.insert(std::pair<std::string, unsigned int>(fn, newChunkRef));
        chunkRef = newChunkRef;
        logger->log("Loaded script: " + fn);
    }

    s->run(chunkRef, script.arguments);

    /*lua_pop(m_luaState, 1);
    lua_pushthreadtotable(m_luaState, m_numOfScripts, m_luaState);
    m_numOfScripts++;*/

    m_scripts.push_back(s);
}

std::string Scripter::executeCommand(std::string& command) {
    std::string returnMessage;

    lua_checkstack(m_luaState, 10);

    LuaScript* s = new LuaScript(m_luaState);
    s->loadScriptString(command);
    //s->run(0, std::vector<Argument>{}); /// TODO: FIX

    /*lua_pop(m_luaState, 1);
    lua_pushthreadtotable(m_luaState, m_numOfScripts, m_luaState);
    m_numOfScripts++;*/

    m_scripts.push_back(s);

    return "NO_MSG";
}
