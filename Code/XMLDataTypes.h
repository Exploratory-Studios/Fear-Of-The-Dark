#pragma once

#include "SaveDataTypes.h"

#include <string>
#include <glm/glm.hpp>
#include <rapidxml/rapidxml.hpp>
#include <initializer_list>

namespace XMLModule {

	class GenericData;
	class TileData;

	enum class AttributeType {
		STRING,
		UNSIGNED_INT,
		INT,
		FLOAT,
		BOOL,
		FILEPATH_TEXTURE,
		FILEPATH_BUMPMAP,
		SCRIPT,
		STRING_FACTION,
		VECTOR_UNSIGNED_INT,
		VEC2
	};

	class GenericData; // Forward Dec.

	class AttributeBase { // Just some template acrobatics.
		public:
			template<class T>
			void setData(T data);
			template<class T>
			T getData();
			template<class T>
			T* getDataPtr();
			virtual ~AttributeBase() {}

			std::string name;
			AttributeType type;

		protected:
			virtual void setDefault() = 0;
	};

	template<class T>
	class Attribute : public AttributeBase {
			friend class GenericData;
		public:
			Attribute() {}
			Attribute(std::string nameP, AttributeType typeP, T* data) {
				/**
				 * @brief Constructs an Attribute, linking it to the member variable represented by `data`
				 * @param nameP
				 * @param type
				 * @param data
				 */
				name = nameP;
				type = typeP;

				m_data = data;
				setDefault();
			}
			~Attribute() {};

			void setData(T data) {
				*m_data = data;
			}

			T getData() {
				return *m_data;
			}

			T const* getDataPtr() {
				return m_data;
			}

			/*boost::variant<std::string,
			      unsigned int,
			      int,
			      float,
			      bool,
			      std::vector<unsigned int>,
			      std::vector<int>,
			      glm::vec2> data; // Basically just an easier union.*/

		private:
			T* m_data = nullptr;
			void setDefault() {
				setData((T)(-1));
			}
			/*
				"if your function template still depends on some unspecified template parameters,
					it is a true template and it has to be defined in header file.
				But once you "fix" all the parameters (by explicit specialization),
					it is no longer a template.
				It becomes an ordinary function,
					that has to be declared in header file
					and defined only once in some implementation file."
																				~ AnT, 2013, "stackoverflow.com/questions/17575735/multiple-definition-of-a-function-in-templated-class"
			*/
	};

	class GenericData {
			/**
			    Responsibilities:
			     - Act as a base class, providing read and write functions
			     - Hold a map of Attributes, using their name as the key.
			     - Provide functions to initialize (for read/write) attributes (protected)
			     - Provide a function to read map elements.
			**/
		public:
			GenericData() {}
			GenericData(::std::vector<AttributeBase*> attrs); // Children will use the constructor to list attributes in the map.
			~GenericData() {}

			void init(::rapidxml::xml_node<>* node); // Inits all attribute values. (Read)
			void write(::rapidxml::xml_node<>* node); // Writes to XML document

			/*void getAttribute(::std::string name, std::string& dataPtr); // Just puts the data into the pointer
				void getAttribute(::std::string name, unsigned int& dataPtr); // Just puts the data into the pointer
				void getAttribute(::std::string name, int& dataPtr); // Just puts the data into the pointer
				void getAttribute(::std::string name, float& dataPtr); // Just puts the data into the pointer
				void getAttribute(::std::string name, bool& dataPtr); // Just puts the data into the pointer
				void getAttribute(::std::string name, std::vector<unsigned int>& dataPtr); // Just puts the data into the pointer
				void getAttribute(::std::string name, glm::vec2& dataPtr); // Just puts the data into the pointer*/

			template<typename T>
			T getAttributeByName(::std::string name);

			//template<class T> OUT OF USE.
			//T & getAttributeT(std::string index); // Templated, probably better version of above.

			SaveDataTypes::MetaData getMetaData() {
				return m_metadata;
			}

			/// Members!
			std::string name;
			unsigned int id;

