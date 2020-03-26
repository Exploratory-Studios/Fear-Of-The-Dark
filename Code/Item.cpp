#include "Item.h"

#include "Tile.h"
#include "XMLData.h"

Item::Item(short unsigned int quantity, unsigned int id, bool loadTex) : m_id(id), m_quantity(quantity) {
    XML_ItemData d = XMLData::getItemData(id);

    m_texturePath = d.textureFilepath;
    m_useScriptId = d.useScriptID;
    m_weight = d.weight;
    m_name = d.name;

    if(loadTex) loadTexture();
}

Item::Item(short unsigned int quantity, ItemIDs id, bool loadTex) : m_id((unsigned int)id), m_quantity(quantity) {
    XML_ItemData d = XMLData::getItemData((unsigned int)id);

    m_texturePath = d.textureFilepath;
    m_useScriptId = d.useScriptID;
    m_weight = d.weight;
    m_name = d.name;

    if(loadTex) loadTexture();
}

void Item::onRightClick(Tile* selectedBlock) {
    if(m_useScriptId != -1) {
        ScriptQueue::activateScript(m_useScriptId, generateLuaData() + "\n" + selectedBlock->generateLuaData());
    }
}
