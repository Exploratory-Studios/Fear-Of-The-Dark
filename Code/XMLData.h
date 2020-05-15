#pragma once

#include <glm/glm.hpp>
#include <string>
#include <map>
#include <rapidxml/rapidxml.hpp>

#include "Scripting/ScriptQueue.h"
#include "PresetValues.h"

#include "SaveDataTypes.h"

struct XML_TileData;
struct XML_EntityNPCData;
struct XML_EntityProjectileData;
struct XML_EntityItemData;
struct XML_ItemData;
struct XML_ParticleData;

void getValue(rapidxml::xml_node<>* parent, std::string valueName, std::string& variable);
void getValue(rapidxml::xml_node<>* parent, std::string valueName, float& variable);
void getValue(rapidxml::xml_node<>* parent, std::string valueName, int& variable);
void getValue(rapidxml::xml_node<>* parent, std::string valueName, bool& variable);

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
    private:
        static void loadXMLTileData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/Blocks.xml");

        static void loadXMLParticleData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/Particles.xml");

        static void loadXMLEntityNPCData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/NPCs.xml");
        static void loadXMLEntityProjectileData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/Projectiles.xml");
        static void loadXMLEntityItemData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/ItemEntities.xml");

        static void loadXMLItemData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/Items.xml");


        static XML_TileData readTileData(rapidxml::xml_node<>* node);

        static XML_ParticleData readParticleData(rapidxml::xml_node<>* node);

        static XML_EntityNPCData readEntityNPCData(rapidxml::xml_node<>* node);
        static XML_EntityProjectileData readEntityProjectileData(rapidxml::xml_node<>* node);
        static XML_EntityItemData readEntityItemData(rapidxml::xml_node<>* node);

        static XML_ItemData readItemData(rapidxml::xml_node<>* node);


        static std::map<unsigned int, XML_TileData> m_tileData;

        static std::map<unsigned int, XML_ParticleData> m_particleData;

        static std::map<unsigned int, XML_EntityNPCData> m_entityNPCData;
        static std::map<unsigned int, XML_EntityProjectileData> m_entityProjectileData;
        static std::map<unsigned int, XML_EntityItemData> m_entityItemData;

        static std::map<unsigned int, XML_ItemData> m_itemData;
};

struct XML_TileData {
    std::string textureFilepath;
    std::string bumpMapFilepath;
    float emittedLight = 0.0f;
    float emittedHeat = 0.0f;
    glm::vec2 size = glm::vec2(1.0f);
    bool solid = true;
    bool drawn = true;
    bool natural = false;
    bool transparent = false;
    int updateScriptID = -1;
    int tickScriptID = -1;
    int destructionScriptID = -1;
    int interactScriptID_walkedOn = -1;
    int interactScriptID_used = -1;
    MetaData defaultMD;
    /// TODO: Inventory
};

struct XML_ParticleData {
    std::string textureFilepath;
    std::string bumpMapFilepath;
    std::string scriptPath;
    float decayRate = 0.0f;
};

struct XML_EntityNPCData {
    unsigned int id = (unsigned int)-1;

    std::string textureFilepath;
    std::string bumpMapFilepath;

    glm::vec2 size = glm::vec2(1.0f);

    int updateScriptID = -1;
    int tickScriptID = -1;
    int attackScriptID = -1;
    int tradeTableID = -1;
    int dropTableID = -1;
    int dialogueID = -1;

    bool fallDamage = true;
    bool canDie = true;

    float speed = 0.01f;
    float jumpHeight = 1.92f; // (3 blocks default) This is the actual value the entity should be using.
    float maxHealth = 100.0f;

    Categories::Faction faction;

    MetaData defaultMD;

    /// TODO: Move dialogue, trades, and drops to XML format?
};

struct XML_EntityProjectileData {
    unsigned int id = (unsigned int)-1;

    std::string textureFilepath;
    std::string bumpMapFilepath;

    glm::vec2 size = glm::vec2(1.0f);

    bool collides = true;

    int updateScriptID = -1;
    int tickScriptID = -1;

    float speed = 0.1f;
    float damage = 1.0f;

    MetaData defaultMD;
};

struct XML_EntityItemData {
    unsigned int id = (unsigned int)-1;

    std::string textureFilepath;
    std::string bumpMapFilepath;

    glm::vec2 size = glm::vec2(1.0f);

    int updateScriptID = -1;
    int tickScriptID = -1;
    unsigned int itemID = 0;

    MetaData defaultMD;
};

struct XML_ItemData {
    std::string textureFilepath;
    std::string name;

    int useScriptID = -1;

    float weight = 0.0f;

    MetaData defaultMD;
};
