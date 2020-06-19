#include "EntityProjectile.h"

#include "XMLData.h"

EntityProjectile::EntityProjectile(glm::vec2 pos, unsigned int layer, unsigned int id, SaveDataTypes::MetaData data, bool loadTex) : Entity(pos, layer, SaveDataTypes::MetaData())
{
    m_type = EntityTypes::PROJECTILE;

    m_id = id;

    XMLModule::EntityProjectileData d = XMLModule::XMLData::getEntityProjectileData(id);

    d.getAttribute("texture", m_texturePath);
    d.getAttribute("bumpMap", m_bumpMapPath);
    d.getAttribute("size", m_size);
    d.getAttribute("updateScript", m_updateScriptId);
    d.getAttribute("tickScript", m_tickScriptId);
    d.getAttribute("speed", m_speed);
    d.getAttribute("damage", m_damage);
    d.getAttribute("collides", m_collideWithBlocks);

    m_metaData = d.getMetaData();

    if(loadTex) {
        loadTexture();
    }
}

EntityProjectile::EntityProjectile(glm::vec2 pos, unsigned int layer, EntityIDs id, SaveDataTypes::MetaData data, bool loadTex) : Entity(pos, layer, SaveDataTypes::MetaData())
{
    m_type = EntityTypes::PROJECTILE;

    m_id = (unsigned int)id;

    XMLModule::EntityProjectileData d = XMLModule::XMLData::getEntityProjectileData(m_id);

    d.getAttribute("texture", m_texturePath);
    d.getAttribute("bumpMap", m_bumpMapPath);
    d.getAttribute("size", m_size);
    d.getAttribute("updateScript", m_updateScriptId);
    d.getAttribute("tickScript", m_tickScriptId);
    d.getAttribute("speed", m_speed);
    d.getAttribute("damage", m_damage);
    d.getAttribute("collides", m_collideWithBlocks);

    m_metaData = d.getMetaData();

    if(loadTex) {
        loadTexture();
    }
}

EntityProjectile::~EntityProjectile()
{
    //dtor
}
