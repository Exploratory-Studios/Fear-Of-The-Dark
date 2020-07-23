#include "NPCInventory.h"

NPCInventory::NPCInventory(float maxWeight, std::string& name) : InventoryBase(name), m_absMaxWeight(maxWeight) {

}

NPCInventory::~NPCInventory() {

}

void NPCInventory::initGUI(CEGUI::FrameWindow* frame) {
	//frame->setTitleBarEnabled(true);
	//frame->getTitlebar()->setDraggingEnabled(true);
	//frame->getTitlebar()->setText("[font='AmaticBold-22']TEST");
}

float NPCInventory::getSpeedMultiplier() {
	// y = -x^4 + 1; Gives us a nice exponential curve (from 0,1 to 1,0), where x = m_weight / m_maxWeight and y = the multiplier
	if(m_weight > 0.0f) {
		float multiplier = -std::pow(m_weight / MAX_WEIGHT, 4) + 1.0f;

		if(multiplier < 0.0f) multiplier = 0.0f;
		if(std::isnan(multiplier)) multiplier = 1.0f;

		return multiplier;
	}
	return 1.0f;
}

bool NPCInventory::operator_canAddItem(Item* item) {
	// Check if the inventory's weight + item's weight*quantity < m_absMaxWeight
	return ((getCurrentWeight() + item->getWeight() * item->getQuantity()) <= m_absMaxWeight);
}
