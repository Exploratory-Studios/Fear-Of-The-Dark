#include "EntityProjectile.h"

#include "XMLData.h"

#include "Factory.h"

EntityProjectile::EntityProjectile(glm::vec2 pos, unsigned int layer, unsigned int id, SaveDataTypes::MetaData data, bool loadTex) : Entity(pos, layer, SaveDataTypes::MetaData()) {
	m_id = id;

	init();

	if(loadTex) {
		//loadTexture();
	}
}

EntityProjectile::EntityProjectile(glm::vec2 pos, unsigned int layer, EntityIDs id, SaveDataTypes::MetaData data, bool loadTex) : Entity(pos, layer, SaveDataTypes::MetaData()) {
	m_id = (unsigned int)id;

	init();

	if(loadTex) {
		//loadTexture();
	}
}

void EntityProjectile::init() {
	m_type = XMLModule::EntityType::PROJECTILE;

	XMLModule::EntityProjectileData d = XMLModule::XMLData::getEntityProjectileData(m_id);

	m_size = d.size;
	m_updateScriptId = d.updateScript.getID();
	m_tickScriptId = d.tickScript.getID();
	m_speed = d.speed;
	m_damage = d.damage;
	m_collideWithBlocks = d.collides;
	m_gravity = d.gravity;
	m_lifeTime = d.lifeTime;
	m_knockback = d.knockback;
	m_buffIDs = d.buffIDs;

	m_metaData = d.getMetaData();

	m_anim.init(d.animationID);
}

EntityProjectile::~EntityProjectile() {
	//dtor
}

void EntityProjectile::collideWithTiles() {
	if(m_collideWithBlocks) {
		std::vector<glm::vec2> positions;

		checkTilePosition(positions,
		                  m_position.x + m_size.x / 2.0f,
		                  m_position.y + m_size.y / 2.0f);

		if(positions.size() > 0) {
			// We did collide, destroy this
			Factory::getEntityManager()->queueEntityToRemove(getUUID());
			m_active = false;
		}
	}
}

bool EntityProjectile::collideWithOther(Entity* other) {
	bool collisionPossible = false;

	glm::vec2 otherPos = other->getPosition();
	glm::vec2 otherSize = other->getSize();

	float xDist = (otherPos.x + otherSize.x / 2.0f) - (m_position.x + m_size.x / 2.0f);

	if(std::abs(xDist) > (otherSize.x + m_size.x) / 2.0f) {
		return false; // Collision will no longer be possible
	}

	if(m_active) {
		float yDist = (otherPos.y + otherSize.y / 2.0f) - (m_position.y + m_size.y / 2.0f);

		if(std::abs(yDist) > (otherSize.y + m_size.y) / 2.0f) {
			return true; // As shown above, collision would be possible on the X axis, so return true.
		} // Else, we are colliding.

		if(other->getType() == XMLModule::EntityType::NPC || other->getType() == XMLModule::EntityType::PROJECTILE) {
			Factory::getEntityManager()->queueEntityToRemove(this);
			m_active = false;
		}
	}

	return true;
}

void EntityProjectile::draw(GLEngine::SpriteBatch& sb, float time, int layerDifference, float xOffset) {
	if(m_draw) {
		glm::vec4 destRect = glm::vec4(m_position.x + (xOffset * CHUNK_SIZE), m_position.y, m_size.x, m_size.y);

		float depth = getDepth();

		m_anim.draw(sb, GLEngine::ColourRGBA8(255, 255, 255, 255), destRect, depth, glm::normalize(m_velocity));
	}
}

void EntityProjectile::drawNormal(GLEngine::SpriteBatch& sb, float time, int layerDifference, float xOffset) {
	// Do nothing for right now
	/// TODO: Add normal maps to animations.
}

void EntityProjectile::onUpdate(float timeStep, unsigned int selfIndex) {
	if(m_lifeTime > 0.0f) {
		m_lifeTime -= timeStep;
		if(m_lifeTime <= 0.0f) Factory::getEntityManager()->queueEntityToRemove(this);
	}
}

void EntityProjectile::onTick() {
	m_anim.tick();
}
