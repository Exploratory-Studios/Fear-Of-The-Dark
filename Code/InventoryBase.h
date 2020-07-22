#pragma once

#include <vector>
#include <Camera2D.h>
#include <GUI.h>

#include "GUI_InventoryReceiver.h"

#include "PresetValues.h"
#include "Item.h"

class InventoryBase {
	public:
		InventoryBase(std::string& name);
		virtual ~InventoryBase();

		void init();

		bool addItem(Item* newItem); // Returns false if the item didn't pass the test, true otherwise.
		void subtractItem(Item* item);

		virtual void copyFrom(InventoryBase* other) {
			m_items = other->getItems();
			m_weight = other->getCurrentWeight();
		}

		Item* getItem(unsigned int x) {
			if(m_items.size() > x) {
				return m_items[x];
			} else {
				return nullptr;
			}
		}
		unsigned int getItemIndex(Item* item) const {
			if(item) {
				for(unsigned int i = 0; i < m_items.size(); i++) {
					if(m_items[i]->getID() == item->getID() && m_items[i]->getQuantity() >= item->getQuantity()) return i;
				}
			}
			return (unsigned int) - 1;
		}
		std::vector<Item*>& getItems() {
			return m_items;
		}
		float getCurrentWeight();

		void updateWeight();
		void update();
		void draw(float x, float y, GLEngine::Camera2D& cam); // This moves the position of the CEGUI widgets to the in-game coords given. Useful.

		void setToDraw(bool setting) {
			m_frameWindow->setEnabled(setting);
			m_frameWindow->setVisible(setting);
		}

		SaveDataTypes::InventoryData getInventorySaveData();

	protected:
		float m_weight = 0.0f; // The average person can carry about 40-45lbs of weight and still walk normally
		virtual void initGUI(CEGUI::FrameWindow* frame) {};
		std::vector<Item*> m_items{};

		virtual bool operator_canAddItem(Item* item) {
			return true;
		}

	private:
		// This should never needed to be touched by derived classes.
		/// CEGUI Portion
		glm::vec4 m_destRect; // This is the position & size relative to the x and y given to the draw() function
		CEGUI::FrameWindow* m_frameWindow = nullptr;
		CEGUI::GUI_InventoryReceiver* m_grid = nullptr;
};
