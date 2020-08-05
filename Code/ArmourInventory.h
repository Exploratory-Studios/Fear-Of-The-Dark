#pragma once

#include "InventoryBase.h"

#include "ItemArmour.h"

class ArmourInventory : public InventoryBase {
	public:
		ArmourInventory(std::string& name, bool automaticResizing = false, bool initGUI = true, CEGUI::Window* parent = nullptr);
		~ArmourInventory();

		float getTotalDamageThreshold() {
			float total = 0.0f;
			for(unsigned int i = 0; i < m_gridItems.size(); i++) {
				total += m_gridItems[i]->getData<ItemArmour>()->getThreshold();
			}
			return total;
		}

		float getTotalDamageResistance() {
			float total = 0.0f;
			for(unsigned int i = 0; i < m_gridItems.size(); i++) {
				total += m_gridItems[i]->getData<ItemArmour>()->getResistance();
			}
			return total;
		}

};
