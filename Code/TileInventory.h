#pragma once

#include "InventoryBase.h"

class TileInventory : public InventoryBase {
	public:
		TileInventory(unsigned int maxItems, std::string& name);
		~TileInventory();

	protected:
		unsigned int m_maxNumItems = 1;

		virtual bool operator_canAddItem(Item* item) override;
		virtual void initGUI(CEGUI::FrameWindow* frame);

};
