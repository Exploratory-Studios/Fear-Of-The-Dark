#include "EntityItem.h"

#include "XMLData.h"

EntityItem::EntityItem(glm::vec2 pos, unsigned int layer, unsigned int id, MetaData data, bool loadTex) : Entity(pos, layer, MetaData())
{
    m_type = EntityTypes::ITEM;

    m_id = id;

    XML_EntityItemData d = XMLData::getEntityItemData(id);

    m_texturePath = d.textureFilepath;
    m_bumpMapPath = d.bumpMapFilepath;
    m_size = d.size;
    m_updateScriptId = d.updateScriptID;
    m_tickScriptId = d.tickScriptID;
    m_itemId = d.itemID;

    m_metaData = d.defaultMD;
    m_metaData += data; // Use the overloaded operator to simply add/overwrite defaults.

    if(loadTex) {
        loadTexture();
    }
}

EntityItem::EntityItem(glm::vec2 pos, unsigned int layer, EntityIDs id, MetaData data, bool loadTex) : Entity(pos, layer, MetaData())
{
    m_type = EntityTypes::ITEM;

    m_id = (unsigned int)id;

    XML_EntityItemData d = XMLData::getEntityItemData((unsigned int)id);

    m_texturePath = d.textureFilepath;
    m_bumpMapPath = d.bumpMapFilepath;
    m_size = d.size;
    m_updateScriptId = d.updateScriptID;
    m_tickScriptId = d.tickScriptID;
    m_itemId = d.itemID;

    m_metaData = d.defaultMD;
    m_metaData += data; // Use the overloaded operator to simply add/overwrite defaults.

    if(loadTex) {
        loadTexture();
    }
}

EntityItem::~EntityItem()
{
    //dtor
}
