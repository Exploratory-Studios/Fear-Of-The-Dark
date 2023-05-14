#pragma once

#include <string>
#include <glm/glm.hpp>
#include <rapidxml/rapidxml.hpp>
#include <initializer_list>

#include <LuaScript.hpp>
#include <XMLDataManager.hpp>

namespace XMLModule {

	enum class AttackType { MELEE, RANGED, MAGIC };
	enum class EntityType { NPC, PROJECTILE, ITEM };
	enum class ItemType { CONSUMABLE, ARMOUR, WEAPON, BLOCK, MISC };
	enum class TileType { TILE, CONTAINER };

	class TileData : public BARE2D::XMLData {
	  public:
		TileData() : type(TileType::TILE) {
			nodeName								  = "tile";
			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<BARE2D::Texture>("texture", BARE2D::AttributeType::TEXTURE, &texture),
				new BARE2D::Attribute<BARE2D::Texture>("bumpmap", BARE2D::AttributeType::TEXTURE, &bumpmap),
				new BARE2D::Attribute<float>("emittedLight", BARE2D::AttributeType::FLOAT, &emittedLight),
				new BARE2D::Attribute<float>("emittedHeat", BARE2D::AttributeType::FLOAT, &emittedHeat),
				new BARE2D::Attribute<glm::vec2>("size", BARE2D::AttributeType::VEC2, &size),
				new BARE2D::Attribute<bool>("isSolid", BARE2D::AttributeType::BOOL, &isSolid),
				new BARE2D::Attribute<bool>("isDrawn", BARE2D::AttributeType::BOOL, &isDrawn),
				new BARE2D::Attribute<bool>("isNatural", BARE2D::AttributeType::BOOL, &isNatural),
				new BARE2D::Attribute<bool>("isTransparent", BARE2D::AttributeType::BOOL, &isTransparent),
				new BARE2D::Attribute<BARE2D::LuaScript>("updateScript", BARE2D::AttributeType::SCRIPT, &updateScript),
				new BARE2D::Attribute<BARE2D::LuaScript>("tickScript", BARE2D::AttributeType::SCRIPT, &tickScript),
				new BARE2D::Attribute<BARE2D::LuaScript>("destructionScript",
														 BARE2D::AttributeType::SCRIPT,
														 &destructionScript),
				new BARE2D::Attribute<BARE2D::LuaScript>("interactScript_walkedOn",
														 BARE2D::AttributeType::SCRIPT,
														 &interactScript_walkedOn),
				new BARE2D::Attribute<BARE2D::LuaScript>("interactScript_used",
														 BARE2D::AttributeType::SCRIPT,
														 &interactScript_used)};

			addAttributes(attrs);
		}

		virtual ~TileData() {
		}

		static XMLData* cloneType() {
			return new TileData();
		}

