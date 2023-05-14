#include "EntityItem.h"

#include <XMLDataManager.hpp>

#include "CustomXMLTypes.h"

#include "Factory.h"

EntityItem::EntityItem(glm::vec2 pos, unsigned int layer, unsigned int id) : Entity(pos, layer) {
	m_id = id;

	init();
}

EntityItem::EntityItem(glm::vec2 pos, unsigned int layer, EntityIDs id) : Entity(pos, layer) {
	m_id = (unsigned int)id;

	init();
}

void EntityItem::init() {
	m_type = XMLModule::EntityType::ITEM;

	XMLModule::EntityItemData d = getEntityItemData(m_id);

	m_texture	   = d.texture;
	m_bumpmap	   = d.bumpmap;
	m_size		   = d.size;
	m_updateScript = d.updateScript;
	m_tickScript   = d.tickScript;
	m_itemId	   = d.item;
	m_gravity 	   = !d.floating;
}

void EntityItem::init(SaveDataTypes::EntityItemData& data) {
	Entity::init(data);

	init();
}

EntityItem::~EntityItem() {
	//dtor
}

bool EntityItem::collideWithOther(Entity* other) {
	return false;
}

Item* EntityItem::getItem() {
	return Factory::createItem(m_itemId, 1);
}
