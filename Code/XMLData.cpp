#include "XMLData.h"

namespace XMLModule {

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

    void getMetaData(rapidxml::xml_node<>* parent, SaveDataTypes::MetaData& mdVar) {
        /// Retrieves ALL existing children from `parent`, then sets their names as the keys, with their values as the values in a MetaData object, stored in `mdVar`. Removes each node systematically from parent after adding them to mdVar.

        SaveDataTypes::MetaData md;

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

        loadXMLData(ASSETS_FOLDER_PATH + "/Data/Blocks.xml", XMLDataType::TILE);
        Logger::getInstance()->log("Loaded data (Tiles)...");

        loadXMLData(ASSETS_FOLDER_PATH + "/Data/Particles.xml", XMLDataType::PARTICLE);
        Logger::getInstance()->log("Loaded data (Particles)...");

        loadXMLData(ASSETS_FOLDER_PATH + "/Data/NPCs.xml", XMLDataType::ENTITY_NPC);
        Logger::getInstance()->log("Loaded data (NPC Entities)...");

        loadXMLData(ASSETS_FOLDER_PATH + "/Data/Projectiles.xml", XMLDataType::ENTITY_PROJECTILE);
        Logger::getInstance()->log("Loaded data (Projectile Entities)...");

        loadXMLData(ASSETS_FOLDER_PATH + "/Data/ItemEntities.xml", XMLDataType::ENTITY_ITEM);
        Logger::getInstance()->log("Loaded data (Item Entities)...");

        loadXMLData(ASSETS_FOLDER_PATH + "/Data/Items.xml", XMLDataType::ITEM);
        Logger::getInstance()->log("Loaded data (Items)...");

        loadXMLData(ASSETS_FOLDER_PATH + "/Data/Biomes.xml", XMLDataType::BIOME);
        Logger::getInstance()->log("Loaded data (Biomes)...");

        loadXMLData(ASSETS_FOLDER_PATH + "/Data/Eras.xml", XMLDataType::ERA);
        Logger::getInstance()->log("Loaded data (Eras)...");

        loadXMLData(ASSETS_FOLDER_PATH + "/Data/Loot.xml", XMLDataType::LOOT_DROP);
        Logger::getInstance()->log("Loaded data (Loot Drops)...");

        loadXMLData(ASSETS_FOLDER_PATH + "/Data/Loot.xml", XMLDataType::LOOT_TABLE);
        Logger::getInstance()->log("Loaded data (Loot Tables)...");

        loadXMLData(ASSETS_FOLDER_PATH + "/Data/Structures.xml", XMLDataType::STRUCTURE);
        Logger::getInstance()->log("Loaded data (Structures)...");

        loadXMLData(ASSETS_FOLDER_PATH + "/Data/Quests.xml", XMLDataType::QUEST_OBJECTIVE);
        Logger::getInstance()->log("Loaded data (Quest Objectives)...");

        loadXMLData(ASSETS_FOLDER_PATH + "/Data/Quests.xml", XMLDataType::QUEST);
        Logger::getInstance()->log("Loaded data (Quests)...");

        Logger::getInstance()->log("Loaded all data successfully!");
    }

    std::map<unsigned int, GenericData*> XMLData::m_tileData;
    std::map<unsigned int, GenericData*> XMLData::m_particleData;
    std::map<unsigned int, GenericData*> XMLData::m_entityNPCData;
    std::map<unsigned int, GenericData*> XMLData::m_entityProjectileData;
    std::map<unsigned int, GenericData*> XMLData::m_entityItemData;
    std::map<unsigned int, GenericData*> XMLData::m_itemData;
    std::map<unsigned int, GenericData*> XMLData::m_biomeData;
    std::map<unsigned int, GenericData*> XMLData::m_eraData;
    std::map<unsigned int, GenericData*> XMLData::m_lootDropData;
    std::map<unsigned int, GenericData*> XMLData::m_lootTableData;
    std::map<unsigned int, GenericData*> XMLData::m_structureData;
    std::map<unsigned int, GenericData*> XMLData::m_questData;
    std::map<unsigned int, GenericData*> XMLData::m_questObjectiveData;

