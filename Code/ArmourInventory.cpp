#include "ArmourInventory.h"

#include "Singletons.h"

#include <CEGUI/CEGUI.h>

bool armourInventory_verification(CEGUI::GUI_InventoryItem& item) {
	Item* i = item.getData<Item>();

	if(dynamic_cast<ItemArmour*>(i)) { // This item is an armour
		return true;
	}
	return false;
}

ArmourInventory::ArmourInventory(std::string&	name,
								 bool			automaticResizing /*= false*/,
								 bool			initGUI /*= true*/,
								 CEGUI::Window* parent /*= nullptr*/) :
	InventoryBase(name, automaticResizing, initGUI, parent) {
}

ArmourInventory::~ArmourInventory() {
}

void ArmourInventory::initInventoryGUI() {
	Singletons::getGUI()->setActiveContext(1); // Set this to the 2nd context (Inventory systems)

	if(m_parent) {
		m_frameWindow =
			static_cast<CEGUI::FrameWindow*>(Singletons::getGUI()->createWidget(m_parent,
																				"FOTDSkin/FrameWindow",
																				glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
																				glm::vec4(0.0f),
																				m_name + "_Inventory"));
	} else {
		m_frameWindow =
			static_cast<CEGUI::FrameWindow*>(Singletons::getGUI()->createWidget("FOTDSkin/FrameWindow",
																				glm::vec4(-0.2f, -0.2f, 0.35f, 0.5f),
																				glm::vec4(0.0f),
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

	m_pane = static_cast<CEGUI::ScrollablePane*>(Singletons::getGUI()->createWidget(m_frameWindow,
																					"FOTDSkin/ScrollablePane",
																					glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
																					glm::vec4(0.0f),
																					m_name + "_Inventory_PANE"));
	m_pane->setMouseInputPropagationEnabled(true);

	m_gridItems.clear();

	m_grid = static_cast<CEGUI::GUI_InventoryReceiver*>(Singletons::getGUI()->createWidget(
		m_pane,
		"InventoryReceiver",
		glm::vec4(0.05f, 0.05f, INVENTORY_BOX_WIDTH * 5.0f, INVENTORY_BOX_HEIGHT * 5.0f),
		glm::vec4(0.0f),
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
	
	m_grid->setVerificationFunction(&armourInventory_verification);
}
