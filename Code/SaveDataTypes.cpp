#include "SaveDataTypes.h"

#include "Entity.h"
#include "EntityItem.h"
#include "EntityProjectile.h"
#include "EntityPlayer.h"

#include "ScriptQueue.h"

#include "InventoryBase.h"
#include "NPCInventory.h"

#include "LuaHeaders.h"
#include "MetaData.h"

void SaveDataTypes::TileData::save(std::ofstream& file) {
	file.write(reinterpret_cast<char*>(&pos.x), sizeof(float));
	file.write(reinterpret_cast<char*>(&pos.y), sizeof(float));
	file.write(reinterpret_cast<char*>(&layer), sizeof(float));
	file.write(reinterpret_cast<char*>(&id), sizeof(unsigned int));
	metaData.save(file);
}

void SaveDataTypes::TileData::read(std::ifstream& file) {
	file.read(reinterpret_cast<char*>(&pos.x), sizeof(float));
	file.read(reinterpret_cast<char*>(&pos.y), sizeof(float));
	file.read(reinterpret_cast<char*>(&layer), sizeof(float));
	file.read(reinterpret_cast<char*>(&id), sizeof(unsigned int));
	metaData.read(file);
}

SaveDataTypes::ItemData::ItemData(Item i) : id(i.getID()), quantity(i.getQuantity()), metaData(i.getMetaData()) {

}

void SaveDataTypes::ItemData::save(std::ofstream& file) {
	file.write(reinterpret_cast<char*>(&id), sizeof(unsigned int));
	file.write(reinterpret_cast<char*>(&quantity), sizeof(unsigned int));
	metaData.save(file);
}

void SaveDataTypes::ItemData::read(std::ifstream& file) {
	file.read(reinterpret_cast<char*>(&id), sizeof(unsigned int));
	file.read(reinterpret_cast<char*>(&quantity), sizeof(unsigned int));
	metaData.read(file);
}

SaveDataTypes::InventoryData::InventoryData(InventoryBase inventory) {
	for(unsigned int i = 0; i < inventory.getItems().size(); i++) {
		itemData.emplace_back(*(inventory.getItems()[i]));
	}
}

void SaveDataTypes::InventoryData::save(std::ofstream& file) {
	for(unsigned int i = 0; i < itemData.size(); i++) {
		itemData[i].save(file);
	}
}

void SaveDataTypes::InventoryData::read(std::ifstream& file) {
	for(unsigned int i = 0; i < itemData.size(); i++) {
		itemData[i].read(file);
	}
}

SaveDataTypes::EntityData::EntityData(Entity e) : position(e.getPosition()),
	layer(e.getLayer()),
	velocity(e.getVelocity()),
	id(e.getID()),
	md(e.getMetaData()) {

}

void SaveDataTypes::EntityData::save(std::ofstream& file) {
	file.write(reinterpret_cast<char*>(&position.x), sizeof(float));
	file.write(reinterpret_cast<char*>(&position.y), sizeof(float));
	file.write(reinterpret_cast<char*>(&layer), sizeof(unsigned int));
	file.write(reinterpret_cast<char*>(&velocity.x), sizeof(float));
	file.write(reinterpret_cast<char*>(&velocity.y), sizeof(float));
	file.write(reinterpret_cast<char*>(&id), sizeof(unsigned int));
	md.save(file);
}

void SaveDataTypes::EntityData::read(std::ifstream& file) {
	file.read(reinterpret_cast<char*>(&position.x), sizeof(float));
	file.read(reinterpret_cast<char*>(&position.y), sizeof(float));
	file.read(reinterpret_cast<char*>(&layer), sizeof(unsigned int));
	file.read(reinterpret_cast<char*>(&velocity.x), sizeof(float));
	file.read(reinterpret_cast<char*>(&velocity.y), sizeof(float));
	file.read(reinterpret_cast<char*>(&id), sizeof(unsigned int));
	md.read(file);
}

SaveDataTypes::EntityItemData::EntityItemData(EntityItem i) : EntityData(i) {

}

SaveDataTypes::EntityNPCData::EntityNPCData(EntityNPC e) : inventory(static_cast<InventoryBase>(*(e.getInventory()))),
	health(e.getHealth()),
	EntityData(e) {

}