    void XMLData::loadXMLData(std::string filepath, XMLDataType type) {
        /** Loads all XML data into the map **/

        // Open file at filepath
        std::ifstream file;
        file.open(filepath);

        if(file.fail()) { // Handle exceptions
            Logger::getInstance()->log("ERROR: XML file unable to be loaded: " + filepath, true);
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

        std::map<unsigned int, GenericData*>* mapForWrite = nullptr;
        std::string nodeName;

        switch((unsigned int)type) {
            case (unsigned int)XMLDataType::TILE: {
                mapForWrite = &m_tileData;
                nodeName = "tile";
                break;
            }
            case (unsigned int)XMLDataType::PARTICLE: {
                mapForWrite = &m_particleData;
                nodeName = "particle";
                break;
            }
            case (unsigned int)XMLDataType::ENTITY_NPC: {
                mapForWrite = &m_entityNPCData;
                nodeName = "npc";
                break;
            }
            case (unsigned int)XMLDataType::ENTITY_PROJECTILE: {
                mapForWrite = &m_entityProjectileData;
                nodeName = "projectile";
                break;
            }
            case (unsigned int)XMLDataType::ENTITY_ITEM: {
                mapForWrite = &m_entityItemData;
                nodeName = "itemEntity";
                break;
            }
            case (unsigned int)XMLDataType::ITEM: {
                mapForWrite = &m_itemData;
                nodeName = "item";
                break;
            }
            case (unsigned int)XMLDataType::BIOME: {
                mapForWrite = &m_biomeData;
                nodeName = "biome";
                break;
            }
            case (unsigned int)XMLDataType::ERA: {
                mapForWrite = &m_eraData;
                nodeName = "era";
                break;
            }
            case (unsigned int)XMLDataType::LOOT_DROP: {
                mapForWrite = &m_lootDropData;
                nodeName = "lootDrop";
                break;
            }
            case (unsigned int)XMLDataType::LOOT_TABLE: {
                mapForWrite = &m_lootTableData;
                nodeName = "lootTable";
                break;
            }
            case (unsigned int)XMLDataType::STRUCTURE: {
                mapForWrite = &m_structureData;
                nodeName = "structure";
                break;
            }
            case (unsigned int)XMLDataType::QUEST: {
                mapForWrite = &m_questData;
                nodeName = "quest";
                break;
            }
            case (unsigned int)XMLDataType::QUEST_OBJECTIVE: {
                mapForWrite = &m_questObjectiveData;
                nodeName = "objective";
                break;
            }
            default: {
                Logger::getInstance()->log("ERROR: Type not supported!", true);
                break;
            }
        }

        for(rapidxml::xml_node<>* node = doc.first_node((const char*)nodeName.c_str()); node; node = node->next_sibling((const char*)nodeName.c_str())) {
            GenericData* d = nullptr;

            switch((unsigned int)type) {
                case (unsigned int)XMLDataType::TILE: {
                    d = new TileData();
                    break;
                }
                case (unsigned int)XMLDataType::PARTICLE: {
                    d = new ParticleData();
                    break;
                }
                case (unsigned int)XMLDataType::ENTITY_NPC: {
                    d = new EntityNPCData();
                    break;
                }
                case (unsigned int)XMLDataType::ENTITY_PROJECTILE: {
                    d = new EntityProjectileData();
                    break;
                }
                case (unsigned int)XMLDataType::ENTITY_ITEM: {
                    d = new EntityItemData();
                    break;
                }
                case (unsigned int)XMLDataType::ITEM: {
                    d = new ItemData();
                    break;
                }
                case (unsigned int)XMLDataType::BIOME: {
                    d = new BiomeData();
                    break;
                }
                case (unsigned int)XMLDataType::ERA: {
                    d = new EraData();
                    break;
                }
                case (unsigned int)XMLDataType::LOOT_DROP: {
                    d = new LootDropData();
                    break;
                }
                case (unsigned int)XMLDataType::LOOT_TABLE: {
                    d = new LootTableData();
                    break;
                }
                case (unsigned int)XMLDataType::STRUCTURE: {
                    d = new StructureData();
                    break;
                }
                case (unsigned int)XMLDataType::QUEST: {
                    d = new QuestData();
                    break;
                }
                case (unsigned int)XMLDataType::QUEST_OBJECTIVE: {
                    d = new QuestObjectiveData();
                    break;
                }
                default: {
                    Logger::getInstance()->log("ERROR: Type not supported!", true);
                    break;
                }
            }

            d->init(node); // Actually do the read.

            unsigned int id;
            d->getAttribute("id", id);

            mapForWrite->insert(std::pair<unsigned int, GenericData*>(id, d));
        }
    }

    /// Tiles

    TileData XMLData::getTileData(unsigned int id) {
        auto index = m_tileData.find(id);

        if(index == m_tileData.end()) {
            Logger::getInstance()->log("ERROR: Couldn't find tile data with ID: " + std::to_string(id), true);
            TileData t;
            return t;
        }

        return *static_cast<TileData*>(index->second);
    }

    /// Particles

    ParticleData XMLData::getParticleData(unsigned int id) {
        auto index = m_particleData.find(id);

        if(index == m_particleData.end()) {
            Logger::getInstance()->log("ERROR: Couldn't find particle data with ID: " + std::to_string(id), true);
            ParticleData t;
            return t;
        }

        return *static_cast<ParticleData*>(index->second);
    }

    /// Entities: NPCS

    EntityNPCData XMLData::getEntityNPCData(unsigned int id) {
        auto index = m_entityNPCData.find(id);

        if(index == m_entityNPCData.end()) {
            Logger::getInstance()->log("ERROR: Couldn't find entity (NPC) data with ID: " + std::to_string(id), true);
            EntityNPCData t;
            return t;
        }

        return *static_cast<EntityNPCData*>(index->second);
    }

    /// Entities: Projectiles

    EntityProjectileData XMLData::getEntityProjectileData(unsigned int id) {
        auto index = m_entityProjectileData.find(id);

        if(index == m_entityProjectileData.end()) {
            Logger::getInstance()->log("ERROR: Couldn't find entity (Projectile) data with ID: " + std::to_string(id), true);
            EntityProjectileData t;
            return t;
        }

        return *static_cast<EntityProjectileData*>(index->second);
    }

    /// Entities: Items

    EntityItemData XMLData::getEntityItemData(unsigned int id) {
        auto index = m_entityItemData.find(id);

        if(index == m_entityItemData.end()) {
            Logger::getInstance()->log("ERROR: Couldn't find entity (Item) data with ID: " + std::to_string(id), true);
            EntityItemData t;
            return t;
        }

        return *static_cast<EntityItemData*>(index->second);
    }


    /// Real Items

    ItemData XMLData::getItemData(unsigned int id) {
        auto index = m_itemData.find(id);

        if(index == m_itemData.end()) {
            Logger::getInstance()->log("ERROR: Couldn't find item data with ID: " + std::to_string(id), true);
            ItemData t;
            return t;
        }

        return *static_cast<ItemData*>(index->second);
    }

    /// Biomes

    BiomeData XMLData::getBiomeData(unsigned int id) {
        auto index = m_biomeData.find(id);

        if(index == m_biomeData.end()) {
            Logger::getInstance()->log("ERROR: Couldn't find biome data with ID: " + std::to_string(id), true);
            BiomeData b;
            return b;
        }

        return *static_cast<BiomeData*>(index->second);
    }

    unsigned int XMLData::getTotalBiomes() {
        return m_biomeData.size();
    }

    /// Eras

    EraData XMLData::getEraData(unsigned int id) {
        auto index = m_eraData.find(id);

        if(index == m_eraData.end()) {
            Logger::getInstance()->log("ERROR: Couldn't find era data with ID: " + std::to_string(id), true);
            EraData e;
            return e;
        }

        return *static_cast<EraData*>(index->second);
    }

    /// LootDrops

    LootDropData XMLData::getLootDropData(unsigned int id) {
        auto index = m_lootDropData.find(id);

        if(index == m_lootDropData.end()) {
            Logger::getInstance()->log("ERROR: Couldn't find loot drop data with ID: " + std::to_string(id), true);
            LootDropData d;
            return d;
        }

        return *static_cast<LootDropData*>(index->second);
    }

    /// LootTables

    LootTableData XMLData::getLootTableData(unsigned int id) {
        auto index = m_lootTableData.find(id);

        if(index == m_lootTableData.end()) {
            Logger::getInstance()->log("ERROR: Couldn't find loot drop data with ID: " + std::to_string(id), true);
            LootTableData d;
            return d;
        }

        return *static_cast<LootTableData*>(index->second);
    }

    /// Structures

    StructureData XMLData::getStructureData(unsigned int id) {
        auto index = m_structureData.find(id);

        if(index == m_structureData.end()) {
            Logger::getInstance()->log("ERROR: Couldn't find structure data with ID: " + std::to_string(id), true);
            StructureData s;
            return s;
        }

        return *static_cast<StructureData*>(index->second);
    }

    /// Quests

    QuestData XMLData::getQuestData(unsigned int id) {
        auto index = m_questData.find(id);

        if(index == m_questData.end()) {
            Logger::getInstance()->log("ERROR: Couldn't find quest data with ID: " + std::to_string(id), true);
            QuestData s;
            return s;
        }

        return *static_cast<QuestData*>(index->second);
    }

    /// QuestObjectives

    QuestObjectiveData XMLData::getQuestObjectiveData(unsigned int id) {
        auto index = m_questObjectiveData.find(id);

        if(index == m_questObjectiveData.end()) {
            Logger::getInstance()->log("ERROR: Couldn't find quest objective data with ID: " + std::to_string(id), true);
            QuestObjectiveData s;
            return s;
        }

        return *static_cast<QuestObjectiveData*>(index->second);
    }

}

