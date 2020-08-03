#pragma once

#include "InventoryBase.h" // Base class: <global>::InventoryBase

bool weaponInventory_verification(CEGUI::GUI_InventoryItem& item) {
	Item* i = item.getData<Item>();

	//if(dynamic_cast<ItemWeapon*>(i)) {
	return true;
	//}
	return false;
}

class WeaponInventory : public InventoryBase {
	public:
		WeaponInventory(std::string& name, bool automaticResizing = false, bool initGUI = true, CEGUI::Window* parent = nullptr);
		~WeaponInventory();

};
