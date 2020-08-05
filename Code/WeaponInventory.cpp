#include "WeaponInventory.h"

bool weaponInventory_verification(CEGUI::GUI_InventoryItem& item) {
	Item* i = item.getData<Item>();

	//if(dynamic_cast<ItemWeapon*>(i)) {
	return true;
	//}
	return false;
}

WeaponInventory::WeaponInventory(std::string& name, bool automaticResizing, bool initGUI, CEGUI::Window* parent) : InventoryBase(name, automaticResizing, initGUI, parent) {
	m_grid->setVerificationFunction(&weaponInventory_verification);
}

WeaponInventory::~WeaponInventory() {
}
