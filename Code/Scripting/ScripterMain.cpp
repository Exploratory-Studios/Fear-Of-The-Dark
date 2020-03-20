#include "ScripterMain.h"

#include "ScriptQueue.h"
#include "../Entity.h"
#include "../Entities.h"
#include "../QuestManager.h"

#include <Camera2D.h>

Scripter::Scripter() {

}

void Scripter::init(World* world, QuestManager* qm, GameplayScreen* gs) {
    m_luaState = luaL_newstate();

    addFunction(LuaScript::l_setBlock, "setBlock");
    addFunction(LuaScript::l_removeBlock, "removeBlock");
    addFunction(LuaScript::l_showBlock, "showBlock");
    addFunction(LuaScript::l_hideBlock, "hideBlock");

    addFunction(LuaScript::l_addEntity, "addEntity");
    addFunction(LuaScript::l_removeEntity, "removeEntity");
    addFunction(LuaScript::l_showEntity, "showEntity");
    addFunction(LuaScript::l_hideEntity, "hideEntity");

    addFunction(LuaScript::l_setTime, "setTime");
    addFunction(LuaScript::l_teleport, "teleport");
    addFunction(LuaScript::l_giveItem, "giveItem");

    addFunction(LuaScript::l_setPlayerCanInteract, "setPlayerCanInteract");
    addFunction(LuaScript::l_setPlayerStat_sanity, "setPlayerStat_sanity");
    addFunction(LuaScript::l_setPlayerGodMode, "setPlayerGodMode");

    addFunction(LuaScript::l_setFlag, "setFlag");

    addFunction(LuaScript::l_setEra, "setEra");

    addFunction(LuaScript::l_camera_setLocked, "camera_setLocked");
    addFunction(LuaScript::l_camera_setPosition, "camera_setPosition");
    addFunction(LuaScript::l_camera_move, "camera_move");
    addFunction(LuaScript::l_camera_smoothMove, "camera_smoothMove");

    addFunction(LuaScript::l_delay, "delay");
    addFunction(LuaScript::l_pause, "pause");
    addFunction(LuaScript::l_play, "unpause");

    addFunction(LuaScript::l_startTrade, "startTrade");
    addFunction(LuaScript::l_startDialogue, "startDialogue");
}

void Scripter::update(World* world, ScriptQueue* sq, QuestManager* qm, GameplayScreen* gs) {
    // Get active script from ScriptQueue
    // Execute script
    // Take script off active list
    // Repeat until no active scripts exist

    std::vector<Script*> scripts;

    scripts.clear();

    for(unsigned int i = 0; i < sq->m_activeScripts.size(); i++) {
        scripts.push_back(&(sq->m_activeScripts[i]));
    }

    if(scripts.size() > 0) {
        for(unsigned int i = 0; i < scripts.size(); i++) {
            executeScript(world, qm, gs, (scripts[i]));
            sq->deactivateScript(i);
        }
    }

    for(unsigned int i = 0; i < m_scripts.size(); i++) {
        m_scripts[i]->update(m_luaState);
        if(m_scripts[i]->isFinished()) {
            delete m_scripts[i];
            for(int j = i; j < m_scripts.size()-1; j++) {
                m_scripts[j] = m_scripts[j+1];
            }
            m_scripts.pop_back();
        }
    }
}

std::string Scripter::executeScript(World* world, QuestManager* qm, GameplayScreen* gs, Script* script) {
    std::string returnMessage;

    LuaScript* s = new LuaScript(m_luaState);
    if(script->isFile()) {
        s->runScriptFile(script->getFileName());
    } else {
        s->runScriptString(script->getText());
    }

    m_scripts.push_back(s);

    return "NO_MSG";
}

std::string Scripter::executeCommand(World* world, QuestManager* qm, GameplayScreen* gs, std::string& command) {
    std::string returnMessage;

    LuaScript* s = new LuaScript(m_luaState);
    s->runScriptString(command);

    m_scripts.push_back(s);

    return "NO_MSG";
}

