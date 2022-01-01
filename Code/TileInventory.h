#pragma once

#include "InventoryBase.h"

class TileInventory : public InventoryBase {
  public:
	TileInventory(unsigned int maxItems, std::string& name);
	~TileInventory();
	
	void initGUI();

  protected:
	unsigned int m_maxNumItems = 1;
};
