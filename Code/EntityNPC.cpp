#include "EntityNPC.h"

#include "World.h"
#include "Tile.h"

#include "EntityItem.h"
#include "EntityProjectile.h"
#include "Entities/EntityFunctions.h"

#include "NPCInventory.h"

#include "Attack.h"
#include "ItemArmour.h"

#include "XMLData.h"
#include "Factory.h"

EntityNPC::EntityNPC(glm::vec2 pos, unsigned int layer, unsigned int id, SaveDataTypes::MetaData data, bool loadTex) : Entity(pos, layer, SaveDataTypes::MetaData()) {
	m_id = id;

	init(m_id);
}

EntityNPC::EntityNPC(glm::vec2 pos, unsigned int layer, EntityIDs id, SaveDataTypes::MetaData data, bool loadTex) : Entity(pos, layer, SaveDataTypes::MetaData()) {
	m_id = (unsigned int)id;

	init(m_id);
}

void EntityNPC::init(unsigned int id) {
	m_type = XMLModule::EntityType::NPC;

	XMLModule::EntityNPCData d = XMLModule::XMLData::getEntityNPCData(m_id);

	m_size = d.size;
	m_takesFallDamage = d.isDamagedByFalls;
	m_canDie = !d.isInvincible;
	m_runSpeed = d.speed;
	m_jumpHeight = std::sqrt(d.jumpHeight * 2.0f * (1.225f / 60.0f));
	m_maxHealth = d.maxHealth;
	m_faction = (Categories::Faction)d.faction;
	m_gravity = d.gravity;

	m_metaData = d.getMetaData();

	m_health = m_maxHealth;

	initLimbs();

	m_inventory = std::make_shared<NPCInventory>(15.0f, m_UUID);
	m_inventory->init();
}

void EntityNPC::initLimbs() {
	/// TODO: Expand XML a bit

	// Load XML:
	XMLModule::EntityNPCData data = XMLModule::XMLData::getEntityNPCData(m_id);

	// Create limbs with animations. (Loads animations as it goes)
	for(unsigned int i = 0; i < data.skinAnimationIDs.size(); i++) {
		m_body.addLimb(AnimationModule::Limb(AnimationModule::Animation(data.skinAnimationIDs[i]), i));
	}

	// Load skeletal animations
	m_currentAttackAnim = new AnimationModule::SkeletalAnimation(); // Just allocate the memory.

	if(data.idleAnimationID != -1) {
		m_idleAnimation = new AnimationModule::SkeletalAnimation(data.idleAnimationID);
		m_idleAnimation->setToLoop(true);
	}

	if(data.lowVelAnimationID != -1) {
		m_lowVelAnimation = new AnimationModule::SkeletalAnimation(data.lowVelAnimationID);
		m_lowVelAnimation->setToLoop(true);
	}

	if(data.flinchAnimationID != -1) {
		m_flinchAnimation = new AnimationModule::SkeletalAnimation(data.flinchAnimationID);
		m_flinchAnimation->setToLoop(true);
	}
	// Load the rest...

	// Activate idle animation to start
	m_body.activateAnimation(m_idleAnimation);



	//////// For debugging purposes

	/*AnimationModule::Animation torsoIdle;
	torsoIdle.init(0);

	AnimationModule::Limb torso;
	torso.init(torsoIdle, 0);

	m_idleAnimation.init(1);


	torso.activateSkeletalAnimation(m_idleAnimation);

	m_limbs.push_back(torso);*/

	//////// XXXXXXXXXXXXXXXXXXXXXX
}

EntityNPC::~EntityNPC() {

}

void EntityNPC::dispose() {
	if(m_idleAnimation) delete m_idleAnimation; // These must be deleted only when a copy operation doesn't take place. These pointers are also stored in the m_body, so we need to be careful with how we delete em.
	if(m_lowVelAnimation) delete m_lowVelAnimation; // (Copying the body object copies the pointers into it, but if the pointers are deleted, then they point to a weird place)
	if(m_highVelAnimation) delete m_highVelAnimation;
	if(m_upAnimation) delete m_upAnimation;
	if(m_downAnimation) delete m_downAnimation;
}

