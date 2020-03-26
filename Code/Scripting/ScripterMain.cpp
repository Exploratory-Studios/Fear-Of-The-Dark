#include "ScripterMain.h"

#include "ScriptQueue.h"
#include "../Entity.h"
#include "../QuestManager.h"
#include "../XMLData.h"
#include "../World.h"

#include "../EntityNPC.h"
#include "../EntityPlayer.h"

#include <Camera2D.h>
#include <ParticleBatch2D.h>

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

Scripter::Scripter() {

}

void Scripter::init(World* world, QuestManager* qm, GameplayScreen* gs, AudioManager* audio, GLEngine::ParticleEngine2D* particleEngine) {
    m_luaState = luaL_newstate();

    luaL_openlibs(m_luaState);

    //lua_newtable(m_luaState);

    addDepsToRegistry(m_luaState, world, qm, gs, audio, particleEngine);

    // Utility
    addFunction(LuaScript::l_playSound, "playSound");
    addFunction(LuaScript::l_createParticle, "createParticle");

    // Setters
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

    addFunction(LuaScript::l_getEntitiesNear, "getEntitiesNear");
    addFunction(LuaScript::l_getEntitiesArea, "getEntitiesNear");

    addFunction(LuaScript::l_getPlayer, "getPlayer");
    addFunction(LuaScript::l_getSpeakingEntity, "getSelectedEntity");

    addFunction(LuaScript::l_getEntityPosition, "getEntityPosition");

    addFunction(LuaScript::l_getCurrentFrame, "getFrame");

    addFunction(LuaScript::l_log, "log");

    /*    // Setters
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

        static int l_getEntityPosition(lua_State* L); // takes entity UUID, returns a 2-index array*/
}

void Scripter::update() {
    // Get active script from ScriptQueue
    // Execute script
    // Take script off active list
    // Repeat until no active scripts exist

    std::vector<Script*> scripts;

    scripts.clear();

    for(unsigned int i = 0; i < ScriptQueue::m_activeScripts.size(); i++) {
        Script* s = new Script;
        *s = ScriptQueue::m_activeScripts[i];
        scripts.push_back(s); // Make a copy!
    }

    if(scripts.size() > 0) {
        for(unsigned int i = 0; i < scripts.size(); i++) {
            executeScript(scripts[i]);
            ScriptQueue::deactivateScript(i);
        }
    }

    for(unsigned int i = 0; i < m_scripts.size(); i++) {
        m_scripts[i]->update(m_luaState);
        if(m_scripts[i]->isFinished()) {
            delete m_scripts[i];
            for(int j = i; j < m_scripts.size(); j++) {
                m_scripts[j] = m_scripts[j+1];
            }
            m_scripts.pop_back();
        }
    }
}

std::string Scripter::executeScript(Script* script) {
    std::string returnMessage;

    lua_checkstack(m_luaState, 10);

    LuaScript* s = new LuaScript(m_luaState);
    if(script->preCommand.length() > 0) s->runScriptString(script->preCommand);
    if(script->isFile()) {
        s->runScriptFile(script->getFileName());
    } else {
        s->runScriptString(script->getText());
    }

    /*lua_pop(m_luaState, 1);
    lua_pushthreadtotable(m_luaState, m_numOfScripts, m_luaState);
    m_numOfScripts++;*/

    m_scripts.push_back(s);

    return "NO_MSG";
}

