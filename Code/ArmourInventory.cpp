#include "ArmourInventory.h"

bool armourInventory_verification(CEGUI::GUI_InventoryItem& item) {
	Item* i = item.getData<Item>();

	if(dynamic_cast<ItemArmour*>(i)) { // This item is an armour
		return true;
	}
	return false;
}

ArmourInventory::ArmourInventory(std::string& name, bool automaticResizing/*= false*/, bool initGUI/*= true*/, CEGUI::Window* parent/*= nullptr*/) : InventoryBase(name, automaticResizing, initGUI, parent) {
	m_grid->setVerificationFunction(&armourInventory_verification);
}

ArmourInventory::~ArmourInventory() {

}
