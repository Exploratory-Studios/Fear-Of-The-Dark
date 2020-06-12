#include "XMLDataTypes.h"

#include "XMLData.h"

XML_Object::XML_Object(unsigned int id, std::string name, XMLDataType type) : id(id), name(name), m_XMLtype(type) {

}

XMLDataType XML_Object::getType() {
    return m_XMLtype;
}

/// Tiles

XML_TileData::XML_TileData(unsigned int id, std::string name) : XML_Object(id, name, XMLDataType::TILE) {

}

void XML_TileData::read(rapidxml::xml_node<>* node) {
    getValue(node, "texture", textureFilepath);
    textureFilepath = ASSETS_FOLDER_PATH + "/Textures/Blocks/" + textureFilepath;
    getValue(node, "bumpMap", bumpMapFilepath);
    bumpMapFilepath = ASSETS_FOLDER_PATH + "/Textures/BumpMaps/" + bumpMapFilepath;

    getValue(node, "emittedLight", emittedLight);
    getValue(node, "emittedHeat", emittedHeat);

    getValue(node, "sizeX", (float&)size.x);
    getValue(node, "sizeY", (float&)size.y);

    getValue(node, "isSolid", (bool&)solid);
    getValue(node, "isDrawn", (bool&)drawn);
    getValue(node, "isNatural", (bool&)natural);
    getValue(node, "isTransparent", (bool&)transparent);

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
        updateScriptID = ScriptingModule::ScriptQueue::addScript(updateSID);
    }
    if(tickSID.length() > 0) {
        tickScriptID = ScriptingModule::ScriptQueue::addScript(tickSID);
    }
    if(destructionSID.length() > 0) {
        destructionScriptID = ScriptingModule::ScriptQueue::addScript(destructionSID);
    }
    if(walkScriptID.length() > 0) {
        interactScriptID_walkedOn = ScriptingModule::ScriptQueue::addScript(walkScriptID);
    }
    if(RClickScriptID.length() > 0) {
        interactScriptID_used = ScriptingModule::ScriptQueue::addScript(RClickScriptID);
    }

    // At this point, the doc is empty, save for the few extra MetaData bits. Now we need to add those.
    getMetaData(node, defaultMD);
}

void XML_TileData::write(rapidxml::xml_node<>* node) {

}

/// Particles

XML_ParticleData::XML_ParticleData(unsigned int id, std::string name) : XML_Object(id, name, XMLDataType::PARTICLE) {

}

void XML_ParticleData::read(rapidxml::xml_node<>* node) {
    getValue(node, "texture", textureFilepath);
    textureFilepath = ASSETS_FOLDER_PATH + "/Textures/Particles/" + textureFilepath;
    getValue(node, "bumpMap", bumpMapFilepath);
    bumpMapFilepath = ASSETS_FOLDER_PATH + "/Textures/BumpMaps/" + bumpMapFilepath;
    getValue(node, "script", scriptPath);
    scriptPath = ASSETS_FOLDER_PATH + "/Scripts/" + scriptPath;

    getValue(node, "decayRate", decayRate);
}

void XML_ParticleData::write(rapidxml::xml_node<>* node) {

}

/// Entities (Base class)

XML_EntityData::XML_EntityData(unsigned int id, std::string name, XMLDataType type) : XML_Object(id, name, type) {

}

void XML_EntityData::read(rapidxml::xml_node<>* node) {
    getValue(node, "texture", textureFilepath);
    textureFilepath = ASSETS_FOLDER_PATH + "/Textures/Entities/Items/" + textureFilepath;
    getValue(node, "bumpMap", bumpMapFilepath);
    bumpMapFilepath = ASSETS_FOLDER_PATH + "/Textures/BumpMaps/" + bumpMapFilepath;

    getValue(node, "sizeX", (float&)size.x);
    getValue(node, "sizeY", (float&)size.y);

    std::string updateSID = "";
    getValue(node, "updateScript", updateSID);
    std::string tickSID = "";
    getValue(node, "tickScript", tickSID);

    if(updateSID.length() > 0) {
        updateScriptID = ScriptingModule::ScriptQueue::addScript(updateSID);
    }
    if(tickSID.length() > 0) {
        tickScriptID = ScriptingModule::ScriptQueue::addScript(tickSID);
    }

    // At this point, the doc is empty, save for the few extra MetaData bits. Now we need to add those.
    getMetaData(node, defaultMD);
}

