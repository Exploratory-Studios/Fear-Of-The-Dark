#include "TileInventory.h"

TileInventory::TileInventory(unsigned int maxItems, std::string& name) : InventoryBase(name), m_maxNumItems(maxItems) {
	
}

TileInventory::~TileInventory() {
}

void TileInventory::initGUI() {
	if(!m_initedGUI) {
		initInventoryGUI(); // To ensure that the InventoryBase (or, yknow, the entire base of this class) is properly set up
		m_grid->setVerificationFunction([=](CEGUI::GUI_InventoryItem& item) -> bool {
			if((item.getData<Item>())->getQuantity() + getCount() <= m_maxNumItems) {
				return true;
			}
			return false;
		});
	}
	m_initedGUI = true;
}