std::vector<unsigned int> nearEntityTarget(World* world, glm::vec2 nearTo) {

        unsigned int chunkIndex = std::floor(nearTo.x / CHUNK_SIZE);

        float nearestDistance;
        unsigned int nearestId;
        unsigned int nearestChunkId;

        bool found = false;

        for(unsigned int i = 0; i < world->getEntities().size(); i++) { /// TODO: Optimize this
            float dist = std::sqrt(std::abs(nearTo.x - world->getEntities()[i]->getPosition().x) + std::abs(nearTo.y - world->getEntities()[i]->getPosition().y));
            if(dist < nearestDistance || !found) {
                nearestDistance = dist;
                nearestId = i;
                nearestChunkId = chunkIndex;
                found = true;
            }
        }

        std::vector<unsigned int> ret;
        ret.push_back(nearestId);

        return ret;
}

std::vector<unsigned int> areaEntityTarget(World* world, glm::vec2 pos1, glm::vec2 pos2) {
    unsigned int chunk1 = std::floor(pos1.x / CHUNK_SIZE) + WORLD_SIZE;
    unsigned int chunk2 = std::floor(pos2.x / CHUNK_SIZE) + WORLD_SIZE;

    std::vector<unsigned int> ret;

    for(unsigned int i = 0; i <= chunk1-chunk2; i++) {
        for(unsigned int j = 0; j < world->getEntities().size(); j++) {
            if(world->getEntities()[j]->getPosition().x >= pos1.x && world->getEntities()[j]->getPosition().x <= pos2.x) {
                if(world->getEntities()[j]->getPosition().y >= pos1.y && world->getEntities()[j]->getPosition().y <= pos2.y) {
                    ret.push_back(j);
                }
            }
        }
    }

    return ret;
}

std::vector<unsigned int> playerEntityTarget(World* world) {
    std::vector<unsigned int> ret;

    unsigned int p = 0;

    for(int i = 0; i < world->getEntities().size(); i++) {
        if(world->getEntities()[i] == world->getPlayer()) {
            p = i;
            break;
        }
    }

    ret.push_back(p);

    return ret;
}

std::vector<unsigned int> speakerEntityTarget(World* world) {
    std::vector<unsigned int> ret;

    unsigned int s = 0;

    for(int i = 0; i < world->getEntities().size(); i++) {
        if(world->getEntities()[i] == world->getPlayer()->getSelectedEntity()) {
            s = i;
            break;
        }
    }

    ret.push_back(s);

    return ret;
}

std::vector<glm::vec2> relativePositionTarget(World* world, Entity* relativeTo, glm::vec2 relativePos) {
    std::vector<glm::vec2> ret;

    ret.push_back(relativeTo->getPosition() + relativePos);

    return ret;
}

std::vector<glm::vec2> areaPositionTarget(World* world, glm::vec2 pos1, glm::vec2 pos2) {
    std::vector<glm::vec2> ret;

    for(int y = pos1.y; y <= pos2.y; y++) {
        for(int x = pos1.x; x <= pos2.x; x++) {
            ret.push_back(glm::vec2(x, y));
        }
    }

    return ret;
}

void setBlock(World* world, unsigned int id, glm::vec2 pos, int layer, MetaData metaData) {
    world->setTile(createBlock(id, pos, layer, metaData)); // Set the block, of course
    /// TODO: compile array of chunks in init()
}

void removeBlock(World* world, int x, int y, unsigned int layer) {
    int chunk = std::floor(x / CHUNK_SIZE); // What chunk index it belongs to

    world->setTile(new BlockAir(glm::vec2(x, y), layer));
}

void showBlock(World* world, int x, int y, unsigned int layer) {
    int chunk = std::floor(x / CHUNK_SIZE); // What chunk index it belongs to

    world->getTile(x, y, layer)->setToDraw(true); // Make it not-transparent
}

void hideBlock(World* world, int x, int y, unsigned int layer) {
    int chunk = std::floor(x / CHUNK_SIZE); // What chunk index it belongs to

    world->getTile(x, y, layer)->setToDraw(false); // Make it not-transparent
}