std::string Scripter::executeCommand(std::string& command) {
    std::string returnMessage;

    lua_checkstack(m_luaState, 10);

    LuaScript* s = new LuaScript(m_luaState);
    s->runScriptString(command);

    /*lua_pop(m_luaState, 1);
    lua_pushthreadtotable(m_luaState, m_numOfScripts, m_luaState);
    m_numOfScripts++;*/

    m_scripts.push_back(s);

    return "NO_MSG";
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

/// Wrapper functions:

#define GET_WORLD_UPVALUE lua_pushlightuserdata(L, (void *)&WORLD_KEY); lua_gettable(L, LUA_REGISTRYINDEX); World* world = static_cast<World*>(lua_touserdata(L, -1));
#define GET_QUESTMANAGER_UPVALUE lua_pushlightuserdata(L, (void *)&QUESTMANAGER_KEY); lua_gettable(L, LUA_REGISTRYINDEX); QuestManager* qm = static_cast<QuestManager*>(lua_touserdata(L, -1));
#define GET_GAMEPLAYSCREEN_UPVALUE lua_pushlightuserdata(L, (void *)&GAMEPLAYSCREEN_KEY); lua_gettable(L, LUA_REGISTRYINDEX); GameplayScreen* gs = static_cast<GameplayScreen*>(lua_touserdata(L, -1));
#define GET_AUDIO_UPVALUE lua_pushlightuserdata(L, (void *)&AUDIOMANAGER_KEY); lua_gettable(L, LUA_REGISTRYINDEX); AudioManager* audio = static_cast<AudioManager*>(lua_touserdata(L, -1));
#define GET_PARTICLE_UPVALUE lua_pushlightuserdata(L, (void *)&PARTICLEENGINE_KEY); lua_gettable(L, LUA_REGISTRYINDEX); GLEngine::ParticleEngine2D* particles = static_cast<GLEngine::ParticleEngine2D*>(lua_touserdata(L, -1));

/*std::vector<glm::vec2> relativePositionTarget(World* world, Entity* relativeTo, glm::vec2 relativePos) {
    std::vector<glm::vec2> ret;

    ret.push_back(relativeTo->getPosition() + relativePos);

    return ret;
}

std::vector<glm::vec2> areaPositionTarget(World* world, glm::vec2 pos1, glm::vec2 pos2) {*/

int LuaScript::l_playSound(lua_State* L) {
    GET_AUDIO_UPVALUE;

    unsigned int id = (int)lua_tonumber(L, 1);
    float volume = lua_tonumber(L, 1);

    audio->playSoundEffect(id, volume);

    return 0;
}
#include <iostream>
void ParticleUpdate::operator() (GLEngine::Particle2D& particle, float deltaTime) const {
    lua_State* thread2 = lua_newthread(T);

    // This simply sets the variables for the particle's position, velocity, life, and width
    std::string varString = "particleX,particleY,particleXVel,particleYVel,particleLife,particleWidth,particleAlpha=";
    varString += std::to_string(particle.position.x) + ",";
    varString += std::to_string(particle.position.y) + ",";
    varString += std::to_string(particle.velocity.x) + ",";
    varString += std::to_string(particle.velocity.y) + ",";
    varString += std::to_string(particle.life) + ",";
    varString += std::to_string(particle.width) + ",";
    varString += std::to_string(particle.color.a);

    int errCode = luaL_loadstring(thread2, (char*)varString.c_str());

    if(errCode) {
        printf("error code %i: %s\n", errCode, lua_tostring(thread2, -1));
        thread2 = 0;
        lua_pop(T, 1); // Pop the thread from the stack
        return;
    }

    errCode = lua_resume(thread2, NULL, 0);

    if(errCode != LUA_OK && errCode != LUA_YIELD) {
        printf("error code %i: %s\n", errCode, lua_tostring(thread2, -1));
        thread2 = 0;
        lua_pop(T, 1); // Pop the thread from the stack
        return;
    }

    errCode = luaL_loadfile(thread2, scriptPath.c_str());

    if(errCode) {
        printf("error code %i: %s\n", errCode, lua_tostring(thread2, -1));
        thread2 = 0;
        lua_pop(T, 1); // Pop the thread from the stack
        return;
    }

    errCode = lua_resume(thread2, NULL, 0);

    if(errCode != LUA_OK && errCode != LUA_YIELD) {
        printf("error code %i: %s\n", errCode, lua_tostring(thread2, -1));
        thread2 = 0;
        lua_pop(T, 1); // Pop the thread from the stack
        return;
    }

    //stackDump(thread2);

    float particleX, particleY, particleVelX, particleVelY, particleLife, particleWidth, particleAlpha;

    particleX = lua_tonumber(thread2, 1);
    particleY = lua_tonumber(thread2, 2);
    particleVelX = lua_tonumber(thread2, 3);
    particleVelY = lua_tonumber(thread2, 4);
    particleLife = lua_tonumber(thread2, 5);
    particleWidth = lua_tonumber(thread2, 6);
    particleAlpha = lua_tonumber(thread2, 7);

    particle.position = glm::vec2(particleX, particleY);
    particle.velocity = glm::vec2(particleVelX, particleVelY);
    particle.life = particleLife;
    particle.width = particleWidth;
    particle.color.a = particleAlpha;

    thread2 = 0;
    lua_pop(T, 1); // Pop the thread from the stack
}

GLEngine::Particle2D* LuaScript::m_particle;

int LuaScript::l_createParticle(lua_State* L) {
    GET_PARTICLE_UPVALUE;

    unsigned int id = (int)lua_tonumber(L, 1);
    float x = lua_tonumber(L, 2);
    float y = lua_tonumber(L, 3);
    float xVel = lua_tonumber(L, 4);
    float yVel = lua_tonumber(L, 5);
    float width = lua_tonumber(L, 6);

    lua_pop(L, 6);

    XML_ParticleData d = XMLData::getParticleData(id);

    ParticleUpdate updateFunc(d.scriptPath, L);

    GLEngine::ParticleBatch2D* batch = particles->getParticleBatch(MAX_TYPE_PARTICLES, d.decayRate, d.textureFilepath, d.bumpMapFilepath, updateFunc);

    m_particle = batch->addParticle(glm::vec2(x, y), glm::vec2(xVel, yVel), GLEngine::ColourRGBA8(255, 255, 255, 255), width);
}

int LuaScript::l_setBlock(lua_State* L) {
    GET_WORLD_UPVALUE;
    unsigned int id = (int)lua_tonumber(L, 1);
    int x = (int)lua_tonumber(L, 2);
    int y = (int)lua_tonumber(L, 3);
    int layer = (int)lua_tonumber(L, 4);
    std::string metaData = std::string(lua_tostring(L, 5));

    lua_pop(L, 5);

    MetaData md(metaData);
    setBlock(world, id, glm::vec2(x, y), layer, md);
    return 0; // no returned values
}

int LuaScript::l_removeBlock(lua_State* L) {
    GET_WORLD_UPVALUE;
    int x = (int)lua_tonumber(L, 1); // get function argument
    int y = (int)lua_tonumber(L, 2); // get function argument
    int layer = (int)lua_tonumber(L, 3); // get function argument
    lua_pop(L, 3);

    removeBlock(world, x, y, layer); // calling C++ function with this argument...
    return 0; // no returned values
}

int LuaScript::l_showBlock(lua_State* L) {
    GET_WORLD_UPVALUE;
    int x = (int)lua_tonumber(L, 1); // get function argument
    int y = (int)lua_tonumber(L, 2); // get function argument
    int layer = (int)lua_tonumber(L, 3); // get function argument

    lua_pop(L, 3);

    showBlock(world, x, y, layer); // calling C++ function with this argument...
    return 0; // no returned values
}

int LuaScript::l_hideBlock(lua_State* L) {
    GET_WORLD_UPVALUE;
    int x = (int)lua_tonumber(L, 1); // get function argument
    int y = (int)lua_tonumber(L, 2); // get function argument
    int layer = (int)lua_tonumber(L, 3); // get function argument

    lua_pop(L, 3);

    hideBlock(world, x, y, layer); // calling C++ function with this argument...
    return 0; // no returned values
}

int LuaScript::l_addEntity(lua_State* L) {
    GET_WORLD_UPVALUE;
    int id = (int)lua_tonumber(L, 1);
    int x = lua_tonumber(L, 2); // get function argument
    int y = lua_tonumber(L, 3); // get function argument
    int layer = (int)lua_tonumber(L, 4); // get function argument

    lua_pop(L, 4);

    addEntity(world, id, glm::vec2(x, y), layer); // calling C++ function with this argument...
    return 0; // no returned values
}

int LuaScript::l_removeEntity(lua_State* L) {
    GET_WORLD_UPVALUE;
    std::string entityUUID = lua_tostring(L, 1);

    lua_pop(L, 1);

    removeEntity(world, entityUUID); // calling C++ function with this argument...
    return 0; // no returned values
}

int LuaScript::l_showEntity(lua_State* L) {
    GET_WORLD_UPVALUE;
    std::string entityUUID = lua_tostring(L, 1);

    lua_pop(L, 1);

    showEntity(world, entityUUID); // calling C++ function with this argument...
    return 0; // no returned values
}

int LuaScript::l_hideEntity(lua_State* L) {
    GET_WORLD_UPVALUE;
    std::string entityUUID = lua_tostring(L, 1);

    lua_pop(L, 1);

    hideEntity(world, entityUUID); // calling C++ function with this argument...
    return 0; // no returned values
}

int LuaScript::l_setTime(lua_State* L) {
    GET_WORLD_UPVALUE;
    unsigned int time = (int)lua_tonumber(L, 1);

    lua_pop(L, 1);

    setTime(world, time);

    return 0;
}

int LuaScript::l_teleport(lua_State* L) {
    GET_WORLD_UPVALUE;

    std::string entityUUID = lua_tostring(L, 1);

    int x = lua_tonumber(L, 2);
    int y = lua_tonumber(L, 3);

    lua_pop(L, 3);


    teleport(world, entityUUID, glm::vec2(x, y));

    return 0;
}

int LuaScript::l_giveItem(lua_State* L) {
    GET_WORLD_UPVALUE;

    std::string entityUUID = lua_tostring(L, 1);

    int id = (int)lua_tonumber(L, 2);
    int quantity = (int)lua_tonumber(L, 3);

    lua_pop(L, 3);


    giveItem(world, entityUUID, id, quantity);

    return 0;
}

int LuaScript::l_setPlayerCanInteract(lua_State* L) {
    GET_WORLD_UPVALUE;

    bool canInteract = lua_toboolean(L, 1);

    lua_pop(L, 1);


    setPlayerCanInteract(world, canInteract);

    return 0;
}

int LuaScript::l_setFlag(lua_State* L) {
    GET_QUESTMANAGER_UPVALUE;

    unsigned int id = (int)lua_tonumber(L, 1);
    bool val = lua_toboolean(L, 2);

    lua_pop(L, 2);


    setFlag(qm, id, val);

    return 0;
}

int LuaScript::l_setEra(lua_State* L) {
    GET_WORLD_UPVALUE;

    std::string era = std::string(lua_tostring(L, 1));

    lua_pop(L, 1);


    setEra(world, era);

    return 0;
}

int LuaScript::l_setPlayerGodMode(lua_State* L) {
    GET_WORLD_UPVALUE;

    bool mode = lua_toboolean(L, 1);

    lua_pop(L, 1);


    setPlayerGodMode(world, mode);

    return 0;
}

int LuaScript::l_camera_setLocked(lua_State* L) {
    GET_GAMEPLAYSCREEN_UPVALUE;

    bool locked = lua_toboolean(L, 1);

    lua_pop(L, 1);


    camera_setLocked(gs, locked);

    return 0;
}

int LuaScript::l_camera_setPosition(lua_State* L) {
    GET_GAMEPLAYSCREEN_UPVALUE;

    unsigned int x = lua_tonumber(L, 1);
    unsigned int y = lua_tonumber(L, 2);

    lua_pop(L, 2);


    camera_setPosition(gs, glm::vec2(x, y));

    return 0;
}

int LuaScript::l_camera_move(lua_State* L) {
    GET_GAMEPLAYSCREEN_UPVALUE;

    unsigned int x = lua_tonumber(L, 1);
    unsigned int y = lua_tonumber(L, 2);

    lua_pop(L, 2);


    camera_move(gs, glm::vec2(x, y));

    return 0;
}

int LuaScript::l_camera_smoothMove(lua_State* L) {
    GET_GAMEPLAYSCREEN_UPVALUE;

    unsigned int x = lua_tonumber(L, 1);
    unsigned int y = lua_tonumber(L, 2);

    float speed = lua_tonumber(L, 3);

    lua_pop(L, 3);


    camera_smoothMove(gs, glm::vec2(x, y), speed);

    return 0;
}

int LuaScript::l_delay(lua_State* L) {
    unsigned int delay = (int)lua_tonumber(L, 1);

    lua_pop(L, 1);


    lua_pushinteger(L, delay);

    return lua_yieldk(L, 0, 0, 0);
}

int LuaScript::l_pause(lua_State* L) {
    GET_GAMEPLAYSCREEN_UPVALUE;

    pause(gs);

    return 0;
}

int LuaScript::l_play(lua_State* L) {
    GET_GAMEPLAYSCREEN_UPVALUE;

    play(gs);

    return 0;
}

int LuaScript::l_setPlayerStat_sanity(lua_State* L) {
    GET_WORLD_UPVALUE;

    float sanity = lua_tonumber(L, 1);

    lua_pop(L, 1);


    setPlayerStat_sanity(world, sanity);

    return 0;
}

int LuaScript::l_startTrade(lua_State* L) {
    GET_QUESTMANAGER_UPVALUE;

    unsigned int id = (int)lua_tonumber(L, 1);

    lua_pop(L, 1);


    startTrade(qm, id);

    return 0;
}

int LuaScript::l_startDialogue(lua_State* L) {
    GET_QUESTMANAGER_UPVALUE;

    unsigned int id = (int)lua_tonumber(L, 1);

    lua_pop(L, 1);


    startDialogue(qm, id);

    return 0;
}

int LuaScript::l_getEntitiesNear(lua_State* L) { // returns UUIDs
    GET_WORLD_UPVALUE;

    float x = lua_tonumber(L, 1);
    float y = lua_tonumber(L, 2);
    float dist = lua_tonumber(L, 3);

    lua_pop(L, 3);


    glm::vec2 pos(x, y);

    std::vector<Entity*> ents = nearEntityTarget(world, pos, dist);

    lua_newtable(L);

    for(int i = 0; i < ents.size(); i++) {
        std::string id = ents[i]->getUUID().c_str();
        lua_pushstringtotable(L, i+1, (char*)id.c_str());
    }

    return 1;
}

int LuaScript::l_getEntitiesArea(lua_State* L) {
    GET_WORLD_UPVALUE;

    float x1 = lua_tonumber(L, 1);
    float y1 = lua_tonumber(L, 2);
    float x2 = lua_tonumber(L, 3);
    float y2 = lua_tonumber(L, 4);

    lua_pop(L, 4);


    glm::vec2 pos1(x2, y1);
    glm::vec2 pos2(x2, y2);

    std::vector<Entity*> ents = areaEntityTarget(world, pos1, pos2);

    lua_newtable(L);

    for(int i = 0; i < ents.size(); i++) {
        std::string id = ents[i]->getUUID().c_str();
        lua_pushstringtotable(L, i+1, (char*)id.c_str());
    }

    return 1;
}

int LuaScript::l_getPlayer(lua_State* L) {
    GET_WORLD_UPVALUE;

    EntityPlayer* p = world->getPlayer();
    std::string id = p->getUUID();
    lua_pushstring(L, (char*)id.c_str());

    return 1;
}

int LuaScript::l_getSpeakingEntity(lua_State* L) {
    GET_WORLD_UPVALUE;

    Entity* s = world->getPlayer()->getSelectedEntity();
    std::string id = s->getUUID();
    lua_pushstring(L, (char*)id.c_str());

    return 1;
}

int LuaScript::l_getEntityPosition(lua_State* L) {
    GET_WORLD_UPVALUE;

    std::string UUID = lua_tostring(L, 1);

    glm::vec2 pos = world->getEntityByUUID(UUID)->getPosition();

    lua_newtable(L);
    lua_pushnumbertotable(L, 1, pos.x);
    lua_pushnumbertotable(L, 2, pos.y);

    return 1;
}

int LuaScript::l_getCurrentFrame(lua_State* L) {
    GET_WORLD_UPVALUE;

    lua_pushnumber(L, world->getFrame());

    return 1;
}

int LuaScript::l_log(lua_State* L) {
    std::string log = lua_tostring(L, 1);

    lua_pop(L, 1);

    Logger::getInstance()->log("LUA MESSAGE: " + log);
    return 0;
}
