#include "InventoryBase.h"

#include <string>
#include <math.h>

#include "Factory.h"

InventoryBase::InventoryBase(std::string& name, bool automaticResizing/* = true*/, bool initGUI/* = true*/, CEGUI::Window* parent/* = nullptr*/) : m_automaticallyResizes(automaticResizing) {
	if(initGUI) {
		Factory::getGUI()->setActiveContext(1); // Set this to the 2nd context (Inventory systems)

		if(parent) {
			m_frameWindow = static_cast<CEGUI::FrameWindow*>(Factory::getGUI()->createWidget(parent, "FOTDSkin/FrameWindow", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f), name + "_Inventory"));
		} else {
			m_frameWindow = static_cast<CEGUI::FrameWindow*>(Factory::getGUI()->createWidget("FOTDSkin/FrameWindow", glm::vec4(-0.2f, -0.2f, 0.35f, 0.5f), glm::vec4(0.0f), name + "_Inventory"));
		}
		m_frameWindow->setCloseButtonEnabled(false);
		m_frameWindow->setDragMovingEnabled(true);
		m_frameWindow->setRollupEnabled(false);
		m_frameWindow->setSizingEnabled(false);
		m_frameWindow->setTitleBarEnabled(false);
		//m_frameWindow->setProperty("BackgroundColours", "tl: 80FFFFFF tr: 80FFFFFF bl: 80FFFFFF br: 80FFFFFF");
		m_frameWindow->setProperty("CaptionColour", "FF101010");
		m_frameWindow->subscribeEvent(CEGUI::Window::EventMouseDoubleClick, CEGUI::Event::Subscriber(&InventoryBase::onDoubleClick, this));
		m_frameWindow->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&InventoryBase::onMouseUp, this));
		m_frameWindow->subscribeEvent(CEGUI::Window::EventMouseMove, CEGUI::Event::Subscriber(&InventoryBase::onMouseMove, this));
		m_frameWindow->subscribeEvent(CEGUI::Window::EventMouseLeavesArea, CEGUI::Event::Subscriber(&InventoryBase::onMouseLeave, this));

		m_pane = static_cast<CEGUI::ScrollablePane*>(Factory::getGUI()->createWidget(m_frameWindow, "FOTDSkin/ScrollablePane", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f), name + "_Inventory_PANE"));
		m_pane->setMouseInputPropagationEnabled(true);

		m_gridItems.clear();

		m_grid = static_cast<CEGUI::GUI_InventoryReceiver*>(Factory::getGUI()->createWidget(m_pane, "InventoryReceiver", glm::vec4(0.05f, 0.05f, INVENTORY_BOX_WIDTH * 5.0f, INVENTORY_BOX_HEIGHT * 5.0f), glm::vec4(0.0f), name + "_Inventory_GRID"));
		m_grid->setContentSize(INVENTORY_WIDTH, 5);
		m_grid->setUserString("BlockImage", "FOTDSkin/InventoryBox");
		m_grid->setMouseInputPropagationEnabled(true);
		m_grid->subscribeEvent(CEGUI::Element::EventChildAdded, CEGUI::Event::Subscriber(&InventoryBase::onDragDropItemAdded, this));
		m_grid->subscribeEvent(CEGUI::Element::EventChildRemoved, CEGUI::Event::Subscriber(&InventoryBase::onDragDropItemRemoved, this));

		m_destRect = glm::vec4(-0.4f, -0.25f, 0.4f, 0.4f);

		setToDraw(false);

		m_initedGUI = initGUI;

		Factory::getGUI()->setActiveContext(0); // Reset back to normal GUI context.
	}
}

bool InventoryBase::onDoubleClick(const CEGUI::EventArgs& e) {
	const CEGUI::MouseEventArgs args = static_cast<const CEGUI::MouseEventArgs&>(e);
	m_grabbingGUI = m_movable;

	return true;
}

bool InventoryBase::onMouseMove(const CEGUI::EventArgs& e) {
	const CEGUI::MouseEventArgs args = static_cast<const CEGUI::MouseEventArgs&>(e);
	if(m_grabbingGUI) {
		m_destRect.x += args.moveDelta.d_x / Factory::getGameCamera()->getScreenWidth();
		m_destRect.y += args.moveDelta.d_y / Factory::getGameCamera()->getScreenHeight();
	}
	return true;
}

