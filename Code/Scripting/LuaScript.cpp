#include "LuaScript.h"

#include "ScriptingHelperFunctions.h"

#include "../World.h"
#include "../QuestManager.h"
#include "../GameplayScreen.h"
#include "../AudioManager.h"
#include <ParticleBatch2D.h>
#include "LuaScript.h"
#include "ScriptQueue.h"

#include <Camera2D.h>

#include "../ExtraFunctions.h"

#include "../XMLData.h"

#include "../EntityNPC.h"
#include "../EntityPlayer.h"

#include "../Tile.h"

/*std::vector<glm::vec2> relativePositionTarget(World* world, Entity* relativeTo, glm::vec2 relativePos) {
    std::vector<glm::vec2> ret;

    ret.push_back(relativeTo->getPosition() + relativePos);

    return ret;
}

std::vector<glm::vec2> areaPositionTarget(World* world, glm::vec2 pos1, glm::vec2 pos2) {*/

int LuaScript::loadScriptFile(std::string& filepath) {
    int errCode = luaL_loadfile(T, filepath.c_str());

    if(errCode) {
        printf("error (load script lua0) code %i: %s\n", errCode, lua_tostring(T, -1));
        destroy();
        return -1;
    }

    // Now that the file is loaded, we need to put it in the registry and return that value
    int ref = luaL_ref(T, LUA_REGISTRYINDEX);
    if(ref == LUA_REFNIL) {
        Logger::getInstance()->log("LUA ERROR: REFNIL on new file load!", true);
    }
    return ref;
}

int LuaScript::loadScriptString(std::string script) {
    std::string sc = script;

    int errCode = luaL_loadstring(T, (char*)sc.c_str());

    if(errCode) {
        printf("error (load string lua0) code %i: %s\n", errCode, lua_tostring(T, -1));
        destroy();
        return 1;
    }

    return 0;
}

void LuaScript::run(unsigned int chunkIndex, std::vector<Argument>& args) {
    lua_rawgeti(T, LUA_REGISTRYINDEX, chunkIndex);
    createArgumentsTable(T, args);
    int err = lua_pcall(T, 1, 0, 0);

    if(err) {
        printf("error (run script lua) code %i: %s\n", err, lua_tostring(T, -1));
        destroy();
        return;
    }

    lua_settop(T, 0);
}

void LuaScript::update(lua_State* state) {
    if(lua_status(T) == LUA_OK) {
        m_finished = true;
        return;
    } else if(lua_status(T) == LUA_YIELD) { // Is the thread currently paused?
        int delayLeft = lua_tointeger(T, -1); // Get the delay from the top of the stack
        delayLeft--; // Take one frame away
        lua_pop(T, 1); // Take the delay value off of the stack completely

        if(delayLeft <= 0) {
            lua_resume(T, state, 0); // Continue the script
        } else {
            lua_pushinteger(T, delayLeft); // Otherwise, re-add the new delay value
        }
        return;
    } else {
        Logger::getInstance()->log("LUA ERROR: " + std::string(lua_tostring(T, -1)), true);
        destroy();
    }
}

int LuaScript::l_playSound(lua_State* L) {
    AudioManager* audio = static_cast<AudioManager*>(getUpvalue(L, AUDIOMANAGER_KEY));

    unsigned int id = (int)lua_tonumber(L, 1);
    float volume = lua_tonumber(L, 1);

    audio->playSoundEffect(id, volume);

    return 0;
}

