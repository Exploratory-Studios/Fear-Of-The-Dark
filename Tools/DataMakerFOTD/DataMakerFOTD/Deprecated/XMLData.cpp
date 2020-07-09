#include "XMLData.h"

#include <fstream>

void getVector(rapidxml::xml_node<>* parent, std::string valueName, std::string childName, std::vector<std::string>& vec) {
    /**
        Loops over getValue, getting all values out of a tag:

            <entities>
                <entityID>3</entityID>
                <entityID>4</entityID>
                <someotherelement>6</someotherelement>
            </entities>

        getVector(p, "entities", "entityID", vec) yields a vector of integers with elements: 3,4 and leaves the document as:

            <entities>
                <someotherelement>6</someotherelement>
            </entities>
    **/

    rapidxml::xml_node<>* n = parent->first_node((char*)valueName.c_str()); // Gets <entities> in the example.

    if(n) {
        std::string temp;
        while(getValue(n, childName, temp)) {
            vec.push_back(temp);
        }
    }
}

void getVector(rapidxml::xml_node<>* parent, std::string valueName, std::string childName, std::vector<float>& vec) {
    rapidxml::xml_node<>* n = parent->first_node((char*)valueName.c_str()); // Gets <entities> in the example.

    if(n) {
        float temp;
        while(getValue(n, childName, temp)) {
            vec.push_back(temp);
        }
    }
}

void getVector(rapidxml::xml_node<>* parent, std::string valueName, std::string childName, std::vector<int>& vec) {
    rapidxml::xml_node<>* n = parent->first_node((char*)valueName.c_str()); // Gets <entities> in the example.

    if(n) {
        int temp;
        while(getValue(n, childName, temp)) {
            vec.push_back(temp);
        }
    }
}

void getVector(rapidxml::xml_node<>* parent, std::string valueName, std::string childName, std::vector<unsigned int>& vec) {
    rapidxml::xml_node<>* n = parent->first_node((char*)valueName.c_str()); // Gets <entities> in the example.

    if(n) {
        unsigned int temp;
        while(getValue(n, childName, temp)) {
            vec.push_back(temp);
        }
    }
}

void getVector(rapidxml::xml_node<>* parent, std::string valueName, std::string childName, std::vector<bool>& vec) {
    rapidxml::xml_node<>* n = parent->first_node((char*)valueName.c_str()); // Gets <entities> in the example.

    if(n) {
        bool temp;
        while(getValue(n, childName, temp)) {
            vec.push_back(temp);
        }
    }
}


bool getValue(rapidxml::xml_node<>* parent, std::string valueName, std::string& variable) {
    /// Places value of node with name `valueName` into `variable` and removes the node from the doc.
    rapidxml::xml_node<>* n = parent->first_node((char*)valueName.c_str());
    if(n) {
        if(std::string(n->value()) != std::string("")) {
            variable = n->value();
        }
        parent->remove_node(n);
        return true;
    }
    return false;
}

bool getValue(rapidxml::xml_node<>* parent, std::string valueName, float& variable) {
    /// Places value of node with name `valueName` into `variable` and removes the node from the doc.
    rapidxml::xml_node<>* n = parent->first_node((char*)valueName.c_str());
    if(n) {
        if(std::string(n->value()) != std::string("")) {
            variable = std::stof(n->value());
        }
        parent->remove_node(n);
        return true;
    }
    return false;
}

bool getValue(rapidxml::xml_node<>* parent, std::string valueName, int& variable) {
    /// Places value of node with name `valueName` into `variable` and removes the node from the doc.
    rapidxml::xml_node<>* n = parent->first_node((char*)valueName.c_str());
    if(n) {
        if(std::string(n->value()) != std::string("")) {
            variable = std::stoi(n->value());
        }
        parent->remove_node(n);
        return true;
    }
    return false;
}

