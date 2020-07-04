#pragma once

#include <glm/glm.hpp>
#include <string>
#include <map>
#include <rapidxml/rapidxml.hpp>

#include "ScriptQueue.h"
#include "PresetValues.h"

#include "SaveDataTypes.h"


#include "XMLDataTypes.h"

namespace XMLModule {

	enum class XMLDataType {
		TILE,
		PARTICLE,
		ENTITY_NPC,
		ENTITY_PROJECTILE,
		ENTITY_ITEM,
		ITEM,
		BIOME,
		ERA,
		LOOT_DROP,
		LOOT_TABLE,
		STRUCTURE,
		QUEST,
		QUEST_OBJECTIVE,
		DIALOGUE_QUESTION,
		DIALOGUE_RESPONSE,
		ANIMATION
	};

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

	void getMetaData(rapidxml::xml_node<>* parent, SaveDataTypes::MetaData& mdVar);

	class XMLData {
		public:
			static void init();

			static TileData getTileData(unsigned int id);
			static ParticleData getParticleData(unsigned int id);
			static EntityNPCData getEntityNPCData(unsigned int id);
			static EntityProjectileData getEntityProjectileData(unsigned int id);
			static EntityItemData getEntityItemData(unsigned int id);
			static ItemData getItemData(unsigned int id);
			static BiomeData getBiomeData(unsigned int id);
			static EraData getEraData(unsigned int id);
			static LootDropData getLootDropData(unsigned int id);
			static LootTableData getLootTableData(unsigned int id);
			static StructureData getStructureData(unsigned int id);
			static QuestData getQuestData(unsigned int id);
			static QuestObjectiveData getQuestObjectiveData(unsigned int id);
			static DialogueQuestionData getDialogueQuestionData(unsigned int id);
			static DialogueResponseData getDialogueResponseData(unsigned int id);
			static AnimationData getAnimationData(unsigned int id);

			static unsigned int getTotalBiomes(); // Returns the number of biomes

		private:
			static void loadXMLData(std::string filepath, XMLDataType type); // Loads any valid map of GenericData children.

			static std::map<unsigned int, GenericData*> m_tileData;
			static std::map<unsigned int, GenericData*> m_particleData;
			static std::map<unsigned int, GenericData*> m_entityNPCData;
			static std::map<unsigned int, GenericData*> m_entityProjectileData;
			static std::map<unsigned int, GenericData*> m_entityItemData;
			static std::map<unsigned int, GenericData*> m_itemData;
			static std::map<unsigned int, GenericData*> m_biomeData;
			static std::map<unsigned int, GenericData*> m_eraData;
			static std::map<unsigned int, GenericData*> m_lootDropData;
			static std::map<unsigned int, GenericData*> m_lootTableData;
			static std::map<unsigned int, GenericData*> m_structureData;
			static std::map<unsigned int, GenericData*> m_questData;
			static std::map<unsigned int, GenericData*> m_questObjectiveData;
			static std::map<unsigned int, GenericData*> m_dialogueQuestionData;
			static std::map<unsigned int, GenericData*> m_dialogueResponseData;
			static std::map<unsigned int, GenericData*> m_animationData;
	};

}
