#include "SaveDataTypes.h"

#include <LuaScriptQueue.hpp>
#include <LuaHeaders.hpp>

#include "Singletons.h"

#include "Entity.h"
#include "EntityItem.h"
#include "EntityProjectile.h"
#include "EntityPlayer.h"

#include "InventoryBase.h"
#include "NPCInventory.h"

#include "FluidDomain.h"

SaveDataTypes::FluidData::FluidData(FluidModule::FluidDomain* domain) {
	id = domain->getID();

	xSize = domain->getDomainXSize();
	ySize = domain->getDomainYSize();

	std::vector<std::vector<FluidModule::DensityField*>> fields = domain->getFields();

	for(unsigned int x = 0; x < fields.size(); x++) {
		for(unsigned int y = 0; y < fields[x].size(); y++) {
			if(fields[x][y]) {
				FluidDensityData d(*fields[x][y]);
				d.x = x;
				d.y = y;
				densityFields.push_back(d);
			}
		}
	}
}

void SaveDataTypes::FluidData::save(std::ofstream& file) {
	file.write(reinterpret_cast<char*>(&id), sizeof(unsigned int));

	file.write(reinterpret_cast<char*>(&xSize), sizeof(unsigned int));
	file.write(reinterpret_cast<char*>(&ySize), sizeof(unsigned int));

	unsigned int numFields = densityFields.size();
	file.write(reinterpret_cast<char*>(&numFields), sizeof(unsigned int));

	for(unsigned int i = 0; i < densityFields.size(); i++) {
		densityFields[i].save(file);
	}
}

void SaveDataTypes::FluidData::read(std::ifstream& file) {
	file.read(reinterpret_cast<char*>(&id), sizeof(unsigned int));

	file.read(reinterpret_cast<char*>(&xSize), sizeof(unsigned int));
	file.read(reinterpret_cast<char*>(&ySize), sizeof(unsigned int));

	unsigned int numDensityFields = 0;
	file.read(reinterpret_cast<char*>(&numDensityFields), sizeof(unsigned int));

	densityFields.resize(numDensityFields);

	for(unsigned int i = 0; i < densityFields.size(); i++) {
		densityFields[i].read(file);
	}
}

SaveDataTypes::FluidDensityData::FluidDensityData() {
	densities	   = new float[FLUID_PARTITION_SIZE * FLUID_PARTITION_SIZE];
	deltaDensities = new float[FLUID_PARTITION_SIZE * FLUID_PARTITION_SIZE];
}

SaveDataTypes::FluidDensityData::FluidDensityData(FluidModule::DensityField cell) {
	densities	   = new float[FLUID_PARTITION_SIZE * FLUID_PARTITION_SIZE];
	deltaDensities = new float[FLUID_PARTITION_SIZE * FLUID_PARTITION_SIZE];

	for(unsigned int i = 0; i < FLUID_PARTITION_SIZE * FLUID_PARTITION_SIZE; i++) {
		densities[i]	  = cell.densities[i].density;
		deltaDensities[i] = cell.deltaDensities[i].density;
	}
	inEquilibrium = cell.inEquilibrium;
}

void SaveDataTypes::FluidDensityData::save(std::ofstream& file) {
	file.write(reinterpret_cast<char*>(&x), sizeof(unsigned int));
	file.write(reinterpret_cast<char*>(&y), sizeof(unsigned int));
	for(unsigned int i = 0; i < FLUID_PARTITION_SIZE * FLUID_PARTITION_SIZE; i++) {
		file.write(reinterpret_cast<char*>(&densities[i]), sizeof(float));
		file.write(reinterpret_cast<char*>(&deltaDensities[i]), sizeof(float));
	}
	file.write(reinterpret_cast<char*>(&inEquilibrium), sizeof(bool));
}

void SaveDataTypes::FluidDensityData::read(std::ifstream& file) {
	file.read(reinterpret_cast<char*>(&x), sizeof(unsigned int));
	file.read(reinterpret_cast<char*>(&y), sizeof(unsigned int));
	for(unsigned int i = 0; i < FLUID_PARTITION_SIZE * FLUID_PARTITION_SIZE; i++) {
		file.read(reinterpret_cast<char*>(&densities[i]), sizeof(float));
		file.read(reinterpret_cast<char*>(&deltaDensities[i]), sizeof(float));
	}
	file.read(reinterpret_cast<char*>(&inEquilibrium), sizeof(bool));
}

void SaveDataTypes::TileData::save(std::ofstream& file) {
	file.write(reinterpret_cast<char*>(&pos.x), sizeof(float));
	file.write(reinterpret_cast<char*>(&pos.y), sizeof(float));
	file.write(reinterpret_cast<char*>(&layer), sizeof(float));
	file.write(reinterpret_cast<char*>(&id), sizeof(unsigned int));
}

void SaveDataTypes::TileData::read(std::ifstream& file) {
	file.read(reinterpret_cast<char*>(&pos.x), sizeof(float));
	file.read(reinterpret_cast<char*>(&pos.y), sizeof(float));
	file.read(reinterpret_cast<char*>(&layer), sizeof(float));
	file.read(reinterpret_cast<char*>(&id), sizeof(unsigned int));
}

SaveDataTypes::ItemData::ItemData(Item i) : id(i.getID()), quantity(i.getQuantity()) {
}

