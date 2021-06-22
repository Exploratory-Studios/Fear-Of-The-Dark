#pragma once

/*
 *	This file holds all the differenct classes to save/load the world's state - Tiles w/ metadata, entities, etc.
 * 
 */

#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include <memory>
#include <fstream>

#include "PresetValues.h"
#include "MetaData.h"

class lua_State;
class Entity;
class EntityNPC;
class EntityProjectile;
class EntityItem;
class EntityPlayer;
class InventoryBase;
class Item;

namespace ScriptingModule {
	class Argument;
}

namespace SaveDataTypes {

	class TileData {
		public:
			void save(std::ofstream& file);
			void read(std::ifstream& file);

			glm::vec2 pos;
			float layer;
			unsigned int id;
			MetaData metaData;
	};

	struct ItemData {
		ItemData() {}
		ItemData(Item i);

		void read(std::ifstream& file);
		void save(std::ofstream& file);

		unsigned int id;
		unsigned int quantity;
		MetaData metaData;
	};

	struct InventoryData {
		InventoryData() {}
		InventoryData(InventoryBase inv);

		void read(std::ifstream& file);
		void save(std::ofstream& file);

		std::vector<ItemData> itemData;
	};

	struct EntityData {
			virtual void read(std::ifstream& file);
			virtual void save(std::ofstream& file);

			glm::vec2 position;
			unsigned int layer;
			glm::vec2 velocity;
			unsigned int id;
			MetaData md;

		protected:
			EntityData() {}
			EntityData(Entity e);

	};

	struct EntityItemData : public EntityData {
		EntityItemData() : EntityData() {}
		EntityItemData(EntityItem i);
	};

	struct EntityNPCData : public EntityData {
		EntityNPCData() : EntityData() {}
		EntityNPCData(EntityNPC e);

		virtual void read(std::ifstream& file);
		virtual void save(std::ofstream& file);

		InventoryData inventory;
		float health;
	};

	struct EntityProjectileData : public EntityData {
		EntityProjectileData() : EntityData() {}
		EntityProjectileData(EntityProjectile p);
	};

	struct EntityPlayerData : public EntityNPCData {
		EntityPlayerData() : EntityNPCData() {}
		EntityPlayerData(EntityPlayer p);

		virtual void read(std::ifstream& file);
		virtual void save(std::ofstream& file);

		float sanity;
		float thirst;
		float hunger;
		float exhaustion;
		float stamina;
	};

	class ChunkData {
		public:
			ChunkData();
			void save(std::ofstream& file);
			void read(std::ifstream& file);

			std::vector<std::vector<std::vector<TileData>>> tiles;
			unsigned int biomeID;
	};

	class WorldData {
		public:
			WorldData();
			void save(std::ofstream& file);
			void read(std::ifstream& file);

			std::vector<ChunkData> chunks;
			std::vector<EntityData> entities;
	};

}