bool getValue(rapidxml::xml_node<>* parent, std::string valueName, unsigned int& variable) {
    /// Places value of node with name `valueName` into `variable` and removes the node from the doc.
    rapidxml::xml_node<>* n = parent->first_node((char*)valueName.c_str());
    if(n) {
        if(std::string(n->value()) != std::string("")) {
            variable = std::stoul(n->value());
        }
        parent->remove_node(n);
        return true;
    }
    return false;
}

bool getValue(rapidxml::xml_node<>* parent, std::string valueName, bool& variable) {
    /// Places value of node with name `valueName` into `variable` and removes the node from the doc. Returns true on successful value find
    rapidxml::xml_node<>* n = parent->first_node((char*)valueName.c_str());
    if(n) {
        if(std::string(n->value()) != std::string("")) {
            if(std::string(n->value()) == "0" || std::string(n->value()) == "false") {
                variable = false;
            } else {
                variable = true;
            }
        }
        parent->remove_node(n);
        return true;
    }
    return false;
}

bool XMLData::inited = false;

std::map<unsigned int, XML_TileData> XMLData::m_tileData;
std::map<unsigned int, XML_ParticleData> XMLData::m_particleData;
std::map<unsigned int, XML_EntityNPCData> XMLData::m_entityNPCData;
std::map<unsigned int, XML_EntityProjectileData> XMLData::m_entityProjectileData;
std::map<unsigned int, XML_EntityItemData> XMLData::m_entityItemData;
std::map<unsigned int, XML_ItemData> XMLData::m_itemData;
std::map<unsigned int, XML_BiomeData> XMLData::m_biomeData;
std::map<unsigned int, XML_EraData> XMLData::m_eraData;
std::map<unsigned int, XML_LootDrop> XMLData::m_lootDropData;
std::map<unsigned int, XML_LootTable> XMLData::m_lootTableData;
std::map<unsigned int, XML_StructureData> XMLData::m_structureData;

std::string XMLData::init(std::string directoryPath) {
    std::string ret = "";

    if(!inited) {
        ret += loadXMLTileData(directoryPath + "/Blocks.xml");
        ret += loadXMLParticleData(directoryPath + "/Particles.xml");
        ret += loadXMLEntityNPCData(directoryPath + "/NPCs.xml");
        ret += loadXMLEntityProjectileData(directoryPath + "/Projectiles.xml");
        ret += loadXMLEntityItemData(directoryPath + "/ItemEntities.xml");
        ret += loadXMLItemData(directoryPath + "/Items.xml");
        loadXMLBiomeData(directoryPath + "/Biomes.xml");
        loadXMLLootDropData(directoryPath + "/Loot.xml");
        loadXMLLootTableData(directoryPath + "/Loot.xml");
        loadXMLEraData(directoryPath + "/Eras.xml");
        loadXMLStructureData(directoryPath + "/Structures.xml");
        inited = true;
    } else {
        ret = "ERR";
    }

    return ret;
}

/// Tiles
std::string XMLData::loadXMLTileData(std::string filepath) {
    /** Loads all tile data into the tileData map **/

    // Open file at filepath
    std::ifstream file;
    file.open(filepath);

    if(file.fail()) { // Handle exceptions
        return "File failed to load for Tiles! " + filepath + "\n";
    }

    // Load all the text in the file to a string
    std::string text;
    std::string line;
    while(getline(file, line)) {
        text += line + "\n";
    }

    rapidxml::xml_document<> doc;
    doc.parse<rapidxml::parse_full>((char*)text.c_str());

    for(rapidxml::xml_node<>* node = doc.first_node("tile"); node; node = node->next_sibling("tile")) {
        XML_TileData d = XMLData::readTileData(node);
        unsigned int id = std::stoi(node->first_attribute("id")->value());
        std::string name = node->first_attribute("name")->value();
        d.id = id;
        d.name = name;
        m_tileData.insert(std::pair<unsigned int, XML_TileData>(id, d));
    }

    return "Tiles Good!\n";
}

XML_TileData XMLData::getTileData(unsigned int id) {
    auto index = m_tileData.find(id);

    if(index == m_tileData.end()) {
        XML_TileData t;
        return t;
    }

    return index->second;
}

