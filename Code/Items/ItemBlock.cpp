#include "ItemBlock.h"

#include "Singletons.h"
#include "Factory.h"
#include "XMLData.h"

#include "Tile.h"

ItemBlock::ItemBlock(short unsigned int quantity, unsigned int id, bool loadTex) : Item(quantity, id, loadTex) {
	init();
}

ItemBlock::~ItemBlock() {
	delete m_block;
}

void ItemBlock::init() {
	XMLModule::ItemBlockData d = XMLModule::XMLData::getItemBlockData(m_id);

	unsigned int id = d.blockID;

	m_block = Factory::createTile(m_id, glm::vec2(0.0f, 0.0f), 0);
}

void ItemBlock::onRightClick(Tile* selectedBlock) {
	Item::onRightClick(selectedBlock);

	if(selectedBlock->getID() == (unsigned int)TileIDs::AIR)
		placeBlock(selectedBlock);
}

void ItemBlock::setTileToPlace(Tile& tile) {
	*m_block = tile; // Don't create a new tile object or use the one that could possibly be deleted.
}

void ItemBlock::placeBlock(Tile* selected) {
	Singletons::getWorld()->setTile(Factory::createTile(m_block->getID(), selected->getPosition(), selected->getLayer(), m_block->getMetaData(), true));
}