void XML_EntityData::write(rapidxml::xml_node<>* node) {

}

/// Entity_NPCs

XML_EntityNPCData::XML_EntityNPCData(unsigned int id, std::string name) : XML_EntityData(id, name, XMLDataType::ENTITY_NPC) {

}

void XML_EntityNPCData::read(rapidxml::xml_node<>* node) {
    XML_EntityData::read(node);

    getValue(node, "isDamagedByFalls", (bool&)fallDamage);
    getValue(node, "isInvincible", (bool&)canDie);
    canDie = !canDie; // just for readability's sake

    getValue(node, "speed", (float&)speed);
    getValue(node, "jumpHeight", (float&)jumpHeight);

    jumpHeight = std::sqrt(jumpHeight * 2.0f * (1.225f / 60.0f));

    getValue(node, "maxHealth", (float&)maxHealth);

    std::string factionString = "";
    getValue(node, "faction", (std::string&)factionString);

    if(factionString == "evil") {
        faction = Categories::Faction::EVIL;
    } else if(factionString == "bad") {
        faction = Categories::Faction::BAD;
    } else if(factionString == "neutral") {
        faction = Categories::Faction::NEUTRAL;
    } else if(factionString == "good") {
        faction = Categories::Faction::GOOD;
    } else if(factionString == "benign") {
        faction = Categories::Faction::BENIGN;
    } else {
        faction = Categories::Faction::NEUTRAL;
    }
}

void XML_EntityNPCData::write(rapidxml::xml_node<>* node) {

}

/// Entity_Projectiles

XML_EntityProjectileData::XML_EntityProjectileData(unsigned int id, std::string name) : XML_EntityData(id, name, XMLDataType::ENTITY_PROJECTILE) {

}

void XML_EntityProjectileData::read(rapidxml::xml_node<>* node) {
    XML_EntityData::read(node);

    getValue(node, "collides", (bool&)collides);

    getValue(node, "speed", (float&)speed);
    getValue(node, "damage", (float&)damage);
}

void XML_EntityProjectileData::write(rapidxml::xml_node<>* node) {

}

/// Entity_Items

XML_EntityItemData::XML_EntityItemData(unsigned int id, std::string name) : XML_EntityData(id, name, XMLDataType::ENTITY_ITEM) {

}

void XML_EntityItemData::read(rapidxml::xml_node<>* node) {
    XML_EntityData::read(node);

    getValue(node, "item", (int&)itemID);
}

void XML_EntityItemData::write(rapidxml::xml_node<>* node) {

}

/// Items

XML_ItemData::XML_ItemData(unsigned int id, std::string name) : XML_Object(id, name, XMLDataType::ITEM) {

}

void XML_ItemData::read(rapidxml::xml_node<>* node) {
    getValue(node, "texture", textureFilepath);
    textureFilepath = ASSETS_FOLDER_PATH + "/Textures/Items/" + textureFilepath;

    getValue(node, "weight", (float&)weight);

    std::string useScriptSID = "";
    getValue(node, "useScript", useScriptSID);

    if(useScriptSID.length() > 0) {
        useScriptID = ScriptingModule::ScriptQueue::addScript(useScriptSID);
    }

    getMetaData(node, defaultMD);
}

void XML_ItemData::write(rapidxml::xml_node<>* node) {

}

/// Biomes

XML_BiomeData::XML_BiomeData(unsigned int id, std::string name) : XML_Object(id, name, XMLDataType::BIOME) {

}

