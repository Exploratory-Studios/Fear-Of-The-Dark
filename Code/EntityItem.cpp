#include "EntityItem.h"

#include "XMLData.h"

EntityItem::EntityItem(glm::vec2 pos, unsigned int layer, unsigned int id, SaveDataTypes::MetaData data, bool loadTex) : Entity(pos, layer, SaveDataTypes::MetaData())
{
    m_type = EntityTypes::ITEM;

    m_id = id;

    XMLModule::EntityItemData d = XMLModule::XMLData::getEntityItemData(id);

    d.getAttribute("texture", m_texturePath);
    d.getAttribute("bumpMap", m_bumpMapPath);
    d.getAttribute("size", m_size);
    d.getAttribute("updateScript", m_updateScriptId);
    d.getAttribute("tickScript", m_tickScriptId);
    d.getAttribute("item", m_itemId);

    m_metaData = d.getMetaData();

    if(loadTex) {
        loadTexture();
    }
}

EntityItem::EntityItem(glm::vec2 pos, unsigned int layer, EntityIDs id, SaveDataTypes::MetaData data, bool loadTex) : Entity(pos, layer, SaveDataTypes::MetaData())
{
    m_type = EntityTypes::ITEM;

    m_id = (unsigned int)id;

    XMLModule::EntityItemData d = XMLModule::XMLData::getEntityItemData((unsigned int)id);

    d.getAttribute("texture", m_texturePath);
    d.getAttribute("bumpMap", m_bumpMapPath);
    d.getAttribute("size", m_size);
    d.getAttribute("updateScript", m_updateScriptId);
    d.getAttribute("tickScript", m_tickScriptId);
    d.getAttribute("item", m_itemId);

    m_metaData = d.getMetaData();

    if(loadTex) {
        loadTexture();
    }
}

EntityItem::~EntityItem()
{
    //dtor
}