		protected:
			template<class T>
			void addAttribute(std::string name, AttributeType type, T* data);
			void addAttribute(AttributeBase* a);

			void addAttributes(std::vector<AttributeBase*> attrs);

			std::unordered_map<std::string, AttributeBase*> m_attributes;
			SaveDataTypes::MetaData m_metadata;
	};

	class TileData : public GenericData {
		public:
			TileData() : texture("") {
				std::vector<AttributeBase*> attrs = {
					new Attribute<std::string>("texture", AttributeType::FILEPATH_TEXTURE, &texture),
					new Attribute<std::string>("bumpMap", AttributeType::FILEPATH_BUMPMAP, &bumpMap),
					new Attribute<float>("emittedLight", AttributeType::FLOAT, &emittedLight),
					new Attribute<float>("emittedHeat", AttributeType::FLOAT, &emittedHeat),
					new Attribute<glm::vec2>("size", AttributeType::VEC2, &size),
					new Attribute<bool>("isSolid", AttributeType::BOOL, &isSolid),
					new Attribute<bool>("isDrawn", AttributeType::BOOL, &isDrawn),
					new Attribute<bool>("isNatural", AttributeType::BOOL, &isNatural),
					new Attribute<bool>("isTransparent", AttributeType::BOOL, &isTransparent),
					new Attribute<unsigned int>("updateScript", AttributeType::SCRIPT, &updateScript),
					new Attribute<unsigned int>("tickScript", AttributeType::SCRIPT, &tickScript),
					new Attribute<unsigned int>("destructionScript", AttributeType::SCRIPT, &destructionScript),
					new Attribute<unsigned int>("interactScript_walkedOn", AttributeType::SCRIPT, &interactScript_walkedOn),
					new Attribute<unsigned int>("interactScript_used", AttributeType::SCRIPT, &interactScript_used)
				};

				addAttributes(attrs);

			}

			virtual ~TileData() {}

			std::string texture = "", bumpMap = "";
			float emittedLight = 0.0f, emittedHeat = 0.0f;
			glm::vec2 size;
			bool isSolid, isDrawn, isNatural, isTransparent;
			unsigned int updateScript, tickScript, destructionScript, interactScript_walkedOn, interactScript_used;
	};

	class ParticleData : public GenericData {
		public:
			ParticleData() {
				std::vector<AttributeBase*> attrs = {
					new Attribute<std::string>("texture", AttributeType::FILEPATH_TEXTURE, &texture),
					new Attribute<std::string>("bumpMap", AttributeType::FILEPATH_BUMPMAP, &bumpMap),
					new Attribute<unsigned int>("script", AttributeType::SCRIPT, &script),
					new Attribute<float>("decayRate", AttributeType::FLOAT, &decayRate)
				};

				addAttributes(attrs);
			}
			virtual ~ParticleData() {}

			std::string texture, bumpMap;
			unsigned int script;
			float decayRate;
	};

	class EntityData : public GenericData {
		public:
			EntityData() {
				Attribute<std::string>* texA = new Attribute<std::string>("texture", AttributeType::FILEPATH_TEXTURE, &texture);
				Attribute<std::string>* bumpA = new Attribute<std::string>("bumpMap", AttributeType::FILEPATH_BUMPMAP, &bumpMap);

				Attribute<glm::vec2>* sizeA = new Attribute<glm::vec2>("size", AttributeType::VEC2, &size);

				Attribute<unsigned int>* updateA = new Attribute<unsigned int>("updateScript", AttributeType::SCRIPT, &updateScript);
				Attribute<unsigned int>* tickA = new Attribute<unsigned int>("tickScript", AttributeType::SCRIPT, &tickScript);

				addAttribute(texA);
				addAttribute(bumpA);
				addAttribute(sizeA);
				addAttribute(updateA);
				addAttribute(tickA);
			}

			~EntityData() {}

			std::string texture, bumpMap;
			glm::vec2 size;
			unsigned int updateScript, tickScript;

	};