unsigned int addEntity(World* world, unsigned int id, glm::vec2 position, unsigned int layer) {
    world->addEntity(createEntity(id, position, layer));
}

void removeEntity(World* world, unsigned int index) { // index retrieved from entityTarget
    world->removeEntity(index);
}

void showEntity(World* world, unsigned int index) {
    world->getEntities()[index]->setToDraw(true);
}

void hideEntity(World* world, unsigned int index) {
    world->getEntities()[index]->setToDraw(false);
}

void setTime(World* world, unsigned int time) {
    world->setTime(time);
}

void teleport(World* world, std::vector<unsigned int> entityIndex, glm::vec2 pos) {
    for(unsigned int i = 0; i < entityIndex.size(); i++) {
        world->getEntities()[entityIndex[i]]->setPosition(pos);
    }
}

void giveItem(World* world, std::vector<unsigned int> entityIndex, unsigned int id, unsigned int quantity) {
    for(unsigned int i = 0; i < entityIndex.size(); i++) {
        world->getEntities()[entityIndex[i]]->giveItem(createItem(id, quantity));
        /// TODO:
    }
}

void setPlayerCanInteract(World* world, bool canInteract) {
    world->getPlayer()->setCanInteract(canInteract);
}

void setFlag(QuestManager* qm, unsigned int id, bool val) {
    qm->setFlag(id, val);
}

void setEra(World* world, std::string era) {
    if(era == "neolithic") {
        world->setWorldEra(WorldEra::NEOLITHIC_ERA);
    } else if(era == "common") {
        world->setWorldEra(WorldEra::COMMON_ERA);
    } else if(era == "future") {
        world->setWorldEra(WorldEra::FUTURE_ERA);
    }
}

void setPlayerGodMode(World* world, bool godMode) {
    world->getPlayer()->setGodMode(godMode);
}

void camera_setLocked(GameplayScreen* gs, bool locked) {
    gs->setCameraLocked(locked);
}

void camera_setPosition(GameplayScreen* gs, glm::vec2 pos) {
    camera_setLocked(gs, true);
    gs->getCamera()->setPosition(pos);
}

void camera_move(GameplayScreen* gs, glm::vec2 relPos) {
    camera_setLocked(gs, true);
    gs->getCamera()->setPosition(gs->getCamera()->getPosition() + relPos);
}

void camera_smoothMove(GameplayScreen* gs, glm::vec2 relPos, float speed) {
    camera_setLocked(gs, true);

    glm::vec2 newPos = gs->getCamera()->getPosition() + relPos;

    gs->setSmoothMoveTarget(newPos);
    gs->setSmoothMoveSpeed(speed);
}

void pause(GameplayScreen* gs) {
    gs->pauseForCutscene();
}

void play(GameplayScreen* gs) {
    gs->unpauseCutscene();
}

void setPlayerStat_sanity(World* world, float sanity) {
    world->getPlayer()->setSanity(sanity);
}

void startTrade(QuestManager* qm, unsigned int tableID) {
    qm->setTradeTableId(tableID);
    qm->setTradingStarted(true);
}

void startDialogue(QuestManager* qm, unsigned int questionID) {
    qm->setQuestionId(questionID);
    qm->setDialogueStarted(true);
}

/// Wrapper functions:

#define GET_WORLD_UPVALUE World* world = static_cast<World*>(lua_touserdata(L, lua_upvalueindex(1)))
#define GET_QUESTMANAGER_UPVALUE QuestManager* qm = static_cast<QuestManager*>(lua_touserdata(L, lua_upvalueindex(2)))
#define GET_GAMEPLAYSCREEN_UPVALUE GameplayScreen* gs = static_cast<GameplayScreen*>(lua_touserdata(L, lua_upvalueindex(3)))

