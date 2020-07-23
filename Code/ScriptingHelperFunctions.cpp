#include "ScriptingHelperFunctions.h"

#include "World.h"
#include "QuestManager.h"
#include "GameplayScreen.h"
#include "AudioManager.h"
#include "LuaScript.h"
#include "ScriptQueue.h"

#include <Camera2D.h>

#include "ExtraFunctions.h"

#include "XMLData.h"

#include "EntityNPC.h"
#include "EntityPlayer.h"

#include "Tile.h"
#include "Item.h"

#include "Factory.h"

namespace ScriptingModule {

	void pushDepsToRegistry(lua_State* L, World* w, QuestModule::QuestManager* qm, GameplayScreen* gs, AudioManager* am, GLEngine::ParticleEngine2D* p) {
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
		void* ret = lua_touserdata(L, -1);
		lua_pop(L, 1);
		return ret;
	}

	void createArgumentsTable(lua_State* T, std::vector<Argument>& args) { // Creates and labels globals for user use.
		lua_newtable(T); // Create the "main" table.
		for(unsigned int i = 0; i < args.size(); i++) {
			if(args[i].isMetadata) {
				continue; // We have another loop to deal with these guys.
			}
			lua_pushstring(T, (const char*)args[i].key.c_str());
			lua_pushstring(T, (const char*)args[i].val.c_str());
			lua_settable(T, -3);
		}
		lua_pushstring(T, (const char*)"metadata"); // Set the key for the metadata table. (the key in the main table)
		lua_newtable(T); // This is our metadata table (the value in the main table)
		for(unsigned int i = 0; i < args.size(); i++) {
			if(!args[i].isMetadata) {
				continue; // We don't need to re-process these
			}
			lua_pushstring(T, (const char*)args[i].key.c_str()); // Add a value to the metadata table
			lua_pushstring(T, (const char*)args[i].val.c_str());
			lua_settable(T, -3);
		}
		lua_settable(T, -3); // Add the metadata table to the main table
	}






	void setBlock(World* world, unsigned int id, glm::vec2 pos, int layer, SaveDataTypes::MetaData metaData) {
		float correctedX = ((int)pos.x + WORLD_SIZE) % WORLD_SIZE;
		world->setTile(createTile(id, glm::vec2(correctedX, pos.y), layer, false, metaData)); // Set the block, of course
		/// TODO: compile array of chunks in init()
	}

	void removeBlock(World* world, int x, int y, unsigned int layer) {
		//int chunk = std::floor(x / CHUNK_SIZE); // What chunk index it belongs to

		world->setTile(new Tile(glm::vec2(x, y), layer, 0, SaveDataTypes::MetaData(), false)); /// TODO: Constant for air
	}

	void showBlock(World* world, int x, int y, unsigned int layer) {
		//int chunk = std::floor(x / CHUNK_SIZE); // What chunk index it belongs to

		world->getTile(x, y, layer)->setToDraw(true); // Make it not-transparent
	}

	void hideBlock(World* world, int x, int y, unsigned int layer) {
		//int chunk = std::floor(x / CHUNK_SIZE); // What chunk index it belongs to

		world->getTile(x, y, layer)->setToDraw(false); // Make it not-transparent
	}

	Tile* getBlock(World* world, glm::vec2 pos, int layer) {
		return world->getTile(pos.x, pos.y, layer);
	}

	unsigned int addEntity(World* world, unsigned int id, glm::vec2 position, unsigned int layer) {
		Factory::getEntityManager()->queueEntityToAdd(createEntity(position, layer, id, SaveDataTypes::MetaData(), true));

		return 0;
	}

	void removeEntity(World* world, std::string UUID) { // index retrieved from entityTarget
		Factory::getEntityManager()->queueEntityToRemove(UUID);
	}

	void showEntity(World* world, std::string UUID) {
		Factory::getEntityManager()->getEntityByUUID(UUID)->setToDraw(true);
	}

	void hideEntity(World* world, std::string UUID) {
		Factory::getEntityManager()->getEntityByUUID(UUID)->setToDraw(false);
	}

	void setTime(World* world, unsigned int time) {
		world->setTime(time);
	}

	void teleport(World* world, std::string UUID, glm::vec2 pos) {
		Factory::getEntityManager()->getEntityByUUID(UUID)->setPosition(pos);
	}

