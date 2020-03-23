#include "XMLData.h"

void getValue(rapidxml::xml_node<>* parent, std::string valueName, std::string& variable) {
    rapidxml::xml_node<>* n = parent->first_node((char*)valueName.c_str());
    if(n) {
        variable = n->value();
    }
}

void getValue(rapidxml::xml_node<>* parent, std::string valueName, float& variable) {
    rapidxml::xml_node<>* n = parent->first_node((char*)valueName.c_str());
    if(n) {
        variable = std::stof(n->value());
    }
}

void getValue(rapidxml::xml_node<>* parent, std::string valueName, int& variable) {
    rapidxml::xml_node<>* n = parent->first_node((char*)valueName.c_str());
    if(n) {
        variable = std::stoi(n->value());
    }
}

void getValue(rapidxml::xml_node<>* parent, std::string valueName, bool& variable) {
    rapidxml::xml_node<>* n = parent->first_node((char*)valueName.c_str());
    if(n) {
        if(std::string(n->value()) == "0") {
            variable = false;
        } else {
            variable = true;
        }
    }
}

void XMLData::init() {
    loadXMLTileData();
    loadXMLParticleData();
}

std::map<unsigned int, XML_TileData> XMLData::m_tileData;
std::map<unsigned int, XML_ParticleData> XMLData::m_particleData;

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
        unsigned int id = std::stoi(node->first_attribute()->value());
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

    getValue(node, "sizeX", d.size.x);
    getValue(node, "sizeX", d.size.y);

    getValue(node, "isSolid", (bool&)d.solid);
    getValue(node, "isDrawn", (bool&)d.drawn);
    getValue(node, "isNatural", (bool&)d.natural);
    getValue(node, "isTransparent", (bool&)d.transparent);

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
        d.updateScriptID = ScriptQueue::addScript(ASSETS_FOLDER_PATH + "Scripts/" + updateSID);
    }
    if(tickSID.length() > 0) {
        d.tickScriptID = ScriptQueue::addScript(ASSETS_FOLDER_PATH + "Scripts/" + tickSID);
    }
    if(destructionSID.length() > 0) {
        d.destructionScriptID = ScriptQueue::addScript(ASSETS_FOLDER_PATH + "Scripts/" + destructionSID);
    }
    if(walkScriptID.length() > 0) {
        d.interactScriptID_walkedOn = ScriptQueue::addScript(ASSETS_FOLDER_PATH + "Scripts/" + walkScriptID);
    }
    if(RClickScriptID.length() > 0) {
        d.interactScriptID_used = ScriptQueue::addScript(ASSETS_FOLDER_PATH + "Scripts/" + RClickScriptID);
    }

    return d;
}

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
        unsigned int id = std::stoi(node->first_attribute()->value());
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

    getValue(node, "decayRate", d.decayRate);

    return d;
}