void EntityNPC::draw(GLEngine::SpriteBatch& sb, float time, int layerDifference, float xOffset) {
	if(m_draw) {
		glm::vec4 destRect = glm::vec4(m_position.x + (xOffset * CHUNK_SIZE), m_position.y, m_size.x, m_size.y);

		float depth = getDepth();

		m_body.draw(sb, GLEngine::ColourRGBA8(255, 255, 255, 255), destRect, depth);

		onDraw(sb, time, layerDifference, xOffset);
	}
}

void EntityNPC::drawNormal(GLEngine::SpriteBatch& sb, float time, int layerDifference, float xOffset) {
	if(m_draw) {
		glm::vec4 destRect = glm::vec4(m_position.x + (xOffset * CHUNK_SIZE), m_position.y, m_size.x, m_size.y);

		float depth = getDepth();
		
		m_body.drawNormal(sb, destRect, depth);
	}
}

bool EntityNPC::collideWithOther(Entity* other) {
	bool collisionPossible = false;

	glm::vec2 otherPos = other->getPosition();
	glm::vec2 otherSize = other->getSize();

	float xDist = (otherPos.x + otherSize.x / 2.0f) - (m_position.x + m_size.x / 2.0f);

	if(std::abs(xDist) > (otherSize.x + m_size.x) / 2.0f) {
		return false; // Collision will no longer be possible
	}

	float yDist = (otherPos.y + otherSize.y / 2.0f) - (m_position.y + m_size.y / 2.0f);

	if(std::abs(yDist) > (otherSize.y + m_size.y) / 2.0f) {
		return true; // As shown above, collision would be possible on the X axis, so return true.
	} // Else, we are colliding.

	if(other->getType() == XMLModule::EntityType::NPC) {
		EntityNPC* ent = dynamic_cast<EntityNPC*>(other);

		float depth = xDist - (m_size.x / 2.0f + otherSize.x / 2.0f);
		float force = (depth / 2.0f) * (depth / 2.0f) / ((m_size.x / 2.0f + otherSize.x / 2.0f) * 512.0f);

		m_position.x -= force;
		ent->setPosition(glm::vec2(ent->getPosition().x + force, ent->getPosition().y));
	} else if(other->getType() == XMLModule::EntityType::ITEM) {
		EntityItem* ent = dynamic_cast<EntityItem*>(other);

		float dist = std::sqrt(xDist * xDist + yDist * yDist);

		if(dist <= 3.0f) { // Move to NPC
			other->setPosition(other->getPosition() + glm::vec2(xDist / 2, yDist / 2));
			if(dist <= 1.0f) { // Add to inventory
				m_inventory->addItem(ent->getItem());
				Factory::getEntityManager()->queueEntityToRemove(this);
			}
		}
	} else if(other->getType() == XMLModule::EntityType::PROJECTILE) {
		EntityProjectile* ent = dynamic_cast<EntityProjectile*>(other);

		if(ent->isActive()) {
			applyDamage(ent->getDamage()); // This will process armour and stuff.
			applyKnockback(ent->getKnockback(), ent->getPosition());
			for(unsigned int i = 0; i < ent->getBuffs().size(); i++) {
				applyBuff(ent->getBuffs()[i]);
			}
			applyFlinch();
			ent->setActive(false);
		}
	}

	return true;
}

