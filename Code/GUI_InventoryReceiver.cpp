#include "GUI_InventoryReceiver.h"
#include "GUI_InventoryItem.h"

#include <CEGUI/Image.h>

namespace CEGUI {
	const String GUI_InventoryReceiver::WidgetTypeName("InventoryReceiver");
	const String GUI_InventoryReceiver::EventNamespace("InventoryReceiver");

	GUI_InventoryReceiver::GUI_InventoryReceiver(const String& type, const String& name) :
		Window(type, name), m_verificationFunction([](GUI_InventoryItem& a) -> bool { return true; }),
		m_receiving(true) {
		setDragDropTarget(true);
	}

	bool GUI_InventoryReceiver::addItemAtLocation(GUI_InventoryItem& item, int x, int y) {
		if(itemWillFitAtLocation(item, x, y) &&
		   (m_verificationFunction(item) || dynamic_cast<GUI_InventoryReceiver*>(item.getParent()) == this)) {
			GUI_InventoryReceiver* old_receiver = dynamic_cast<GUI_InventoryReceiver*>(item.getParent());

			if(old_receiver)
				old_receiver->removeItem(item);

			item.setLocationOnReceiver(x, y);
			writeItemToContentMap(item);
			addChild(&item);

			// set position and size.  This ensures the items visually match the
			// logical content map.
			item.setPosition(UVector2(UDim(static_cast<float>(x) / getContentWidth(), 0),
									  UDim(static_cast<float>(y) / getContentHeight(), 0)));
			item.setSize(USize(UDim(static_cast<float>(item.getContentWidth()) / getContentWidth(), 0),
							   UDim(static_cast<float>(item.getContentHeight()) / getContentHeight(), 0)));

			return true;
		}

		return false;
	}

	void GUI_InventoryReceiver::removeItem(GUI_InventoryItem& item) {
		if(item.getParent() != this || item.locationOnReceiverX() == -1 || item.locationOnReceiverY() == -1)
			return;

		eraseItemFromContentMap(item);
		item.setLocationOnReceiver(-1, -1);
		removeChild(&item);
	}

	void GUI_InventoryReceiver::writeItemToContentMap(const GUI_InventoryItem& item) {
		if(item.locationOnReceiverX() == -1 || item.locationOnReceiverY() == -1)
			return;

		for(int y = 0; y < item.getContentHeight(); ++y) {
			const int map_y = item.locationOnReceiverY() + y;

			for(int x = 0; x < item.getContentWidth(); ++x) {
				const int map_x = item.locationOnReceiverX() + x;

				int val;
				if(item.isSolidAtLocation(x, y)) {
					val = item.getID();
				} else {
					val = m_content.getElementAtLocation(map_x, map_y);
				}
				m_content.setElementAtLocation(map_x, map_y, val);
			}
		}

		invalidate();
	}

	void GUI_InventoryReceiver::eraseItemFromContentMap(const GUI_InventoryItem& item) {
		if(item.locationOnReceiverX() == -1 || item.locationOnReceiverY() == -1)
			return;

		for(int y = 0; y < item.getContentHeight(); ++y) {
			const int map_y = item.locationOnReceiverY() + y;

			for(int x = 0; x < item.getContentWidth(); ++x) {
				const int map_x = item.locationOnReceiverX() + x;

				int val;
				if(item.isSolidAtLocation(x, y)) {
					val = -1;
				} else {
					val = m_content.getElementAtLocation(map_x, map_y);
				}

				m_content.setElementAtLocation(map_x, map_y, val);
			}
		}

		invalidate();
	}

	bool GUI_InventoryReceiver::itemWillFitAtLocation(const GUI_InventoryItem& item, int x, int y) {
		if(x < 0 || y < 0)
			return false;

		if(x + item.getContentWidth() > m_content.getWidth() || y + item.getContentHeight() > m_content.getHeight())
			return false;

		if(!m_receiving)
			return false;

		const bool already_attached = this == item.getParent();
		// if item is already attatched erase its data from the content map so the
		// test result is reliable.
		if(already_attached)
			eraseItemFromContentMap(item);

		bool result = true;
		for(int item_y = 0; item_y < item.getContentHeight() && result; ++item_y) {
			for(int item_x = 0; item_x < item.getContentWidth() && result; ++item_x) {
				if(m_content.getElementAtLocation(item_x + x, item_y + y) != item.getID() &&
				   m_content.getElementAtLocation(item_x + x, item_y + y) != -1 &&
				   item.isSolidAtLocation(item_x, item_y))
					result = false;
			}
		}

		// re-write item into content map if we erased it earlier.
		if(already_attached)
			writeItemToContentMap(item);

		return result;
	}

	void GUI_InventoryReceiver::onDragDropItemDropped(DragDropEventArgs& e) {
		GUI_InventoryItem* item = dynamic_cast<GUI_InventoryItem*>(e.dragDropItem);

		if(!item)
			return;

		const Sizef square_size(squarePixelSize());

		Rectf item_area(item->getUnclippedOuterRect().get());
		item_area.offset(Vector2f(square_size.d_width / 2, square_size.d_height / 2));

		const int drop_x = gridXLocationFromPixelPosition(item_area.left());
		const int drop_y = gridYLocationFromPixelPosition(item_area.top());

		addItemAtLocation(*item, drop_x, drop_y);
	}

	void GUI_InventoryReceiver::populateGeometryBuffer() {
		if(!isUserStringDefined("BlockImage"))
			return;

		const Image* img = PropertyHelper<Image*>::fromString(getUserString("BlockImage"));

		if(!img)
			return;

		const Sizef square_size(squarePixelSize());

		for(int y = 0; y < m_content.getHeight(); ++y) {
			for(int x = 0; x < m_content.getWidth(); ++x) {
				argb_t colour = 0xFFFFFFFF;
				if(m_content.getElementAtLocation(x, y))
					colour = 0xFFFFFFFF;

				img->render(*d_geometry,
							Vector2f(x * square_size.d_width + 1, y * square_size.d_height + 1),
							Sizef(square_size.d_width - 2, square_size.d_height - 2),
							0,
							ColourRect(colour));
			}
		}
	}

	Rectf GUI_InventoryReceiver::gridBasePixelRect() const {
		return getChildContentArea().get();
	}

} // namespace CEGUI
