#pragma once

#include "InventoryBase.h"

class NPCInventory : public InventoryBase {
  public:
	NPCInventory(float maxWeight, std::string& name, bool initGUI = false);
	~NPCInventory();
	
	virtual void initInventoryGUI() override;

	virtual void copyFrom(InventoryBase* other) override {
		m_items	 = other->getItems();
		m_weight = other->getCurrentWeight();

		m_absMaxWeight = static_cast<NPCInventory*>(other)->getAbsMaxWeight();
	}

	float getAbsMaxWeight() {
		return m_absMaxWeight;
	}

	float getSpeedMultiplier();

  protected:

	// The max weight at which you can still walk the normal speed, defined in PRESETS file. Called MAX_WEIGHT
	float m_absMaxWeight =
		75.0f; // The weight at which you can't walk anymore (around 70-75lbs). 2x the max weight at normal walking speed
};