void ParticleUpdate::updateFunction(GLEngine::Particle2D& particle, float deltaTime, std::string& filepath, lua_State** T) {
    if(*T) { // A pointer to T would not change unless we ``delete T;``, but a pointer to a pointer of T would make sure we can know if the original *T = 0.
        int top = lua_gettop(*T);

        lua_checkstack(*T, 3);
        lua_State* thread2 = lua_newthread(*T);

        int ref = luaL_ref(*T, LUA_REGISTRYINDEX);

        int errCode = luaL_loadfile(thread2, filepath.c_str());

        if(errCode) {
            printf("error (load particle lua0) code %i: %s\n", errCode, lua_tostring(thread2, -1));
            thread2 = 0;
            return;
        }

        // Sets global table of arguments
        std::vector<Argument> args = {
            { "particleX", std::to_string(particle.position.x) },
            { "particleY", std::to_string(particle.position.y) },
            { "particleXVel", std::to_string(particle.velocity.x) },
            { "particleYVel", std::to_string(particle.velocity.y) },
            { "particleLife", std::to_string(particle.life) },
            { "particleWidth", std::to_string(particle.width) },
            { "particleAlpha", std::to_string(particle.color.a) }
        };

        lua_newtable(thread2); // Make table of arguments
        for(unsigned int i = 0; i < args.size(); i++) {
            const char* key = args[i].key.c_str();
            const char* val = args[i].val.c_str();

            lua_pushstring(thread2, key);
            lua_pushstring(thread2, val);

            lua_settable(thread2, -3);
        }

        errCode = lua_pcall(thread2, 1, 1, 0); // Run actual function

        if(errCode != LUA_OK && errCode != LUA_YIELD) {
            printf("error (run particle lua0) code %i: %s\n", errCode, lua_tostring(thread2, -1));
            thread2 = 0;
            return;
        }

        float particleX, particleY, particleVelX, particleVelY, particleLife, particleWidth, particleAlpha;

        lua_getfield(thread2, -1, "particleX");
        particleX = lua_tonumber(thread2, -1);
        lua_pop(thread2, 1);

        lua_getfield(thread2, -1, "particleY");
        particleY = lua_tonumber(thread2, -1);
        lua_pop(thread2, 1);

        lua_getfield(thread2, -1, "particleXVel");
        particleVelX = lua_tonumber(thread2, -1);
        lua_pop(thread2, 1);

        lua_getfield(thread2, -1, "particleYVel");
        particleVelY = lua_tonumber(thread2, -1);
        lua_pop(thread2, 1);

        lua_getfield(thread2, -1, "particleLife");
        particleLife = lua_tonumber(thread2, -1);
        lua_pop(thread2, 1);

        lua_getfield(thread2, -1, "particleWidth");
        particleWidth = lua_tonumber(thread2, -1);
        lua_pop(thread2, 1);

        lua_getfield(thread2, -1, "particleAlpha");
        particleAlpha = lua_tonumber(thread2, -1);
        lua_pop(thread2, 1);

        lua_settop(thread2, 0); // Empty stack for this thread

        /*particleX = lua_tonumber(thread2, 1);
        particleY = lua_tonumber(thread2, 2);
        particleVelX = lua_tonumber(thread2, 3);
        particleVelY = lua_tonumber(thread2, 4);
        particleLife = lua_tonumber(thread2, 5);
        particleWidth = lua_tonumber(thread2, 6);
        particleAlpha = lua_tonumber(thread2, 7);*/

        particle.position = glm::vec2(particleX, particleY);
        particle.velocity = glm::vec2(particleVelX, particleVelY);
        particle.life = particleLife;
        particle.width = particleWidth;
        particle.color.a = particleAlpha;

        luaL_unref(*T, LUA_REGISTRYINDEX, ref);

        lua_settop(*T, top); // Make sure that everything that wasn't there before isn't there now.

        thread2 = 0;
    }
}

lua_State* LuaScript::m_parent;

int LuaScript::l_createParticle(lua_State* L) {
    unsigned int id = (int)lua_tonumber(L, 1);
    float x = lua_tonumber(L, 2);
    float y = lua_tonumber(L, 3);
    float xVel = lua_tonumber(L, 4);
    float yVel = lua_tonumber(L, 5);
    float width = lua_tonumber(L, 6);

    lua_pop(L, 6);

    GLEngine::ParticleEngine2D* particles = static_cast<GLEngine::ParticleEngine2D*>(getUpvalue(L, PARTICLEENGINE_KEY));

    XML_ParticleData d = XMLData::getParticleData(id);

    ParticleUpdate updater(d.scriptPath, &m_parent);
    std::function<void(GLEngine::Particle2D&, float)> boundFunctor = updater.getBoundFunctor();

    GLEngine::ParticleBatch2D* batch = particles->getParticleBatch(MAX_TYPE_PARTICLES, d.decayRate, d.textureFilepath, d.bumpMapFilepath, boundFunctor);

    batch->addParticle(glm::vec2(x, y), glm::vec2(xVel, yVel), GLEngine::ColourRGBA8(255, 255, 255, 255), width);

    return 0;
}

int LuaScript::l_setBlock(lua_State* L) {
    World* world = static_cast<World*>(getUpvalue(L, WORLD_KEY));
    unsigned int id = (int)lua_tonumber(L, 1);
    int x = (int)lua_tonumber(L, 2);
    int y = (int)lua_tonumber(L, 3);
    int layer = (int)lua_tonumber(L, 4);
    std::string metaData = std::string(lua_tostring(L, 5));

    lua_pop(L, 5);

    MetaData md; /// TODO: Read table from Lua
    setBlock(world, id, glm::vec2(x, y), layer, md);
    return 0; // no returned values
}

