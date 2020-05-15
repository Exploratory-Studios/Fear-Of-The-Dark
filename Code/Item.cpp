#include "Item.h"

#include "Tile.h"
#include "XMLData.h"

Item::Item(short unsigned int quantity, unsigned int id, bool loadTex) : m_id(id), m_quantity(quantity) {
    XML_ItemData d = XMLData::getItemData(id);

    m_texturePath = d.textureFilepath;
    m_useScriptId = d.useScriptID;
    m_weight = d.weight;
    m_name = d.name;

    m_metaData = d.defaultMD;

    if(loadTex) loadTexture();
}

Item::Item(short unsigned int quantity, ItemIDs id, bool loadTex) : m_id((unsigned int)id), m_quantity(quantity) {
    XML_ItemData d = XMLData::getItemData((unsigned int)id);

    m_texturePath = d.textureFilepath;
    m_useScriptId = d.useScriptID;
    m_weight = d.weight;
    m_name = d.name;

    m_metaData = d.defaultMD;

    if(loadTex) loadTexture();
}

void Item::onRightClick(Tile* selectedBlock) {
    if(m_useScriptId != -1) {
        std::vector<Argument> concatenatedArgs(generateLuaData());
        std::vector<Argument> b = selectedBlock->generateLuaData();
        concatenatedArgs.insert(concatenatedArgs.end(), b.begin(), b.end());

        ScriptQueue::activateScript(m_useScriptId, concatenatedArgs);
    }
}

ItemData Item::getItemSaveData() {
    ItemData ret;
    ret.id = m_id;
    ret.quantity = m_quantity;
    ret.metaData = m_metaData;

    return ret;
}