void EntityNPC::collideWithTiles(World* world) {
	/// Tile collision
	{
		/// Many thanks to Ben Arnold. He taught me almost everything I know about programming through his Youtube channel, "Makinggameswithben"
		/// This is just a small piece of code that handles and reacts to dynamic rectangle and tile collisions
		std::vector<glm::vec2> collideTilePositions;
		std::vector<glm::vec2> groundTilePositions;

		float x = m_position.x, y = m_position.y, width = m_size.x, height = m_size.y;

		glm::vec2 posBL(x, y);
		glm::vec2 posBR(x + width, y);
		glm::vec2 posTL(x, y + height);
		glm::vec2 posTR(x + width, y + height);

		const float testVar = 1.0f / 8.0f;

		// Check for ground/ceiling

		// Bottom right corner
		checkTilePosition(world, groundTilePositions,
		                  posBR.x - testVar,
		                  posBR.y);


		// Bottom left corner
		checkTilePosition(world, groundTilePositions,
		                  posBL.x + testVar,
		                  posBL.y);


		// Top right corner
		checkTilePosition(world, groundTilePositions,
		                  posTR.x - testVar,
		                  posTR.y);


		// Top left corner
		checkTilePosition(world, groundTilePositions,
		                  posTL.x + testVar,
		                  posTL.y);

		// Top/Bottom sides
		for(float yMod = 0.0f; yMod < height; yMod += height) {
			for(float xMod = 0.0f; xMod < width - (2 * testVar); xMod += 1.0f) {
				checkTilePosition(world, groundTilePositions,
				                  posBL.x + xMod + testVar,
				                  posBL.y + yMod);
			}
		}



		// Check the sides (not ground)
		checkTilePosition(world, collideTilePositions,
		                  posBR.x,
		                  posBR.y + testVar);

		checkTilePosition(world, collideTilePositions,
		                  posBL.x,
		                  posBL.y + testVar);

		checkTilePosition(world, collideTilePositions,
		                  posTL.x,
		                  posTL.y - testVar);

		checkTilePosition(world, collideTilePositions,
		                  posTR.x,
		                  posTR.y - testVar);

		// Sides
		for(float xMod = 0; xMod <= width; xMod += width) {
			for(float yMod = 0.0f; yMod < height - (2 * testVar); yMod += 1.0f) {
				checkTilePosition(world, collideTilePositions,
				                  posBL.x + xMod,
				                  posBL.y + yMod + testVar);
			}
		}

		/// Collision prediction time!

		/*float increment = 0.1f;
		int signX = (m_velocity.x > 0.0f) ? 1 : -1;
		int signY = (m_velocity.y > 0.0f) ? 1 : -1;

		std::vector<glm::vec2> predictiveTiles;

		for(int i = 0; i < std::abs(m_velocity.x) / (increment * width) + increment; i++) {
			for(int j = 0; j < std::abs(m_velocity.y) / (increment * height) + increment; j++) {
				glm::vec2 p_posBL(x + (i * increment * width)*signX, y + (j * increment * height)*signY);
				glm::vec2 p_posBR(x + (i * increment * width)*signX + width, y + (j * increment * height)*signY);
				glm::vec2 p_posTL(x + (i * increment * width)*signX, y + (j * increment * height)*signY + height);
				glm::vec2 p_posTR(x + (i * increment * width)*signX + width, y + (j * increment * height)*signY + height);

				checkTilePosition(world, predictiveTiles,
				                  p_posBR.x,
				                  p_posBR.y);

				checkTilePosition(world, predictiveTiles,
				                  p_posBL.x,
				                  p_posBL.y);

				checkTilePosition(world, predictiveTiles,
				                  p_posTL.x,
				                  p_posTL.y);

				checkTilePosition(world, predictiveTiles,
				                  p_posTR.x,
				                  p_posTR.y);
			}
		}*/

		for(unsigned int i = 0; i < collideTilePositions.size(); i++) {
			collideWithTile(collideTilePositions[i], false);
		}

		//if(predictiveTiles.size() > 0) collideWithTile(predictiveTiles[0], false);

		for(unsigned int i = 0; i < groundTilePositions.size(); i++) {
			collideWithTile(groundTilePositions[i], true);
		}
	}
}

