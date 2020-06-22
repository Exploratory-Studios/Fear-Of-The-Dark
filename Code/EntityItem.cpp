#include "EntityItem.h"

#include "XMLData.h"

EntityItem::EntityItem(glm::vec2 pos, unsigned int layer, unsigned int id, SaveDataTypes::MetaData data, bool loadTex) : Entity(pos, layer, SaveDataTypes::MetaData()) {
	m_type = EntityTypes::ITEM;

	m_id = id;

	XMLModule::EntityItemData d = XMLModule::XMLData::getEntityItemData(id);

	m_texturePath = d.texture;
	m_bumpMapPath = d.bumpMap;
	m_size = d.size;
	m_updateScriptId = d.updateScript;
	m_tickScriptId = d.tickScript;
	m_itemId = d.item;

	m_metaData = d.getMetaData();

	if(loadTex) {
		loadTexture();
	}
}

EntityItem::EntityItem(glm::vec2 pos, unsigned int layer, EntityIDs id, SaveDataTypes::MetaData data, bool loadTex) : Entity(pos, layer, SaveDataTypes::MetaData()) {
	m_type = EntityTypes::ITEM;

	m_id = (unsigned int)id;

	XMLModule::EntityItemData d = XMLModule::XMLData::getEntityItemData(m_id);

	m_texturePath = d.texture;
	m_bumpMapPath = d.bumpMap;
	m_size = d.size;
	m_updateScriptId = d.updateScript;
	m_tickScriptId = d.tickScript;
	m_itemId = d.item;

	m_metaData = d.getMetaData();

	if(loadTex) {
		loadTexture();
	}
}

EntityItem::~EntityItem() {
	//dtor
}