bool InventoryBase::onMouseUp(const CEGUI::EventArgs& e) {
	m_grabbingGUI = false;
	return true;
}

bool InventoryBase::onMouseLeave(const CEGUI::EventArgs& e) {
	m_grabbingGUI = false;
	return false;
}

bool InventoryBase::onDragDropItemRemoved(const CEGUI::EventArgs& e) {
	if(m_resizing) return true;

	// This needs to remove the item from this' m_grid and m_items (call subtractItem with the data(item*) from Args::dragDropItem)
	// IF the item that's called this event has a drop target of !m_grid
	const CEGUI::WindowEventArgs args = static_cast<const CEGUI::WindowEventArgs&>(e);

	CEGUI::GUI_InventoryItem* removed_item = static_cast<CEGUI::GUI_InventoryItem*>(args.window);

	if(removed_item->currentDropTargetIsValid()) return false; // This means that it stays in the same receiver

	Item* item = removed_item->getData<Item>();
	queueSubtraction(item);

	return true;
}

bool InventoryBase::onDragDropItemAdded(const CEGUI::EventArgs& e) {
	if(m_resizing) return true;

	if((m_gridItems.size() / INVENTORY_WIDTH) == m_grid->getContentHeight() - 2 && m_automaticallyResizes) {
		resizeInventoryWidget();
	}

	const CEGUI::WindowEventArgs args = static_cast<const CEGUI::WindowEventArgs&>(e);
	CEGUI::GUI_InventoryItem* added_item = static_cast<CEGUI::GUI_InventoryItem*>(args.window);

	Item* item = added_item->getData<Item>();

	if(getItemIndex(item) != (unsigned int) - 1) return false; // It's already in this receiver. Don't do anything.

	m_items.push_back(item); // This should ensure that m_items and m_gridItems have the same indices.
	m_gridItems.push_back(added_item);

	return true;
}

InventoryBase::~InventoryBase() {
	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	winMgr.destroyWindow(m_frameWindow);
}

void InventoryBase::init() { // This must be seperate from the constructor due to the virtual, overridden function
	if(m_initedGUI) initGUI(m_frameWindow);
}

bool InventoryBase::addItem(Item* newItem) {
	m_weight += newItem->getWeight() * newItem->getQuantity();

	for(unsigned int i = 0; i < m_items.size(); i++) {
		if(m_items[i]->getID() == newItem->getID()) {
			m_items[i]->addToQuantity(newItem->getQuantity());
			return true;
		}
	}

	if(m_initedGUI) createInventoryItem(newItem);
	return true;
}

void InventoryBase::queueSubtraction(Item* item) {
	for(unsigned int i = 0; i < m_itemsToRemove.size(); i++) {
		if(m_itemsToRemove[i] == item) {
			return; // avoid doubles. CEGUI has a bug (I believe), that triggers two remove events and one add event when an item is moved from one reciever to the same reciever.
		}
	}

	m_itemsToRemove.push_back(item);
}

void InventoryBase::subtractItem(Item* item) {
	unsigned int index = (unsigned int) - 1;

	for(unsigned int i = 0; i < m_items.size(); i++) {
		if(item == m_items[i]) {
			index = i;
			break;
		}
	}

	if(index != (unsigned int) - 1) {
		if(m_initedGUI) m_grid->removeItem(*m_gridItems[index]);
		for(unsigned int j = index; j < m_items.size() - 1; j++) {
			m_items[j] = m_items[j + 1];
			m_gridItems[j] = m_gridItems[j + 1];
		}
		m_items.pop_back();
		m_gridItems.pop_back(); // ensure we affect both the m_items and m_gridItems
	}

	updateWeight();
}

void InventoryBase::updateWeight() { // Only use if something needs verification or is bugged
	m_weight = 0;
	for(unsigned int i = 0; i < m_items.size(); i++) {
		m_weight += m_items[i]->getWeight() * m_items[i]->getQuantity();
	}
}

void InventoryBase::update() {
	for(unsigned int i = 0; i < m_itemsToRemove.size(); i++) {
		subtractItem(m_itemsToRemove[i]);
	}
	m_itemsToRemove.clear();

	for(unsigned int i = 0; i < m_items.size(); i++) {
		if(m_items[i]->getQuantity() <= 0) {
			subtractItem(m_items[i]);
		}
	}
}

float InventoryBase::getCurrentWeight() {
	return m_weight;
}