void EntityNPC::onUpdate(World* world, float timeStep, unsigned int selfIndex) {
	m_body.update();

	if(m_takesFallDamage) {
		if(m_velocity.y < 0.0f) {
			m_fallenDistance += -m_velocity.y * timeStep;
		} else {
			if(m_fallenDistance > 4.0f) {
				m_health -= std::pow(m_fallenDistance - 4.0f, 1.5f) * 0.08f;
			}
			m_fallenDistance = 0.0f;
		}
	}

	updateAttack();
	updateMovement(world);
	updateLightLevel(world);

	setAITarget(world, selfIndex);

	m_inventory->update();

	if(m_health <= 0.0f) {
		die(world);
	}

	/*if(m_velocity.x > MAX_SPEED * m_inventory->getSpeedMultiplier() * std::pow(m_stamina, 0.4f)) {
	    m_velocity.x = MAX_SPEED * m_inventory->getSpeedMultiplier() * std::pow(m_stamina, 0.4f);
	} else if(m_velocity.x < -MAX_SPEED * m_inventory->getSpeedMultiplier() * std::pow(m_stamina, 0.4f)) {
	    m_velocity.x = -MAX_SPEED * m_inventory->getSpeedMultiplier() * std::pow(m_stamina, 0.4f);
	}*/
}

void EntityNPC::onTick(World* world) {
	m_body.tick();

	for(unsigned int i = 0; i < m_buffs.size(); i++) {
		m_buffs[i].tick();
	}
}

SaveDataTypes::EntityNPCData EntityNPC::getNPCSaveData() {
	SaveDataTypes::EntityNPCData ret;
	ret.id = m_id;
	ret.velocity = m_velocity;
	ret.position = m_position;
	ret.layer = m_layer;
	ret.md = m_metaData;

	ret.health = m_health;
	ret.inventory = m_inventory->getInventorySaveData();

	return ret;
}

void EntityNPC::giveItem(Item* item) {
	if(m_inventory) {
		m_inventory->addItem(item);
	} else {
		Logger::getInstance()->log("ERROR: Entity inventory not initialized, could not give item", true);
	}
}

std::shared_ptr<NPCInventory> EntityNPC::getInventory() {
	return m_inventory;
}

void EntityNPC::setInventory(std::shared_ptr<NPCInventory> inventory) {
	m_inventory = inventory;
}

void EntityNPC::die(World* world) {
	if(m_canDie) {
		/* if(m_lootTableStart.getRarity() != -1.0f) {
		     for(int i = 0; i < m_lootRolls; i++) {
		         // (unsigned int)m_lootTableStart.roll()
		         //world->queueEntityToAdd(item);
		     }
		 }*/ /// TODO: Re-enable Drops
		m_isDead = true;
	} else {
		m_isDead = false; // Just in case
	}
}

void EntityNPC::updateMovement(World* world) {
	EntityFunctions::basic_straight(m_controls, m_targets, m_curTarget, this); /// TODO: Make entities able to have different AI

	if(m_controls[0]) { // UP
		if(m_onGround) {
			m_velocity.y = m_jumpHeight;
			m_onGround = false;
		}
	}
	if(m_controls[1]) { // DOWN
		/// TODO: implement crouching
	}
	if(m_controls[2] && !m_controls[3]) { // LEFT
		if(std::abs(m_velocity.x) < MAX_SPEED) {
			m_velocity.x -= m_runSpeed;
		}
	} else if(m_controls[3] && !m_controls[2]) { // RIGHT
		if(std::abs(m_velocity.x) < MAX_SPEED) {
			m_velocity.x += m_runSpeed;
		}
		if(m_state != MovementState::LOW_VEL) {
			m_state = MovementState::LOW_VEL;
			m_body.changeAnimation(m_lowVelAnimation);
		}
	} else {
		m_velocity.x *= 0.9f;
		if(m_velocity.x < 0.1f) {
			if(m_state != MovementState::IDLE) {
				m_state = MovementState::IDLE;
				m_body.changeAnimation(m_idleAnimation);
			}
		}
	}
	if(m_controls[4]) { // Backwards (layer++)
		moveDownLayer(world);
	} else if(m_controls[5]) { // Forwards (layer--)
		moveUpLayer(world);
	}

	if(m_velocity.x > MAX_SPEED) {
		m_velocity.x = MAX_SPEED;
	} else if(m_velocity.x < -MAX_SPEED) {
		m_velocity.x = -MAX_SPEED;
	}

	for(int i = 0; i < 6; i++) {
		m_controls[i] = false;
	}
}

