#include "ArmourInventory.h"

ArmourInventory::ArmourInventory(std::string& name, bool automaticResizing, bool initGUI, CEGUI::Window* parent) : InventoryBase(name, automaticResizing, initGUI, parent) {
	m_grid->setVerificationFunction(&armourInventory_verification);
}

ArmourInventory::~ArmourInventory() {
}
