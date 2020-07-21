#include "InventoryBase.h"

#include <string>
#include <math.h>

#include "Factory.h"

InventoryBase::InventoryBase(std::string& name) {
	//m_frameWindow = static_cast<CEGUI::FrameWindow*>(Factory::getGUI()->createWidget(nullptr, "FOTDSkin/FrameWindow", glm::vec4(0.0f, 0.0f, 0.33f, 0.33f), glm::vec4(0.0f), name + "_NPCInventory"));

	initGUI(m_frameWindow);
}

InventoryBase::~InventoryBase() {
	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	winMgr.destroyWindow(m_frameWindow);
}

bool InventoryBase::addItem(Item* newItem) {
	if(operator_canAddItem(newItem)) {
		m_weight += newItem->getWeight() * newItem->getQuantity();

		for(unsigned int i = 0; i < m_items.size(); i++) {
			if(m_items[i]->getID() == newItem->getID()) {
				m_items[i]->addToQuantity(newItem->getQuantity());
			}
		}

		m_items.push_back(newItem);
		return true;
	} else {
		return false;
	}
}

void InventoryBase::subtractItem(Item* item) {
	getItem(getItemIndex(item))->addToQuantity(-item->getQuantity());
}

void InventoryBase::updateWeight() { // Only use if something needs verification or is bugged
	m_weight = 0;
	for(unsigned int i = 0; i < m_items.size(); i++) {
		m_weight += m_items[i]->getWeight() * m_items[i]->getQuantity();
	}
}

void InventoryBase::update() {
	for(unsigned int i = 0; i < m_items.size(); i++) {
		if(m_items[i]->getQuantity() <= 0) {
			for(unsigned int j = i + 1; j < m_items.size(); j++) {
				m_items[i] = m_items[j];
			}
			m_items.pop_back();
		}
	}
}

float InventoryBase::getCurrentWeight() {
	return m_weight;
}

void InventoryBase::draw(float x, float y, GLEngine::Camera2D& cam) {
	// Set m_frameWindow position and size based on camera's convert screen to world and screen dims (retrieved from cam)

	// Convert x and y to screen coords
	// Convert those screen coords to percentages
	// Move the frame window

	// Convert x and y
	glm::vec2 screenCoords = cam.convertWorldToScreen(glm::vec2(x, y));

	// Convert screenCoords to percentages
	glm::vec2 percentages = screenCoords / glm::vec2(cam.getScreenWidth(), cam.getScreenHeight());

	// Move the frame window
	m_frameWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(percentages.x, 0.0f), CEGUI::UDim(percentages.y, 0.0f)));


	/// Currently, we don't set the size, but we could using the camera's scale (if we wanted to)

}

SaveDataTypes::InventoryData InventoryBase::getInventorySaveData() {
	SaveDataTypes::InventoryData ret;
	for(unsigned int i = 0; i < m_items.size(); i++) {
		ret.itemData.push_back(m_items[i]->getItemSaveData());
	}
	return ret;
}