	void giveItem(World* world, std::string UUID, unsigned int id, unsigned int quantity) {
		EntityNPC* e = dynamic_cast<EntityNPC*>(Factory::getEntityManager()->getEntityByUUID(UUID));
		if(e) e->giveItem(new Item(quantity, id, true));
	}

	void setPlayerCanInteract(World* world, bool canInteract) {
		Factory::getEntityManager()->getPlayer()->setCanInteract(canInteract);
	}

	void setFlag(QuestModule::QuestManager* qm, unsigned int id, bool val) {
		qm->setFlag(id, val);
	}

	bool getFlag(QuestModule::QuestManager* qm, unsigned int id) {
		return qm->getFlag(id);
	}

	void setEra(World* world, unsigned int id) {
		world->setWorldEra(id);
	}

	void setPlayerGodMode(World* world, bool godMode) {
		//world->getPlayer()->setGodMode(godMode);
	}

	void camera_setLocked(GameplayScreen* gs, bool locked) {
		gs->setCameraLocked(locked);
	}

	void camera_setPosition(GameplayScreen* gs, glm::vec2 pos) {
		camera_setLocked(gs, true);
		Factory::getGameCamera()->setPosition(pos);
	}

	void camera_move(GameplayScreen* gs, glm::vec2 relPos) {
		camera_setLocked(gs, true);
		Factory::getGameCamera()->setPosition(Factory::getGameCamera()->getPosition() + relPos);
	}

	void camera_smoothMove(GameplayScreen* gs, glm::vec2 relPos, float speed) {
		camera_setLocked(gs, true);

		glm::vec2 newPos = Factory::getGameCamera()->getPosition() + relPos;

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
		Factory::getEntityManager()->getPlayer()->setSanity(sanity);
	}

	/*void startTrade(QuestModule::QuestManager* qm, unsigned int tableID) {
	    qm->setTradeTableId(tableID);
	    qm->setTradingStarted(true);
	}

	void startDialogue(QuestModule::QuestManager* qm, unsigned int questionID) {
	    qm->setQuestionId(questionID);
	    qm->setDialogueStarted(true);
	}*/

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
		std::vector<Entity*> ret;

		for(unsigned int i = 0; i < Factory::getEntityManager()->getEntities().size(); i++) { /// TODO: Optimize this
			float dist = std::sqrt(std::abs(nearTo.x - Factory::getEntityManager()->getEntities()[i]->getPosition().x) + std::abs(nearTo.y - Factory::getEntityManager()->getEntities()[i]->getPosition().y));
			if(dist < minDist) {
				ret.push_back(Factory::getEntityManager()->getEntities()[i]);
			}
		}

		return ret;
	}

	std::vector<Entity*> areaEntityTarget(World* world, glm::vec2 pos1, glm::vec2 pos2) {
		unsigned int chunk1 = std::floor(pos1.x / CHUNK_SIZE) + WORLD_SIZE;
		unsigned int chunk2 = std::floor(pos2.x / CHUNK_SIZE) + WORLD_SIZE;

		std::vector<Entity*> ret;

		for(unsigned int i = 0; i <= chunk1 - chunk2; i++) {
			for(unsigned int j = 0; j < Factory::getEntityManager()->getEntities().size(); j++) {
				if(Factory::getEntityManager()->getEntities()[j]->getPosition().x >= pos1.x && Factory::getEntityManager()->getEntities()[j]->getPosition().x <= pos2.x) {
					if(Factory::getEntityManager()->getEntities()[j]->getPosition().y >= pos1.y && Factory::getEntityManager()->getEntities()[j]->getPosition().y <= pos2.y) {
						ret.push_back(Factory::getEntityManager()->getEntities()[j]);
					}
				}
			}
		}

		return ret;
	}

	std::vector<unsigned int> playerEntityTarget(World* world) {
		std::vector<unsigned int> ret;

		unsigned int p = 0;

		for(unsigned int i = 0; i < Factory::getEntityManager()->getEntities().size(); i++) {
			Entity* ent = Factory::getEntityManager()->getEntities()[i];
			if(dynamic_cast<EntityPlayer*>(ent) == Factory::getEntityManager()->getPlayer()) {
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

		for(unsigned int i = 0; i < Factory::getEntityManager()->getEntities().size(); i++) {
			if(Factory::getEntityManager()->getEntities()[i] == Factory::getEntityManager()->getPlayer()->getSelectedEntity()) {
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

}
