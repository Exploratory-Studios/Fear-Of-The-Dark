#include "Item.h"

#include "Tile.h"
#include "XMLData.h"

Item::Item(short unsigned int quantity, unsigned int id, bool loadTex) : m_id(id), m_quantity(quantity) {
    XMLModule::ItemData d = XMLModule::XMLData::getItemData(id);

    d.getAttribute("texture", m_texturePath);
    d.getAttribute("useScript", m_useScriptId);
    d.getAttribute("weight", m_weight);
    d.getAttribute("name", m_name);

    m_metaData = d.getMetaData();

    if(loadTex) loadTexture();
}

Item::Item(short unsigned int quantity, ItemIDs id, bool loadTex) : m_id((unsigned int)id), m_quantity(quantity) {
    XMLModule::ItemData d = XMLModule::XMLData::getItemData((unsigned int)id);

    d.getAttribute("texture", m_texturePath);
    d.getAttribute("useScript", m_useScriptId);
    d.getAttribute("weight", m_weight);
    d.getAttribute("name", m_name);

    m_metaData = d.getMetaData();

    if(loadTex) loadTexture();
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