/*std::vector<glm::vec2> relativePositionTarget(World* world, Entity* relativeTo, glm::vec2 relativePos) {
    std::vector<glm::vec2> ret;

    ret.push_back(relativeTo->getPosition() + relativePos);

    return ret;
}

std::vector<glm::vec2> areaPositionTarget(World* world, glm::vec2 pos1, glm::vec2 pos2) {*/
int LuaScript::l_setBlock(lua_State* L) {
    GET_WORLD_UPVALUE;
    unsigned int id = lua_tointeger(L, 1);
    int x = lua_tointeger(L, 2);
    int y = lua_tointeger(L, 3);
    int layer = lua_tointeger(L, 4);
    std::string metaData = std::string(lua_tostring(L, 5));
    MetaData md(metaData);
    setBlock(world, id, glm::vec2(x, y), layer, md);
    return 0; // no returned values
}

int LuaScript::l_removeBlock(lua_State* L) {
    GET_WORLD_UPVALUE;
    int x = lua_tointeger(L, 1); // get function argument
    int y = lua_tointeger(L, 2); // get function argument
    int layer = lua_tointeger(L, 3); // get function argument
    removeBlock(world, x, y, layer); // calling C++ function with this argument...
    return 0; // no returned values
}

int LuaScript::l_showBlock(lua_State* L) {
    GET_WORLD_UPVALUE;
    int x = lua_tointeger(L, 1); // get function argument
    int y = lua_tointeger(L, 2); // get function argument
    int layer = lua_tointeger(L, 3); // get function argument
    showBlock(world, x, y, layer); // calling C++ function with this argument...
    return 0; // no returned values
}

int LuaScript::l_hideBlock(lua_State* L) {
    GET_WORLD_UPVALUE;
    int x = lua_tointeger(L, 1); // get function argument
    int y = lua_tointeger(L, 2); // get function argument
    int layer = lua_tointeger(L, 3); // get function argument
    hideBlock(world, x, y, layer); // calling C++ function with this argument...
    return 0; // no returned values
}

int LuaScript::l_addEntity(lua_State* L) {
    GET_WORLD_UPVALUE;
    int id = lua_tointeger(L, 1);
    int x = lua_tointeger(L, 2); // get function argument
    int y = lua_tointeger(L, 3); // get function argument
    int layer = lua_tointeger(L, 4); // get function argument
    addEntity(world, id, glm::vec2(x, y), layer); // calling C++ function with this argument...
    return 0; // no returned values
}

int LuaScript::l_removeEntity(lua_State* L) {
    GET_WORLD_UPVALUE;
    int index = lua_tointeger(L, 1); // get function argument
    removeEntity(world, index); // calling C++ function with this argument...
    return 0; // no returned values
}

int LuaScript::l_showEntity(lua_State* L) {
    GET_WORLD_UPVALUE;
    int index = lua_tointeger(L, 1); // get function argument
    showEntity(world, index); // calling C++ function with this argument...
    return 0; // no returned values
}

int LuaScript::l_hideEntity(lua_State* L) {
    GET_WORLD_UPVALUE;
    int index = lua_tointeger(L, 1); // get function argument
    hideEntity(world, index); // calling C++ function with this argument...
    return 0; // no returned values
}

int LuaScript::l_setTime(lua_State* L) {
    GET_WORLD_UPVALUE;
    unsigned int time = lua_tointeger(L, 1);
    setTime(world, time);
}

int LuaScript::l_teleport(lua_State* L) {
    GET_WORLD_UPVALUE;

    std::vector<unsigned int> entities;

    lua_len(L, 1); // Size of 'table' (array)

    int vecSize = lua_tonumber(L, lua_gettop(L));
    lua_pop(L, 1);

    for (int i = 1; i <= vecSize; i++)
    {
        lua_rawgeti(L, 1, i);  // push to the top of the stack
        int Top = lua_gettop(L);
        entities.push_back(lua_tonumber(L, Top)); // take the number from the top of the stack and put it into entities
        lua_pop(L, 1); // pop top number
    }

    int x = lua_tointeger(L, 2);
    int y = lua_tointeger(L, 3);

    teleport(world, entities, glm::vec2(x, y));
}

