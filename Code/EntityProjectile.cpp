#include "EntityProjectile.h"

#include "XMLData.h"

EntityProjectile::EntityProjectile(glm::vec2 pos, unsigned int layer, unsigned int id, MetaData data, bool loadTex) : Entity(pos, layer, data)
{
    m_type = EntityTypes::PROJECTILE;

    m_id = id;

    XML_EntityProjectileData d = XMLData::getEntityProjectileData(id);

    m_texturePath = d.textureFilepath;
    m_bumpMapPath = d.bumpMapFilepath;
    m_size = d.size;
    m_updateScriptId = d.updateScriptID;
    m_tickScriptId = d.tickScriptID;
    m_speed = d.speed;
    m_damage = d.damage;
    m_collideWithBlocks = d.collides;

    if(loadTex) {
        loadTexture();
    }
}

EntityProjectile::EntityProjectile(glm::vec2 pos, unsigned int layer, EntityIDs id, MetaData data, bool loadTex) : Entity(pos, layer, data)
{
    m_type = EntityTypes::PROJECTILE;

    m_id = (unsigned int)id;

    XML_EntityProjectileData d = XMLData::getEntityProjectileData((unsigned int)id);

    m_texturePath = d.textureFilepath;
    m_bumpMapPath = d.bumpMapFilepath;
    m_size = d.size;
    m_updateScriptId = d.updateScriptID;
    m_tickScriptId = d.tickScriptID;
    m_speed = d.speed;
    m_damage = d.damage;
    m_collideWithBlocks = d.collides;

    if(loadTex) {
        loadTexture();
    }
}

EntityProjectile::~EntityProjectile()
{
    //dtor
}
