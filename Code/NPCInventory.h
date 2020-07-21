#pragma once

#include "InventoryBase.h"

class NPCInventory : public InventoryBase {
	public:
		NPCInventory(float maxWeight, std::string& name);
		~NPCInventory();

		float getSpeedMultiplier();

	protected:
		virtual bool operator_canAddItem(Item* item) override;
		virtual void initGUI(CEGUI::FrameWindow* frame);

		// The max weight at which you can still walk the normal speed, defined in PRESETS file. Called MAX_WEIGHT
		float m_absMaxWeight = 75.0f; // The weight at which you can't walk anymore (around 70-75lbs). 2x the max weight at normal walking speed

};