void EntityNPC::pathfindToTarget(World* world, glm::vec3 target, bool goLeft) {
	/**
	    1. Set outer bounds (how far away the algorithm will expand before deciding there's no path)
	    2. 'Link' all accessible tiles as "NavTile"s, effectively creating a system of nodes
	    3. Calculate costs of each path to target coords
	    4. Use the shortest route, adding each NavTile's coords to AI targets
	    5. Return!
	*/

	m_targets.clear();

	/// TODO: Implement jumping over gaps!

	// 1. Set outer bounds
	int outerBoundXMin = m_position.x < target.x ? m_position.x : target.x; // The lesser of the two
	int outerBoundXMax = m_position.x > target.x ? m_position.x : target.x; // The greater of the two


	// 2. Link accessible tiles from start
	NavTile* start = nullptr;

	int jumpHeight = std::floor((60.0f * m_jumpHeight * m_jumpHeight) / 2.45f + 0.1f);

	start = expandTile(world, glm::vec3((int)m_position.x, (int)m_position.y, m_layer), jumpHeight, m_size, nullptr, target);

	// Check to make sure if there's a path:
	if(!start) {
		/// TODO: Implement wandering.
		return;
	}


	// 4. Use shortest route, adding each NavTile coords to targets

	/**
	    Construct 'frontier' (just the child tiles of start)

	    Repeat until a path is found (Just one):
	        Choose smallest h value on frontier, remove from frontier and add children to frontier (expand node)
	        if tile with smallest h value is target, go backwards. Start with the tile with smallest h, and add each parent to targets.

	*/
	std::vector<NavTile*> frontier;
	frontier.push_back(start);

	bool pathFound = false;
	NavTile* targetNavTile = nullptr;

	// Expand start tile, add em to frontier, then expand nextTile with lowest h, add em to frontier, repeat.


	unsigned int searchIteration = 0;
	unsigned int highestH = 0;

	while(!pathFound && highestH < 100 && searchIteration < 50 && frontier.size() > 0) {

		// Choose lowest h value
		int lowestIndex = 0;
		for(unsigned int i = 0; i < frontier.size(); i++) {
			if(frontier[lowestIndex]->h > frontier[i]->h) lowestIndex = i;
			if(frontier[i]->h > highestH) highestH = frontier[i]->h;
		}

		if(frontier[lowestIndex]->pos == target) {
			// We found a path!
			targetNavTile = frontier[lowestIndex];
			pathFound = true;
		}

		// Expand and remove (if there are no nextNodes, it will simply be removed)
		for(unsigned int i = 0; i < frontier[lowestIndex]->nextNodes.size(); i++) {
			addToFrontier(expandTile(world, frontier[lowestIndex]->nextNodes[i], jumpHeight, m_size, frontier[lowestIndex], target), frontier);
		}
		for(unsigned int i = lowestIndex; i < frontier.size() - 1; i++) {
			frontier[i] = frontier[i + 1];
		}
		frontier.pop_back();

		searchIteration++;
	}


	if(pathFound) {
		std::vector<glm::vec3> targetsReversed; // Targets to go to, reversed :)

		while(targetNavTile) {
			glm::vec3 p(targetNavTile->pos);
			targetsReversed.push_back(p);
			targetNavTile = targetNavTile->parent;
		}

		for(unsigned int i = 1; i < targetsReversed.size(); i++) { // We don't need the very last one, that's just on top of the entity
			m_targets.push_back(targetsReversed[targetsReversed.size() - 1 - i]);
		}
	}

	return;
}