		TileType		  type;
		BARE2D::Texture	  texture, bumpmap;
		float			  emittedLight = 0.0f, emittedHeat = 0.0f;
		glm::vec2		  size	  = glm::vec2(1.0f);
		bool			  isSolid = true, isDrawn = true, isNatural = false, isTransparent = false;
		BARE2D::LuaScript updateScript, tickScript, destructionScript, interactScript_walkedOn, interactScript_used;
	};

	class TileContainerData : public TileData {
	  public:
		TileContainerData() : TileData() {
			nodeName = "tileContainer";
			type	 = TileType::CONTAINER;

			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<unsigned int>("maxItems", BARE2D::AttributeType::UNSIGNED_INT, &maxItems)};

			addAttributes(attrs);
		}

		static XMLData* cloneType() {
			return new TileContainerData();
		}

		unsigned int maxItems;
	};

	class ParticleData : public BARE2D::XMLData {
	  public:
		ParticleData() {
			nodeName								  = "particle";
			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<BARE2D::Texture>("texture", BARE2D::AttributeType::TEXTURE, &texture),
				new BARE2D::Attribute<BARE2D::Texture>("bumpmap", BARE2D::AttributeType::TEXTURE, &bumpmap),
				new BARE2D::Attribute<BARE2D::LuaScript>("script", BARE2D::AttributeType::SCRIPT, &script),
				new BARE2D::Attribute<float>("decayRate", BARE2D::AttributeType::FLOAT, &decayRate)};

			addAttributes(attrs);
		}
		virtual ~ParticleData() {
		}

		static XMLData* cloneType() {
			return new ParticleData();
		}

		BARE2D::Texture	  texture, bumpmap;
		BARE2D::LuaScript script;
		float			  decayRate = 1.0f;
	};

	class EntityData : public BARE2D::XMLData {
	  public:
		EntityData() {
			nodeName = "entity";
			BARE2D::Attribute<glm::vec2>* sizeA =
				new BARE2D::Attribute<glm::vec2>("size", BARE2D::AttributeType::VEC2, &size);

			BARE2D::Attribute<BARE2D::LuaScript>* updateA =
				new BARE2D::Attribute<BARE2D::LuaScript>("updateScript", BARE2D::AttributeType::SCRIPT, &updateScript);
			BARE2D::Attribute<BARE2D::LuaScript>* tickA =
				new BARE2D::Attribute<BARE2D::LuaScript>("tickScript", BARE2D::AttributeType::SCRIPT, &tickScript);
			BARE2D::Attribute<bool>* floatingA =
				new BARE2D::Attribute<bool>("floating", BARE2D::AttributeType::BOOL, &floating);

			addAttribute(sizeA);
			addAttribute(updateA);
			addAttribute(tickA);
			addAttribute(floatingA);
		}

		~EntityData() {
		}

		static XMLData* cloneType() {
			return new EntityData();
		}

		glm::vec2		  size = glm::vec2(1.0f);
		BARE2D::LuaScript updateScript, tickScript;
		EntityType		  type;
		bool			  floating = false;
	};

	class EntityNPCData : public EntityData {
	  public:
		EntityNPCData() {
			nodeName = "npc";
			type	 = EntityType::NPC;

			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<float>("speed", BARE2D::AttributeType::FLOAT, &speed),
				new BARE2D::Attribute<float>("jumpHeight", BARE2D::AttributeType::FLOAT, &jumpHeight),
				new BARE2D::Attribute<float>("maxHealth", BARE2D::AttributeType::FLOAT, &maxHealth),
				new BARE2D::Attribute<std::string>("faction", BARE2D::AttributeType::STRING, &faction),
				new BARE2D::Attribute<bool>("isDamagedByFalls", BARE2D::AttributeType::BOOL, &isDamagedByFalls),
				new BARE2D::Attribute<bool>("isInvincible", BARE2D::AttributeType::BOOL, &isInvincible),
				new BARE2D::Attribute<unsigned int>("idleAnimationID",
													BARE2D::AttributeType::UNSIGNED_INT,
													&idleAnimationID),
				new BARE2D::Attribute<unsigned int>("lowVelAnimationID",
													BARE2D::AttributeType::UNSIGNED_INT,
													&lowVelAnimationID),
				new BARE2D::Attribute<unsigned int>("flinchAnimationID",
													BARE2D::AttributeType::UNSIGNED_INT,
													&flinchAnimationID),
				new BARE2D::Attribute<std::vector<unsigned int>>("skinAnimationIDs",
																 BARE2D::AttributeType::VECTOR_UNSIGNED_INT,
																 &skinAnimationIDs)};

			addAttributes(attrs);
		}

		virtual ~EntityNPCData() {
		}

		static XMLData* cloneType() {
			return new EntityNPCData();
		}

		float					  speed = 0.1f, jumpHeight = 1.0f, maxHealth = 100.0f;
		std::string				  faction;
		bool					  isDamagedByFalls = true, isInvincible = false;
		unsigned int			  idleAnimationID = -1, lowVelAnimationID = -1, flinchAnimationID = -1;
		std::vector<unsigned int> skinAnimationIDs;
	};

	class EntityProjectileData : public EntityData {
	  public:
		EntityProjectileData() {
			nodeName = "projectile";
			type	 = EntityType::PROJECTILE;

			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<unsigned int>("animationID", BARE2D::AttributeType::UNSIGNED_INT, &animationID),
				new BARE2D::Attribute<float>("speed", BARE2D::AttributeType::FLOAT, &speed),
				new BARE2D::Attribute<bool>("collides", BARE2D::AttributeType::BOOL, &collides),
				new BARE2D::Attribute<float>("damage", BARE2D::AttributeType::FLOAT, &damage),
				new BARE2D::Attribute<float>("lifeTime", BARE2D::AttributeType::FLOAT, &lifeTime),
				new BARE2D::Attribute<float>("knockback", BARE2D::AttributeType::FLOAT, &knockback),
				new BARE2D::Attribute<std::vector<unsigned int>>("buffIDs",
																 BARE2D::AttributeType::VECTOR_UNSIGNED_INT,
																 &buffIDs)};

			addAttributes(attrs);
		}

		virtual ~EntityProjectileData() {
		}

		static XMLData* cloneType() {
			return new EntityProjectileData();
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

			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<BARE2D::Texture>("texture", BARE2D::AttributeType::TEXTURE, &texture),
				new BARE2D::Attribute<BARE2D::Texture>("bumpmap", BARE2D::AttributeType::TEXTURE, &bumpmap),
				new BARE2D::Attribute<unsigned int>("item", BARE2D::AttributeType::UNSIGNED_INT, &item)};

			addAttributes(attrs);
		}

		static XMLData* cloneType() {
			return new EntityItemData();
		}

		unsigned int	item;
		BARE2D::Texture texture, bumpmap;
	};

	class ItemData : public BARE2D::XMLData {
	  public:
		ItemData() {
			nodeName								  = "item";
			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<BARE2D::Texture>("texture", BARE2D::AttributeType::TEXTURE, &texture),
				new BARE2D::Attribute<std::string>("description", BARE2D::AttributeType::STRING, &description),
				new BARE2D::Attribute<float>("weight", BARE2D::AttributeType::FLOAT, &weight)};

			addAttributes(attrs);
		}

		static XMLData* cloneType() {
			return new ItemData();
		}

		BARE2D::Texture texture;
		std::string		description;
		float			weight = 0.0f;
		ItemType		type;
	};

	class ItemConsumableData : public ItemData {
	  public:
		ItemConsumableData() {
			nodeName = "itemConsumable";
			type	 = ItemType::CONSUMABLE;

			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<BARE2D::LuaScript>("useScript", BARE2D::AttributeType::SCRIPT, &useScript)};

			addAttributes(attrs);
		}

		static XMLData* cloneType() {
			return new ItemConsumableData();
		}

		BARE2D::LuaScript useScript;
	};

	class ItemArmourData : public ItemData {
	  public:
		ItemArmourData() {
			nodeName = "itemArmour";
			type	 = ItemType::ARMOUR;

			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<std::vector<unsigned int>>("animationIDs",
																 BARE2D::AttributeType::VECTOR_UNSIGNED_INT,
																 &animationIDs),
				new BARE2D::Attribute<std::vector<unsigned int>>("limbIndices",
																 BARE2D::AttributeType::VECTOR_UNSIGNED_INT,
																 &limbIndices),
				new BARE2D::Attribute<float>("threshold", BARE2D::AttributeType::FLOAT, &threshold),
				new BARE2D::Attribute<float>("resistance", BARE2D::AttributeType::FLOAT, &resistance),
				new BARE2D::Attribute<BARE2D::LuaScript>("tickScript", BARE2D::AttributeType::SCRIPT, &tickScript)};

			addAttributes(attrs);
		}

		static XMLData* cloneType() {
			return new ItemArmourData();
		}

		std::vector<unsigned int> animationIDs;
		std::vector<unsigned int> limbIndices;
		float					  threshold	 = 0;
		float					  resistance = 0.0f;
		BARE2D::LuaScript		  tickScript;
	};

	class ItemWeaponData : public ItemData {
	  public:
		ItemWeaponData() {
			nodeName = "itemWeapon";
			type	 = ItemType::WEAPON;

			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<unsigned int>("attackID", BARE2D::AttributeType::UNSIGNED_INT, &attackID)};

			addAttributes(attrs);
		}

		static XMLData* cloneType() {
			return new ItemWeaponData();
		}

		unsigned int attackID;
	};

	class ItemBlockData : public ItemData {
	  public:
		ItemBlockData() {
			nodeName = "itemBlock";
			type	 = ItemType::BLOCK;

			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<unsigned int>("blockID", BARE2D::AttributeType::UNSIGNED_INT, &blockID)};

			addAttributes(attrs);
		}

		static XMLData* cloneType() {
			return new ItemBlockData();
		}

		unsigned int blockID;
	};

	class BiomeData : public BARE2D::XMLData {
	  public:
		BiomeData() {
			nodeName								  = "biome";
			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<BARE2D::Texture>("backgroundTexture",
													   BARE2D::AttributeType::TEXTURE,
													   &backgroundTexture),
				new BARE2D::Attribute<unsigned int>("baseHeight", BARE2D::AttributeType::UNSIGNED_INT, &baseHeight),
				new BARE2D::Attribute<unsigned int>("maxHeightDiff",
													BARE2D::AttributeType::UNSIGNED_INT,
													&maxHeightDiff),
				new BARE2D::Attribute<float>("maxTemperature", BARE2D::AttributeType::FLOAT, &maxTemperature),
				new BARE2D::Attribute<float>("baseTemperature", BARE2D::AttributeType::FLOAT, &baseTemperature),
				new BARE2D::Attribute<float>("flatness", BARE2D::AttributeType::FLOAT, &flatness),
				new BARE2D::Attribute<std::vector<unsigned int>>("entities",
																 BARE2D::AttributeType::VECTOR_UNSIGNED_INT,
																 &entities)};

			addAttributes(attrs);
		}

		static XMLData* cloneType() {
			return new BiomeData();
		}

		BARE2D::Texture			  backgroundTexture;
		unsigned int			  baseHeight, maxHeightDiff;
		float					  maxTemperature = 0.0f, baseTemperature = 0.0f, flatness = 1.0f;
		std::vector<unsigned int> entities;

		/// TODO:
		//std::vector<unsigned int> structureSpawnIds = {}; // All structures (defined by Structure objects)
		//std::vector<unsigned int> surfaceBlockIds = {}; // All blocks that make up the first few of the surface
		//std::vector<unsigned int> undergroundBlockIds = {}; // All blocks that make up the rest of the ground
	};

	class EraData : public BARE2D::XMLData {
	  public:
		EraData() {
			nodeName								  = "era";
			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<std::vector<unsigned int>>("biomes",
																 BARE2D::AttributeType::VECTOR_UNSIGNED_INT,
																 &biomes)};
			addAttributes(attrs);
		}

		static XMLData* cloneType() {
			return new EraData();
		}

		std::vector<unsigned int> biomes;
	};

	class LootDropData : public BARE2D::XMLData {
	  public:
		LootDropData() {
			nodeName								  = "lootDrop";
			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<unsigned int>("itemID", BARE2D::AttributeType::UNSIGNED_INT, &itemID),
				new BARE2D::Attribute<unsigned int>("minDrop", BARE2D::AttributeType::UNSIGNED_INT, &minDrop),
				new BARE2D::Attribute<unsigned int>("maxDrop", BARE2D::AttributeType::UNSIGNED_INT, &maxDrop),
				new BARE2D::Attribute<float>("chance", BARE2D::AttributeType::FLOAT, &chance)};

			addAttributes(attrs);
		}

		static XMLData* cloneType() {
			return new LootDropData();
		}

		unsigned int itemID, minDrop = 1, maxDrop = 1;
		float		 chance;
	};

	class LootTableData : public BARE2D::XMLData {
	  public:
		LootTableData() {
			nodeName								  = "lootTable";
			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<std::vector<unsigned int>>("drops",
																 BARE2D::AttributeType::VECTOR_UNSIGNED_INT,
																 &drops)};

			addAttributes(attrs);
		}

		static XMLData* cloneType() {
			return new LootTableData();
		}

		std::vector<unsigned int> drops;
	};

	class StructureData : public BARE2D::XMLData {
	  public:
		StructureData() {
			nodeName								  = "structure";
			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<unsigned int>("structureID", BARE2D::AttributeType::UNSIGNED_INT, &structureID),
				new BARE2D::Attribute<unsigned int>("biomeID", BARE2D::AttributeType::UNSIGNED_INT, &biomeID),
				new BARE2D::Attribute<float>("chance", BARE2D::AttributeType::FLOAT, &chance),
				new BARE2D::Attribute<unsigned int>("maxAmnt", BARE2D::AttributeType::UNSIGNED_INT, &maxAmnt),
				new BARE2D::Attribute<unsigned int>("minAmnt", BARE2D::AttributeType::UNSIGNED_INT, &minAmnt)};
			addAttributes(attrs);
		}

		static XMLData* cloneType() {
			return new StructureData();
		}

		unsigned int structureID, biomeID, maxAmnt = 1, minAmnt = 1;
		float		 chance;
	};

	class QuestData : public BARE2D::XMLData {
	  public:
		QuestData() {
			nodeName								  = "quest";
			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<std::vector<unsigned int>>("objectives",
																 BARE2D::AttributeType::VECTOR_UNSIGNED_INT,
																 &objectives),
				new BARE2D::Attribute<BARE2D::LuaScript>("completionScript",
														 BARE2D::AttributeType::SCRIPT,
														 &completionScript)};

			addAttributes(attrs);
		}

		static XMLData* cloneType() {
			return new QuestData();
		}

		std::vector<unsigned int> objectives;
		BARE2D::LuaScript		  completionScript;
	};

	class QuestObjectiveData : public BARE2D::XMLData {
	  public:
		QuestObjectiveData() {
			nodeName								  = "questObjective";
			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<std::string>("text", BARE2D::AttributeType::STRING, &text),
				new BARE2D::Attribute<BARE2D::LuaScript>("confirmationScript",
														 BARE2D::AttributeType::SCRIPT,
														 &confirmationScript)};

			addAttributes(attrs);
		}

		static XMLData* cloneType() {
			return new QuestObjectiveData();
		}

		std::string		  text;
		BARE2D::LuaScript confirmationScript;
	};

	class DialogueQuestionData :
		public BARE2D::XMLData // Asked by the NPC.
	{
	  public:
		DialogueQuestionData() {
			nodeName								  = "question";
			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<std::string>("text", BARE2D::AttributeType::STRING, &text),
				new BARE2D::Attribute<std::vector<unsigned int>>(
					"nextResponses",
					BARE2D::AttributeType::VECTOR_UNSIGNED_INT,
					&nextResponses) // List of possible responses. Shown iff all required flags are true.
			};

			addAttributes(attrs);
		}

		static XMLData* cloneType() {
			return new DialogueQuestionData();
		}

		std::string				  text = "UNDEFINED";
		std::vector<unsigned int> nextResponses;
	};

	class DialogueResponseData :
		public BARE2D::XMLData // Said by the player.
	{
	  public:
		DialogueResponseData() {
			nodeName								  = "response";
			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<std::string>("text", BARE2D::AttributeType::STRING, &text),
				new BARE2D::Attribute<std::vector<unsigned int>>("requiredFlags",
																 BARE2D::AttributeType::VECTOR_UNSIGNED_INT,
																 &requiredFlags), // Requires all to be true
				new BARE2D::Attribute<unsigned int>("nextQuestion",
													BARE2D::AttributeType::UNSIGNED_INT,
													&nextQuestion) // -1 for none
			};

			addAttributes(attrs);
		}

		static XMLData* cloneType() {
			return new DialogueResponseData();
		}

		std::string				  text = "UNDEFINED";
		std::vector<unsigned int> requiredFlags;
		unsigned int			  nextQuestion = (unsigned int)-1;
	};

	class AnimationData : public BARE2D::XMLData {
	  public:
		AnimationData() {
			nodeName								  = "animation";
			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<BARE2D::Texture>("texture", BARE2D::AttributeType::TEXTURE, &texture),
				new BARE2D::Attribute<BARE2D::Texture>("normalMap", BARE2D::AttributeType::TEXTURE, &bumpmap),
				new BARE2D::Attribute<unsigned int>("y", BARE2D::AttributeType::UNSIGNED_INT, &y),
				new BARE2D::Attribute<unsigned int>("width", BARE2D::AttributeType::UNSIGNED_INT, &width),
				new BARE2D::Attribute<unsigned int>("height", BARE2D::AttributeType::UNSIGNED_INT, &height),
				new BARE2D::Attribute<unsigned int>("frames", BARE2D::AttributeType::UNSIGNED_INT, &frames)};

			addAttributes(attrs);
		};

		static XMLData* cloneType() {
			return new AnimationData();
		}

		BARE2D::Texture texture, bumpmap;
		unsigned int	y = 0, width, height, frames;
	};

	class SkeletalAnimationData : public BARE2D::XMLData {
	  public:
		SkeletalAnimationData() {
			nodeName								  = "skeletalAnimation";
			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<std::vector<float>>("angles", BARE2D::AttributeType::VECTOR_FLOAT, &angles),
				new BARE2D::Attribute<std::vector<glm::vec2>>("offsets", BARE2D::AttributeType::VECTOR_VEC2, &offsets),
				new BARE2D::Attribute<std::vector<glm::vec2>>("centresOfRotation",
															  BARE2D::AttributeType::VECTOR_VEC2,
															  &centresOfRotation),
				new BARE2D::Attribute<std::vector<unsigned int>>("limbIndices",
																 BARE2D::AttributeType::VECTOR_UNSIGNED_INT,
																 &limbIndices),
				new BARE2D::Attribute<bool>("repeats", BARE2D::AttributeType::BOOL, &repeats)};

			addAttributes(attrs);
		}

		static XMLData* cloneType() {
			return new SkeletalAnimationData();
		}

		std::vector<float>		  angles;
		std::vector<glm::vec2>	  offsets;
		std::vector<glm::vec2>	  centresOfRotation;
		std::vector<unsigned int> limbIndices;
		bool					  repeats = false;
	};

	class AttackData : public BARE2D::XMLData {
	  public:
		AttackData() {
			nodeName								  = "attack";
			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<unsigned int>("leadInSkeletalAnimationID",
													BARE2D::AttributeType::UNSIGNED_INT,
													&leadInAnimationID),
				new BARE2D::Attribute<unsigned int>("leadOutSkeletalAnimationID",
													BARE2D::AttributeType::UNSIGNED_INT,
													&leadOutAnimationID)};

			addAttributes(attrs);
		};

		static XMLData* cloneType() {
			return new AttackData();
		}

		unsigned int leadInAnimationID, leadOutAnimationID;
		AttackType	 type;
	};

	class MeleeAttackData : public AttackData {
	  public:
		MeleeAttackData() {
			nodeName = "meleeAttack";
			type	 = AttackType::MELEE;

			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<unsigned int>("projectileID", BARE2D::AttributeType::UNSIGNED_INT, &projectileID)};

			addAttributes(attrs);
		};

		static XMLData* cloneType() {
			return new MeleeAttackData();
		}

		unsigned int projectileID;
	};

	class RangedAttackData : public AttackData {
	  public:
		RangedAttackData() {
			nodeName = "rangedAttack";
			type	 = AttackType::RANGED;

			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<unsigned int>("projectileID", BARE2D::AttributeType::UNSIGNED_INT, &projectileID),
				new BARE2D::Attribute<unsigned int>("numProjectiles",
													BARE2D::AttributeType::UNSIGNED_INT,
													&numProjectiles),
				new BARE2D::Attribute<float>("angleWidth", BARE2D::AttributeType::FLOAT, &angleWidth)};

			addAttributes(attrs);
		};

		static XMLData* cloneType() {
			return new RangedAttackData();
		}

		unsigned int projectileID;
		unsigned int numProjectiles = 1;
		float		 angleWidth; // 6 degrees default IN RADIANS!
	};

	class MagicAttackData : public AttackData {
	  public:
		MagicAttackData() {
			nodeName = "magicAttack";
			type	 = AttackType::MAGIC;

			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<BARE2D::LuaScript>("script", BARE2D::AttributeType::SCRIPT, &script)};

			addAttributes(attrs);
		};

		static XMLData* cloneType() {
			return new MagicAttackData();
		}

		BARE2D::LuaScript script;
	};

	class BuffData : public BARE2D::XMLData {
	  public:
		BuffData() {
			nodeName								  = "buff";
			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<BARE2D::LuaScript>("tickScript", BARE2D::AttributeType::SCRIPT, &tickScript),
				new BARE2D::Attribute<BARE2D::Texture>("texture", BARE2D::AttributeType::TEXTURE, &texture),
				new BARE2D::Attribute<std::string>("description", BARE2D::AttributeType::STRING, &description),
				new BARE2D::Attribute<unsigned int>("duration", BARE2D::AttributeType::UNSIGNED_INT, &duration)};

			addAttributes(attrs);
		}

		static XMLData* cloneType() {
			return new BuffData();
		}

		BARE2D::LuaScript tickScript;
		BARE2D::Texture	  texture;
		std::string		  description;
		unsigned int	  duration = 15;
	};

	class FluidData : public BARE2D::XMLData {
	  public:
		FluidData() {
			nodeName								  = "fluid";
			std::vector<BARE2D::AttributeBase*> attrs = {
				new BARE2D::Attribute<BARE2D::Texture>("texture", BARE2D::AttributeType::TEXTURE, &texture),
				new BARE2D::Attribute<float>("gravitationalConstant", BARE2D::AttributeType::FLOAT, &gravConstant),
				new BARE2D::Attribute<float>("idealDensity", BARE2D::AttributeType::FLOAT, &idealDensity),
				new BARE2D::Attribute<float>("viscosity", BARE2D::AttributeType::FLOAT, &viscosity),
				new BARE2D::Attribute<float>("trickle", BARE2D::AttributeType::FLOAT, &trickleConstant),
				new BARE2D::Attribute<unsigned int>("red", BARE2D::AttributeType::UNSIGNED_INT, &red),
				new BARE2D::Attribute<unsigned int>("green", BARE2D::AttributeType::UNSIGNED_INT, &green),
				new BARE2D::Attribute<unsigned int>("blue", BARE2D::AttributeType::UNSIGNED_INT, &blue),
				new BARE2D::Attribute<unsigned int>("alpha", BARE2D::AttributeType::UNSIGNED_INT, &alpha)};

			addAttributes(attrs);
		}

		static XMLData* cloneType() {
			return new FluidData();
		}

		BARE2D::Texture texture;
		float			gravConstant	= 0.7f;
		float			idealDensity	= 1.0f;
		float			viscosity		= 1.0f;
		float			trickleConstant = 0.0f;
		unsigned int	red = 255, green = 0, blue = 255, alpha = 255;
	};
} // namespace XMLModule