int LuaScript::l_removeBlock(lua_State* L) {
    World* world = static_cast<World*>(getUpvalue(L, WORLD_KEY));
    int x = (int)lua_tonumber(L, 1); // get function argument
    int y = (int)lua_tonumber(L, 2); // get function argument
    int layer = (int)lua_tonumber(L, 3); // get function argument
    lua_pop(L, 3);

    removeBlock(world, x, y, layer); // calling C++ function with this argument...
    return 0; // no returned values
}

int LuaScript::l_showBlock(lua_State* L) {
    World* world = static_cast<World*>(getUpvalue(L, WORLD_KEY));
    int x = (int)lua_tonumber(L, 1); // get function argument
    int y = (int)lua_tonumber(L, 2); // get function argument
    int layer = (int)lua_tonumber(L, 3); // get function argument

    lua_pop(L, 3);

    showBlock(world, x, y, layer); // calling C++ function with this argument...
    return 0; // no returned values
}

int LuaScript::l_hideBlock(lua_State* L) {
    World* world = static_cast<World*>(getUpvalue(L, WORLD_KEY));
    int x = (int)lua_tonumber(L, 1); // get function argument
    int y = (int)lua_tonumber(L, 2); // get function argument
    int layer = (int)lua_tonumber(L, 3); // get function argument

    lua_pop(L, 3);

    hideBlock(world, x, y, layer); // calling C++ function with this argument...
    return 0; // no returned values
}

int LuaScript::l_setBlockSize(lua_State* L) {
    World* world = static_cast<World*>(getUpvalue(L, WORLD_KEY));
    int x = (int)lua_tonumber(L, 1); // get function argument
    int y = (int)lua_tonumber(L, 2); // get function argument
    int layer = (int)lua_tonumber(L, 3); // get function argument

    float xS = lua_tonumber(L, 4);
    float yS = lua_tonumber(L, 5);

    lua_pop(L, 5);

    world->getTile(x, y, layer)->setSize(glm::vec2(xS, yS));

    return 0;
}

int LuaScript::l_getBlockData(lua_State* L) {
    World* world = static_cast<World*>(getUpvalue(L, WORLD_KEY));
    int x = (int)lua_tonumber(L, 1);
    int y = (int)lua_tonumber(L, 2);
    int l = (int)lua_tonumber(L, 3);

    lua_pop(L, 3);

    Tile* t = getBlock(world, glm::vec2(x, y), l);
    std::vector<Argument> args = t->generateLuaData();
    createArgumentsTable(L, args); // Creates and pushes back table

    return 1; // 1 return value
}

int LuaScript::l_setBlockMetaData(lua_State* L) {
    World* world = static_cast<World*>(getUpvalue(L, WORLD_KEY));
    int x = (int)lua_tonumber(L, 1);
    int y = (int)lua_tonumber(L, 2);
    int l = (int)lua_tonumber(L, 3);

    MetaData md;
    md.readFromLuaTable(L, -1);

    lua_pop(L, 4);

    Tile* t = getBlock(world, glm::vec2(x, y), l);
    t->setMetaData(md);

    return 0;
}

int LuaScript::l_addEntity(lua_State* L) {
    World* world = static_cast<World*>(getUpvalue(L, WORLD_KEY));
    int id = (int)lua_tonumber(L, 1);
    int x = lua_tonumber(L, 2); // get function argument
    int y = lua_tonumber(L, 3); // get function argument
    int layer = (int)lua_tonumber(L, 4); // get function argument

    lua_pop(L, 4);

    addEntity(world, id, glm::vec2(x, y), layer); // calling C++ function with this argument...
    return 0; // no returned values
}

int LuaScript::l_removeEntity(lua_State* L) {
    World* world = static_cast<World*>(getUpvalue(L, WORLD_KEY));
    std::string entityUUID = lua_tostring(L, 1);

    lua_pop(L, 1);

    removeEntity(world, entityUUID); // calling C++ function with this argument...
    return 0; // no returned values
}

int LuaScript::l_showEntity(lua_State* L) {
    World* world = static_cast<World*>(getUpvalue(L, WORLD_KEY));
    std::string entityUUID = lua_tostring(L, 1);

    lua_pop(L, 1);

    showEntity(world, entityUUID); // calling C++ function with this argument...
    return 0; // no returned values
}