bool EntityNPC::fitsOnTile(World* world, Tile* t, bool needsFloor) {
	bool fits = true;
	int start = needsFloor ? -1 : 0;

	for(int y = start; fits && y < m_size.y; y++) {
		for(int x = 0; fits && x < m_size.x; x++) {
			Tile* tmp = world->getTile(t->getPosition().x + x, t->getPosition().y + y, t->getLayer());
			if(tmp) {
				if(y >= 0) {
					if(tmp->isSolid()) fits = false;
				} else {
					if(!tmp->isSolid()) fits = false;
				}
			}
		}
	}
	return fits;
}

NavTile* EntityNPC::expandTile(World* world, glm::vec3 pos, int jumpHeight, glm::vec2 size, NavTile* parent, glm::vec3 target) {

	NavTile* ret = new NavTile;
	ret->parent = parent;
	ret->pos = pos;

	// Check Left
	Tile* left = world->getTile(pos.x - 1, pos.y, pos.z);
	if(fitsOnTile(world, left, true)) {
		glm::vec3 p(left->getPosition().x, left->getPosition().y, left->getLayer());
		if(!parent || p != parent->pos) {
			ret->nextNodes.push_back(p);
		}
	} else {
		for(int i = 0; i < jumpHeight; i++) { /// TODO: Implement some sort of 'safe' fall distance
			Tile* leftDown = world->getTile(pos.x - 1, pos.y - i, pos.z); // Check tiles below so that we can fall distances.
			if(leftDown) {
				if(fitsOnTile(world, leftDown)) {
					glm::vec3 p(leftDown->getPosition().x, leftDown->getPosition().y, leftDown->getLayer());
					if(!parent || p != parent->pos) {
						ret->nextNodes.push_back(p);
					}
				}
			}
		}
	}

	Tile* right = world->getTile(pos.x + 1, pos.y, pos.z);
	if(fitsOnTile(world, right, true)) {
		glm::vec3 p(right->getPosition().x, right->getPosition().y, right->getLayer());
		if(!parent || p != parent->pos) {
			ret->nextNodes.push_back(p);
		}
	} else {
		for(int i = 0; i < jumpHeight; i++) { /// TODO: Implement some sort of 'safe' fall distance
			Tile* rightDown = world->getTile(pos.x + 1, pos.y - i, pos.z); // Check tiles below so that we can fall distances.
			if(rightDown) {
				if(fitsOnTile(world, rightDown)) {
					glm::vec3 p(rightDown->getPosition().x, rightDown->getPosition().y, rightDown->getLayer());
					if(!parent || p != parent->pos) {
						ret->nextNodes.push_back(p);
					}
				}
			}
		}
	}


	if(pos.z < WORLD_DEPTH - 1) {
		// Check Behind
		Tile* back = world->getTile(pos.x, pos.y, pos.z + 1);
		if(fitsOnTile(world, back, true)) {
			glm::vec3 p(back->getPosition().x, back->getPosition().y, back->getLayer());
			if(!parent || p != parent->pos) {
				ret->nextNodes.push_back(p);
			}
		} else {
			for(int i = 0; i < jumpHeight; i++) { /// TODO: Implement some sort of 'safe' fall distance
				Tile* backDown = world->getTile(pos.x, pos.y - i, pos.z + 1); // Check tiles below so that we can fall distances.
				if(backDown) {
					if(fitsOnTile(world, backDown)) {
						glm::vec3 p(backDown->getPosition().x, backDown->getPosition().y, backDown->getLayer());
						if(!parent || p != parent->pos) {
							ret->nextNodes.push_back(p);
						}
					}
				}
			}
		}
	}


	if(pos.z > 0) {
		// Check In Front
		Tile* front = world->getTile(pos.x, pos.y, pos.z - 1);
		if(fitsOnTile(world, front, true)) {
			glm::vec3 p(front->getPosition().x, front->getPosition().y, front->getLayer());
			if(!parent || p != parent->pos) {
				ret->nextNodes.push_back(p);
			}
		} else {
			for(int i = 0; i < jumpHeight; i++) { /// TODO: Implement some sort of 'safe' fall distance
				Tile* frontDown = world->getTile(pos.x, pos.y - i, pos.z - 1); // Check tiles below so that we can fall distances.
				if(frontDown) {
					if(fitsOnTile(world, frontDown)) {
						glm::vec3 p(frontDown->getPosition().x, frontDown->getPosition().y, frontDown->getLayer());
						if(!parent || p != parent->pos) {
							ret->nextNodes.push_back(p);
						}
					}
				}
			}
		}
	}

	// Check up!
	for(int i = 0; i < jumpHeight; i++) {
		Tile* up = world->getTile(pos.x, pos.y + i, pos.z);
		if(up) {
			if(fitsOnTile(world, up, false)) {
				glm::vec3 p(up->getPosition().x, up->getPosition().y, up->getLayer());
				if(!parent || p != parent->pos) {
					ret->nextNodes.push_back(p);
				}
			} else {
				break; // Something is blocking our jump
			}
		}
	}

	calculateCost(world, ret, target);

	return ret;
}

