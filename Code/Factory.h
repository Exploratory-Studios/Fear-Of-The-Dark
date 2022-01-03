#pragma once

#include <glm/glm.hpp>

class Entity;
class Tile;
class Item;
namespace CombatModule {
	class Attack;
}

class Factory {
  public:
	static Entity*				 createEntity(unsigned int id, glm::vec2 pos, unsigned int layer);
	static Tile*				 createTile(unsigned int id, glm::vec2 pos, unsigned int layer);
	static Item*				 createItem(unsigned int id, short unsigned int quantity = 1);
	static CombatModule::Attack* createAttack(unsigned int id, ::Entity* owner);
};