	class EntityNPCData : public EntityData {
		public:
			EntityNPCData() {
				std::vector<AttributeBase*> attrs = {
					new Attribute<float>("speed", AttributeType::FLOAT, &speed),
					new Attribute<float>("jumpHeight", AttributeType::FLOAT, &jumpHeight),
					new Attribute<float>("maxHealth", AttributeType::FLOAT, &maxHealth),
					new Attribute<unsigned int>("faction", AttributeType::STRING_FACTION, &faction),
					new Attribute<bool>("isDamagedByFalls", AttributeType::BOOL, &isDamagedByFalls),
					new Attribute<bool>("isInvincible", AttributeType::BOOL, &isInvincible)
				};

				addAttributes(attrs);
			}

			virtual ~EntityNPCData() {}

			float speed, jumpHeight, maxHealth;
			unsigned int faction;
			bool isDamagedByFalls, isInvincible;
	};

	class EntityProjectileData : public EntityData {
		public:
			EntityProjectileData() {
				std::vector<AttributeBase*> attrs = {
					new Attribute<float>("speed", AttributeType::FLOAT, &speed),
					new Attribute<bool>("collides", AttributeType::BOOL, &collides),
					new Attribute<float>("damage", AttributeType::FLOAT, &damage)
				};

				addAttributes(attrs);
			}

			virtual ~EntityProjectileData() {}

			float speed, damage;
			bool collides;
	};

	class EntityItemData : public EntityData {
		public:
			EntityItemData() {
				std::vector<AttributeBase*> attrs = {
					new Attribute<unsigned int>("item", AttributeType::UNSIGNED_INT, &item)
				};

				addAttributes(attrs);
			}

			unsigned int item;
	};

	class ItemData : public GenericData {
		public:
			ItemData() {
				std::vector<AttributeBase*> attrs = {
					new Attribute<std::string>("texture", AttributeType::FILEPATH_TEXTURE, &texture),
					new Attribute<float>("weight", AttributeType::FLOAT, &weight),
					new Attribute<unsigned int>("useScript", AttributeType::SCRIPT, &useScript)
				};

				addAttributes(attrs);
			}

			std::string texture;
			float weight;
			unsigned int useScript;
	};

	class BiomeData : public GenericData {
		public:
			BiomeData() {
				std::vector<AttributeBase*> attrs = {
					new Attribute<std::string>("backgroundTexture", AttributeType::FILEPATH_TEXTURE, &backgroundTexture),
					new Attribute<unsigned int>("baseHeight", AttributeType::UNSIGNED_INT, &baseHeight),
					new Attribute<unsigned int>("maxHeightDiff", AttributeType::UNSIGNED_INT, &maxHeightDiff),
					new Attribute<float>("maxTemperature", AttributeType::FLOAT, &maxTemperature),
					new Attribute<float>("baseTemperature", AttributeType::FLOAT, &baseTemperature),
					new Attribute<float>("flatness", AttributeType::FLOAT, &flatness),
					new Attribute<std::vector<unsigned int>>("entities/entityID", AttributeType::VECTOR_UNSIGNED_INT, &entities)
				};

				addAttributes(attrs);
			}

			std::string backgroundTexture;
			unsigned int baseHeight, maxHeightDiff;
			float maxTemperature, baseTemperature, flatness;
			std::vector<unsigned int> entities;

			/// TODO:
			//std::vector<unsigned int> structureSpawnIds = {}; // All structures (defined by Structure objects)
			//std::vector<unsigned int> surfaceBlockIds = {}; // All blocks that make up the first few of the surface
			//std::vector<unsigned int> undergroundBlockIds = {}; // All blocks that make up the rest of the ground

	};

	class EraData : public GenericData {
		public:
			EraData() {
				std::vector<AttributeBase*> attrs = {
					new Attribute<std::vector<unsigned int>>("biomes/biomeID", AttributeType::VECTOR_UNSIGNED_INT, &biomes)
				};
				addAttributes(attrs);
			}

			std::vector<unsigned int> biomes;
	};

