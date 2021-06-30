#pragma once

#include "InventoryBase.h" // Base class: <global>::InventoryBase

class WeaponInventory : public InventoryBase {
  public:
	WeaponInventory(std::string&   name,
					bool		   automaticResizing = false,
					bool		   initGUI			 = true,
					CEGUI::Window* parent			 = nullptr);
	~WeaponInventory();
};
