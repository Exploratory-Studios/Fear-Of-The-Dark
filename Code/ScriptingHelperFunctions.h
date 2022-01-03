#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include <LuaScriptQueue.hpp>
#include <LuaHeaders.hpp>

class World;
namespace QuestModule {
	class QuestManager;
}
class GameplayScreen;
class AudioManager;
class Entity;
class Tile;

namespace ScriptingModule {

	static const char* WORLD_KEY		  = "world";
	static const char* GAMEPLAYSCREEN_KEY = "gameplayscreen";
	static const char* QUESTMANAGER_KEY	  = "questmanager";
	static const char* AUDIOMANAGER_KEY	  = "audiomanager";
	static const char* PARTICLEENGINE_KEY = "particleengine";

	/// Wrapper functions:

	void setBlock(World* world, unsigned int id, glm::vec2 pos, int layer);
	void removeBlock(World* world, int x, int y, unsigned int layer);
	void showBlock(World* world, int x, int y, unsigned int layer);
	void hideBlock(World* world, int x, int y, unsigned int layer);

	Tile* getBlock(World* world, glm::vec2 pos, int layer);

	unsigned int addEntity(World* world, unsigned int id, glm::vec2 position, unsigned int layer);
	void		 removeEntity(World* world, std::string UUID);
	void		 showEntity(World* world, std::string UUID);
	void		 hideEntity(World* world, std::string UUID);

	void setTime(World* world, unsigned int time);
	void teleport(World* world, std::string UUID, glm::vec2 pos);
	void giveItem(World* world, std::string UUID, unsigned int id, unsigned int quantity);

	void setPlayerCanInteract(World* world, bool canInteract);
	void setPlayerGodMode(World* world, bool godMode);
	void setPlayerStat_sanity(World* world, float sanity);

	void setFlag(QuestModule::QuestManager* qm, unsigned int id, bool val);
	bool getFlag(QuestModule::QuestManager* qm, unsigned int id);

	void setEra(World* world, unsigned int id);

	void camera_setLocked(GameplayScreen* gs, bool locked);
	void camera_setPosition(GameplayScreen* gs, glm::vec2 pos);
	void camera_move(GameplayScreen* gs, glm::vec2 relPos);
	void camera_smoothMove(GameplayScreen* gs, glm::vec2 relPos, float speed);

	void pause(GameplayScreen* gs);
	void play(GameplayScreen* gs);

	void startTrade(QuestModule::QuestManager* qm, unsigned int tableID);
	void startDialogue(QuestModule::QuestManager* qm, unsigned int questionID);

	std::vector<Entity*> nearEntityTarget(World* world, glm::vec2 nearTo, float minDist);
	std::vector<Entity*> areaEntityTarget(World*	world,
										  glm::vec2 pos1,
										  glm::vec2 pos2); // returns an array of entities' indices

} // namespace ScriptingModule