#define getTileData(id)				BARE2D::XMLDataManager::getData<XMLModule::TileData>("tile", id)
#define getTileContainerData(id)	BARE2D::XMLDataManager::getData<XMLModule::TileContainerData>("tileContainer", id)
#define getParticleData(id)			BARE2D::XMLDataManager::getData<XMLModule::ParticleData>("particle", id)
#define getEntityData(id)			BARE2D::XMLDataManager::getData<XMLModule::EntityData>("entity", id)
#define getEntityNPCData(id)		BARE2D::XMLDataManager::getData<XMLModule::EntityNPCData>("npc", id)
#define getEntityProjectileData(id) BARE2D::XMLDataManager::getData<XMLModule::EntityProjectileData>("projectile", id)
#define getEntityItemData(id)		BARE2D::XMLDataManager::getData<XMLModule::EntityItemData>("entityItem", id)
#define getItemData(id)				BARE2D::XMLDataManager::getData<XMLModule::ItemData>("item", id)
#define getItemConsumableData(id)	BARE2D::XMLDataManager::getData<XMLModule::ItemConsumableData>("itemConsumable", id)
#define getItemArmourData(id)		BARE2D::XMLDataManager::getData<XMLModule::ItemArmourData>("itemArmour", id)
#define getItemWeaponData(id)		BARE2D::XMLDataManager::getData<XMLModule::ItemWeaponData>("itemWeapon", id)
#define getItemBlockData(id)		BARE2D::XMLDataManager::getData<XMLModule::ItemBlockData>("itemBlock", id)
#define getBiomeData(id)			BARE2D::XMLDataManager::getData<XMLModule::BiomeData>("biome", id)
#define getEraData(id)				BARE2D::XMLDataManager::getData<XMLModule::EraData>("era", id)
#define getLootDropData(id)			BARE2D::XMLDataManager::getData<XMLModule::LootDropData>("lootDrop", id)
#define getLootTableData(id)		BARE2D::XMLDataManager::getData<XMLModule::LootTableData>("lootTable", id)
#define getStructureData(id)		BARE2D::XMLDataManager::getData<XMLModule::StructureData>("structure", id)
#define getQuestData(id)			BARE2D::XMLDataManager::getData<XMLModule::QuestData>("quest", id)
#define getQuestObjectiveData(id)	BARE2D::XMLDataManager::getData<XMLModule::QuestObjectiveData>("questObjective", id)
#define getDialogueQuestionData(id) \
	BARE2D::XMLDataManager::getData<XMLModule::DialogueQuestionData>("dialogueQuestion", id)
#define getDialogueResponseData(id) \
	BARE2D::XMLDataManager::getData<XMLModule::DialogueResponseData>("dialogueResponse", id)
#define getAnimationData(id) BARE2D::XMLDataManager::getData<XMLModule::AnimationData>("animation", id)
#define getSkeletalAnimationData(id) \
	BARE2D::XMLDataManager::getData<XMLModule::SkeletalAnimationData>("skeletalAnimation", id)
#define getAttackData(id)		BARE2D::XMLDataManager::getData<XMLModule::AttackData>("attack", id)
#define getMeleeAttackData(id)	BARE2D::XMLDataManager::getData<XMLModule::MeleeAttackData>("meleeAttack", id)
#define getRangedAttackData(id) BARE2D::XMLDataManager::getData<XMLModule::RangedAttackData>("rangedAttack", id)
#define getMagicAttackData(id)	BARE2D::XMLDataManager::getData<XMLModule::MagicAttackData>("magicAttack", id)
#define getBuffData(id)			BARE2D::XMLDataManager::getData<XMLModule::BuffData>("buff", id)
#define getFluidData(id)		BARE2D::XMLDataManager::getData<XMLModule::FluidData>("fluid", id)
