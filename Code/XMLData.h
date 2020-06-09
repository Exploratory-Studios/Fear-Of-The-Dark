#pragma once

#include <glm/glm.hpp>
#include <string>
#include <map>
#include <rapidxml/rapidxml.hpp>

#include "ScriptQueue.h"
#include "PresetValues.h"

#include "SaveDataTypes.h"


#include "XMLDataTypes.h"


void getVector(rapidxml::xml_node<>* parent, std::string valueName, std::string childName, std::vector<std::string>& vec); // Retrieves a string vector from a node
void getVector(rapidxml::xml_node<>* parent, std::string valueName, std::string childName, std::vector<float>& vec); // Retrieves a float vector from a node
void getVector(rapidxml::xml_node<>* parent, std::string valueName, std::string childName, std::vector<int>& vec); // Retrieves an int vector from a node
void getVector(rapidxml::xml_node<>* parent, std::string valueName, std::string childName, std::vector<unsigned int>& vec); // Retrieves an unsigned int vector from a node
void getVector(rapidxml::xml_node<>* parent, std::string valueName, std::string childName, std::vector<bool>& vec); // Retrieves a bool vector from a node

bool getValue(rapidxml::xml_node<>* parent, std::string valueName, std::string& variable);
bool getValue(rapidxml::xml_node<>* parent, std::string valueName, float& variable);
bool getValue(rapidxml::xml_node<>* parent, std::string valueName, int& variable);
bool getValue(rapidxml::xml_node<>* parent, std::string valueName, unsigned int& variable);
bool getValue(rapidxml::xml_node<>* parent, std::string valueName, bool& variable);

void getMetaData(rapidxml::xml_node<>* parent, MetaData& mdVar);

class XMLData
{
    public:
        static void init();

        static XML_TileData getTileData(unsigned int id);
        static XML_ParticleData getParticleData(unsigned int id);
        static XML_EntityNPCData getEntityNPCData(unsigned int id);
        static XML_EntityProjectileData getEntityProjectileData(unsigned int id);
        static XML_EntityItemData getEntityItemData(unsigned int id);
        static XML_ItemData getItemData(unsigned int id);
        static XML_BiomeData getBiomeData(unsigned int id);
        static XML_EraData getEraData(unsigned int id);
        static XML_LootDrop getLootDropData(unsigned int id);
        static XML_LootTable getLootTableData(unsigned int id);
        static XML_StructureData getStructureData(unsigned int id);
        static XML_QuestData getQuestData(unsigned int id);
        static XML_QuestObjectiveData getQuestObjectiveData(unsigned int id);

        static unsigned int getTotalBiomes(); // Returns the number of biomes

    private:
        static void loadXMLTileData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/Blocks.xml");
        static void loadXMLParticleData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/Particles.xml");
        static void loadXMLEntityNPCData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/NPCs.xml");
        static void loadXMLEntityProjectileData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/Projectiles.xml");
        static void loadXMLEntityItemData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/ItemEntities.xml");
        static void loadXMLItemData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/Items.xml");
        static void loadXMLBiomeData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/Biomes.xml");
        static void loadXMLEraData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/Eras.xml");
        static void loadXMLLootDropData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/Loot.xml");
        static void loadXMLLootTableData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/Loot.xml");
        static void loadXMLStructureData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/Structures.xml");
        static void loadXMLQuestData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/Quests.xml");
        static void loadXMLQuestObjectiveData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/Quests.xml");


        static XML_TileData readTileData(rapidxml::xml_node<>* node);
        static XML_ParticleData readParticleData(rapidxml::xml_node<>* node);
        static XML_EntityNPCData readEntityNPCData(rapidxml::xml_node<>* node);
        static XML_EntityProjectileData readEntityProjectileData(rapidxml::xml_node<>* node);
        static XML_EntityItemData readEntityItemData(rapidxml::xml_node<>* node);
        static XML_ItemData readItemData(rapidxml::xml_node<>* node);
        static XML_BiomeData readBiomeData(rapidxml::xml_node<>* node);
        static XML_EraData readEraData(rapidxml::xml_node<>* node);
        static XML_LootDrop readLootDropData(rapidxml::xml_node<>* node);
        static XML_LootTable readLootTableData(rapidxml::xml_node<>* node);
        static XML_StructureData readStructureData(rapidxml::xml_node<>* node);
        static XML_QuestData readQuestData(rapidxml::xml_node<>* node);
        static XML_QuestObjectiveData readQuestObjectiveData(rapidxml::xml_node<>* node);


        static std::map<unsigned int, XML_TileData> m_tileData;
        static std::map<unsigned int, XML_ParticleData> m_particleData;
        static std::map<unsigned int, XML_EntityNPCData> m_entityNPCData;
        static std::map<unsigned int, XML_EntityProjectileData> m_entityProjectileData;
        static std::map<unsigned int, XML_EntityItemData> m_entityItemData;
        static std::map<unsigned int, XML_ItemData> m_itemData;
        static std::map<unsigned int, XML_BiomeData> m_biomeData;
        static std::map<unsigned int, XML_EraData> m_eraData;
        static std::map<unsigned int, XML_LootDrop> m_lootDropData;
        static std::map<unsigned int, XML_LootTable> m_lootTableData;
        static std::map<unsigned int, XML_StructureData> m_structureData;
        static std::map<unsigned int, XML_QuestData> m_questData;
        static std::map<unsigned int, XML_QuestObjectiveData> m_questObjectiveData;
};