int LuaScript::l_hideEntity(lua_State* L) {
    World* world = static_cast<World*>(getUpvalue(L, WORLD_KEY));
    std::string entityUUID = lua_tostring(L, 1);

    lua_pop(L, 1);

    hideEntity(world, entityUUID); // calling C++ function with this argument...
    return 0; // no returned values
}

int LuaScript::l_setEntityMetaData(lua_State* L) {
    World* world = static_cast<World*>(getUpvalue(L, WORLD_KEY));
    std::string entityUUID = lua_tostring(L, 1);
    lua_pop(L, 1);

    // Now we should just have the metadata table.
    MetaData md;
    md.readFromLuaTable(L, -1);

    lua_pop(L, 1); // Remove table.

    world->getEntityByUUID(entityUUID)->setMetaData(md);

    return 0; // no returned values
}

int LuaScript::l_setTime(lua_State* L) {
    World* world = static_cast<World*>(getUpvalue(L, WORLD_KEY));
    unsigned int time = (int)lua_tonumber(L, 1);

    lua_pop(L, 1);

    setTime(world, time);

    return 0;
}

int LuaScript::l_teleport(lua_State* L) {
    World* world = static_cast<World*>(getUpvalue(L, WORLD_KEY));

    std::string entityUUID = lua_tostring(L, 1);

    int x = lua_tonumber(L, 2);
    int y = lua_tonumber(L, 3);

    lua_pop(L, 3);


    teleport(world, entityUUID, glm::vec2(x, y));

    return 0;
}

int LuaScript::l_giveItem(lua_State* L) {
    World* world = static_cast<World*>(getUpvalue(L, WORLD_KEY));

    std::string entityUUID = lua_tostring(L, 1);

    int id = (int)lua_tonumber(L, 2);
    int quantity = (int)lua_tonumber(L, 3);

    lua_pop(L, 3);


    giveItem(world, entityUUID, id, quantity);

    return 0;
}

int LuaScript::l_setPlayerCanInteract(lua_State* L) {
    World* world = static_cast<World*>(getUpvalue(L, WORLD_KEY));

    bool canInteract = lua_toboolean(L, 1);

    lua_pop(L, 1);


    setPlayerCanInteract(world, canInteract);

    return 0;
}

int LuaScript::l_setFlag(lua_State* L) {
    QuestManager* qm = static_cast<QuestManager*>(getUpvalue(L, QUESTMANAGER_KEY));

    unsigned int id = (int)lua_tonumber(L, 1);
    bool val = lua_toboolean(L, 2);

    lua_pop(L, 2);


    setFlag(qm, id, val);

    return 0;
}

int LuaScript::l_setEra(lua_State* L) {
    World* world = static_cast<World*>(getUpvalue(L, WORLD_KEY));

    std::string era = std::string(lua_tostring(L, 1));

    lua_pop(L, 1);


    setEra(world, era);

    return 0;
}

int LuaScript::l_setPlayerGodMode(lua_State* L) {
    World* world = static_cast<World*>(getUpvalue(L, WORLD_KEY));

    bool mode = lua_toboolean(L, 1);

    lua_pop(L, 1);


    setPlayerGodMode(world, mode);

    return 0;
}

int LuaScript::l_camera_setLocked(lua_State* L) {
    GameplayScreen* gs = static_cast<GameplayScreen*>(getUpvalue(L, GAMEPLAYSCREEN_KEY));

    bool locked = lua_toboolean(L, 1);

    lua_pop(L, 1);


    camera_setLocked(gs, locked);

    return 0;
}

int LuaScript::l_camera_setPosition(lua_State* L) {
    GameplayScreen* gs = static_cast<GameplayScreen*>(getUpvalue(L, GAMEPLAYSCREEN_KEY));

    unsigned int x = lua_tonumber(L, 1);
    unsigned int y = lua_tonumber(L, 2);

    lua_pop(L, 2);


    camera_setPosition(gs, glm::vec2(x, y));

    return 0;
}

int LuaScript::l_camera_move(lua_State* L) {
    GameplayScreen* gs = static_cast<GameplayScreen*>(getUpvalue(L, GAMEPLAYSCREEN_KEY));

    unsigned int x = lua_tonumber(L, 1);
    unsigned int y = lua_tonumber(L, 2);

    lua_pop(L, 2);


    camera_move(gs, glm::vec2(x, y));

    return 0;
}