void EntityNPC::calculateCost(World* world, NavTile* tile, glm::vec3 target) {
	tile->h = world->getDistance(glm::vec2(tile->pos.x, tile->pos.y), glm::vec2(target.x, target.y)) + std::abs(tile->pos.z - target.z); /// TODO: Implement crossover
	if(tile->parent) tile->h += tile->parent->h / 2.0f; // The distance to the entity is more important than the path. Plus, this improves performance.
}

void EntityNPC::addToFrontier(NavTile* tile, std::vector<NavTile*>& frontier) {
	bool clone = false;

	for(unsigned int i = 0; i < frontier.size(); i++) {
		if(frontier[i]->pos == tile->pos) {
			if(frontier[i]->h > tile->h) { // Tile is a better route to get to frontier[i]
				frontier[i] = tile;
			}
			clone = true;
		}
	}

	if(clone) return;
	frontier.push_back(tile);
}

void EntityNPC::setAITarget(World* world, unsigned int selfIndex) {
	unsigned int entCount = Factory::getEntityManager()->getEntities().size();

	EntityNPC* targetL = nullptr;
	EntityNPC* targetR = nullptr;

	for(int i = (int)selfIndex - 1; i > -entCount - selfIndex; i--) {
		// This loop will run negatively, for length of world->getEntities().size() MAX. Often breaks out of loop early
		unsigned int normalized = (i + entCount * 2) % entCount; // index from 0-size

		if(normalized == selfIndex) continue;

		Entity* target = Factory::getEntityManager()->getEntities()[normalized];
		if(target->getType() == XMLModule::EntityType::NPC) {
			EntityNPC* targetNPC = dynamic_cast<EntityNPC*>(target);

			if(getFaction() > Categories::Faction::NEUTRAL &&
			        targetNPC->getFaction() <= Categories::Faction::NEUTRAL) {
				targetL = targetNPC;
				break;
			}
		}
	}

	for(int i = (int)selfIndex + 1; i < entCount + selfIndex; i++) { // Runs at most once through every entity.
		unsigned int normalized = i % entCount; // index from 0-size;

		if(normalized == selfIndex) continue;

		Entity* target = Factory::getEntityManager()->getEntities()[normalized];
		if(target->getType() == XMLModule::EntityType::NPC) {
			EntityNPC* targetNPC = dynamic_cast<EntityNPC*>(target);

			if(getFaction() > Categories::Faction::NEUTRAL &&
			        targetNPC->getFaction() <= Categories::Faction::NEUTRAL) {
				targetR = targetNPC;
				break;
			}
		}
	}

	if(targetL || targetR) {
		float distToTargetL = targetL ? world->getDistance(m_position, targetL->getPosition()) : (unsigned int) - 1;
		float distToTargetR = targetR ? world->getDistance(m_position, targetR->getPosition()) : (unsigned int) - 1;

		if(distToTargetL < distToTargetR) {
			pathfindToTarget(world, glm::vec3((int)targetL->getPosition().x, (int)targetL->getPosition().y, targetL->getLayer()), true);
		} else {
			pathfindToTarget(world, glm::vec3((int)targetR->getPosition().x, (int)targetR->getPosition().y, targetR->getLayer()), false);
		}
	}
}