XML_TileData XMLData::readTileData(rapidxml::xml_node<>* node) {
    XML_TileData d;
    getValue(node, "texture", d.textureFilepath);
    getValue(node, "bumpMap", d.bumpMapFilepath);

    getValue(node, "emittedLight", d.emittedLight);
    getValue(node, "emittedHeat", d.emittedHeat);

    getValue(node, "sizeX", (float&)d.size.x);
    getValue(node, "sizeY", (float&)d.size.y);

    getValue(node, "isSolid", (bool&)d.solid);
    getValue(node, "isDrawn", (bool&)d.drawn);
    getValue(node, "isNatural", (bool&)d.natural);
    getValue(node, "isTransparent", (bool&)d.transparent);

    std::string updateSID = "";
    getValue(node, "updateScript", updateSID);
    d.updateScriptID = updateSID;
    std::string tickSID = "";
    getValue(node, "tickScript", tickSID);
    d.tickScriptID = tickSID;
    std::string destructionSID = "";
    getValue(node, "destructionScript", destructionSID);
    d.destructionScriptID = destructionSID;
    std::string walkScriptID = "";
    getValue(node, "interactScript_walkedOn", walkScriptID);
    d.interactScriptID_walkedOn = walkScriptID;
    std::string RClickScriptID = "";
    getValue(node, "interactScript_used", RClickScriptID);
    d.interactScriptID_used = RClickScriptID;

    return d;
}


/// Particles
std::string XMLData::loadXMLParticleData(std::string filepath) {
    /** Loads all tile data into the tileData map **/

    // Open file at filepath
    std::ifstream file;
    file.open(filepath);

    if(file.fail()) { // Handle exceptions
        return "File failed to load for Particles! " + filepath + "\n";
    }

    // Load all the text in the file to a string
    std::string text;
    std::string line;
    while(getline(file, line)) {
        text += line + "\n";
    }

    rapidxml::xml_document<> doc;
    doc.parse<rapidxml::parse_full>((char*)text.c_str());

    for(rapidxml::xml_node<>* node = doc.first_node("particle"); node; node = node->next_sibling("particle")) {
        XML_ParticleData d = XMLData::readParticleData(node);
        unsigned int id = std::stoi(node->first_attribute("id")->value());
        std::string name = node->first_attribute("name")->value();
        d.name = name;
        d.id = id;
        m_particleData.insert(std::pair<unsigned int, XML_ParticleData>(id, d));
    }
    return "Particles Good!\n";
}

XML_ParticleData XMLData::getParticleData(unsigned int id) {
    auto index = m_particleData.find(id);

    if(index == m_particleData.end()) {
        XML_ParticleData t;
        return t;
    }

    return index->second;
}

XML_ParticleData XMLData::readParticleData(rapidxml::xml_node<>* node) {
    XML_ParticleData d;
    getValue(node, "texture", d.textureFilepath);
    getValue(node, "bumpMap", d.bumpMapFilepath);
    getValue(node, "script", d.scriptPath);
    getValue(node, "decayRate", d.decayRate);

    return d;
}


/// Entities: NPCS

std::string XMLData::loadXMLEntityNPCData(std::string filepath) {
    /** Loads all NPC entity data into the tileData map **/

    // Open file at filepath
    std::ifstream file;
    file.open(filepath);

    if(file.fail()) { // Handle exceptions
        return "File failed to load for NPCs! " + filepath + "\n";
    }

    // Load all the text in the file to a string
    std::string text;
    std::string line;
    while(getline(file, line)) {
        text += line + "\n";
    }

    rapidxml::xml_document<> doc;
    doc.parse<rapidxml::parse_full>((char*)text.c_str());

    for(rapidxml::xml_node<>* node = doc.first_node("npc"); node; node = node->next_sibling("npc")) {
        XML_EntityNPCData d = XMLData::readEntityNPCData(node);
        unsigned int id = std::stoi(node->first_attribute("id")->value());
        d.id = id;
        std::string name = node->first_attribute("name")->value();
        d.name = name;
        d.id = id;
        m_entityNPCData.insert(std::pair<unsigned int, XML_EntityNPCData>(id, d));
    }
    return "NPCs Good!\n";
}

