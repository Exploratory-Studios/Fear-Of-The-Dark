#include "TileInventory.h"

TileInventory::TileInventory(unsigned int maxItems, std::string& name) : InventoryBase(name), m_maxNumItems(maxItems) {
	m_grid->setVerificationFunction([=](CEGUI::GUI_InventoryItem& item) -> bool {
		if((item.getData<Item>())->getQuantity() + getCount() <= m_maxNumItems) {
			return true;
		}
		return false;
	});
}

TileInventory::~TileInventory() {
}

void TileInventory::initGUI(CEGUI::FrameWindow* frame) {
}
