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
	XMLModule::TileContainerData d = XMLModule::XMLData::getTileContainerData(m_id);

	std::string name = "TILE" + std::to_string(m_pos.x) + "," + std::to_string(m_pos.y);
	m_inventory = new TileInventory(d.maxItems, name);
	m_inventory->setToDraw(true);
}

void TileContainer::onDraw(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, glm::vec4& pos, float& depth) {
	m_inventory->draw(sb, sf, pos.x, m_pos.y); // Pos is adjusted so that it works cross-world (chunk 0 <-> chunk [world_size-1])
}
