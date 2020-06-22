#include "EntityProjectile.h"

#include "XMLData.h"

EntityProjectile::EntityProjectile(glm::vec2 pos, unsigned int layer, unsigned int id, SaveDataTypes::MetaData data, bool loadTex) : Entity(pos, layer, SaveDataTypes::MetaData()) {
	m_type = EntityTypes::PROJECTILE;

	m_id = id;

	XMLModule::EntityProjectileData d = XMLModule::XMLData::getEntityProjectileData(id);

	m_texturePath = d.texture;
	m_bumpMapPath = d.bumpMap;
	m_size = d.size;
	m_updateScriptId = d.updateScript;
	m_tickScriptId = d.tickScript;
	m_speed = d.speed;
	m_damage = d.damage;
	m_collideWithBlocks = d.collides;

	m_metaData = d.getMetaData();

	if(loadTex) {
		loadTexture();
	}
}

EntityProjectile::EntityProjectile(glm::vec2 pos, unsigned int layer, EntityIDs id, SaveDataTypes::MetaData data, bool loadTex) : Entity(pos, layer, SaveDataTypes::MetaData()) {
	m_type = EntityTypes::PROJECTILE;

	m_id = (unsigned int)id;

	XMLModule::EntityProjectileData d = XMLModule::XMLData::getEntityProjectileData(m_id);

	m_texturePath = d.texture;
	m_bumpMapPath = d.bumpMap;
	m_size = d.size;
	m_updateScriptId = d.updateScript;
	m_tickScriptId = d.tickScript;
	m_speed = d.speed;
	m_damage = d.damage;
	m_collideWithBlocks = d.collides;

	m_metaData = d.getMetaData();

	if(loadTex) {
		loadTexture();
	}
}

EntityProjectile::~EntityProjectile() {
	//dtor
}
