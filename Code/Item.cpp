#include "Item.h"

#include "Tile.h"
#include "XMLData.h"

Item::Item(short unsigned int quantity, unsigned int id, bool loadTex) : m_id(id), m_quantity(quantity) {
	init();

	if(loadTex) loadTexture();
}

Item::Item(short unsigned int quantity, ItemIDs id, bool loadTex) : m_id((unsigned int)id), m_quantity(quantity) {
	init();

	if(loadTex) loadTexture();
}

void Item::init() {
	XMLModule::ItemData d = XMLModule::XMLData::getItemData(m_id);

	m_texturePath = d.texture;
	m_weight = d.weight;
	m_name = d.name;

	m_metaData = d.getMetaData();
}

void Item::onRightClick(Tile* selectedBlock) {
	if(m_useScriptId != -1) {
		std::vector<ScriptingModule::Argument> concatenatedArgs(generateLuaData());
		std::vector<ScriptingModule::Argument> b = selectedBlock->generateLuaData();
		concatenatedArgs.insert(concatenatedArgs.end(), b.begin(), b.end());

		ScriptingModule::ScriptQueue::activateScript(m_useScriptId, concatenatedArgs);
	}
}

SaveDataTypes::ItemData Item::getItemSaveData() {
	SaveDataTypes::ItemData ret;
	ret.id = m_id;
	ret.quantity = m_quantity;
	ret.metaData = m_metaData;

	return ret;
}