XML_EntityNPCData XMLData::getEntityNPCData(unsigned int id) {
    auto index = m_entityNPCData.find(id);

    if(index == m_entityNPCData.end()) {
        XML_EntityNPCData t;
        return t;
    }

    return index->second;
}

XML_EntityNPCData XMLData::readEntityNPCData(rapidxml::xml_node<>* node) {
    XML_EntityNPCData d;
    getValue(node, "texture", d.textureFilepath);
    getValue(node, "bumpMap", d.bumpMapFilepath);

    getValue(node, "sizeX", (float&)d.size.x);
    getValue(node, "sizeY", (float&)d.size.y);

    getValue(node, "isDamagedByFalls", (bool&)d.fallDamage);
    getValue(node, "isInvincible", (bool&)d.canDie);
    d.canDie = !d.canDie; // just for readability's sake

    getValue(node, "speed", (float&)d.speed);
    getValue(node, "jumpHeight", (float&)d.jumpHeight);

    getValue(node, "maxHealth", (float&)d.maxHealth);

    getValue(node, "faction", d.faction);



    std::string updateSID = "";
    getValue(node, "updateScript", updateSID);
    d.updateScriptID = updateSID;
    std::string tickSID = "";
    getValue(node, "tickScript", tickSID);
    d.tickScriptID = tickSID;
    std::string attackSID = "";
    getValue(node, "attackScript", attackSID);
    d.attackScriptID = attackSID;

    return d;
}


/// Entities: Projectiles

std::string XMLData::loadXMLEntityProjectileData(std::string filepath) {
    /** Loads all NPC entity data into the tileData map **/

    // Open file at filepath
    std::ifstream file;
    file.open(filepath);

    if(file.fail()) { // Handle exceptions
        return "File failed to load for Projectiles! " + filepath + "\n";
    }

    // Load all the text in the file to a string
    std::string text;
    std::string line;
    while(getline(file, line)) {
        text += line + "\n";
    }

    rapidxml::xml_document<> doc;
    doc.parse<rapidxml::parse_full>((char*)text.c_str());

    for(rapidxml::xml_node<>* node = doc.first_node("projectile"); node; node = node->next_sibling("projectile")) {
        XML_EntityProjectileData d = XMLData::readEntityProjectileData(node);
        unsigned int id = std::stoi(node->first_attribute("id")->value());
        d.id = id;
        std::string name = node->first_attribute("name")->value();
        d.name = name;
        m_entityProjectileData.insert(std::pair<unsigned int, XML_EntityProjectileData>(id, d));
    }
    return "Projectiles Good!\n";
}

XML_EntityProjectileData XMLData::getEntityProjectileData(unsigned int id) {
    auto index = m_entityProjectileData.find(id);

    if(index == m_entityProjectileData.end()) {
        XML_EntityProjectileData t;
        return t;
    }

    return index->second;
}

XML_EntityProjectileData XMLData::readEntityProjectileData(rapidxml::xml_node<>* node) {
    XML_EntityProjectileData d;
    getValue(node, "texture", d.textureFilepath);
    getValue(node, "bumpMap", d.bumpMapFilepath);

    getValue(node, "sizeX", (float&)d.size.x);
    getValue(node, "sizeY", (float&)d.size.y);

    getValue(node, "collides", (bool&)d.collides);

    getValue(node, "speed", (float&)d.speed);
    getValue(node, "damage", (float&)d.damage);

    std::string updateSID = "";
    getValue(node, "updateScript", updateSID);
    d.updateScriptID = updateSID;
    std::string tickSID = "";
    getValue(node, "tickScript", tickSID);
    d.tickScriptID = tickSID;

    return d;
}


/// Entities: Items

