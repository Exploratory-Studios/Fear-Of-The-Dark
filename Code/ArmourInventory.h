#pragma once

#include "InventoryBase.h"

#include "ItemArmour.h"

bool armourInventory_verification(CEGUI::GUI_InventoryItem& item) {
	Item* i = item.getData<Item>();

	if(dynamic_cast<ItemArmour*>(i)) { // This item is an armour
		return true;
	}
	return false;
}

class ArmourInventory : public InventoryBase {
	public:
		ArmourInventory(std::string& name, bool automaticResizing = false, bool initGUI = true, CEGUI::Window* parent = nullptr);
		~ArmourInventory();

		float getTotalDamageThreshold() {
			float total = 0.0f;
			for(unsigned int i = 0; i < m_gridItems.size(); i++) {
				total += m_gridItems.getData<ItemArmour>()->getThreshold();
			}
			return total;
		}

		float getTotalDamageResistance() {
			float total = 0.0f;
			for(unsigned int i = 0; i < m_gridItems.size(); i++) {
				total += m_gridItems.getData<ItemArmour>()->getResistance();
			}
			return total;
		}

};
