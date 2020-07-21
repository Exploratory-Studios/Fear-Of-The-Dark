#include "TileInventory.h"

TileInventory::TileInventory(unsigned int maxItems, std::string& name) : InventoryBase(name), m_maxNumItems(maxItems) {

}

TileInventory::~TileInventory() {

}

void TileInventory::initGUI(CEGUI::FrameWindow* frame) {

}

bool TileInventory::operator_canAddItem(Item* item) {
	// If there's already an item of the same ID in the inventory, return true. Else, if m_items->size() + item->quantity <= m_maxnumItems
	if(getItemIndex(item) != (unsigned int)-1) {
		return true;
	} else {
		return ((m_items.size() + item->getQuantity()) <= m_maxNumItems);
	}
}
