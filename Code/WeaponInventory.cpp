#include "WeaponInventory.h"

WeaponInventory::WeaponInventory(std::string& name, bool automaticResizing, bool initGUI, CEGUI::Window* parent) : InventoryBase(name, automaticResizing, initGUI, parent) {
	m_grid->setVerificationFunction(&weaponInventory_verification);
}

WeaponInventory::~WeaponInventory() {
}