std::string XMLData::loadXMLEntityItemData(std::string filepath) {
    /** Loads all NPC entity data into the tileData map **/

    // Open file at filepath
    std::ifstream file;
    file.open(filepath);

    if(file.fail()) { // Handle exceptions
        return "File failed to load for EntityItems! " + filepath + "\n";
    }

    // Load all the text in the file to a string
    std::string text;
    std::string line;
    while(getline(file, line)) {
        text += line + "\n";
    }

    rapidxml::xml_document<> doc;
    doc.parse<rapidxml::parse_full>((char*)text.c_str());

    for(rapidxml::xml_node<>* node = doc.first_node("itemEntity"); node; node = node->next_sibling("itemEntity")) {
        XML_EntityItemData d = XMLData::readEntityItemData(node);
        unsigned int id = std::stoi(node->first_attribute("id")->value());
        d.id = id;
        std::string name = node->first_attribute("name")->value();
        d.name = name;
        m_entityItemData.insert(std::pair<unsigned int, XML_EntityItemData>(id, d));
    }
    return "ItemEntities Good!\n";
}

XML_EntityItemData XMLData::getEntityItemData(unsigned int id) {
    auto index = m_entityItemData.find(id);

    if(index == m_entityItemData.end()) {
        XML_EntityItemData t;
        return t;
    }

    return index->second;
}

XML_EntityItemData XMLData::readEntityItemData(rapidxml::xml_node<>* node) {
    XML_EntityItemData d;
    getValue(node, "texture", d.textureFilepath);
    getValue(node, "bumpMap", d.bumpMapFilepath);

    getValue(node, "sizeX", (float&)d.size.x);
    getValue(node, "sizeY", (float&)d.size.y);

    getValue(node, "item", (int&)d.itemID);

    std::string updateSID = "";
    getValue(node, "updateScript", updateSID);
    d.updateScriptID = updateSID;
    std::string tickSID = "";
    getValue(node, "tickScript", tickSID);
    d.tickScriptID = tickSID;

    return d;
}


/// Real Items

std::string XMLData::loadXMLItemData(std::string filepath) {
    /** Loads all NPC entity data into the tileData map **/

    // Open file at filepath
    std::ifstream file;
    file.open(filepath);

    if(file.fail()) { // Handle exceptions
        return "File failed to load for Items! " + filepath + "\n";
    }

    // Load all the text in the file to a string
    std::string text;
    std::string line;
    while(getline(file, line)) {
        text += line + "\n";
    }

    rapidxml::xml_document<> doc;
    doc.parse<rapidxml::parse_full>((char*)text.c_str());

    for(rapidxml::xml_node<>* node = doc.first_node("item"); node; node = node->next_sibling("item")) {
        XML_ItemData d = XMLData::readItemData(node);
        unsigned int id = std::stoi(node->first_attribute("id")->value());
        std::string name = node->first_attribute("name")->value();
        d.id = id;
        d.name = name;
        m_itemData.insert(std::pair<unsigned int, XML_ItemData>(id, d));
    }
    return "Items Good!\n";
}

XML_ItemData XMLData::getItemData(unsigned int id) {
    auto index = m_itemData.find(id);

    if(index == m_itemData.end()) {
        XML_ItemData t;
        return t;
    }

    return index->second;
}

XML_ItemData XMLData::readItemData(rapidxml::xml_node<>* node) {
    XML_ItemData d;
    getValue(node, "texture", d.textureFilepath);

    getValue(node, "weight", (float&)d.weight);

    std::string useScriptSID = "";
    getValue(node, "useScript", useScriptSID);
    d.useScriptID = useScriptSID;

    return d;
}

/// Biomes

std::string XMLData::loadXMLBiomeData(std::string filepath) {
    /** Loads all biome data into the biomeData map **/

    // Open file at filepath
    std::ifstream file;
    file.open(filepath);

    if(file.fail()) { // Handle exceptions
        return "File failed to load for Biomes! " + filepath + "\n";
    }

    // Load all the text in the file to a string
    std::string text;
    std::string line;
    while(getline(file, line)) {
        text += line + "\n";
    }

    rapidxml::xml_document<> doc;
    doc.parse<rapidxml::parse_full>((char*)text.c_str());

    for(rapidxml::xml_node<>* node = doc.first_node("biome"); node; node = node->next_sibling("biome")) {
        XML_BiomeData b = XMLData::readBiomeData(node);
        unsigned int id = std::stoi(node->first_attribute("id")->value());
        std::string name = node->first_attribute("name")->value();
        b.id = id;
        b.name = name;
        m_biomeData.insert(std::pair<unsigned int, XML_BiomeData>(id, b));
    }

    return "Biomes Good!\n";
}