void SaveDataTypes::EntityNPCData::save(std::ofstream& file) {
	file.write(reinterpret_cast<char*>(&position.x), sizeof(float));
	file.write(reinterpret_cast<char*>(&position.y), sizeof(float));
	file.write(reinterpret_cast<char*>(&layer), sizeof(unsigned int));
	file.write(reinterpret_cast<char*>(&velocity.x), sizeof(float));
	file.write(reinterpret_cast<char*>(&velocity.y), sizeof(float));
	file.write(reinterpret_cast<char*>(&id), sizeof(unsigned int));
	md.save(file);

	inventory.save(file);
	file.write(reinterpret_cast<char*>(&health), sizeof(float));
}

void SaveDataTypes::EntityNPCData::read(std::ifstream& file) {
	file.read(reinterpret_cast<char*>(&position.x), sizeof(float));
	file.read(reinterpret_cast<char*>(&position.y), sizeof(float));
	file.read(reinterpret_cast<char*>(&layer), sizeof(unsigned int));
	file.read(reinterpret_cast<char*>(&velocity.x), sizeof(float));
	file.read(reinterpret_cast<char*>(&velocity.y), sizeof(float));
	file.read(reinterpret_cast<char*>(&id), sizeof(unsigned int));
	md.read(file);

	inventory.read(file);
	file.read(reinterpret_cast<char*>(&health), sizeof(float));
}

SaveDataTypes::EntityProjectileData::EntityProjectileData(EntityProjectile p) : EntityData(p) {

}

SaveDataTypes::EntityPlayerData::EntityPlayerData(EntityPlayer p) : sanity(p.getSanity()),
	thirst(p.getThirst()),
	hunger(p.getHunger()),
	exhaustion(p.getExhaustion()),
	stamina(p.getStamina()),
	EntityNPCData(p) {

}

void SaveDataTypes::EntityPlayerData::save(std::ofstream& file) {
	file.write(reinterpret_cast<char*>(&position.x), sizeof(float));
	file.write(reinterpret_cast<char*>(&position.y), sizeof(float));
	file.write(reinterpret_cast<char*>(&layer), sizeof(unsigned int));
	file.write(reinterpret_cast<char*>(&velocity.x), sizeof(float));
	file.write(reinterpret_cast<char*>(&velocity.y), sizeof(float));
	file.write(reinterpret_cast<char*>(&id), sizeof(unsigned int));
	md.save(file);

	inventory.save(file);
	file.write(reinterpret_cast<char*>(&health), sizeof(float));

	file.write(reinterpret_cast<char*>(&sanity), sizeof(float));
	file.write(reinterpret_cast<char*>(&thirst), sizeof(float));
	file.write(reinterpret_cast<char*>(&hunger), sizeof(float));
	file.write(reinterpret_cast<char*>(&exhaustion), sizeof(float));
	file.write(reinterpret_cast<char*>(&stamina), sizeof(float));
}

void SaveDataTypes::EntityPlayerData::read(std::ifstream& file) {
	file.read(reinterpret_cast<char*>(&position.x), sizeof(float));
	file.read(reinterpret_cast<char*>(&position.y), sizeof(float));
	file.read(reinterpret_cast<char*>(&layer), sizeof(unsigned int));
	file.read(reinterpret_cast<char*>(&velocity.x), sizeof(float));
	file.read(reinterpret_cast<char*>(&velocity.y), sizeof(float));
	file.read(reinterpret_cast<char*>(&id), sizeof(unsigned int));
	md.read(file);

	inventory.read(file);
	file.read(reinterpret_cast<char*>(&health), sizeof(float));

	file.read(reinterpret_cast<char*>(&sanity), sizeof(float));
	file.read(reinterpret_cast<char*>(&thirst), sizeof(float));
	file.read(reinterpret_cast<char*>(&hunger), sizeof(float));
	file.read(reinterpret_cast<char*>(&exhaustion), sizeof(float));
	file.read(reinterpret_cast<char*>(&stamina), sizeof(float));
}

void SaveDataTypes::ChunkData::save(std::ofstream& file) {
	for(int y = 0; y < WORLD_HEIGHT; y++) {
		for(int x = 0; x < CHUNK_SIZE; x++) {
			for(int k = 0; k < WORLD_DEPTH; k++) {
				tiles[y][x][k].save(file);
			}
		}
	}
	file.write(reinterpret_cast<char*>(&(biomeID)), sizeof(unsigned int));
}

void SaveDataTypes::ChunkData::read(std::ifstream& file) {
	for(int y = 0; y < WORLD_HEIGHT; y++) {
		for(int x = 0; x < CHUNK_SIZE; x++) {
			for(int k = 0; k < WORLD_DEPTH; k++) {
				tiles[y][x][k].read(file);
			}
		}
	}
	file.read(reinterpret_cast<char*>(&(biomeID)), sizeof(unsigned int));
}
