#pragma once

#include <glm/glm.hpp>
#include <string>
#include <map>
#include <rapidxml/rapidxml.hpp>

#include "XMLDataTypes.h"

/*#define HOME_PATH UNDEFINED_HOME_ERROR

#ifdef WINDOWS
#undef HOME_PATH
#define HOME_PATH std::string((std::string)getenv("APPDATA") + "/Exploratory Studios/Fear Of The Dark")
#endif // WINDOWS
#ifdef LINUX
#undef HOME_PATH
#define HOME_PATH std::string((std::string)getenv("HOME") + "/.exploratory/Fear Of The Dark")
#endif // LINUX

#define ASSETS_FOLDER_PATH HOME_PATH + std::string("/Assets/")*/

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

class XMLData
{
    friend class MainScreen;

    public:
        static std::string init(std::string directoryPath);

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

        static unsigned int getTotalBiomes(); // Returns the number of biomes

    private:
        static std::string loadXMLTileData(std::string filepath);
        static std::string loadXMLParticleData(std::string filepath);
        static std::string loadXMLEntityNPCData(std::string filepath);
        static std::string loadXMLEntityProjectileData(std::string filepath);
        static std::string loadXMLEntityItemData(std::string filepath);
        static std::string loadXMLItemData(std::string filepath);
        static std::string loadXMLBiomeData(std::string filepath);
        static std::string loadXMLEraData(std::string filepath);
        static std::string loadXMLLootDropData(std::string filepath);
        static std::string loadXMLLootTableData(std::string filepath);
        static std::string loadXMLStructureData(std::string filepath);


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

        static bool inited;
};
