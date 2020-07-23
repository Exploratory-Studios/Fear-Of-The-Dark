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

	void getVector(rapidxml::xml_node<>* parent, std::string valueName, std::string childName, std::vector<glm::vec2>& vec) {
		rapidxml::xml_node<>* n = parent->first_node((char*)valueName.c_str()); // Gets <entities> in the example.

		if(n) {
			glm::vec2 temp;
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

	void getVector(rapidxml::xml_node<>* parent, std::string valueName, std::string childName, std::vector<GenericData>& vec) {
		rapidxml::xml_node<>* n = parent->first_node((char*)valueName.c_str()); // Gets <entities> in the example.

		if(n) {
			GenericData temp;
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

	bool getValue(rapidxml::xml_node<>* parent, std::string valueName, glm::vec2& variable) {
		/// Places value of node with name `valueName` into `variable` and removes the node from the doc.
		rapidxml::xml_node<>* n = parent->first_node((char*)valueName.c_str());
		if(n) {
			if(std::string(n->value()) != std::string("")) {
				float x = 0.0f, y = 0.0f;
				unsigned int separatorIndex = std::string(n->value()).find(",");

				x = std::stof(std::string(n->value()).substr(0, separatorIndex));
				y = std::stof(std::string(n->value()).substr(separatorIndex + 1));

				variable = glm::vec2(x, y);
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

	bool getValue(rapidxml::xml_node<>* parent, std::string valueName, GenericData& variable) {
		/// Places value of node with name `valueName` into `variable` and removes the node from the doc. Returns true on successful value find
		rapidxml::xml_node<>* n = parent->first_node((char*)valueName.c_str());
		if(n) {
			if(std::string(n->value()) != std::string("")) {
				variable.init(n);
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

	std::map<unsigned int, GenericData*> XMLData::m_tileData;
	std::map<unsigned int, GenericData*> XMLData::m_particleData;
	std::map<unsigned int, GenericData*> XMLData::m_entityData;
	std::map<unsigned int, GenericData*> XMLData::m_itemData;
	std::map<unsigned int, GenericData*> XMLData::m_biomeData;
	std::map<unsigned int, GenericData*> XMLData::m_eraData;
	std::map<unsigned int, GenericData*> XMLData::m_lootDropData;
	std::map<unsigned int, GenericData*> XMLData::m_lootTableData;
	std::map<unsigned int, GenericData*> XMLData::m_structureData;
	std::map<unsigned int, GenericData*> XMLData::m_questData;
	std::map<unsigned int, GenericData*> XMLData::m_questObjectiveData;
	std::map<unsigned int, GenericData*> XMLData::m_dialogueQuestionData;
	std::map<unsigned int, GenericData*> XMLData::m_dialogueResponseData;
	std::map<unsigned int, GenericData*> XMLData::m_animationData;
	std::map<unsigned int, GenericData*> XMLData::m_skeletalAnimationData;
	std::map<unsigned int, GenericData*> XMLData::m_attackData;
	std::map<unsigned int, GenericData*> XMLData::m_buffData;


	void XMLData::init(std::string filepath) {
		Logger::getInstance()->log("Beginning to load data...");

		std::vector<std::string> files{ "Blocks", "Particles", "Entities", "Items", "Biomes", "Eras", "Loot", "Structures", "Quests", "Dialogue", "Animations", "Attacks", "Buffs" };

		for(std::string& s : files) {
			loadXMLData(filepath + "/Data/" + s + ".xml");
			Logger::getInstance()->log("Loaded data (" + s + ")...");
		}

		Logger::getInstance()->log("Loaded all data successfully!");
	}

	void XMLData::write(std::string filepath) {
		Logger::getInstance()->log("Beginning to write data...");

		// files and nodeNames should have equal size. That much is assumed to be true at runtime
		std::vector<std::string> files   { "Blocks", "Blocks", "Particles", "Entities", "Entities", "Entities", "Items", "Items", "Items", "Biomes", "Eras", "Loot", "Loot", "Structures", "Quests", "Dialogue", "Dialogue", "Dialogue", "Animations", "Animations", "Attacks", "Attacks", "Attacks", "Buffs" };
		std::vector<std::string> nodeNames{ "tile", "tileContainer", "particle",  "npc",  "projectile",  "itemEntity",   "consumable", "armour", "weapon",  "biome",   "era", "lootDrop", "lootTable", "structure", "quest", "questObjective", "question", "response", "animation", "skeletalAnimation", "meleeAttack", "rangedAttack", "magicAttack", "buff" };

		for(unsigned int i = 0; i < files.size(); i++) {
			writeXMLData(filepath + "/Data/" + files[i] + ".xml", nodeNames[i]);
			Logger::getInstance()->log("Wrote data (" + files[i] + ": " + nodeNames[i] + ")...");
		}

		Logger::getInstance()->log("Wrote all data successfully!");
	}

	void XMLData::addData(GenericData* data, std::string& nodename) {
		// First, retrieve the correct node name and map.
		std::map<unsigned int, GenericData*>* mapForWrite = getMapFromNodename(nodename);

		if(mapForWrite) {
			// Actually add the data

			//mapForWrite.insert(std::pair<unsigned int, GenericData*>(data->id, data));

		} else {
			Logger::getInstance()->log("ERROR: Could not add data with node name: " + nodename + " to XML data singleton.", true);
		}
	}

	GenericData* XMLData::createDataFromNodename(std::string& name) {
		GenericData* d = nullptr;

		if(name == "tile") {
			d = new TileData();
		} else if(name == "tileContainer") {
			d = new TileContainerData();
		} else if(name == "particle") {
			d = new ParticleData();
		} else if(name == "npc") {
			d = new EntityNPCData();
		} else if(name == "projectile") {
			d = new EntityProjectileData();
		} else if(name == "itemEntity") {
			d = new EntityItemData();
		} else if(name == "item") {
			d = new ItemData();
		} else if(name == "biome") {
			d = new BiomeData();
		} else if(name == "era") {
			d = new EraData();
		} else if(name == "lootDrop") {
			d = new LootDropData();
		} else if(name == "lootTable") {
			d = new LootTableData();
		} else if(name == "structure") {
			d = new StructureData();
		} else if(name == "quest") {
			d = new QuestData();
		} else if(name == "objective") {
			d = new QuestObjectiveData();
		} else if(name == "question") {
			d = new DialogueQuestionData();
		} else if(name == "response") {
			d = new DialogueResponseData();
		} else if(name == "animation") {
			d = new AnimationData();
		} else if(name == "skeletalAnimation") {
			d = new SkeletalAnimationData();
		} else if(name == "meleeAttack") {
			d = new MeleeAttackData();
		} else if(name == "rangedAttack") {
			d = new RangedAttackData();
		} else if(name == "magicAttack") {
			d = new MagicAttackData();
		} else if(name == "buff") {
			d = new BuffData();
		} else {
			Logger::getInstance()->log("ERROR: Type '" + name + "' not supported!", true);
		}

		return d;
	}

	std::map<unsigned int, GenericData*>* XMLData::getMapFromNodename(std::string& name) {
		std::map<unsigned int, GenericData*>* mapForWrite = nullptr;

		if(name == "tile" || name == "tileContainer") {
			mapForWrite = &m_tileData;
		} else if(name == "particle") {
			mapForWrite = &m_particleData;
		} else if(name == "npc" || name == "projectile" || name == "itemEntity") {
			mapForWrite = &m_entityData;
		} else if(name == "item") {
			mapForWrite = &m_itemData;
		} else if(name == "biome") {
			mapForWrite = &m_biomeData;
		} else if(name == "era") {
			mapForWrite = &m_eraData;
		} else if(name == "lootDrop") {
			mapForWrite = &m_lootDropData;
		} else if(name == "lootTable") {
			mapForWrite = &m_lootTableData;
		} else if(name == "structure") {
			mapForWrite = &m_structureData;
		} else if(name == "quest") {
			mapForWrite = &m_questData;
		} else if(name == "objective") {
			mapForWrite = &m_questObjectiveData;
		} else if(name == "question") {
			mapForWrite = &m_dialogueQuestionData;
		} else if(name == "response") {
			mapForWrite = &m_dialogueResponseData;
		} else if(name == "animation") {
			mapForWrite = &m_animationData;
		} else if(name == "skeletalAnimation") {
			mapForWrite = &m_skeletalAnimationData;
		} else if(name == "meleeAttack" || name == "rangedAttack" || name == "magicAttack") {
			mapForWrite = &m_attackData;
		} else if(name == "buff") {
			mapForWrite = &m_buffData;
		} else {
			Logger::getInstance()->log("ERROR: Type '" + name + "' not supported!", true);
		}
		return mapForWrite;
	}

	void XMLData::loadXMLData(std::string filepath) {
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

		for(rapidxml::xml_node<>* node = doc.first_node(); node; node = node->next_sibling()) {
			if(node->type() != rapidxml::node_comment) {
				std::string name = node->name();

				mapForWrite = getMapFromNodename(name);

				GenericData* d = createDataFromNodename(name);
				d->init(node); // Actually do the read.

				unsigned int id = d->id;

				mapForWrite->insert(std::pair<unsigned int, GenericData*>(id, d));
			}
		}
	}

	void XMLData::writeXMLData(std::string filepath, std::string nodeName) {
		/** Writes all XML data into the file at filepath **/

		// Open file at filepath
		std::ofstream file;
		file.open(filepath);

		if(file.fail()) { // Handle exceptions
			Logger::getInstance()->log("ERROR: XML file unable to be opened or created: " + filepath, true);
			return;
		}

		// Figure out which map we're writing from
		std::map<unsigned int, GenericData*>* mapForWrite = getMapFromNodename(nodeName);

		// Create the document to write into the file.
		rapidxml::xml_document<> doc;
		char* name; // Allocate a string for the node type
		name = doc.allocate_string(nodeName.c_str());

		for(auto element : *mapForWrite) {
			rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_element, name);
			doc.append_node(node);

			element.second->write(node);
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

	TileContainerData XMLData::getTileContainerData(unsigned int id) {
		auto index = m_tileData.find(id);

		if(index == m_tileData.end()) {
			Logger::getInstance()->log("ERROR: Couldn't find tile (container) data with ID: " + std::to_string(id), true);
			TileContainerData t;
			return t;
		}

		return *static_cast<TileContainerData*>(index->second);
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

	EntityData XMLData::getEntityData(unsigned int id) {
		auto index = m_entityData.find(id);

		if(index == m_entityData.end()) {
			Logger::getInstance()->log("ERROR: Couldn't find entity data with ID: " + std::to_string(id), true);
			EntityData t;
			return t;
		}

		return *static_cast<EntityData*>(index->second);
	}

/// Entities: NPCS

	EntityNPCData XMLData::getEntityNPCData(unsigned int id) {
		auto index = m_entityData.find(id);

		if(index == m_entityData.end()) {
			Logger::getInstance()->log("ERROR: Couldn't find entity (NPC) data with ID: " + std::to_string(id), true);
			EntityNPCData t;
			return t;
		}

		return *static_cast<EntityNPCData*>(index->second);
	}

/// Entities: Projectiles

	EntityProjectileData XMLData::getEntityProjectileData(unsigned int id) {
		auto index = m_entityData.find(id);

		if(index == m_entityData.end()) {
			Logger::getInstance()->log("ERROR: Couldn't find entity (Projectile) data with ID: " + std::to_string(id), true);
			EntityProjectileData t;
			return t;
		}

		return *static_cast<EntityProjectileData*>(index->second);
	}

/// Entities: Items

	EntityItemData XMLData::getEntityItemData(unsigned int id) {
		auto index = m_entityData.find(id);

		if(index == m_entityData.end()) {
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

	ItemArmourData XMLData::getItemArmourData(unsigned int id) {
		auto index = m_itemData.find(id);

		if(index == m_itemData.end()) {
			Logger::getInstance()->log("ERROR: Couldn't find item armour data with ID: " + std::to_string(id), true);
			ItemArmourData t;
			return t;
		}

		return *static_cast<ItemArmourData*>(index->second);
	}

	ItemConsumableData XMLData::getItemConsumableData(unsigned int id) {
		auto index = m_itemData.find(id);

		if(index == m_itemData.end()) {
			Logger::getInstance()->log("ERROR: Couldn't find item consumable data with ID: " + std::to_string(id), true);
			ItemConsumableData t;
			return t;
		}

		return *static_cast<ItemConsumableData*>(index->second);
	}

	ItemWeaponData XMLData::getItemWeaponData(unsigned int id) {
		auto index = m_itemData.find(id);

		if(index == m_itemData.end()) {
			Logger::getInstance()->log("ERROR: Couldn't find item weapon data with ID: " + std::to_string(id), true);
			ItemWeaponData t;
			return t;
		}

		return *static_cast<ItemWeaponData*>(index->second);
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

/// DialogueQuestions

	DialogueQuestionData XMLData::getDialogueQuestionData(unsigned int id) {
		auto index = m_dialogueQuestionData.find(id);

		if(index == m_dialogueQuestionData.end()) {
			Logger::getInstance()->log("ERROR: Couldn't find dialogue question data with ID: " + std::to_string(id), true);
			DialogueQuestionData s;
			return s;
		}

		return *static_cast<DialogueQuestionData*>(index->second);
	}

/// DialogueResponses

	DialogueResponseData XMLData::getDialogueResponseData(unsigned int id) {
		auto index = m_dialogueResponseData.find(id);

		if(index == m_dialogueResponseData.end()) {
			Logger::getInstance()->log("ERROR: Couldn't find dialogue response data with ID: " + std::to_string(id), true);
			DialogueResponseData s;
			return s;
		}

		return *static_cast<DialogueResponseData*>(index->second);
	}

/// Animations

	AnimationData XMLData::getAnimationData(unsigned int id) {
		auto index = m_animationData.find(id);

		if(index == m_animationData.end()) {
			Logger::getInstance()->log("ERROR: Couldn't find animation data with ID: " + std::to_string(id), true);
			AnimationData s;
			return s;
		}

		return *static_cast<AnimationData*>(index->second);
	}

/// Skeletal Animations

	SkeletalAnimationData XMLData::getSkeletalAnimationData(unsigned int id) {
		auto index = m_skeletalAnimationData.find(id);

		if(index == m_skeletalAnimationData.end()) {
			Logger::getInstance()->log("ERROR: Couldn't find skeletal animation data with ID: " + std::to_string(id), true);
			SkeletalAnimationData s;
			return s;
		}

		return *static_cast<SkeletalAnimationData*>(index->second);
	}

/// Attack

	AttackData XMLData::getAttackData(unsigned int id) {
		auto index = m_attackData.find(id);

		if(index == m_attackData.end()) {
			Logger::getInstance()->log("ERROR: Couldn't find attack data with ID: " + std::to_string(id), true);
			AttackData s;
			return s;
		}

		return *static_cast<AttackData*>(index->second);
	}

	MeleeAttackData XMLData::getMeleeAttackData(unsigned int id) {
		auto index = m_attackData.find(id);

		if(index == m_attackData.end()) {
			Logger::getInstance()->log("ERROR: Couldn't find attack data with ID: " + std::to_string(id), true);
			MeleeAttackData s;
			return s;
		}

		return *static_cast<MeleeAttackData*>(index->second);
	}

	RangedAttackData XMLData::getRangedAttackData(unsigned int id) {
		auto index = m_attackData.find(id);

		if(index == m_attackData.end()) {
			Logger::getInstance()->log("ERROR: Couldn't find attack data with ID: " + std::to_string(id), true);
			RangedAttackData s;
			return s;
		}

		return *static_cast<RangedAttackData*>(index->second);
	}

	MagicAttackData XMLData::getMagicAttackData(unsigned int id) {
		auto index = m_attackData.find(id);

		if(index == m_attackData.end()) {
			Logger::getInstance()->log("ERROR: Couldn't find attack data with ID: " + std::to_string(id), true);
			MagicAttackData s;
			return s;
		}

		return *static_cast<MagicAttackData*>(index->second);
	}

	BuffData XMLData::getBuffData(unsigned int id) {
		auto index = m_buffData.find(id);

		if(index == m_buffData.end()) {
			Logger::getInstance()->log("ERROR: Couldn't find buff data with ID: " + std::to_string(id), true);
			BuffData s;
			return s;
		}

		return *static_cast<BuffData*>(index->second);
	}

}
