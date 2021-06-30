#pragma once

#include <memory>

#include "Categories.h"

#include "Entity.h"

#include "XMLDataTypes.h"

#include "Buff.h"

struct NavTile { // Pretty much the same thing as a navmesh
	NavTile() {
	}
	NavTile*			   parent = nullptr;
	std::vector<glm::vec3> nextNodes;
	glm::vec3			   pos;
	float				   h	   = 0; // Heuristic (h(n))   ->   absolute distance from end goal.
	bool				   visited = false;
};

enum class MovementState { IDLE, LOW_VEL, HIGH_VEL, UP, DOWN };

class Item;
class NPCInventory;
class ArmourInventory;
class WeaponInventory;
class Tile;
class ItemArmour;

namespace GLEngine {
	class SpriteFont;
}

namespace CEGUI {
	class EventArgs;
	class FrameWindow;
} // namespace CEGUI

class NPCInventoryWrapper {
	// Holds functions and objects for the Armour/Attack changing screen
  public:
	NPCInventoryWrapper(std::string& UUID, std::shared_ptr<NPCInventory> inventory);
	~NPCInventoryWrapper();

	void destroy();

	void setToDraw(bool& setting);

	void draw(GLEngine::SpriteBatch& sb,
			  GLEngine::SpriteFont&	 sf,
			  float					 x,
			  float					 y); // This draws our armourGrid, attacksGrid, and inventory.
	void update();

	CEGUI::FrameWindow*				 m_window = nullptr;
	std::shared_ptr<ArmourInventory> m_armourGrid; // These InventoryBase classes allow us to draw, disable resizing, etc.
	std::shared_ptr<WeaponInventory> m_attacksGrid;
	std::shared_ptr<NPCInventory>	 m_inventory;
};

class EntityNPC : public Entity {
  public:
	EntityNPC(glm::vec2 pos, unsigned int layer, unsigned int id, SaveDataTypes::MetaData data, bool loadTex);
	EntityNPC(glm::vec2 pos, unsigned int layer, EntityIDs id, SaveDataTypes::MetaData data, bool loadTex);
	virtual ~EntityNPC();
	void dispose(); // Deletes some stuff that shouldn't be deleted if a copy operation takes place!

	void init(unsigned int id);

	virtual void draw(GLEngine::SpriteBatch& sb, float time, int layerDifference, float xOffset) override;
	virtual void drawNormal(GLEngine::SpriteBatch& sb, float time, int layerDifference, float xOffset) override;

	virtual void display(GLEngine::SpriteBatch& sb,
						 glm::vec2				position,
						 float					scale); // Displays the entity in UI style.

	void onTalk() {
	}
	void onTrade() {
	}

	virtual void collideWithTiles() override;
	virtual bool collideWithOther(Entity* other) override;
	virtual void onUpdate(float timeStep, unsigned int selfIndex);

	/// Getters
	Categories::Faction getFaction() const {
		return m_faction;
	}
	float getHealth() const {
		return m_health;
	}
	SaveDataTypes::EntityNPCData getNPCSaveData();

	// Inventory
	void								  giveItem(Item* item);
	std::shared_ptr<NPCInventory>&		  getInventory();
	std::shared_ptr<NPCInventoryWrapper>& getInventoryWrapper();
	void setInventory(std::shared_ptr<NPCInventory>& inventory, std::shared_ptr<NPCInventoryWrapper>& wrapper);

	// Combat
	void die();
	void applyDamage(float damage); // This takes armour into account
	void applyKnockback(float knockback, glm::vec2 origin);
	void applyBuff(unsigned int id); // This constructs a new debuff and adds it to the vector.
	void applyFlinch(); // Applies flinch animation and sets current attack to -1 if current attack is active. (!= -1)

	// AI
	void setAITarget(unsigned int selfIndex); /// TODO: Make this work with certain quests, etc.

	// Inventory stuff
	virtual bool event_reskin(const CEGUI::EventArgs& e); // Just calls reskinLimbs, but as a CEGUI Event

  protected:
	// Pathfinding
	void	 updateMovement();
	void	 pathfindToTarget(glm::vec3 target, bool goLeft);
	bool	 fitsOnTile(Tile* t, bool needsFloor = false);
	NavTile* expandTile(glm::vec3 pos, int jumpHeight, glm::vec2 size, NavTile* parent, glm::vec3 target);
	void	 calculateCost(NavTile* tile, glm::vec3 target); // Calculates a cost of a single path from an endnode
	void	 addToFrontier(
			NavTile*			   tile,
			std::vector<NavTile*>& frontier); // Makes sure there are no copies, and handles them if there are.
	std::vector<glm::vec3> m_targets;
	unsigned int		   m_curTarget = 0;

	void								activateAttack(unsigned int attackID);
	void								updateAttack();
	int									m_currentAttackID	= -1;
	AnimationModule::SkeletalAnimation* m_currentAttackAnim = nullptr;
	bool								m_leadingIntoAttack = true;

	virtual void onTick() override;

	void initLimbs();	// Initializes limbs and animations.
	void reskinLimbs(); // Reskins limbs, useful when equipped armour changes.

	// Animation (TODO: Add animation to entity XML)
	AnimationModule::Body				m_body;
	AnimationModule::SkeletalAnimation *m_idleAnimation = nullptr, *m_lowVelAnimation = nullptr,
									   *m_highVelAnimation = nullptr, *m_upAnimation = nullptr,
									   *m_downAnimation = nullptr, *m_flinchAnimation = nullptr;

	// Fall damage
	float m_fallenDistance	= 0.0f;
	bool  m_takesFallDamage = true;

	// Dialogue/Trade
	unsigned int		m_tradeTable = (unsigned int)-1;
	unsigned int		m_dialogueID = (unsigned int)-1;
	Categories::Faction m_faction;

	// Loot
	//DropDatum m_lootTableStart = DropDatum(0, (ItemIDs)(0), 0, 0, -1.0f);
	unsigned int m_lootRolls = 0; // How many times the table is rolled on death
	/// TODO: Re-enable loot/drops

	// Combat
	float			  m_maxHealth		= 100.0f;
	float			  m_health			= m_maxHealth;
	bool			  m_isDead			= false;
	bool			  m_canDie			= true;
	bool			  m_flying			= false;				 // Can it fly?
	glm::vec2		  m_aimingDirection = glm::vec2(1.0f, 0.0f); // normalized vector pointing in the direction of fire.
	std::vector<Buff> m_buffs;

	// Movement
	float		  m_runSpeed;
	float		  m_jumpHeight; // Maximum height = (m_jumpHeight^2)/(2*1.225/60)
	MovementState m_state = MovementState::IDLE;
	// m_jumpHeight = sqrt(2*(1.225/60)*(maximum height))

	// Inventory
	std::shared_ptr<NPCInventory>		 m_inventory;
	std::shared_ptr<NPCInventoryWrapper> m_armourWeaponsInventory;
};
