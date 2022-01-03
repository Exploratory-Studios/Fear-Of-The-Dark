#pragma once

#include <vector>
#include <Camera2D.hpp>
#include <BARECEGUI.hpp>
#include <BasicRenderer.hpp>
#include <FontRenderer.hpp>

#include "GUI_InventoryReceiver.h"

#include "PresetValues.h"
#include "Item.h"

#define INVENTORY_WIDTH		 5
#define INVENTORY_BOX_WIDTH	 0.18f
#define INVENTORY_BOX_HEIGHT 0.18f

class InventoryBase {
  public:
	InventoryBase(std::string&	 name,
				  bool			 automaticResizing = true,
				  bool			 initGUI		   = true,
				  CEGUI::Window* parent			   = nullptr);
	virtual ~InventoryBase();

	void		 init(std::string name, bool autoResize, CEGUI::Window* parent);
	void		 init(SaveDataTypes::InventoryData& data);
	virtual void initInventoryGUI();
	void		 destroy(); // Actually destroys GUI windows used.

	bool addItem(Item* newItem);   // Returns false if the item didn't pass the test, true otherwise. Adds a gridItem.
	void subtractItem(Item* item); // removes from both m_items and m_gridItems
	void queueSubtraction(Item* item);

	void subscribeEvent(const CEGUI::String& evnt, CEGUI::Event::Subscriber sub);

	virtual void copyFrom(InventoryBase* other) {
		m_items	 = other->getItems();
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
				if(item == m_items[i])
					return i;
			}
		}
		return (unsigned int)-1;
	}
	std::vector<Item*>& getItems() {
		return m_items;
	}
	float		 getCurrentWeight();
	unsigned int getCount();

	void updateWeight();
	void update();
	void draw(BARE2D::BasicRenderer* renderer,
			  BARE2D::FontRenderer*	 fontRenderer,
			  float					 x,
			  float					 y,
			  bool relative = true); // This moves the position of the CEGUI widgets to the in-game coords given. Useful.

	void setToDraw(bool setting) {
		m_frameWindow->setEnabled(setting);
		m_frameWindow->setVisible(setting);
	}

	void setContentSize(int w, int h) {
		m_grid->setContentSize(w, h);
	}

	void setMovable(bool setting) {
		m_movable = setting;
	}

	void setDestRect(glm::vec4& destRect) {
		m_destRect = destRect;
		m_frameWindow->setPosition(CEGUI::UVector2(cegui_reldim(destRect.x), cegui_reldim(destRect.y)));
		m_frameWindow->setSize(CEGUI::USize(cegui_reldim(destRect.z), cegui_reldim(destRect.w)));
	}

	SaveDataTypes::InventoryData getInventorySaveData();

	bool onDoubleClick(const CEGUI::EventArgs& e);
	bool onMouseMove(const CEGUI::EventArgs& e);
	bool onMouseUp(const CEGUI::EventArgs& e);
	bool onMouseLeave(const CEGUI::EventArgs& e);
	bool onDragDropItemRemoved(const CEGUI::EventArgs& e);
	bool onDragDropItemAdded(const CEGUI::EventArgs& e);

  protected:
	virtual void onItemAdded(const CEGUI::WindowEventArgs& e) {}; // the args are WindowEventArgs

	float			   m_weight = 0.0f; // The average person can carry about 20kg of weight and still walk normally
	std::vector<Item*> m_items {};

	// This should never *HAVE* to be touched by derived classes. The base class ***should*** manage everything
	/// CEGUI Portion
	glm::vec4			m_destRect; // This is the position & size relative to the x and y given to the draw() function
	CEGUI::FrameWindow* m_frameWindow			  = nullptr;
	CEGUI::ScrollablePane*				   m_pane = nullptr;
	CEGUI::GUI_InventoryReceiver*		   m_grid = nullptr;
	std::vector<CEGUI::GUI_InventoryItem*> m_gridItems;

	bool m_grabbingGUI			= false;
	bool m_resizing				= false;
	bool m_automaticallyResizes = true;
	bool m_initedGUI			= false;
	bool m_movable				= true;

	std::string	   m_name	= "";
	CEGUI::Window* m_parent = nullptr;

	void createInventoryItem(Item* item); // Creates and adds a GUI_InventoryItem to the reciever.
	void resizeInventoryWidget();		  // Resizes the inventory (m_grid) to have the height (necessaryHeight + 1).

	std::vector<Item*> m_itemsToRemove;
	std::vector<Item*> m_itemsToAdd;
};
