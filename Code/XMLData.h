#pragma once

#include <glm/glm.hpp>
#include <string>
#include <map>
#include <rapidxml/rapidxml.hpp>

#include "Scripting/ScriptQueue.h"
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

        static unsigned int getTotalBiomes(); // Returns the number of biomes

    private:
        static void loadXMLTileData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/Blocks.xml");
        static void loadXMLParticleData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/Particles.xml");
        static void loadXMLEntityNPCData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/NPCs.xml");
        static void loadXMLEntityProjectileData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/Projectiles.xml");
        static void loadXMLEntityItemData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/ItemEntities.xml");
        static void loadXMLItemData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/Items.xml");
        static void loadXMLBiomeData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/Biomes.xml");


        static XML_TileData readTileData(rapidxml::xml_node<>* node);
        static XML_ParticleData readParticleData(rapidxml::xml_node<>* node);
        static XML_EntityNPCData readEntityNPCData(rapidxml::xml_node<>* node);
        static XML_EntityProjectileData readEntityProjectileData(rapidxml::xml_node<>* node);
        static XML_EntityItemData readEntityItemData(rapidxml::xml_node<>* node);
        static XML_ItemData readItemData(rapidxml::xml_node<>* node);
        static XML_BiomeData readBiomeData(rapidxml::xml_node<>* node);


        static std::map<unsigned int, XML_TileData> m_tileData;
        static std::map<unsigned int, XML_ParticleData> m_particleData;
        static std::map<unsigned int, XML_EntityNPCData> m_entityNPCData;
        static std::map<unsigned int, XML_EntityProjectileData> m_entityProjectileData;
        static std::map<unsigned int, XML_EntityItemData> m_entityItemData;
        static std::map<unsigned int, XML_ItemData> m_itemData;
        static std::map<unsigned int, XML_BiomeData> m_biomeData;
};
