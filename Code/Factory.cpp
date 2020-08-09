#include "Factory.h"

#include "XMLData.h"

#include "Entity.h"
#include "EntityItem.h"
#include "EntityNPC.h"
#include "EntityProjectile.h"

#include "Tile.h"
#include "TileContainer.h"

#include "Item.h"
#include "ItemArmour.h"
#include "ItemBlock.h"
#include "ItemWeapon.h"

#include "Attack.h"

Entity* Factory::createEntity(unsigned int id, glm::vec2 pos, unsigned int layer, SaveDataTypes::MetaData data, bool loadTex) {
	XMLModule::EntityData e = XMLModule::XMLData::getEntityData(id);

	if(e.type == XMLModule::EntityType::NPC) {
		// An NPC
		return new EntityNPC(pos, layer, id, data, loadTex);
	} else if(e.type == XMLModule::EntityType::ITEM) {
		// An Item
		return new EntityItem(pos, layer, id, data, loadTex);
	} else if(e.type == XMLModule::EntityType::PROJECTILE) {
		// An Projectile
		return new EntityProjectile(pos, layer, id, data, loadTex);
	}

	Logger::getInstance()->log("ERROR: Failed to create entity with ID: " + std::to_string(id), true);
	return nullptr;

}

Tile* Factory::createTile(unsigned int id, glm::vec2 pos, unsigned int layer, SaveDataTypes::MetaData data, bool loadTex) {
	XMLModule::TileData t = XMLModule::XMLData::getTileData(id);

	if(t.type == XMLModule::TileType::TILE) {
		// A regular, inert tile
		return new Tile(pos, layer, id, data, loadTex);
	} else if(t.type == XMLModule::TileType::CONTAINER) {
		// A container tile (like a chest)
		return new TileContainer(pos, layer, id, data, loadTex);
	}

	Logger::getInstance()->log("ERROR: Failed to create tile with ID: " + std::to_string(id), true);
	return nullptr;
}

Item* Factory::createItem(unsigned int id, short unsigned int quantity, bool loadTex) {
	XMLModule::ItemData i = XMLModule::XMLData::getItemData(id);

	if(i.type == XMLModule::ItemType::MISC) {
		// A regular ol' item
		return new Item(quantity, id, loadTex);
	} else if(i.type == XMLModule::ItemType::CONSUMABLE) {
		// A consumable item, destroyed on use
		return nullptr; /// TODO
	} else if(i.type == XMLModule::ItemType::ARMOUR) {
		// A piece of clothing, equippable
		return new ItemArmour(quantity, id, loadTex);
	} else if(i.type == XMLModule::ItemType::WEAPON) {
		// A weapon, starts an attack on use, equippable
		return new ItemWeapon(quantity, id, loadTex);
	} else if(i.type == XMLModule::ItemType::BLOCK) {
		// A weapon, starts an attack on use, equippable
		return new ItemBlock(quantity, id, loadTex);
	}

	Logger::getInstance()->log("ERROR: Failed to create item with ID: " + std::to_string(id), true);
	return nullptr;
}

CombatModule::Attack* Factory::createAttack(unsigned int id, ::Entity* owner) {
	XMLModule::AttackData d = XMLModule::XMLData::getAttackData(id);
	if(d.type == XMLModule::AttackType::MELEE) {
		return new CombatModule::MeleeAttack(id, owner);
	} else if(d.type == XMLModule::AttackType::RANGED) {
		return new CombatModule::RangedAttack(id, owner);
	} else if(d.type == XMLModule::AttackType::MAGIC) {
		return new CombatModule::MagicAttack(id, owner);
	}

	Logger::getInstance()->log("ERROR: Failed to create item with ID: " + std::to_string(id), true);
	return nullptr;
}
