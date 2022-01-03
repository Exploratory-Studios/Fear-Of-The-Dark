#include "TileContainer.h"

#include <XMLDataManager.hpp>
#include "Singletons.h"

#include "TileInventory.h"

TileContainer::TileContainer(glm::vec2 pos, unsigned int layer, unsigned int id) :
	Tile(pos, layer, id) {
	init(); // The parent constructor sets m_id
}

TileContainer::~TileContainer() {
	delete m_inventory;
	m_inventory = nullptr;
}

void TileContainer::init() {
	XMLModule::TileContainerData d = getTileContainerData(m_id);

	std::string name = "TILE_CONTAINER_" + std::to_string(m_pos.x) + "." + std::to_string(m_pos.y);
	m_inventory		 = new TileInventory(d.maxItems, name);
}

void TileContainer::destroy() {
	Tile::destroy();
	if(m_inventory)
		delete m_inventory;
	m_inventory = nullptr;
}

void TileContainer::drawGUI(BARE2D::BasicRenderer* renderer, BARE2D::FontRenderer* fontRenderer, int& xOffset) {
	if(!m_initedGUI) {
		m_initedGUI = true;
		m_inventory->initGUI();
		m_inventory->setToDraw(true);
	}

	glm::vec2 size = Singletons::getGameCamera()->getScreenSizeFromViewedSize(glm::vec2(3.0f, 3.0f));
	glm::vec4 destRect(0.0f, 0.0f, size.x, size.y);
	m_inventory->setDestRect(destRect);

	m_inventory->draw(
		renderer,
		fontRenderer,
		m_pos.x + xOffset - 1.0f,
		m_pos.y + 2.0f); // Pos is adjusted so that it works cross-world (chunk 0 <-> chunk [world_size-1])
	setToDraw(true);
}

void TileContainer::setToDraw(bool toDraw) {
	m_inventory->setToDraw(toDraw);
}

void TileContainer::onUpdate() {
	m_inventory->update();
}
