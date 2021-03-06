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

	void getVector(rapidxml::xml_node<>*	 parent,
				   std::string				 valueName,
				   std::string				 childName,
				   std::vector<std::string>& vec); // Retrieves a string vector from a node
	void getVector(rapidxml::xml_node<>* parent,
				   std::string			 valueName,
				   std::string			 childName,
				   std::vector<float>&	 vec); // Retrieves a float vector from a node
	void getVector(rapidxml::xml_node<>* parent,
				   std::string			 valueName,
				   std::string			 childName,
				   std::vector<int>&	 vec); // Retrieves an int vector from a node
	void getVector(rapidxml::xml_node<>*	  parent,
				   std::string				  valueName,
				   std::string				  childName,
				   std::vector<unsigned int>& vec); // Retrieves an unsigned int vector from a node
	void getVector(rapidxml::xml_node<>*   parent,
				   std::string			   valueName,
				   std::string			   childName,
				   std::vector<glm::vec2>& vec); // Retrieves an unsigned int vector from a node
	void getVector(rapidxml::xml_node<>* parent,
				   std::string			 valueName,
				   std::string			 childName,
				   std::vector<bool>&	 vec); // Retrieves a bool vector from a node
	void getVector(rapidxml::xml_node<>*	parent,
				   std::string				valueName,
				   std::string				childName,
				   std::vector<ScriptData>& vec); // Retrieves a bool vector from a node
	void getVector(rapidxml::xml_node<>*	 parent,
				   std::string				 valueName,
				   std::string				 childName,
				   std::vector<GenericData>& vec); // Retrieves a bool vector from a node

	bool getValue(rapidxml::xml_node<>* parent, std::string valueName, std::string& variable);
	bool getValue(rapidxml::xml_node<>* parent, std::string valueName, float& variable);
	bool getValue(rapidxml::xml_node<>* parent, std::string valueName, int& variable);
	bool getValue(rapidxml::xml_node<>* parent, std::string valueName, unsigned int& variable);
	bool getValue(rapidxml::xml_node<>* parent, std::string valueName, glm::vec2& variable);
	bool getValue(rapidxml::xml_node<>* parent, std::string valueName, bool& variable);
	bool getValue(rapidxml::xml_node<>* parent, std::string valueName, ScriptData& variable);
	bool getValue(rapidxml::xml_node<>* parent, std::string valueName, GenericData& variable);

	void getMetaData(rapidxml::xml_node<>* parent, SaveDataTypes::MetaData& mdVar);

	struct XMLDataFile {
		// Literally just a wrapper to put a map and filename together.
		XMLDataFile(std::string filename, std::initializer_list<std::string> nodeNames);
		std::string										   m_filename;
		std::vector<std::string>						   m_nodeNames;
		std::vector<std::map<unsigned int, GenericData*>*> m_maps;
	};

	class XMLData {
	  public:
		static void init(std::string filepath = ASSETS_FOLDER_PATH);
		static void write(std::string filepath);

		static void addData(GenericData* data, std::string& nodename);	  // Careful, this isn't for regular use.
		static void removeData(GenericData* data, std::string& nodename); // Nor is this

		static GenericData*							 createDataFromNodename(std::string& nodename);
		static std::map<unsigned int, GenericData*>* getMapFromNodename(std::string& nodename);

		static TileData			 getTileData(unsigned int id);
		static TileContainerData getTileContainerData(unsigned int id);

		static ParticleData getParticleData(unsigned int id);

		static EntityData			getEntityData(unsigned int id);
		static EntityNPCData		getEntityNPCData(unsigned int id);
		static EntityProjectileData getEntityProjectileData(unsigned int id);
		static EntityItemData		getEntityItemData(unsigned int id);

		static ItemData			  getItemData(unsigned int id);
		static ItemArmourData	  getItemArmourData(unsigned int id);
		static ItemConsumableData getItemConsumableData(unsigned int id);
		static ItemWeaponData	  getItemWeaponData(unsigned int id);
		static ItemBlockData	  getItemBlockData(unsigned int id);

		static BiomeData getBiomeData(unsigned int id);

		static EraData getEraData(unsigned int id);

		static LootDropData	 getLootDropData(unsigned int id);
		static LootTableData getLootTableData(unsigned int id);

		static StructureData getStructureData(unsigned int id);

		static QuestData		  getQuestData(unsigned int id);
		static QuestObjectiveData getQuestObjectiveData(unsigned int id);

		static DialogueQuestionData getDialogueQuestionData(unsigned int id);
		static DialogueResponseData getDialogueResponseData(unsigned int id);

		static AnimationData		 getAnimationData(unsigned int id);
		static SkeletalAnimationData getSkeletalAnimationData(unsigned int id);

		static AttackData		getAttackData(unsigned int id);
		static MeleeAttackData	getMeleeAttackData(unsigned int id);
		static RangedAttackData getRangedAttackData(unsigned int id);
		static MagicAttackData	getMagicAttackData(unsigned int id);

		static BuffData getBuffData(unsigned int id);

		static FluidData	getFluidData(unsigned int id);
		static unsigned int getFluidCount(); // returns the number of fluids

		static unsigned int getTotalBiomes(); // Returns the number of biomes

		static std::vector<std::string> getNodeNamesFromFile(std::string file);

		static const std::vector<XMLDataFile> m_fileNames;

	  private:
		static void loadXMLData(std::string filepath); // Loads any valid map of GenericData children.
		static void writeXMLData(
			std::string filepath,
			std::map<unsigned int, GenericData*>*
				mapToWrite); // Writes a map (chosen by nodeName) of GenericData children to a file at filepath

		static std::map<unsigned int, GenericData*> m_tileData;
		static std::map<unsigned int, GenericData*> m_particleData;
		static std::map<unsigned int, GenericData*> m_entityData;
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
		static std::map<unsigned int, GenericData*> m_skeletalAnimationData;
		static std::map<unsigned int, GenericData*> m_attackData;
		static std::map<unsigned int, GenericData*> m_buffData;
		static std::map<unsigned int, GenericData*> m_fluidData;
	};

} // namespace XMLModule