void EntityNPC::activateAttack(unsigned int attackID) {
	// This needs to set the m_curAttack variable, as well as start the lead in animation.
	// The update function will make sure that, once the lead in is done, the attack is executed and the lead out is started.
	CombatModule::Attack attack(attackID, this);

	*m_currentAttackAnim = attack.getLeadIn();
	m_currentAttackAnim->setToLoop(false); // If it loops, then we'll never actually execute the attack
	m_currentAttackAnim->restart();

	m_body.activateAnimation(m_currentAttackAnim);

	m_currentAttackID = attackID;
	m_leadingIntoAttack = true; // This tells the program that we're executing the first(!) animation, and haven't executed the attack yet.
}

void EntityNPC::updateAttack() {
	// This needs to check if there's an attack "running" and exit if not
	// If there is an attack, it needs to check if the lead in is finished. If so, execute the attack and set m_leadingIntoAttack. If not, exit
	// If the lead out is finished, then reset m_currentAttackID and set the skeletal animation to the idle animation for all limbs

	// Check if attack is running
	if(m_currentAttackID >= 0) {
		// It is. Now check if the lead-in is finished:
		if(m_currentAttackAnim->isFinished()) {
			// It is. Now check if we're leading in, or out
			if(m_leadingIntoAttack) {
				// We are. Now, execute the attack and start the lead in, while setting m_leadingIntoAttack.
				CombatModule::Attack* attack = CombatModule::createAttack(m_currentAttackID, this);

				attack->execute(m_aimingDirection);

				*m_currentAttackAnim = attack->getLeadOut();
				m_currentAttackAnim->setToLoop(false);
				m_currentAttackAnim->restart();

				m_body.changeAnimation(m_currentAttackAnim);

				m_leadingIntoAttack = false;
			} else {
				// We're not. So, set all limbs to idle and m_currentAttackID to -1 (not attacking)
				m_body.changeAnimation(m_idleAnimation);
				m_currentAttackID = -1;
			}
		}
	}
}

void EntityNPC::applyDamage(float damage) {
	float appliedDamage = 0.0f;

	float averageThreshold = 0.0f;
	float averageResistance = 0.0f;

	for(unsigned int i = 0; i < m_equippedArmour.size(); i++) {
		averageThreshold += m_equippedArmour[i]->getThreshold();
		averageResistance += m_equippedArmour[i]->getResistance();
	}

	averageThreshold /= m_equippedArmour.size();
	averageResistance /= m_equippedArmour.size();

	appliedDamage = std::max(damage - averageThreshold, 0.0f);
	appliedDamage *= (1.0f - averageResistance);

	m_health -= appliedDamage;
}

void EntityNPC::applyKnockback(float knockback, glm::vec2 origin) {
	m_velocity += glm::vec2(knockback) * glm::normalize(m_position + m_size / glm::vec2(2.0f) - origin);
}

void EntityNPC::applyBuff(unsigned int id) {
	m_buffs.emplace_back(this, id);
}

void EntityNPC::applyFlinch() {
	if(m_currentAttackID >= 0) {
		m_body.changeAnimation(m_flinchAnimation);
		m_currentAttackID = -1;
	}
}