unsigned int InventoryBase::getCount() {
	// Count how many total items we have
	unsigned int count = 0;
	for(unsigned int i = 0; i < m_items.size(); i++) {
		count += m_items[i]->getQuantity();
	}

	return count;
}

void InventoryBase::draw(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, float x, float y, bool relative/* = true*/) {
	if(m_initedGUI && m_frameWindow->isVisible()) {

		if(relative) {
			// Set m_frameWindow position and size based on camera's convert screen to world and screen dims (retrieved from cam)

			// Convert x and y to screen coords
			// Convert those screen coords to percentages
			// Move the frame window

			// Convert x and y
			glm::vec2 screenCoords = Factory::getGameCamera()->convertWorldToScreen(glm::vec2(x, y));
			screenCoords.y = Factory::getGameCamera()->getScreenHeight() - screenCoords.y;

			// Convert screenCoords to percentages
			glm::vec2 percentages = screenCoords / glm::vec2(Factory::getGameCamera()->getScreenWidth(), Factory::getGameCamera()->getScreenHeight());
			percentages.x += m_destRect.x;
			percentages.y += m_destRect.y;

			//glm::vec2 percentagesSize = glm::vec2(m_destRect.z, m_destRect.w) * glm::vec2(1.0f / cam.getScale(), cam.getScale());

			// Move the frame window
			m_frameWindow->setPosition(CEGUI::UVector2(cegui_reldim(percentages.x), cegui_reldim(percentages.y)));

			if(percentages.x < 0.0f || percentages.x > 1.0f) {
				setToDraw(false);
			}
			if(percentages.y < 0.0f || percentages.y > 1.0f) {
				setToDraw(false);
			}
			//m_frameWindow->setSize(CEGUI::USize(cegui_reldim(percentagesSize.x), cegui_reldim(percentages.y)));
		}

		{
			// Now draw all items overtop of each m_gridItem (m_items and m_gridItems SHOULD have the same indices)
			for(unsigned int i = 0; i < m_items.size(); i++) {
				glm::vec2 screenCoords = glm::vec2(m_gridItems[i]->getGridBasePixelRect().getPosition().d_x, m_gridItems[i]->getGridBasePixelRect().getPosition().d_y);
				glm::vec2 screenSize = glm::vec2(m_gridItems[i]->getGridBasePixelRect().getWidth(), m_gridItems[i]->getGridBasePixelRect().getHeight());

				float bottomY = m_pane->getViewableArea().top() + m_frameWindow->getInnerRectClipper().top();
				float topY = m_pane->getViewableArea().bottom() + m_frameWindow->getInnerRectClipper().top();

				glm::vec4 destRect(screenCoords.x, Factory::getGameCamera()->getScreenHeight() - screenCoords.y - screenSize.y, screenSize.x, screenSize.y);

				std::string quantityStr = std::to_string(m_items[i]->getQuantity());

				if((screenCoords.y >= bottomY && screenCoords.y + screenSize.y < topY) || m_gridItems[i]->isBeingDragged()) {
					glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

					GLuint textureID = m_items[i]->getTextureId();

					float depth = 0.0f;

					GLEngine::ColourRGBA8 colour(255, 255, 255, 255);

					sb.draw(destRect, uvRect, textureID, depth, colour);
					sf.draw(sb, quantityStr.c_str(), destRect * glm::vec4(1.0f, 1.0f, 0.5f, 0.5f) + glm::vec4(screenSize.x * 0.9f, 0.0f, 0.0f, 0.0f), glm::vec2(0.4f), depth, colour, GLEngine::Justification::RIGHT, uvRect);
				} else if(screenCoords.y < bottomY) {
					// Cut off the top (set the "position" bit of the UV as well as the "size" bit to match)
					float amntCutoff = bottomY - screenCoords.y;
					float portionOfTexture = amntCutoff / screenSize.y;
					float portionOfText = amntCutoff + 6 < screenSize.y * 0.4f ? 0.0f : ((amntCutoff + 6) - screenSize.y * 0.4f) / (screenSize.y * 0.6f);

					if(portionOfTexture < 1.0f) {
						glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f - portionOfTexture);

						GLuint textureID = m_items[i]->getTextureId();

						float depth = 0.0f;

						GLEngine::ColourRGBA8 colour(255, 255, 255, 255 * (1.0f - portionOfTexture));

						sb.draw(destRect + glm::vec4(0.0f, 0.0f, 0.0f, -amntCutoff), uvRect, textureID, depth, colour);
						sf.draw(sb, quantityStr.c_str(), destRect * glm::vec4(1.0f, 1.0f, 0.5f, 0.5f) + glm::vec4(screenSize.x * 0.9f, 0.0f, 0.0f, 0.0f), glm::vec2(0.4f, 0.4f * (1.0f - portionOfText)), depth, colour, GLEngine::Justification::RIGHT, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f - portionOfText));
					}
				} else if(screenCoords.y + screenSize.y >= topY) {
					// Cut off the bottom (set only the "size" bit in the UV)
					float amntCutoff = (screenCoords.y + screenSize.y) - topY;
					float portionOfTexture = amntCutoff / screenSize.y;
					float portionOfText = (amntCutoff) / (screenSize.y * 0.6f);

					if(portionOfTexture < 1.0f) {
						glm::vec4 uvRect(0.0f, portionOfTexture, 1.0f, 1.0f - portionOfTexture);

						GLuint textureID = m_items[i]->getTextureId();

						float depth = 0.0f;

						GLEngine::ColourRGBA8 colour(255, 255, 255, (int)(255.0f * (1.0f - portionOfTexture)));

						sb.draw(destRect + glm::vec4(0.0f, amntCutoff, 0.0f, -amntCutoff), uvRect, textureID, depth, colour);
						sf.draw(sb, quantityStr.c_str(), destRect * glm::vec4(1.0f, 1.0f, 0.5f, 0.5f) + glm::vec4(screenSize.x * 0.9f, amntCutoff, 0.0f, -amntCutoff), glm::vec2(0.4f, 0.4f * (1.0f - portionOfText)), depth, colour, GLEngine::Justification::RIGHT, glm::vec4(0.0f, portionOfText, 1.0f, 1.0f - portionOfText));
					}
				}
			}
		}

	}
}

