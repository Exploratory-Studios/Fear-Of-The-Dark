#pragma once

#include <glm/glm.hpp>

#include "MetaData.h"

class Entity;
class Tile;
class Item;
namespace CombatModule {
	class Attack;
}

class Factory {
  public:
	static Entity*				 createEntity(unsigned int			  id,
											  glm::vec2				  pos,
											  unsigned int			  layer,
											  SaveDataTypes::MetaData data	  = SaveDataTypes::MetaData(),
											  bool					  loadTex = false);
	static Tile*				 createTile(unsigned int			id,
											glm::vec2				pos,
											unsigned int			layer,
											SaveDataTypes::MetaData data	= SaveDataTypes::MetaData(),
											bool					loadTex = false);
	static Item*				 createItem(unsigned int id, short unsigned int quantity = 1, bool loadTex = false);
	static CombatModule::Attack* createAttack(unsigned int id, ::Entity* owner);
};
