#pragma once

#include <glm/glm.hpp>
#include <string>
#include <map>
#include <rapidxml/rapidxml.hpp>

#include "Scripting/ScriptQueue.h"
#include "PresetValues.h"

struct XML_TileData;
struct XML_EntityData;
struct XML_ItemData;
struct XML_ParticleData;

void getValue(rapidxml::xml_node<>* parent, std::string valueName, std::string& variable);

void getValue(rapidxml::xml_node<>* parent, std::string valueName, float& variable);

void getValue(rapidxml::xml_node<>* parent, std::string valueName, int& variable);

void getValue(rapidxml::xml_node<>* parent, std::string valueName, bool& variable);

class XMLData
{
    public:
        static void init();

        static XML_TileData getTileData(unsigned int id);
        static XML_ParticleData getParticleData(unsigned int id);

    private:
        static void loadXMLTileData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/Blocks.xml");
        static void loadXMLParticleData(std::string filepath = ASSETS_FOLDER_PATH + "/Data/Particles.xml");

        static XML_TileData readTileData(rapidxml::xml_node<>* node);
        static XML_ParticleData readParticleData(rapidxml::xml_node<>* node);

        static std::map<unsigned int, XML_TileData> m_tileData;
        static std::map<unsigned int, XML_ParticleData> m_particleData;
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
    /// TODO: Inventory
};

struct XML_ParticleData {
    std::string textureFilepath;
    std::string bumpMapFilepath;
    std::string scriptPath;
    float decayRate = 0.0f;
};
