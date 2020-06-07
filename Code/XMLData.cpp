#include "XMLData.h"

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

    std::string temp;
    while(getValue(n, childName, temp)) {
        vec.push_back(temp);
    }
}

void getVector(rapidxml::xml_node<>* parent, std::string valueName, std::string childName, std::vector<float>& vec) {
    rapidxml::xml_node<>* n = parent->first_node((char*)valueName.c_str()); // Gets <entities> in the example.

    float temp;
    while(getValue(n, childName, temp)) {
        vec.push_back(temp);
    }
}

void getVector(rapidxml::xml_node<>* parent, std::string valueName, std::string childName, std::vector<int>& vec) {
    rapidxml::xml_node<>* n = parent->first_node((char*)valueName.c_str()); // Gets <entities> in the example.

    int temp;
    while(getValue(n, childName, temp)) {
        vec.push_back(temp);
    }
}

void getVector(rapidxml::xml_node<>* parent, std::string valueName, std::string childName, std::vector<unsigned int>& vec) {
    rapidxml::xml_node<>* n = parent->first_node((char*)valueName.c_str()); // Gets <entities> in the example.

    unsigned int temp;
    while(getValue(n, childName, temp)) {
        vec.push_back(temp);
    }
}

void getVector(rapidxml::xml_node<>* parent, std::string valueName, std::string childName, std::vector<bool>& vec) {
    rapidxml::xml_node<>* n = parent->first_node((char*)valueName.c_str()); // Gets <entities> in the example.

    bool temp;
    while(getValue(n, childName, temp)) {
        vec.push_back(temp);
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

void getMetaData(rapidxml::xml_node<>* parent, MetaData& mdVar) {
    /// Retrieves ALL existing children from `parent`, then sets their names as the keys, with their values as the values in a MetaData object, stored in `mdVar`. Removes each node systematically from parent after adding them to mdVar.

    MetaData md;

    rapidxml::xml_node<>* child = parent->first_node(); // Fetch the first node (or 0 if there are none).
    while(child) {
        // Now we have a node, we just need to extract its name & value, then add them to the metadata.
        std::string key, val;
        key = std::string(child->name());
        val = std::string(child->value());

        // Now we set (/add) an element to the MetaData.
        md.setElement(key, val);

        parent->remove_first_node(); // Remove `child` from the doc.
        child = parent->first_node(); // Re-set `child` again
    }

    // Completed MetaData extraction, now we just set the `mdVar`
    mdVar = md;
}

void XMLData::init() {
    Logger::getInstance()->log("Beginning to load data...");
    loadXMLTileData();
    Logger::getInstance()->log("Loaded data (Tiles)...");
    loadXMLParticleData();
    Logger::getInstance()->log("Loaded data (Particles)...");
    loadXMLEntityNPCData();
    Logger::getInstance()->log("Loaded data (NPC Entities)...");
    loadXMLEntityProjectileData();
    Logger::getInstance()->log("Loaded data (Projectile Entities)...");
    loadXMLEntityItemData();
    Logger::getInstance()->log("Loaded data (Item Entities)...");
    loadXMLItemData();
    Logger::getInstance()->log("Loaded data (Items)...");
    loadXMLBiomeData();
    Logger::getInstance()->log("Loaded data (Biomes)...");

    Logger::getInstance()->log("Loaded all data successfully!");
}

std::map<unsigned int, XML_TileData> XMLData::m_tileData;
std::map<unsigned int, XML_ParticleData> XMLData::m_particleData;
std::map<unsigned int, XML_EntityNPCData> XMLData::m_entityNPCData;
std::map<unsigned int, XML_EntityProjectileData> XMLData::m_entityProjectileData;
std::map<unsigned int, XML_EntityItemData> XMLData::m_entityItemData;
std::map<unsigned int, XML_ItemData> XMLData::m_itemData;
std::map<unsigned int, XML_BiomeData> XMLData::m_biomeData;

/// Tiles
void XMLData::loadXMLTileData(std::string filepath) {
    /** Loads all tile data into the tileData map **/

    // Open file at filepath
    std::ifstream file;
    file.open(filepath);

    if(file.fail()) { // Handle exceptions
        Logger::getInstance()->log("ERROR: Tile data XML file unable to be loaded: " + filepath, true);
        return;
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
        m_tileData.insert(std::pair<unsigned int, XML_TileData>(id, d));
    }
}

XML_TileData XMLData::getTileData(unsigned int id) {
    auto index = m_tileData.find(id);

    if(index == m_tileData.end()) {
        Logger::getInstance()->log("ERROR: Couldn't find tile data with ID: " + std::to_string(id), true);
        XML_TileData t;
        return t;
    }

    return index->second;
}

XML_TileData XMLData::readTileData(rapidxml::xml_node<>* node) {
    XML_TileData d;
    getValue(node, "texture", d.textureFilepath);
    d.textureFilepath = ASSETS_FOLDER_PATH + "/Textures/Blocks/" + d.textureFilepath;
    getValue(node, "bumpMap", d.bumpMapFilepath);
    d.bumpMapFilepath = ASSETS_FOLDER_PATH + "/Textures/BumpMaps/" + d.bumpMapFilepath;

    getValue(node, "emittedLight", d.emittedLight);
    getValue(node, "emittedHeat", d.emittedHeat);

    getValue(node, "sizeX", (float&)d.size.x);
    getValue(node, "sizeY", (float&)d.size.y);

    getValue(node, "isSolid", (bool&)d.solid);
    getValue(node, "isDrawn", (bool&)d.drawn);
    getValue(node, "isNatural", (bool&)d.natural);
    getValue(node, "isTransparent", (bool&)d.transparent);

    // Despite their names, these are not IDs, they are paths. /// TODO: Fix this shit.
    std::string updateSID = "";
    getValue(node, "updateScript", updateSID);
    std::string tickSID = "";
    getValue(node, "tickScript", tickSID);
    std::string destructionSID = "";
    getValue(node, "destructionScript", destructionSID);
    std::string walkScriptID = "";
    getValue(node, "interactScript_walkedOn", walkScriptID);
    std::string RClickScriptID = "";
    getValue(node, "interactScript_used", RClickScriptID);

    if(updateSID.length() > 0) {
        d.updateScriptID = ScriptQueue::addScript(updateSID);
    }
    if(tickSID.length() > 0) {
        d.tickScriptID = ScriptQueue::addScript(tickSID);
    }
    if(destructionSID.length() > 0) {
        d.destructionScriptID = ScriptQueue::addScript(destructionSID);
    }
    if(walkScriptID.length() > 0) {
        d.interactScriptID_walkedOn = ScriptQueue::addScript(walkScriptID);
    }
    if(RClickScriptID.length() > 0) {
        d.interactScriptID_used = ScriptQueue::addScript(RClickScriptID);
    }

    // At this point, the doc is empty, save for the few extra MetaData bits. Now we need to add those.
    getMetaData(node, d.defaultMD);

    return d;
}


/// Particles
void XMLData::loadXMLParticleData(std::string filepath) {
    /** Loads all tile data into the tileData map **/

    // Open file at filepath
    std::ifstream file;
    file.open(filepath);

    if(file.fail()) { // Handle exceptions
        Logger::getInstance()->log("ERROR: Particle data XML file unable to be loaded: " + filepath, true);
        return;
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
        m_particleData.insert(std::pair<unsigned int, XML_ParticleData>(id, d));
    }
}

XML_ParticleData XMLData::getParticleData(unsigned int id) {
    auto index = m_particleData.find(id);

    if(index == m_particleData.end()) {
        Logger::getInstance()->log("ERROR: Couldn't find particle data with ID: " + std::to_string(id), true);
        XML_ParticleData t;
        return t;
    }

    return index->second;
}

XML_ParticleData XMLData::readParticleData(rapidxml::xml_node<>* node) {
    XML_ParticleData d;
    getValue(node, "texture", d.textureFilepath);
    d.textureFilepath = ASSETS_FOLDER_PATH + "/Textures/Particles/" + d.textureFilepath;
    getValue(node, "bumpMap", d.bumpMapFilepath);
    d.bumpMapFilepath = ASSETS_FOLDER_PATH + "/Textures/BumpMaps/" + d.bumpMapFilepath;
    getValue(node, "script", d.scriptPath);
    d.scriptPath = ASSETS_FOLDER_PATH + "/Scripts/" + d.scriptPath;

    getValue(node, "decayRate", d.decayRate);

    return d;
}


/// Entities: NPCS

void XMLData::loadXMLEntityNPCData(std::string filepath) {
    /** Loads all NPC entity data into the tileData map **/

    // Open file at filepath
    std::ifstream file;
    file.open(filepath);

    if(file.fail()) { // Handle exceptions
        Logger::getInstance()->log("ERROR: Entity (NPC) data XML file unable to be loaded: " + filepath, true);
        return;
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
        m_entityNPCData.insert(std::pair<unsigned int, XML_EntityNPCData>(id, d));
    }
}

XML_EntityNPCData XMLData::getEntityNPCData(unsigned int id) {
    auto index = m_entityNPCData.find(id);

    if(index == m_entityNPCData.end()) {
        Logger::getInstance()->log("ERROR: Couldn't find entity (NPC) data with ID: " + std::to_string(id), true);
        XML_EntityNPCData t;
        return t;
    }

    return index->second;
}

XML_EntityNPCData XMLData::readEntityNPCData(rapidxml::xml_node<>* node) {
    XML_EntityNPCData d;
    getValue(node, "texture", d.textureFilepath);
    d.textureFilepath = ASSETS_FOLDER_PATH + "/Textures/Entities/NPCs/" + d.textureFilepath;
    getValue(node, "bumpMap", d.bumpMapFilepath);
    d.bumpMapFilepath = ASSETS_FOLDER_PATH + "/Textures/BumpMaps/" + d.bumpMapFilepath;

    getValue(node, "sizeX", (float&)d.size.x);
    getValue(node, "sizeY", (float&)d.size.y);

    getValue(node, "isDamagedByFalls", (bool&)d.fallDamage);
    getValue(node, "isInvincible", (bool&)d.canDie);
    d.canDie = !d.canDie; // just for readability's sake

    getValue(node, "speed", (float&)d.speed);
    getValue(node, "jumpHeight", (float&)d.jumpHeight);

    d.jumpHeight = std::sqrt(d.jumpHeight * 2.0f * (1.225f / 60.0f));

    getValue(node, "maxHealth", (float&)d.maxHealth);

    std::string factionString = "";
    getValue(node, "faction", (std::string&)factionString);

    if(factionString == "evil") {
        d.faction = Categories::Faction::EVIL;
    } else if(factionString == "bad") {
        d.faction = Categories::Faction::BAD;
    } else if(factionString == "neutral") {
        d.faction = Categories::Faction::NEUTRAL;
    } else if(factionString == "good") {
        d.faction = Categories::Faction::GOOD;
    } else if(factionString == "benign") {
        d.faction = Categories::Faction::BENIGN;
    } else {
        d.faction = Categories::Faction::NEUTRAL;
    }



    std::string updateSID = "";
    getValue(node, "updateScript", updateSID);
    std::string tickSID = "";
    getValue(node, "tickScript", tickSID);
    std::string attackSID = "";
    getValue(node, "attackScript", attackSID);

    if(updateSID.length() > 0) {
        d.updateScriptID = ScriptQueue::addScript(updateSID);
    }
    if(tickSID.length() > 0) {
        d.tickScriptID = ScriptQueue::addScript(tickSID);
    }
    if(attackSID.length() > 0) {
        d.attackScriptID = ScriptQueue::addScript(attackSID);
    }

    // At this point, the doc is empty, save for the few extra MetaData bits. Now we need to add those.
    getMetaData(node, d.defaultMD);

    return d;
}


/// Entities: Projectiles

void XMLData::loadXMLEntityProjectileData(std::string filepath) {
    /** Loads all NPC entity data into the tileData map **/

    // Open file at filepath
    std::ifstream file;
    file.open(filepath);

    if(file.fail()) { // Handle exceptions
        Logger::getInstance()->log("ERROR: Entity (Projectile) data XML file unable to be loaded: " + filepath, true);
        return;
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
        m_entityProjectileData.insert(std::pair<unsigned int, XML_EntityProjectileData>(id, d));
    }
}

XML_EntityProjectileData XMLData::getEntityProjectileData(unsigned int id) {
    auto index = m_entityProjectileData.find(id);

    if(index == m_entityProjectileData.end()) {
        Logger::getInstance()->log("ERROR: Couldn't find entity (Projectile) data with ID: " + std::to_string(id), true);
        XML_EntityProjectileData t;
        return t;
    }

    return index->second;
}

XML_EntityProjectileData XMLData::readEntityProjectileData(rapidxml::xml_node<>* node) {
    XML_EntityProjectileData d;
    getValue(node, "texture", d.textureFilepath);
    d.textureFilepath = ASSETS_FOLDER_PATH + "/Textures/Entities/Projectiles/" + d.textureFilepath;
    getValue(node, "bumpMap", d.bumpMapFilepath);
    d.bumpMapFilepath = ASSETS_FOLDER_PATH + "/Textures/BumpMaps/" + d.bumpMapFilepath;

    getValue(node, "sizeX", (float&)d.size.x);
    getValue(node, "sizeY", (float&)d.size.y);

    getValue(node, "collides", (bool&)d.collides);

    getValue(node, "speed", (float&)d.speed);
    getValue(node, "damage", (float&)d.damage);

    std::string updateSID = "";
    getValue(node, "updateScript", updateSID);
    std::string tickSID = "";
    getValue(node, "tickScript", tickSID);

    if(updateSID.length() > 0) {
        d.updateScriptID = ScriptQueue::addScript(updateSID);
    }
    if(tickSID.length() > 0) {
        d.tickScriptID = ScriptQueue::addScript(tickSID);
    }

    // At this point, the doc is empty, save for the few extra MetaData bits. Now we need to add those.
    getMetaData(node, d.defaultMD);

    return d;
}


/// Entities: Items

void XMLData::loadXMLEntityItemData(std::string filepath) {
    /** Loads all NPC entity data into the tileData map **/

    // Open file at filepath
    std::ifstream file;
    file.open(filepath);

    if(file.fail()) { // Handle exceptions
        Logger::getInstance()->log("ERROR: Entity (Item) data XML file unable to be loaded: " + filepath, true);
        return;
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
        m_entityItemData.insert(std::pair<unsigned int, XML_EntityItemData>(id, d));
    }
}

XML_EntityItemData XMLData::getEntityItemData(unsigned int id) {
    auto index = m_entityItemData.find(id);

    if(index == m_entityItemData.end()) {
        Logger::getInstance()->log("ERROR: Couldn't find entity (Item) data with ID: " + std::to_string(id), true);
        XML_EntityItemData t;
        return t;
    }

    return index->second;
}

XML_EntityItemData XMLData::readEntityItemData(rapidxml::xml_node<>* node) {
    XML_EntityItemData d;
    getValue(node, "texture", d.textureFilepath);
    d.textureFilepath = ASSETS_FOLDER_PATH + "/Textures/Entities/Items/" + d.textureFilepath;
    getValue(node, "bumpMap", d.bumpMapFilepath);
    d.bumpMapFilepath = ASSETS_FOLDER_PATH + "/Textures/BumpMaps/" + d.bumpMapFilepath;

    getValue(node, "sizeX", (float&)d.size.x);
    getValue(node, "sizeY", (float&)d.size.y);

    getValue(node, "item", (int&)d.itemID);

    std::string updateSID = "";
    getValue(node, "updateScript", updateSID);
    std::string tickSID = "";
    getValue(node, "tickScript", tickSID);

    if(updateSID.length() > 0) {
        d.updateScriptID = ScriptQueue::addScript(updateSID);
    }
    if(tickSID.length() > 0) {
        d.tickScriptID = ScriptQueue::addScript(tickSID);
    }

    // At this point, the doc is empty, save for the few extra MetaData bits. Now we need to add those.
    getMetaData(node, d.defaultMD);

    return d;
}


/// Real Items

void XMLData::loadXMLItemData(std::string filepath) {
    /** Loads all NPC entity data into the tileData map **/

    // Open file at filepath
    std::ifstream file;
    file.open(filepath);

    if(file.fail()) { // Handle exceptions
        Logger::getInstance()->log("ERROR: Item data XML file unable to be loaded: " + filepath, true);
        return;
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
}

XML_ItemData XMLData::getItemData(unsigned int id) {
    auto index = m_itemData.find(id);

    if(index == m_itemData.end()) {
        Logger::getInstance()->log("ERROR: Couldn't find item data with ID: " + std::to_string(id), true);
        XML_ItemData t;
        return t;
    }

    return index->second;
}

XML_ItemData XMLData::readItemData(rapidxml::xml_node<>* node) {
    XML_ItemData d;
    getValue(node, "texture", d.textureFilepath);
    d.textureFilepath = ASSETS_FOLDER_PATH + "/Textures/Items/" + d.textureFilepath;

    getValue(node, "weight", (float&)d.weight);

    std::string useScriptSID = "";
    getValue(node, "useScript", useScriptSID);

    if(useScriptSID.length() > 0) {
        d.useScriptID = ScriptQueue::addScript(useScriptSID);
    }

    // At this point, the doc is empty, save for the few extra MetaData bits. Now we need to add those.
    getMetaData(node, d.defaultMD);

    return d;
}

/// Biomes

void XMLData::loadXMLBiomeData(std::string filepath) {
    /** Loads all biome data into the biomeData map **/

    // Open file at filepath
    std::ifstream file;
    file.open(filepath);

    if(file.fail()) { // Handle exceptions
        Logger::getInstance()->log("ERROR: Biome data XML file unable to be loaded: " + filepath, true);
        return;
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
}

XML_BiomeData XMLData::getBiomeData(unsigned int id) {
    auto index = m_biomeData.find(id);

    if(index == m_biomeData.end()) {
        Logger::getInstance()->log("ERROR: Couldn't find biome data with ID: " + std::to_string(id), true);
        XML_BiomeData b;
        return b;
    }

    return index->second;
}

XML_BiomeData XMLData::readBiomeData(rapidxml::xml_node<>* node) {

    XML_BiomeData d;

    getValue(node, "baseHeight", d.baseHeight);
    getValue(node, "maxHeightDiff", d.maxHeightDiff);
    getValue(node, "maxTemperature", d.maxTemperature);
    getValue(node, "baseTemperature", d.baseTemperature);
    getValue(node, "flatness", d.flatness);
    getVector(node, "entities", "entityID", d.mobSpawnIds);
    if(getValue(node, "backgroundTexture", d.backgroundTexture)) {
        d.backgroundTexture = ASSETS_FOLDER_PATH + "Textures/" + d.backgroundTexture;
    }

    return d;
}

unsigned int XMLData::getTotalBiomes() {
    return m_biomeData.size();
}
