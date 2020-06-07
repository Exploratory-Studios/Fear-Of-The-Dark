#pragma once

#include "SaveDataTypes.h"


#include <string>
#include <glm/glm.hpp>

struct XML_TileData;
struct XML_EntityNPCData;
struct XML_EntityProjectileData;
struct XML_EntityItemData;
struct XML_ItemData;
struct XML_ParticleData;
struct XML_BiomeData;


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
    unsigned int id;

    std::string textureFilepath;
    std::string name;

    int useScriptID = -1;

    float weight = 0.0f;

    MetaData defaultMD;
};

struct XML_BiomeData {
    unsigned int id;
    std::string name = "UNDEFINED";

    int baseHeight = 10; // The base altitude of the biome.
    int maxHeightDiff = 0; // The most the altitude will change
    float maxTemperature = 20; // The **lowest** the temperature will be (noon, not including sun's effect)
    float baseTemperature = 20; // The **highest** the temperature will be (midnight)
    float flatness = 1; // Higher values are more plateau-ish
    std::vector<unsigned int> mobSpawnIds = {}; // The ids of the mobs that will be spawned in this biome
};