	class LootDropData : public GenericData {
		public:
			LootDropData() {
				std::vector<AttributeBase*> attrs = {
					new Attribute<unsigned int>("itemID", AttributeType::UNSIGNED_INT, &itemID),
					new Attribute<unsigned int>("minDrop", AttributeType::UNSIGNED_INT, &minDrop),
					new Attribute<unsigned int>("maxDrop", AttributeType::UNSIGNED_INT, &maxDrop),
					new Attribute<float>("chance", AttributeType::FLOAT, &chance)
				};

				addAttributes(attrs);
			}

			unsigned int itemID, minDrop, maxDrop;
			float chance;
	};

	class LootTableData : public GenericData {
		public:
			LootTableData() {
				std::vector<AttributeBase*> attrs = {
					new Attribute<std::vector<unsigned int>>("drops/dropID", AttributeType::VECTOR_UNSIGNED_INT, &drops)
				};

				addAttributes(attrs);
			}

			std::vector<unsigned int> drops;
	};

	class StructureData : public GenericData {
		public:
			StructureData() {
				std::vector<AttributeBase*> attrs = {
					new Attribute<unsigned int>("structureID", AttributeType::UNSIGNED_INT, &structureID),
					new Attribute<unsigned int>("biomeID", AttributeType::UNSIGNED_INT, &biomeID),
					new Attribute<float>("chance", AttributeType::FLOAT, &chance),
					new Attribute<unsigned int>("maxAmnt", AttributeType::UNSIGNED_INT, &maxAmnt),
					new Attribute<unsigned int>("minAmnt", AttributeType::UNSIGNED_INT, &minAmnt)
				};
				addAttributes(attrs);
			}

			unsigned int structureID, biomeID, maxAmnt, minAmnt;
			float chance;
	};

	class QuestData : public GenericData {
		public:
			QuestData() {
				std::vector<AttributeBase*> attrs = {
					new Attribute<std::vector<unsigned int>>("objectives/objectiveID", AttributeType::VECTOR_UNSIGNED_INT, &objectives),
					                                      new Attribute<unsigned int>("completionScript", AttributeType::SCRIPT, &completionScript)
				};

				addAttributes(attrs);
			}

			std::vector<unsigned int> objectives;
			unsigned int completionScript;
	};

	class QuestObjectiveData : public GenericData {
		public:
			QuestObjectiveData() {
				std::vector<AttributeBase*> attrs = {
					new Attribute<std::string>("text", AttributeType::STRING, &text),
					new Attribute<unsigned int>("confirmationScript", AttributeType::SCRIPT, &confirmationScript)
				};

				addAttributes(attrs);
			}

			std::string text;
			unsigned int confirmationScript;
	};

	class DialogueQuestionData : public GenericData { // Asked by the NPC.
		public:
			DialogueQuestionData() {
				std::vector<AttributeBase*> attrs = {
					new Attribute<std::string>("text", AttributeType::STRING, &text),
					new Attribute<std::vector<unsigned int>>("nextResponses/responseID", AttributeType::VECTOR_UNSIGNED_INT, &nextResponses) // List of possible responses. Shown iff all required flags are true.
				};

				addAttributes(attrs);
			}

			std::string text;
			std::vector<unsigned int> nextResponses;
	};

	class DialogueResponseData : public GenericData { // Said by the player.
		public:
			DialogueResponseData() {
				std::vector<AttributeBase*> attrs = {
					new Attribute<std::string>("text", AttributeType::STRING, &text),
					new Attribute<std::vector<unsigned int>>("requiredFlags/flagID", AttributeType::VECTOR_UNSIGNED_INT, &requiredFlags), // Requires all to be true
					                                      new Attribute<unsigned int>("nextQuestion", AttributeType::UNSIGNED_INT, &nextQuestion) // -1 for none
				};

				addAttributes(attrs);
			}

			std::string text;
			std::vector<unsigned int> requiredFlags;
			unsigned int nextQuestion;
	};
}

#include "XMLDataTypes.tpp" // Just for templated functions.