void XML_BiomeData::read(rapidxml::xml_node<>* node) {
    getValue(node, "baseHeight", baseHeight);
    getValue(node, "maxHeightDiff", maxHeightDiff);
    getValue(node, "maxTemperature", maxTemperature);
    getValue(node, "baseTemperature", baseTemperature);
    getValue(node, "flatness", flatness);
    getVector(node, "entities", "entityID", mobSpawnIds);
    if(getValue(node, "backgroundTexture", backgroundTexture)) {
        backgroundTexture = ASSETS_FOLDER_PATH + "Textures/BiomeBackgrounds/" + backgroundTexture;
    }
}

void XML_BiomeData::write(rapidxml::xml_node<>* node) {

}

/// Eras

XML_EraData::XML_EraData(unsigned int id, std::string name) : XML_Object(id, name, XMLDataType::ERA) {

}

void XML_EraData::read(rapidxml::xml_node<>* node) {
    getVector(node, "biomes", "biomeID", biomeIds);
}

void XML_EraData::write(rapidxml::xml_node<>* node) {

}

/// Loot_Drops

XML_LootDrop::XML_LootDrop(unsigned int id, std::string name) : XML_Object(id, name, XMLDataType::LOOT_DROP) {

}

void XML_LootDrop::read(rapidxml::xml_node<>* node) {
    getValue(node, "itemID", itemID);
    getValue(node, "minDrop", minDrop);
    getValue(node, "maxDrop", maxDrop);
    getValue(node, "chance", chance);
}

void XML_LootDrop::write(rapidxml::xml_node<>* node) {

}

/// Loot_Tables

XML_LootTable::XML_LootTable(unsigned int id, std::string name) : XML_Object(id, name, XMLDataType::LOOT_TABLE) {

}

void XML_LootTable::read(rapidxml::xml_node<>* node) {
    std::vector<unsigned int> ids;
    getVector(node, "drops", "dropID", ids);

    for(unsigned int i = 0; i < ids.size(); i++) {
        XML_LootDrop* drop = nullptr;
        XMLData::getData(XMLDataType::LOOT_DROP, ids[i], drop);
        drops.push_back(*drop);
    }
}

void XML_LootTable::write(rapidxml::xml_node<>* node) {

}

/// Structures

XML_StructureData::XML_StructureData(unsigned int id, std::string name) : XML_Object(id, name, XMLDataType::STRUCTURE) {

}

void XML_StructureData::read(rapidxml::xml_node<>* node) {
    getValue(node, "structureID", structureID);
    getValue(node, "biomeID", biomeID);
    getValue(node, "chance", chance);
    getValue(node, "maxAmnt", maxAmnt);
    getValue(node, "minAmnt", minAmnt);
}

void XML_StructureData::write(rapidxml::xml_node<>* node) {

}

/// Quests

XML_QuestData::XML_QuestData(unsigned int id, std::string name) : XML_Object(id, name, XMLDataType::QUEST) {

}

void XML_QuestData::read(rapidxml::xml_node<>* node) {
    getValue(node, "completionScript", completionScript);
    getValue(node, "scriptIsFile", scriptIsFile);

    // get objective IDs
    std::vector<unsigned int> ids;
    getVector(node, "objectives", "objectiveID", ids);

    for(unsigned int i = 0; i < ids.size(); i++) {
        XML_QuestObjectiveData* objective = nullptr;
        XMLData::getData(XMLDataType::QUEST_OBJECTIVE, ids[i], objective);
        objectives.push_back(*objective);
    }
}

void XML_QuestData::write(rapidxml::xml_node<>* node) {

}

/// Quest_Objectives

XML_QuestObjectiveData::XML_QuestObjectiveData(unsigned int id, std::string name) : XML_Object(id, name, XMLDataType::QUEST_OBJECTIVE) {

}

void XML_QuestObjectiveData::read(rapidxml::xml_node<>* node) {
    getValue(node, "text", text);
    getValue(node, "confirmationScript", confirmationScript);
    getValue(node, "scriptIsFile", scriptIsFile);
}

void XML_QuestObjectiveData::write(rapidxml::xml_node<>* node) {

}

