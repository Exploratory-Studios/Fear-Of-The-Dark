#include "EntityItem.h"

#include "XMLData.h"

#include "Factory.h"

EntityItem::EntityItem(glm::vec2 pos, unsigned int layer, unsigned int id, SaveDataTypes::MetaData data, bool loadTex) : Entity(pos, layer, SaveDataTypes::MetaData()) {
	m_id = id;

	init();

	if(loadTex) {
		loadTexture();
	}
}

EntityItem::EntityItem(glm::vec2 pos, unsigned int layer, EntityIDs id, SaveDataTypes::MetaData data, bool loadTex) : Entity(pos, layer, SaveDataTypes::MetaData()) {
	m_id = (unsigned int)id;

	init();

	if(loadTex) {
		loadTexture();
	}
}

void EntityItem::init() {
	m_type = XMLModule::EntityType::ITEM;

	XMLModule::EntityItemData d = XMLModule::XMLData::getEntityItemData(m_id);

	m_texturePath = d.texture;
	m_bumpMapPath = d.bumpMap;
	m_size = d.size;
	m_updateScriptId = d.updateScript.getID();
	m_tickScriptId = d.tickScript.getID();
	m_itemId = d.item;

	m_metaData = d.getMetaData();
}

EntityItem::~EntityItem() {
	//dtor
}

bool EntityItem::collideWithOther(Entity* other) {

}

Item* EntityItem::getItem() {
	return Factory::createItem(m_itemId, 1, true);
}
