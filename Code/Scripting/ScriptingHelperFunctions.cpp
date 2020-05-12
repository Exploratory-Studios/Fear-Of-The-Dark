#include "ScriptingHelperFunctions.h"

#include "../World.h"
#include "../QuestManager.h"
#include "../GameplayScreen.h"
#include "../AudioManager.h"
#include "LuaScript.h"
#include "ScriptQueue.h"

#include <Camera2D.h>

#include "../ExtraFunctions.h"

#include "../XMLData.h"

#include "../EntityNPC.h"
#include "../EntityPlayer.h"

void pushDepsToRegistry(lua_State* L, World* w, QuestManager* qm, GameplayScreen* gs, AudioManager* am, GLEngine::ParticleEngine2D* p) {
    setUpvalue(L, WORLD_KEY,          static_cast<void*>(w));
    setUpvalue(L, QUESTMANAGER_KEY,   static_cast<void*>(qm));
    setUpvalue(L, GAMEPLAYSCREEN_KEY, static_cast<void*>(gs));
    setUpvalue(L, AUDIOMANAGER_KEY,   static_cast<void*>(am));
    setUpvalue(L, PARTICLEENGINE_KEY, static_cast<void*>(p));
}

void pushFunction(lua_State* L, lua_CFunction func, std::string name) {
    lua_pushcfunction(L, func);
    lua_setglobal(L, name.c_str());
}

void setUpvalue(lua_State* L, const char* key, void* ptr) {
    lua_pushstring(L, key);
    lua_pushlightuserdata(L, ptr);
    lua_settable(L, LUA_REGISTRYINDEX);
}

void* getUpvalue(lua_State* L, const char* key) {
    lua_pushstring(L, key);
    lua_gettable(L, LUA_REGISTRYINDEX);
    return lua_touserdata(L, -1);
}

void setBlock(World* world, unsigned int id, glm::vec2 pos, int layer, MetaData metaData) {
    float correctedX = ((int)pos.x + WORLD_SIZE) % WORLD_SIZE;
    world->setTile(new Tile(glm::vec2(correctedX, pos.y), layer, id, MetaData(metaData), false)); // Set the block, of course
    /// TODO: compile array of chunks in init()
}

void removeBlock(World* world, int x, int y, unsigned int layer) {
    int chunk = std::floor(x / CHUNK_SIZE); // What chunk index it belongs to

    world->setTile(new Tile(glm::vec2(x, y), layer, 0, MetaData(""), false)); /// TODO: Constant for air
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
    world->addEntity(createEntity(position, layer, id, MetaData(), true));
}

void removeEntity(World* world, std::string UUID) { // index retrieved from entityTarget
    world->removeEntity(UUID);
}

void showEntity(World* world, std::string UUID) {
    world->getEntityByUUID(UUID)->setToDraw(true);
}

void hideEntity(World* world, std::string UUID) {
    world->getEntityByUUID(UUID)->setToDraw(false);
}

void setTime(World* world, unsigned int time) {
    world->setTime(time);
}

void teleport(World* world, std::string UUID, glm::vec2 pos) {
    world->getEntityByUUID(UUID)->setPosition(pos);
}

void giveItem(World* world, std::string UUID, unsigned int id, unsigned int quantity) {
    EntityNPC* e = dynamic_cast<EntityNPC*>(world->getEntityByUUID(UUID));
    if(e) e->giveItem(new Item(quantity, id, true));
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
    //world->getPlayer()->setGodMode(godMode);
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

void lua_pushintegertotable(lua_State* L, int index, int value) {
    // make sure to call lua_newtable(L) before using this function!

    lua_pushinteger(L, index); // Set index ("key")
    lua_pushinteger(L, value); // Set value
    lua_settable(L, -3); // Add value and index to table at index -3
}

void lua_pushnumbertotable(lua_State* L, int index, float value) {
    // make sure to call lua_newtable(L) before using this function!

    lua_pushinteger(L, index); // Set index ("key")
    lua_pushnumber(L, value); // Set value
    lua_settable(L, -3); // Add value and index to table at index -3
}

void lua_pushstringtotable(lua_State* L, int index, char* value) {
    // make sure to call lua_newtable(L) before using this function!

    lua_pushinteger(L, index); // Set index ("key")
    lua_pushstring(L, value); // Set value
    lua_settable(L, -3); // Add value and index to table at index -3
}

void lua_pushthreadtotable(lua_State* L, int index, lua_State* thread) {
    // make sure to call lua_newtable(L) before using this function!

    lua_pushinteger(L, index); // Set index ("key")
    lua_pushthread(thread); // Set value
    lua_settable(L, -3); // Add value and index to table at index -3
}

std::vector<Entity*> nearEntityTarget(World* world, glm::vec2 nearTo, float minDist) {

    bool found = false;
    std::vector<Entity*> ret;

    for(unsigned int i = 0; i < world->getEntities().size(); i++) { /// TODO: Optimize this
        float dist = std::sqrt(std::abs(nearTo.x - world->getEntities()[i]->getPosition().x) + std::abs(nearTo.y - world->getEntities()[i]->getPosition().y));
        if(dist < minDist) {
            ret.push_back(world->getEntities()[i]);
        }
    }

    return ret;
}

std::vector<Entity*> areaEntityTarget(World* world, glm::vec2 pos1, glm::vec2 pos2) {
    unsigned int chunk1 = std::floor(pos1.x / CHUNK_SIZE) + WORLD_SIZE;
    unsigned int chunk2 = std::floor(pos2.x / CHUNK_SIZE) + WORLD_SIZE;

    std::vector<Entity*> ret;

    for(unsigned int i = 0; i <= chunk1-chunk2; i++) {
        for(unsigned int j = 0; j < world->getEntities().size(); j++) {
            if(world->getEntities()[j]->getPosition().x >= pos1.x && world->getEntities()[j]->getPosition().x <= pos2.x) {
                if(world->getEntities()[j]->getPosition().y >= pos1.y && world->getEntities()[j]->getPosition().y <= pos2.y) {
                    ret.push_back(world->getEntities()[j]);
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
        Entity* ent = world->getEntities()[i];
        if(dynamic_cast<EntityPlayer*>(ent) == world->getPlayer()) {
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
