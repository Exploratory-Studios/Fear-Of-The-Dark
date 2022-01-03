#include "Item.h"

#include <XMLDataManager.hpp>

#include "CustomXMLTypes.h"

#include "Tile.h"

Item::Item(short unsigned int quantity, unsigned int id) : m_id(id), m_quantity(quantity) {
	init();
}

void Item::init() {
	XMLModule::ItemData d = getItemData(m_id);

	m_texture = d.texture;
	m_weight  = d.weight;
	m_name	  = d.name;
}

void Item::onRightClick(Tile* selectedBlock) {
	if(m_useScript.inited) {
		BARE2D::LuaScriptQueue::getInstance()->addLuaScript(m_useScript);
	}
}

SaveDataTypes::ItemData Item::getItemSaveData() {
	SaveDataTypes::ItemData ret;
	ret.id		 = m_id;
	ret.quantity = m_quantity;

	return ret;
}
