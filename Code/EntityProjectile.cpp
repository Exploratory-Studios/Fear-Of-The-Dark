#include "EntityProjectile.h"

#include "XMLData.h"

#include "Factory.h"

EntityProjectile::EntityProjectile(glm::vec2 pos, unsigned int layer, unsigned int id, SaveDataTypes::MetaData data, bool loadTex) : Entity(pos, layer, SaveDataTypes::MetaData()) {
	m_id = id;

	init();

	if(loadTex) {
		loadTexture();
	}
}

EntityProjectile::EntityProjectile(glm::vec2 pos, unsigned int layer, EntityIDs id, SaveDataTypes::MetaData data, bool loadTex) : Entity(pos, layer, SaveDataTypes::MetaData()) {
	m_id = (unsigned int)id;

	init();

	if(loadTex) {
		loadTexture();
	}
}

void EntityProjectile::init() {
	m_type = XMLModule::EntityType::PROJECTILE;

	XMLModule::EntityProjectileData d = XMLModule::XMLData::getEntityProjectileData(m_id);

	m_texturePath = d.texture;
	m_bumpMapPath = d.bumpMap;
	m_size = d.size;
	m_updateScriptId = d.updateScript;
	m_tickScriptId = d.tickScript;
	m_speed = d.speed;
	m_damage = d.damage;
	m_collideWithBlocks = d.collides;
	m_gravity = d.gravity;

	m_metaData = d.getMetaData();
}

EntityProjectile::~EntityProjectile() {
	//dtor
}

void EntityProjectile::collideWithTiles(World* world) {
	if(m_collideWithBlocks) {
		std::vector<glm::vec2> positions;

		checkTilePosition(world, positions,
		                  m_position.x + m_size.x / 2.0f,
		                  m_position.y + m_size.y / 2.0f);

		if(positions.size() > 0) {
			// We did collide, destroy this
			Factory::getEntityManager()->removeEntity(getUUID());
		}
	}
}

bool EntityProjectile::collideWithOther(Entity* other) {
	return false;
}
