#include "WeaponInventory.h"

#include "Singletons.h"

#include <CEGUI/CEGUI.h>

bool weaponInventory_verification(CEGUI::GUI_InventoryItem& item) {
	Item* i = item.getData<Item>();

	//if(dynamic_cast<ItemWeapon*>(i)) {
	return true;
	//}
	return false;
}

WeaponInventory::WeaponInventory(std::string& name, bool automaticResizing, bool initGUI, CEGUI::Window* parent) :
	InventoryBase(name, automaticResizing, initGUI, parent) {
}

WeaponInventory::~WeaponInventory() {
}

void WeaponInventory::initInventoryGUI() {
	Singletons::getGUI()->setActiveContext(1); // Set this to the 2nd context (Inventory systems)

	if(m_parent) {
		m_frameWindow =
			static_cast<CEGUI::FrameWindow*>(Singletons::getGUI()->createWidget("FOTDSkin/FrameWindow",
																				glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
																				glm::vec4(0.0f),
																				m_parent,
																				m_name + "_Inventory"));
	} else {
		m_frameWindow =
			static_cast<CEGUI::FrameWindow*>(Singletons::getGUI()->createWidget("FOTDSkin/FrameWindow",
																				glm::vec4(-0.2f, -0.2f, 0.35f, 0.5f),
																				glm::vec4(0.0f),
																				nullptr,
																				m_name + "_Inventory"));
	}
	m_frameWindow->setCloseButtonEnabled(false);
	m_frameWindow->setDragMovingEnabled(true);
	m_frameWindow->setRollupEnabled(false);
	m_frameWindow->setSizingEnabled(false);
	m_frameWindow->setTitleBarEnabled(false);
	//m_frameWindow->setProperty("BackgroundColours", "tl: 80FFFFFF tr: 80FFFFFF bl: 80FFFFFF br: 80FFFFFF");
	m_frameWindow->setProperty("CaptionColour", "FF101010");
	m_frameWindow->subscribeEvent(CEGUI::Window::EventMouseDoubleClick,
								  CEGUI::Event::Subscriber(&InventoryBase::onDoubleClick, (InventoryBase*)this));
	m_frameWindow->subscribeEvent(CEGUI::Window::EventMouseButtonUp,
								  CEGUI::Event::Subscriber(&InventoryBase::onMouseUp, (InventoryBase*)this));
	m_frameWindow->subscribeEvent(CEGUI::Window::EventMouseMove,
								  CEGUI::Event::Subscriber(&InventoryBase::onMouseMove, (InventoryBase*)this));
	m_frameWindow->subscribeEvent(CEGUI::Window::EventMouseLeavesArea,
								  CEGUI::Event::Subscriber(&InventoryBase::onMouseLeave, (InventoryBase*)this));

	m_pane = static_cast<CEGUI::ScrollablePane*>(Singletons::getGUI()->createWidget("FOTDSkin/ScrollablePane",
																					glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
																					glm::vec4(0.0f),
																					m_frameWindow,
																					m_name + "_Inventory_PANE"));
	m_pane->setMouseInputPropagationEnabled(true);

	m_gridItems.clear();

	m_grid = static_cast<CEGUI::GUI_InventoryReceiver*>(Singletons::getGUI()->createWidget(
		"InventoryReceiver",
		glm::vec4(0.05f, 0.05f, INVENTORY_BOX_WIDTH * 5.0f, INVENTORY_BOX_HEIGHT * 5.0f),
		glm::vec4(0.0f),
		m_pane,
		m_name + "_Inventory_GRID"));
	m_grid->setContentSize(INVENTORY_WIDTH, 5);
	m_grid->setUserString("BlockImage", "FOTDSkin/InventoryBox");
	m_grid->setMouseInputPropagationEnabled(true);
	m_grid->subscribeEvent(CEGUI::Element::EventChildAdded,
						   CEGUI::Event::Subscriber(&InventoryBase::onDragDropItemAdded, (InventoryBase*)this));
	m_grid->subscribeEvent(CEGUI::Element::EventChildRemoved,
						   CEGUI::Event::Subscriber(&InventoryBase::onDragDropItemRemoved, (InventoryBase*)this));

	m_destRect = glm::vec4(-0.4f, -0.25f, 0.4f, 0.4f);

	setToDraw(false);

	m_initedGUI = true;

	Singletons::getGUI()->setActiveContext(0); // Reset back to normal GUI context.
	
	m_grid->setVerificationFunction(&weaponInventory_verification);
}
