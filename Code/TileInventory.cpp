#include "TileInventory.h"

TileInventory::TileInventory(unsigned int maxItems, std::string& name) : InventoryBase(name), m_maxNumItems(maxItems) {

}

TileInventory::~TileInventory() {

}

void TileInventory::initGUI(CEGUI::FrameWindow* frame) {

}

bool TileInventory::operator_canAddItem(Item* item) {
	// Count how many total items we have. If count+item->quantity is <= max, return true. False otherwise.
	unsigned int count = 0;
	for(unsigned int i = 0; i < m_items.size(); i++) {
		count += m_items[i]->getQuantity();
	}

	if(count + item->getQuantity() <= m_maxNumItems) {
		return true;
	}
	return false;
}
