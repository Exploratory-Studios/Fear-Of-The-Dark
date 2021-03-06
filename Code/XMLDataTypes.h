#pragma once

#include "SaveDataTypes.h"

#include <string>
#include <glm/glm.hpp>
#include <rapidxml/rapidxml.hpp>
#include <initializer_list>

class ScriptData {
  public:
	ScriptData() {};
	~ScriptData() {};

	unsigned int getID();

	bool		isFile	   = false;
	std::string stringData = "";

  private:
	unsigned int m_id = (unsigned int)-1;
};

namespace XMLModule {

	class GenericData;
	class TileData;

	enum class AttackType { MELEE, RANGED, MAGIC };

	enum class EntityType { NPC, PROJECTILE, ITEM };

	enum class ItemType { CONSUMABLE, ARMOUR, WEAPON, BLOCK, MISC };

	enum class TileType { TILE, CONTAINER };

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
		VECTOR_FLOAT,
		VECTOR_VEC2,
		VEC2
	};

	class AttributeBase { // Just some template acrobatics.
	  public:
		template<typename T> void setData(T data);
		template<typename T> T	  getData();
		template<typename T> T*	  getDataPtr();
		virtual ~AttributeBase() {
		}

		std::string	  name;
		AttributeType type;

	  protected:
		virtual void setDefault() = 0;
	};

	template<typename T> class Attribute : public AttributeBase {
		friend class GenericData;

	  public:
		Attribute() {
		}
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

		std::string getDataString() {
			return std::to_string(getData());
		}

		T const* getDataPtr() {
			return m_data;
		}

	  private:
		T*	 m_data = nullptr;
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
		GenericData();
		GenericData(
			::std::vector<AttributeBase*> attrs); // Children will use the constructor to list attributes in the map.
		~GenericData() {
		}

		void init(::rapidxml::xml_node<>* node);	 // Inits all attribute values. (Read)
		void write(::rapidxml::xml_document<>* doc); // Writes to XML document

		template<typename T> T getAttributeByName(::std::string name);

		std::string getNodeName() {
			return nodeName;
		}

		std::unordered_map<std::string, AttributeBase*> getAttributes() {
			return m_attributes;
		}

		SaveDataTypes::MetaData getMetaData() {
			return m_metadata;
		}

		/// Members!
		std::string	 name;
		unsigned int id;

	  protected:
		template<typename T> void addAttribute(std::string name, AttributeType type, T* data);
		void					  addAttribute(AttributeBase* a);

		void addAttributes(std::vector<AttributeBase*> attrs);

		std::unordered_map<std::string, AttributeBase*> m_attributes;
		SaveDataTypes::MetaData							m_metadata;

		std::string nodeName;
	};

	class TileData : public GenericData {
	  public:
		TileData() : type(TileType::TILE) {
			nodeName						  = "tile";
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
				new Attribute<ScriptData>("updateScript", AttributeType::SCRIPT, &updateScript),
				new Attribute<ScriptData>("tickScript", AttributeType::SCRIPT, &tickScript),
				new Attribute<ScriptData>("destructionScript", AttributeType::SCRIPT, &destructionScript),
				new Attribute<ScriptData>("interactScript_walkedOn", AttributeType::SCRIPT, &interactScript_walkedOn),
				new Attribute<ScriptData>("interactScript_used", AttributeType::SCRIPT, &interactScript_used)};

			addAttributes(attrs);
		}

		virtual ~TileData() {
		}

		TileType	type;
		std::string texture = "", bumpMap = "";
		float		emittedLight = 0.0f, emittedHeat = 0.0f;
		glm::vec2	size	= glm::vec2(1.0f);
		bool		isSolid = true, isDrawn = true, isNatural = false, isTransparent = false;
		ScriptData	updateScript, tickScript, destructionScript, interactScript_walkedOn, interactScript_used;
	};

	class TileContainerData : public TileData {
	  public:
		TileContainerData() : TileData() {
			nodeName = "tileContainer";
			type	 = TileType::CONTAINER;

			std::vector<AttributeBase*> attrs = {
				new Attribute<unsigned int>("maxItems", AttributeType::UNSIGNED_INT, &maxItems)};

			addAttributes(attrs);
		}

		unsigned int maxItems;
	};

	class ParticleData : public GenericData {
	  public:
		ParticleData() {
			nodeName						  = "particle";
			std::vector<AttributeBase*> attrs = {
				new Attribute<std::string>("texture", AttributeType::FILEPATH_TEXTURE, &texture),
				new Attribute<std::string>("bumpMap", AttributeType::FILEPATH_BUMPMAP, &bumpMap),
				new Attribute<ScriptData>("script", AttributeType::SCRIPT, &script),
				new Attribute<float>("decayRate", AttributeType::FLOAT, &decayRate)};

			addAttributes(attrs);
		}
		virtual ~ParticleData() {
		}

		std::string texture, bumpMap;
		ScriptData	script;
		float		decayRate = 1.0f;
	};

	class EntityData : public GenericData {
	  public:
		EntityData() {
			nodeName					= "entity";
			Attribute<glm::vec2>* sizeA = new Attribute<glm::vec2>("size", AttributeType::VEC2, &size);

			Attribute<ScriptData>* updateA =
				new Attribute<ScriptData>("updateScript", AttributeType::SCRIPT, &updateScript);
			Attribute<ScriptData>* tickA = new Attribute<ScriptData>("tickScript", AttributeType::SCRIPT, &tickScript);
			Attribute<bool>*	   gravityA = new Attribute<bool>("gravity", AttributeType::BOOL, &gravity);

			addAttribute(sizeA);
			addAttribute(updateA);
			addAttribute(tickA);
			addAttribute(gravityA);
		}

		~EntityData() {
		}

		glm::vec2  size = glm::vec2(1.0f);
		ScriptData updateScript, tickScript;
		EntityType type;
		bool	   gravity = true;
	};

	class EntityNPCData : public EntityData {
	  public:
		EntityNPCData() {
			nodeName = "npc";
			type	 = EntityType::NPC;

			std::vector<AttributeBase*> attrs = {
				new Attribute<float>("speed", AttributeType::FLOAT, &speed),
				new Attribute<float>("jumpHeight", AttributeType::FLOAT, &jumpHeight),
				new Attribute<float>("maxHealth", AttributeType::FLOAT, &maxHealth),
				new Attribute<unsigned int>("faction", AttributeType::STRING_FACTION, &faction),
				new Attribute<bool>("isDamagedByFalls", AttributeType::BOOL, &isDamagedByFalls),
				new Attribute<bool>("isInvincible", AttributeType::BOOL, &isInvincible),
				new Attribute<unsigned int>("idleAnimationID", AttributeType::UNSIGNED_INT, &idleAnimationID),
				new Attribute<unsigned int>("lowVelAnimationID", AttributeType::UNSIGNED_INT, &lowVelAnimationID),
				new Attribute<unsigned int>("flinchAnimationID", AttributeType::UNSIGNED_INT, &flinchAnimationID),
				new Attribute<std::vector<unsigned int>>("skinAnimationIDs/skinAnimationID",
														 AttributeType::VECTOR_UNSIGNED_INT,
														 &skinAnimationIDs)};

			addAttributes(attrs);
		}

		virtual ~EntityNPCData() {
		}

		float					  speed = 0.1f, jumpHeight = 1.0f, maxHealth = 100.0f;
		unsigned int			  faction;
		bool					  isDamagedByFalls = true, isInvincible = false;
		unsigned int			  idleAnimationID = -1, lowVelAnimationID = -1, flinchAnimationID = -1;
		std::vector<unsigned int> skinAnimationIDs;
	};

	class EntityProjectileData : public EntityData {
	  public:
		EntityProjectileData() {
			nodeName = "projectile";
			type	 = EntityType::PROJECTILE;

			std::vector<AttributeBase*> attrs = {
				new Attribute<unsigned int>("animationID", AttributeType::UNSIGNED_INT, &animationID),
				//new Attribute<std::string>("bumpMap", AttributeType::FILEPATH_BUMPMAP, &bumpMap),
				new Attribute<float>("speed", AttributeType::FLOAT, &speed),
				new Attribute<bool>("collides", AttributeType::BOOL, &collides),
				new Attribute<float>("damage", AttributeType::FLOAT, &damage),
				new Attribute<float>("lifeTime", AttributeType::FLOAT, &lifeTime),
				new Attribute<float>("knockback", AttributeType::FLOAT, &knockback),
				new Attribute<std::vector<unsigned int>>("buffIDs/buffID",
														 AttributeType::VECTOR_UNSIGNED_INT,
														 &buffIDs)};

			addAttributes(attrs);
		}

		virtual ~EntityProjectileData() {
		}

		float					  speed = 0.1f, damage = 1.0f, lifeTime = -1.0f, knockback = 0.0f;
		bool					  collides = false;
		unsigned int			  animationID;
		std::vector<unsigned int> buffIDs;
	};

	class EntityItemData : public EntityData {
	  public:
		EntityItemData() {
			nodeName = "itemEntity";
			type	 = EntityType::ITEM;

			std::vector<AttributeBase*> attrs = {
				new Attribute<std::string>("texture", AttributeType::FILEPATH_TEXTURE, &texture),
				new Attribute<std::string>("bumpMap", AttributeType::FILEPATH_BUMPMAP, &bumpMap),
				new Attribute<unsigned int>("item", AttributeType::UNSIGNED_INT, &item)};

			addAttributes(attrs);
		}

		unsigned int item;
		std::string	 texture, bumpMap;
	};

	class ItemData : public GenericData {
	  public:
		ItemData() {
			nodeName						  = "item";
			std::vector<AttributeBase*> attrs = {
				new Attribute<std::string>("texture", AttributeType::FILEPATH_TEXTURE, &texture),
				new Attribute<std::string>("description", AttributeType::STRING, &description),
				new Attribute<float>("weight", AttributeType::FLOAT, &weight)};

			addAttributes(attrs);
		}

		std::string texture;
		std::string description;
		float		weight = 0.0f;
		ItemType	type;
	};

	class ItemConsumableData : public ItemData {
	  public:
		ItemConsumableData() {
			nodeName = "itemConsumable";
			type	 = ItemType::CONSUMABLE;

			std::vector<AttributeBase*> attrs = {
				new Attribute<ScriptData>("useScript", AttributeType::SCRIPT, &useScriptID)};

			addAttributes(attrs);
		}

		ScriptData useScriptID;
	};

	class ItemArmourData : public ItemData {
	  public:
		ItemArmourData() {
			nodeName = "itemArmour";
			type	 = ItemType::ARMOUR;

			std::vector<AttributeBase*> attrs = {
				new Attribute<std::vector<unsigned int>>("animationIDs/animationID",
														 AttributeType::VECTOR_UNSIGNED_INT,
														 &animationIDs),
				new Attribute<std::vector<unsigned int>>("limbIndices/limbIndex",
														 AttributeType::VECTOR_UNSIGNED_INT,
														 &limbIndices),
				new Attribute<float>("threshold", AttributeType::FLOAT, &threshold),
				new Attribute<float>("resistance", AttributeType::FLOAT, &resistance),
				new Attribute<ScriptData>("tickScript", AttributeType::SCRIPT, &tickScriptID)};

			addAttributes(attrs);
		}

		std::vector<unsigned int> animationIDs;
		std::vector<unsigned int> limbIndices;
		float					  threshold	 = 0;
		float					  resistance = 0.0f;
		ScriptData				  tickScriptID;
	};

	class ItemWeaponData : public ItemData {
	  public:
		ItemWeaponData() {
			nodeName = "itemWeapon";
			type	 = ItemType::WEAPON;

			std::vector<AttributeBase*> attrs = {
				new Attribute<unsigned int>("attackID", AttributeType::UNSIGNED_INT, &attackID)};

			addAttributes(attrs);
		}

		unsigned int attackID;
	};

	class ItemBlockData : public ItemData {
	  public:
		ItemBlockData() {
			nodeName = "itemBlock";
			type	 = ItemType::BLOCK;

			std::vector<AttributeBase*> attrs = {
				new Attribute<unsigned int>("blockID", AttributeType::UNSIGNED_INT, &blockID)};

			addAttributes(attrs);
		}

		unsigned int blockID;
	};

	class BiomeData : public GenericData {
	  public:
		BiomeData() {
			nodeName						  = "biome";
			std::vector<AttributeBase*> attrs = {
				new Attribute<std::string>("backgroundTexture", AttributeType::FILEPATH_TEXTURE, &backgroundTexture),
				new Attribute<unsigned int>("baseHeight", AttributeType::UNSIGNED_INT, &baseHeight),
				new Attribute<unsigned int>("maxHeightDiff", AttributeType::UNSIGNED_INT, &maxHeightDiff),
				new Attribute<float>("maxTemperature", AttributeType::FLOAT, &maxTemperature),
				new Attribute<float>("baseTemperature", AttributeType::FLOAT, &baseTemperature),
				new Attribute<float>("flatness", AttributeType::FLOAT, &flatness),
				new Attribute<std::vector<unsigned int>>("entities/entityID",
														 AttributeType::VECTOR_UNSIGNED_INT,
														 &entities)};

			addAttributes(attrs);
		}

		std::string				  backgroundTexture;
		unsigned int			  baseHeight, maxHeightDiff;
		float					  maxTemperature = 0.0f, baseTemperature = 0.0f, flatness = 1.0f;
		std::vector<unsigned int> entities;

		/// TODO:
		//std::vector<unsigned int> structureSpawnIds = {}; // All structures (defined by Structure objects)
		//std::vector<unsigned int> surfaceBlockIds = {}; // All blocks that make up the first few of the surface
		//std::vector<unsigned int> undergroundBlockIds = {}; // All blocks that make up the rest of the ground
	};

	class EraData : public GenericData {
	  public:
		EraData() {
			nodeName						  = "era";
			std::vector<AttributeBase*> attrs = {
				new Attribute<std::vector<unsigned int>>("biomes/biomeID", AttributeType::VECTOR_UNSIGNED_INT, &biomes)};
			addAttributes(attrs);
		}

		std::vector<unsigned int> biomes;
	};

	class LootDropData : public GenericData {
	  public:
		LootDropData() {
			nodeName						  = "lootDrop";
			std::vector<AttributeBase*> attrs = {
				new Attribute<unsigned int>("itemID", AttributeType::UNSIGNED_INT, &itemID),
				new Attribute<unsigned int>("minDrop", AttributeType::UNSIGNED_INT, &minDrop),
				new Attribute<unsigned int>("maxDrop", AttributeType::UNSIGNED_INT, &maxDrop),
				new Attribute<float>("chance", AttributeType::FLOAT, &chance)};

			addAttributes(attrs);
		}

		unsigned int itemID, minDrop = 1, maxDrop = 1;
		float		 chance;
	};

	class LootTableData : public GenericData {
	  public:
		LootTableData() {
			nodeName						  = "lootTable";
			std::vector<AttributeBase*> attrs = {
				new Attribute<std::vector<unsigned int>>("drops/dropID", AttributeType::VECTOR_UNSIGNED_INT, &drops)};

			addAttributes(attrs);
		}

		std::vector<unsigned int> drops;
	};

	class StructureData : public GenericData {
	  public:
		StructureData() {
			nodeName						  = "structure";
			std::vector<AttributeBase*> attrs = {
				new Attribute<unsigned int>("structureID", AttributeType::UNSIGNED_INT, &structureID),
				new Attribute<unsigned int>("biomeID", AttributeType::UNSIGNED_INT, &biomeID),
				new Attribute<float>("chance", AttributeType::FLOAT, &chance),
				new Attribute<unsigned int>("maxAmnt", AttributeType::UNSIGNED_INT, &maxAmnt),
				new Attribute<unsigned int>("minAmnt", AttributeType::UNSIGNED_INT, &minAmnt)};
			addAttributes(attrs);
		}

		unsigned int structureID, biomeID, maxAmnt = 1, minAmnt = 1;
		float		 chance;
	};

	class QuestData : public GenericData {
	  public:
		QuestData() {
			nodeName						  = "quest";
			std::vector<AttributeBase*> attrs = {
				new Attribute<std::vector<unsigned int>>("objectives/objectiveID",
														 AttributeType::VECTOR_UNSIGNED_INT,
														 &objectives),
				new Attribute<ScriptData>("completionScript", AttributeType::SCRIPT, &completionScript)};

			addAttributes(attrs);
		}

		std::vector<unsigned int> objectives;
		ScriptData				  completionScript;
	};

	class QuestObjectiveData : public GenericData {
	  public:
		QuestObjectiveData() {
			nodeName						  = "questObjective";
			std::vector<AttributeBase*> attrs = {
				new Attribute<std::string>("text", AttributeType::STRING, &text),
				new Attribute<ScriptData>("confirmationScript", AttributeType::SCRIPT, &confirmationScript)};

			addAttributes(attrs);
		}

		std::string text;
		ScriptData	confirmationScript;
	};

	class DialogueQuestionData : public GenericData { // Asked by the NPC.
	  public:
		DialogueQuestionData() {
			nodeName						  = "question";
			std::vector<AttributeBase*> attrs = {
				new Attribute<std::string>("text", AttributeType::STRING, &text),
				new Attribute<std::vector<unsigned int>>(
					"nextResponses/responseID",
					AttributeType::VECTOR_UNSIGNED_INT,
					&nextResponses) // List of possible responses. Shown iff all required flags are true.
			};

			addAttributes(attrs);
		}

		std::string				  text = "UNDEFINED";
		std::vector<unsigned int> nextResponses;
	};

	class DialogueResponseData : public GenericData { // Said by the player.
	  public:
		DialogueResponseData() {
			nodeName						  = "response";
			std::vector<AttributeBase*> attrs = {
				new Attribute<std::string>("text", AttributeType::STRING, &text),
				new Attribute<std::vector<unsigned int>>("requiredFlags/flagID",
														 AttributeType::VECTOR_UNSIGNED_INT,
														 &requiredFlags), // Requires all to be true
				new Attribute<unsigned int>("nextQuestion", AttributeType::UNSIGNED_INT, &nextQuestion) // -1 for none
			};

			addAttributes(attrs);
		}

		std::string				  text = "UNDEFINED";
		std::vector<unsigned int> requiredFlags;
		unsigned int			  nextQuestion = (unsigned int)-1;
	};

	class AnimationData : public GenericData {
	  public:
		AnimationData() {
			nodeName						  = "animation";
			std::vector<AttributeBase*> attrs = {
				new Attribute<std::string>("texture", AttributeType::FILEPATH_TEXTURE, &texture),
				new Attribute<std::string>("normalMap", AttributeType::FILEPATH_BUMPMAP, &normalMap),
				new Attribute<unsigned int>("y", AttributeType::UNSIGNED_INT, &y),
				new Attribute<unsigned int>("width", AttributeType::UNSIGNED_INT, &width),
				new Attribute<unsigned int>("height", AttributeType::UNSIGNED_INT, &height),
				new Attribute<unsigned int>("frames", AttributeType::UNSIGNED_INT, &frames)};

			addAttributes(attrs);
		};

		std::string	 texture, normalMap;
		unsigned int y = 0, width, height, frames;
	};

	class SkeletalAnimationData : public GenericData {
	  public:
		SkeletalAnimationData() {
			nodeName						  = "skeletalAnimation";
			std::vector<AttributeBase*> attrs = {
				new Attribute<std::vector<float>>("angles/angle", AttributeType::VECTOR_FLOAT, &angles),
				new Attribute<std::vector<glm::vec2>>("offsets/offset", AttributeType::VECTOR_VEC2, &offsets),
				new Attribute<std::vector<glm::vec2>>("centresOfRotation/centreOfRotation",
													  AttributeType::VECTOR_VEC2,
													  &centresOfRotation),
				new Attribute<std::vector<unsigned int>>("limbIndices/limbIndex",
														 AttributeType::VECTOR_UNSIGNED_INT,
														 &limbIndices),
				new Attribute<bool>("repeats", AttributeType::BOOL, &repeats)};

			addAttributes(attrs);
		}

		std::vector<float>		  angles;
		std::vector<glm::vec2>	  offsets;
		std::vector<glm::vec2>	  centresOfRotation;
		std::vector<unsigned int> limbIndices;
		bool					  repeats = false;
	};

	class AttackData : public GenericData {
	  public:
		AttackData() {
			nodeName						  = "attack";
			std::vector<AttributeBase*> attrs = {new Attribute<unsigned int>("leadInSkeletalAnimationID",
																			 AttributeType::UNSIGNED_INT,
																			 &leadInAnimationID),
												 new Attribute<unsigned int>("leadOutSkeletalAnimationID",
																			 AttributeType::UNSIGNED_INT,
																			 &leadOutAnimationID)};

			addAttributes(attrs);
		};

		unsigned int leadInAnimationID, leadOutAnimationID;
		AttackType	 type;
	};

	class MeleeAttackData : public AttackData {
	  public:
		MeleeAttackData() {
			nodeName = "meleeAttack";
			type	 = AttackType::MELEE;

			std::vector<AttributeBase*> attrs = {
				new Attribute<unsigned int>("projectileID", AttributeType::UNSIGNED_INT, &projectileID)};

			addAttributes(attrs);
		};

		unsigned int projectileID;
	};

	class RangedAttackData : public AttackData {
	  public:
		RangedAttackData() {
			nodeName = "rangedAttack";
			type	 = AttackType::RANGED;

			std::vector<AttributeBase*> attrs = {
				new Attribute<unsigned int>("projectileID", AttributeType::UNSIGNED_INT, &projectileID),
				new Attribute<unsigned int>("numProjectiles", AttributeType::UNSIGNED_INT, &numProjectiles),
				new Attribute<float>("angleWidth", AttributeType::FLOAT, &angleWidth)};

			addAttributes(attrs);
		};

		unsigned int projectileID;
		unsigned int numProjectiles = 1;
		float		 angleWidth; // 6 degrees default IN RADIANS!
	};

	class MagicAttackData : public AttackData {
	  public:
		MagicAttackData() {
			nodeName = "magicAttack";
			type	 = AttackType::MAGIC;

			std::vector<AttributeBase*> attrs = {new Attribute<ScriptData>("script", AttributeType::SCRIPT, &script)};

			addAttributes(attrs);
		};

		ScriptData script;
	};

	class BuffData : public GenericData {
	  public:
		BuffData() {
			nodeName						  = "buff";
			std::vector<AttributeBase*> attrs = {
				new Attribute<ScriptData>("tickScript", AttributeType::SCRIPT, &tickScript),
				new Attribute<std::string>("texture", AttributeType::FILEPATH_TEXTURE, &texture),
				new Attribute<std::string>("description", AttributeType::STRING, &description),
				new Attribute<unsigned int>("duration", AttributeType::UNSIGNED_INT, &duration)};

			addAttributes(attrs);
		}

		ScriptData	 tickScript;
		std::string	 texture;
		std::string	 description;
		unsigned int duration = 15;
	};

	class FluidData : public GenericData {
	  public:
		FluidData() {
			nodeName						  = "fluid";
			std::vector<AttributeBase*> attrs = {
				new Attribute<std::string>("texture", AttributeType::FILEPATH_TEXTURE, &texture),
				new Attribute<float>("gravitationalConstant", AttributeType::FLOAT, &gravConstant),
				new Attribute<float>("idealDensity", AttributeType::FLOAT, &idealDensity),
				new Attribute<float>("viscosity", AttributeType::FLOAT, &viscosity),
				new Attribute<unsigned int>("red", AttributeType::UNSIGNED_INT, &red),
				new Attribute<unsigned int>("green", AttributeType::UNSIGNED_INT, &green),
				new Attribute<unsigned int>("blue", AttributeType::UNSIGNED_INT, &blue),
				new Attribute<unsigned int>("alpha", AttributeType::UNSIGNED_INT, &alpha)};

			addAttributes(attrs);
		}

		std::string texture = UNDEFINED_ASSET_PATH;
		float		gravConstant = 0.7f;
		float		idealDensity = 1.0f;
		float		viscosity = 1.0f;
		unsigned int red = 255, green = 0, blue = 255, alpha = 255;
	};
} // namespace XMLModule

#include "XMLDataTypes.tpp" // Just for templated functions.