void SaveDataTypes::ItemData::save(std::ofstream& file) {
	file.write(reinterpret_cast<char*>(&id), sizeof(unsigned int));
	file.write(reinterpret_cast<char*>(&quantity), sizeof(unsigned int));
}

void SaveDataTypes::ItemData::read(std::ifstream& file) {
	file.read(reinterpret_cast<char*>(&id), sizeof(unsigned int));
	file.read(reinterpret_cast<char*>(&quantity), sizeof(unsigned int));
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

SaveDataTypes::EntityData::EntityData(Entity e) :
	position(e.getPosition()), layer(e.getLayer()), velocity(e.getVelocity()), id(e.getID()) {
}

void SaveDataTypes::EntityData::save(std::ofstream& file) {
	file.write(reinterpret_cast<char*>(&position.x), sizeof(float));
	file.write(reinterpret_cast<char*>(&position.y), sizeof(float));
	file.write(reinterpret_cast<char*>(&layer), sizeof(unsigned int));
	file.write(reinterpret_cast<char*>(&velocity.x), sizeof(float));
	file.write(reinterpret_cast<char*>(&velocity.y), sizeof(float));
	file.write(reinterpret_cast<char*>(&id), sizeof(unsigned int));
}

void SaveDataTypes::EntityData::read(std::ifstream& file) {
	file.read(reinterpret_cast<char*>(&position.x), sizeof(float));
	file.read(reinterpret_cast<char*>(&position.y), sizeof(float));
	file.read(reinterpret_cast<char*>(&layer), sizeof(unsigned int));
	file.read(reinterpret_cast<char*>(&velocity.x), sizeof(float));
	file.read(reinterpret_cast<char*>(&velocity.y), sizeof(float));
	file.read(reinterpret_cast<char*>(&id), sizeof(unsigned int));
}

SaveDataTypes::EntityItemData::EntityItemData(EntityItem i) : EntityData(i) {
}

SaveDataTypes::EntityNPCData::EntityNPCData(EntityNPC e) :
	inventory(static_cast<InventoryBase>(*(e.getInventory()))), health(e.getHealth()), EntityData(e) {
}

void SaveDataTypes::EntityNPCData::save(std::ofstream& file) {
	EntityData::save(file);

	inventory.save(file);
	file.write(reinterpret_cast<char*>(&health), sizeof(float));
}

void SaveDataTypes::EntityNPCData::read(std::ifstream& file) {
	EntityData::read(file);

	inventory.read(file);
	file.read(reinterpret_cast<char*>(&health), sizeof(float));
}

SaveDataTypes::EntityProjectileData::EntityProjectileData(EntityProjectile p) : EntityData(p) {
}

SaveDataTypes::EntityPlayerData::EntityPlayerData(EntityPlayer p) :
	sanity(p.getSanity()), thirst(p.getThirst()), hunger(p.getHunger()), exhaustion(p.getExhaustion()),
	stamina(p.getStamina()), EntityNPCData(p) {
}

void SaveDataTypes::EntityPlayerData::save(std::ofstream& file) {
	EntityNPCData::save(file);

	file.write(reinterpret_cast<char*>(&sanity), sizeof(float));
	file.write(reinterpret_cast<char*>(&thirst), sizeof(float));
	file.write(reinterpret_cast<char*>(&hunger), sizeof(float));
	file.write(reinterpret_cast<char*>(&exhaustion), sizeof(float));
	file.write(reinterpret_cast<char*>(&stamina), sizeof(float));
}

void SaveDataTypes::EntityPlayerData::read(std::ifstream& file) {
	EntityNPCData::read(file);

	file.read(reinterpret_cast<char*>(&sanity), sizeof(float));
	file.read(reinterpret_cast<char*>(&thirst), sizeof(float));
	file.read(reinterpret_cast<char*>(&hunger), sizeof(float));
	file.read(reinterpret_cast<char*>(&exhaustion), sizeof(float));
	file.read(reinterpret_cast<char*>(&stamina), sizeof(float));
}

SaveDataTypes::ChunkData::ChunkData() {
	for(unsigned int x = 0; x < CHUNK_SIZE; x++) {
		std::vector<std::vector<TileData>> xData;
		for(unsigned int y = 0; y < WORLD_HEIGHT; y++) {
			std::vector<TileData> yData;
			for(unsigned int z = 0; z < WORLD_DEPTH; z++) {
				yData.push_back(TileData());
			}
			xData.push_back(yData);
		}
		tiles.push_back(xData);
	}
}

void SaveDataTypes::ChunkData::save(std::ofstream& file) {
	for(int x = 0; x < CHUNK_SIZE; x++) {
		for(int y = 0; y < WORLD_HEIGHT; y++) {
			for(int k = 0; k < WORLD_DEPTH; k++) {
				tiles[x][y][k].save(file);
			}
		}
	}
	file.write(reinterpret_cast<char*>(&(biomeID)), sizeof(unsigned int));
}

void SaveDataTypes::ChunkData::read(std::ifstream& file) {
	for(int x = 0; x < CHUNK_SIZE; x++) {
		for(int y = 0; y < WORLD_HEIGHT; y++) {
			for(int k = 0; k < WORLD_DEPTH; k++) {
				tiles[x][y][k].read(file);
			}
		}
	}
	file.read(reinterpret_cast<char*>(&(biomeID)), sizeof(unsigned int));
}