int LuaScript::l_giveItem(lua_State* L) {
    GET_WORLD_UPVALUE;

    std::vector<unsigned int> entities;

    lua_len(L, 1); // Size of 'table' (array)

    int vecSize = lua_tonumber(L, lua_gettop(L));
    lua_pop(L, 1);

    for (int i = 1; i <= vecSize; i++)
    {
        lua_rawgeti(L, 1, i);  // push to the top of the stack
        int Top = lua_gettop(L);
        entities.push_back(lua_tonumber(L, Top)); // take the number from the top of the stack and put it into entities
        lua_pop(L, 1); // pop top number
    }

    int id = lua_tointeger(L, 2);
    int quantity = lua_tointeger(L, 3);

    giveItem(world, entities, id, quantity);
}

int LuaScript::l_setPlayerCanInteract(lua_State* L) {
    GET_WORLD_UPVALUE;

    bool canInteract = lua_toboolean(L, 1);

    setPlayerCanInteract(world, canInteract);
}

int LuaScript::l_setFlag(lua_State* L) {
    GET_QUESTMANAGER_UPVALUE;

    unsigned int id = lua_tointeger(L, 1);
    bool val = lua_toboolean(L, 2);

    setFlag(qm, id, val);
}

int LuaScript::l_setEra(lua_State* L) {
    GET_WORLD_UPVALUE;

    std::string era = std::string(lua_tostring(L, 1));

    setEra(world, era);
}

int LuaScript::l_setPlayerGodMode(lua_State* L) {
    GET_WORLD_UPVALUE;

    bool mode = lua_toboolean(L, 1);

    setPlayerGodMode(world, mode);
}

int LuaScript::l_camera_setLocked(lua_State* L) {
    GET_GAMEPLAYSCREEN_UPVALUE;

    bool locked = lua_toboolean(L, 1);

    camera_setLocked(gs, locked);
}

int LuaScript::l_camera_setPosition(lua_State* L) {
    GET_GAMEPLAYSCREEN_UPVALUE;

    unsigned int x = lua_tointeger(L, 1);
    unsigned int y = lua_tointeger(L, 2);

    camera_setPosition(gs, glm::vec2(x, y));
}

int LuaScript::l_camera_move(lua_State* L) {
    GET_GAMEPLAYSCREEN_UPVALUE;

    unsigned int x = lua_tointeger(L, 1);
    unsigned int y = lua_tointeger(L, 2);

    camera_move(gs, glm::vec2(x, y));
}

int LuaScript::l_camera_smoothMove(lua_State* L) {
    GET_GAMEPLAYSCREEN_UPVALUE;

    unsigned int x = lua_tointeger(L, 1);
    unsigned int y = lua_tointeger(L, 2);

    float speed = lua_tonumber(L, 3);

    camera_smoothMove(gs, glm::vec2(x, y), speed);
}

int LuaScript::l_delay(lua_State* L) {
    unsigned int delay = lua_tointeger(L, 1);

    lua_pushinteger(L, delay);

    return lua_yieldk(L, 0, 0, 0);
}

int LuaScript::l_pause(lua_State* L) {
    GET_GAMEPLAYSCREEN_UPVALUE;

    pause(gs);
}

int LuaScript::l_play(lua_State* L) {
    GET_GAMEPLAYSCREEN_UPVALUE;

    play(gs);
}

int LuaScript::l_setPlayerStat_sanity(lua_State* L) {
    GET_WORLD_UPVALUE;

    float sanity = lua_tonumber(L, 1);

    setPlayerStat_sanity(world, sanity);
}

int LuaScript::l_startTrade(lua_State* L) {
    GET_QUESTMANAGER_UPVALUE;

    unsigned int id = lua_tointeger(L, 1);

    startTrade(qm, id);
}

int LuaScript::l_startDialogue(lua_State* L) {
    GET_QUESTMANAGER_UPVALUE;

    unsigned int id = lua_tointeger(L, 1);

    startDialogue(qm, id);
}
