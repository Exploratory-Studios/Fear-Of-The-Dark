#include "TileContainer.h"

#include "XMLData.h"
#include "Singletons.h"

#include "TileInventory.h"

TileContainer::TileContainer(glm::vec2				 pos,
							 unsigned int			 layer,
							 unsigned int			 id,
							 SaveDataTypes::MetaData data,
							 bool					 loadTex) :
	Tile(pos, layer, id, data, loadTex) {
	init(); // The parent constructor sets m_id
}

TileContainer::~TileContainer() {
	delete m_inventory;
}

void TileContainer::init() {
	XMLModule::TileContainerData d = XMLModule::XMLData::getTileContainerData(m_id);

	std::string name = "TILE_CONTAINER_" + std::to_string(m_pos.x) + "." + std::to_string(m_pos.y);
	m_inventory		 = new TileInventory(d.maxItems, name);
	m_inventory->setToDraw(true);
}

void TileContainer::drawGUI(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, int& xOffset) {
	glm::vec2 size = Singletons::getGameCamera()->convertWorldSizeToRelativeScreenSize(glm::vec2(3.0f, 3.0f));
	glm::vec4 destRect(0.0f,
					   0.0f,
					   size.x,
					   size.y);
	m_inventory->setDestRect(destRect);
	
	m_inventory->draw(sb,
					  sf,
					  m_pos.x + xOffset - 1.0f,
					  m_pos.y+2.0f); // Pos is adjusted so that it works cross-world (chunk 0 <-> chunk [world_size-1])
	setToDraw(true);
}

void TileContainer::setToDraw(bool toDraw) {
	m_inventory->setToDraw(toDraw);
}

void TileContainer::onUpdate() {
	m_inventory->update();
}
