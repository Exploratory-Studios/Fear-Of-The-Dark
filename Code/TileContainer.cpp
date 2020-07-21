#include "TileContainer.h"

#include "XMLData.h"

#include "TileInventory.h"

TileContainer::TileContainer(glm::vec2 pos, unsigned int layer, unsigned int id, SaveDataTypes::MetaData data, bool loadTex) : Tile(pos, layer, id, data, loadTex) {
	init(); // The parent constructor sets m_id
}

TileContainer::~TileContainer() {
	delete m_inventory;
}

void TileContainer::init() {
	//XMLModule::TileContainerData d = XMLModule::XMLData::getTileContainerData(m_id);

	//m_inventory = new TileInventory(d.maxItems, "TILE" + std::to_string(m_position.x) + "," + std::to_string(m_position.y));
}

void TileContainer::onDraw(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, glm::vec4& pos, float& depth) {

}