SaveDataTypes::InventoryData InventoryBase::getInventorySaveData() {
	SaveDataTypes::InventoryData ret;
	for(unsigned int i = 0; i < m_items.size(); i++) {
		ret.itemData.push_back(m_items[i]->getItemSaveData());
	}
	return ret;
}

void InventoryBase::createInventoryItem(Item* item) {
	int x, y;
	x = m_gridItems.size() % INVENTORY_WIDTH;
	y = m_gridItems.size() / INVENTORY_WIDTH;

	// Create the item with no name, so we can set it after.
	Factory::getGUI()->setActiveContext(1); // Set GUI to inventory context.
	CEGUI::GUI_InventoryItem* gridItem = static_cast<CEGUI::GUI_InventoryItem*>(Factory::getGUI()->createWidget("FOTDSkin/InventoryItem", glm::vec4(0.05f, 0.05f, 0.9f, 0.9f), glm::vec4(0.0f), ""));
	Factory::getGUI()->setActiveContext(0); // Reset context to global

	std::ostringstream addressStr;
	addressStr << (void const*)gridItem; // converts 'this' to a string. This ensures all item windows have a unique ID
	std::string name = addressStr.str();
	gridItem->setName(gridItem->getName() + "_" + name);

	gridItem->setContentSize(1, 1);
	gridItem->setData(item);
	gridItem->setID(item->getID());
	m_grid->addItemAtLocation(*gridItem, x, y); // This triggers the event to add this to both m_gridItems & m_items
}

void InventoryBase::resizeInventoryWidget() {
	int w, h;
	w = INVENTORY_WIDTH;
	h = (m_gridItems.size() - 1) / INVENTORY_WIDTH + 3;

	m_resizing = true; // disables all events implemented by this class (onItemAdded, etc.)

	m_grid->setContentSize(w, h);
	m_grid->setSize(CEGUI::USize(cegui_reldim(INVENTORY_BOX_WIDTH * w), cegui_reldim(INVENTORY_BOX_HEIGHT * (h))));

	for(int y = 0; y < m_gridItems.size() - 1 / INVENTORY_WIDTH; y++) {
		for(int x = 0; x < INVENTORY_WIDTH && (y * INVENTORY_WIDTH + x) < m_gridItems.size(); x++) {
			m_grid->addItemAtLocation(*m_gridItems[(y * INVENTORY_WIDTH + x)], x, y);
		}
	}

	m_resizing = false; // re-enables all events

}