XML_BiomeData XMLData::getBiomeData(unsigned int id) {
    auto index = m_biomeData.find(id);

    if(index == m_biomeData.end()) {
        XML_BiomeData b;
        return b;
    }

    return index->second;
}

XML_BiomeData XMLData::readBiomeData(rapidxml::xml_node<>* node) {

    XML_BiomeData d;

    getValue(node, "backgroundTexture", d.backgroundTexture);
    getValue(node, "baseHeight", d.baseHeight);
    getValue(node, "maxHeightDiff", d.maxHeightDiff);
    getValue(node, "maxTemperature", d.maxTemperature);
    getValue(node, "baseTemperature", d.baseTemperature);
    getValue(node, "flatness", d.flatness);
    getVector(node, "entities", "entityID", d.mobSpawnIds);
    getVector(node, "structures", "structureID", d.mobSpawnIds);
    getVector(node, "surfaceBlocks", "blockID", d.mobSpawnIds);
    getVector(node, "groundBlocks", "blockID", d.mobSpawnIds);

    return d;
}

unsigned int XMLData::getTotalBiomes() {
    return m_biomeData.size();
}

/// Eras

std::string XMLData::loadXMLEraData(std::string filepath) {
    /** Loads all era data into the biomeData map **/

    // Open file at filepath
    std::ifstream file;
    file.open(filepath);

    if(file.fail()) { // Handle exceptions
        return "File failed to load for Eras! " + filepath + "\n";
    }

    // Load all the text in the file to a string
    std::string text;
    std::string line;
    while(getline(file, line)) {
        text += line + "\n";
    }

    rapidxml::xml_document<> doc;
    doc.parse<rapidxml::parse_full>((char*)text.c_str());

    for(rapidxml::xml_node<>* node = doc.first_node("era"); node; node = node->next_sibling("era")) {
        XML_EraData e = XMLData::readEraData(node);
        unsigned int id = std::stoi(node->first_attribute("id")->value());
        std::string name = node->first_attribute("name")->value();
        e.id = id;
        e.name = name;
        m_eraData.insert(std::pair<unsigned int, XML_EraData>(id, e));
    }
    return "Eras Good!\n";
}

XML_EraData XMLData::getEraData(unsigned int id) {
    auto index = m_eraData.find(id);

    if(index == m_eraData.end()) {
        XML_EraData e;
        return e;
    }

    return index->second;
}

XML_EraData XMLData::readEraData(rapidxml::xml_node<>* node) {

    XML_EraData d;

    getVector(node, "biomes", "biomeID", d.biomeIds);

    return d;
}

/// LootDrops

std::string XMLData::loadXMLLootDropData(std::string filepath) {
    /** Loads all loot drop data into the biomeData map **/

    // Open file at filepath
    std::ifstream file;
    file.open(filepath);

    if(file.fail()) { // Handle exceptions
        return "File failed to load for Loot Drops! " + filepath + "\n";
    }

    // Load all the text in the file to a string
    std::string text;
    std::string line;
    while(getline(file, line)) {
        text += line + "\n";
    }

    rapidxml::xml_document<> doc;
    doc.parse<rapidxml::parse_full>((char*)text.c_str());

    for(rapidxml::xml_node<>* node = doc.first_node("lootDrop"); node; node = node->next_sibling("lootDrop")) {
        XML_LootDrop d = XMLData::readLootDropData(node);
        unsigned int id = std::stoi(node->first_attribute("id")->value());
        std::string name = node->first_attribute("name")->value();
        d.id = id;
        d.name = name;
        m_lootDropData.insert(std::pair<unsigned int, XML_LootDrop>(id, d));
    }
    return "Loot Drops Good!\n";
}