int LuaScript::l_camera_smoothMove(lua_State* L) {
    GameplayScreen* gs = static_cast<GameplayScreen*>(getUpvalue(L, GAMEPLAYSCREEN_KEY));

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
    GameplayScreen* gs = static_cast<GameplayScreen*>(getUpvalue(L, GAMEPLAYSCREEN_KEY));

    pause(gs);

    return 0;
}

int LuaScript::l_play(lua_State* L) {
    GameplayScreen* gs = static_cast<GameplayScreen*>(getUpvalue(L, GAMEPLAYSCREEN_KEY));

    play(gs);

    return 0;
}

int LuaScript::l_setPlayerStat_sanity(lua_State* L) {
    World* world = static_cast<World*>(getUpvalue(L, WORLD_KEY));

    float sanity = lua_tonumber(L, 1);

    lua_pop(L, 1);


    setPlayerStat_sanity(world, sanity);

    return 0;
}

int LuaScript::l_startTrade(lua_State* L) {
    QuestManager* qm = static_cast<QuestManager*>(getUpvalue(L, QUESTMANAGER_KEY));

    unsigned int id = (int)lua_tonumber(L, 1);

    lua_pop(L, 1);


    startTrade(qm, id);

    return 0;
}

int LuaScript::l_startDialogue(lua_State* L) {
    QuestManager* qm = static_cast<QuestManager*>(getUpvalue(L, QUESTMANAGER_KEY));

    unsigned int id = (int)lua_tonumber(L, 1);

    lua_pop(L, 1);


    startDialogue(qm, id);

    return 0;
}

int LuaScript::l_getEntitiesNear(lua_State* L) { // returns UUIDs
    World* world = static_cast<World*>(getUpvalue(L, WORLD_KEY));

    float x = lua_tonumber(L, 1);
    float y = lua_tonumber(L, 2);
    float dist = lua_tonumber(L, 3);

    lua_pop(L, 3);


    glm::vec2 pos(x, y);

    std::vector<Entity*> ents = nearEntityTarget(world, pos, dist);

    lua_newtable(L);

    for(unsigned int i = 0; i < ents.size(); i++) {
        std::string id = ents[i]->getUUID().c_str();
        lua_pushstringtotable(L, i+1, (char*)id.c_str());
    }

    return 1;
}

int LuaScript::l_getEntitiesArea(lua_State* L) {
    World* world = static_cast<World*>(getUpvalue(L, WORLD_KEY));

    float x1 = lua_tonumber(L, 1);
    float y1 = lua_tonumber(L, 2);
    float x2 = lua_tonumber(L, 3);
    float y2 = lua_tonumber(L, 4);

    lua_pop(L, 4);


    glm::vec2 pos1(x1, y1);
    glm::vec2 pos2(x2, y2);

    std::vector<Entity*> ents = areaEntityTarget(world, pos1, pos2);

    lua_newtable(L);

    for(unsigned int i = 0; i < ents.size(); i++) {
        std::string id = ents[i]->getUUID().c_str();
        lua_pushstringtotable(L, i+1, (char*)id.c_str());
    }

    return 1;
}

int LuaScript::l_getPlayer(lua_State* L) {
    World* world = static_cast<World*>(getUpvalue(L, WORLD_KEY));

    EntityPlayer* p = world->getPlayer();
    std::string id = p->getUUID();
    lua_pushstring(L, (char*)id.c_str());

    return 1;
}

int LuaScript::l_getSpeakingEntity(lua_State* L) {
    World* world = static_cast<World*>(getUpvalue(L, WORLD_KEY));

    Entity* s = world->getPlayer()->getSelectedEntity();
    std::string id = s->getUUID();
    lua_pushstring(L, (char*)id.c_str());

    return 1;
}

int LuaScript::l_getEntityPosition(lua_State* L) {
    World* world = static_cast<World*>(getUpvalue(L, WORLD_KEY));

    std::string UUID = lua_tostring(L, 1);

    glm::vec2 pos = world->getEntityByUUID(UUID)->getPosition();

    lua_newtable(L);
    lua_pushnumbertotable(L, 1, pos.x);
    lua_pushnumbertotable(L, 2, pos.y);

    return 1;
}

int LuaScript::l_getCurrentFrame(lua_State* L) {
    World* world = static_cast<World*>(getUpvalue(L, WORLD_KEY));

    lua_pushnumber(L, world->getFrame());

    return 1;
}

int LuaScript::l_log(lua_State* L) {
    std::string log = lua_tostring(L, 1);

    lua_pop(L, 1);

    Logger::getInstance()->log("LUA MESSAGE: " + log);
    return 0;
}