XML_LootDrop XMLData::getLootDropData(unsigned int id) {
    auto index = m_lootDropData.find(id);

    if(index == m_lootDropData.end()) {
        XML_LootDrop d;
        return d;
    }

    return index->second;
}

XML_LootDrop XMLData::readLootDropData(rapidxml::xml_node<>* node) {

    XML_LootDrop d;

    getValue(node, "itemID", d.itemID);
    getValue(node, "minDrop", d.minDrop);
    getValue(node, "maxDrop", d.maxDrop);
    getValue(node, "chance", d.chance);

    return d;
}

/// LootTables

std::string XMLData::loadXMLLootTableData(std::string filepath) {
    /** Loads all loot table data into the biomeData map **/

    // Open file at filepath
    std::ifstream file;
    file.open(filepath);

    if(file.fail()) { // Handle exceptions
        return "File failed to load for Loot Tables! " + filepath + "\n";
    }

    // Load all the text in the file to a string
    std::string text;
    std::string line;
    while(getline(file, line)) {
        text += line + "\n";
    }

    rapidxml::xml_document<> doc;
    doc.parse<rapidxml::parse_full>((char*)text.c_str());

    for(rapidxml::xml_node<>* node = doc.first_node("lootTable"); node; node = node->next_sibling("lootTable")) {
        XML_LootTable d = XMLData::readLootTableData(node);
        unsigned int id = std::stoi(node->first_attribute("id")->value());
        std::string name = node->first_attribute("name")->value();
        d.id = id;
        d.name = name;
        m_lootTableData.insert(std::pair<unsigned int, XML_LootTable>(id, d));
    }
    return "Loot Tables Good!\n";
}

XML_LootTable XMLData::getLootTableData(unsigned int id) {
    auto index = m_lootTableData.find(id);

    if(index == m_lootTableData.end()) {
        XML_LootTable d;
        return d;
    }

    return index->second;
}

XML_LootTable XMLData::readLootTableData(rapidxml::xml_node<>* node) {

    XML_LootTable d;

    std::vector<unsigned int> ids;
    getVector(node, "drops", "dropID", ids);

    for(unsigned int i = 0; i < ids.size(); i++) {
        d.drops.push_back(ids[i]);
    }

    return d;
}

/// Structures

std::string XMLData::loadXMLStructureData(std::string filepath) {
    /** Loads all structure data into the structureData map **/

    // Open file at filepath
    std::ifstream file;
    file.open(filepath);

    if(file.fail()) { // Handle exceptions
        return "File failed to load for Structures! " + filepath + "\n";
    }

    // Load all the text in the file to a string
    std::string text;
    std::string line;
    while(getline(file, line)) {
        text += line + "\n";
    }

    rapidxml::xml_document<> doc;
    doc.parse<rapidxml::parse_full>((char*)text.c_str());

    for(rapidxml::xml_node<>* node = doc.first_node("structure"); node; node = node->next_sibling("structure")) {
        XML_StructureData s = XMLData::readStructureData(node);
        unsigned int id = std::stoi(node->first_attribute("id")->value());
        std::string name = node->first_attribute("name")->value();
        s.id = id;
        s.name = name;
        m_structureData.insert(std::pair<unsigned int, XML_StructureData>(id, s));
    }
    return "Structures Good!\n";
}

XML_StructureData XMLData::getStructureData(unsigned int id) {
    auto index = m_structureData.find(id);

    if(index == m_structureData.end()) {
        XML_StructureData s;
        return s;
    }

    return index->second;
}

XML_StructureData XMLData::readStructureData(rapidxml::xml_node<>* node) {

    XML_StructureData s;

    getValue(node, "structureFilepath", s.structureFilepath);
    getVector(node, "biomes", "biomeID", s.biomeIDs);
    getValue(node, "chance", s.chance);
    getValue(node, "maxAmnt", s.maxAmnt);
    getValue(node, "minAmnt", s.minAmnt);

    return s;
}

