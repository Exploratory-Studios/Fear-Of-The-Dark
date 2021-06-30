#include "GUI_InventoryItem.h"

#include "GUI_InventoryReceiver.h"

#include <CEGUI/Image.h>

namespace CEGUI {

	const String GUI_InventoryItem::WidgetTypeName("InventoryItem");
	const String GUI_InventoryItem::EventNamespace("InventoryItem");

	GUI_InventoryItem::GUI_InventoryItem(const String& type, const String& name) :
		DragContainer(type, name), m_validDropTarget(false), m_recieverLocationX(-1), m_recieverLocationY(-1) {
	}

	void GUI_InventoryItem::setContentSize(int width, int height) {
		GUI_InventoryBase::setContentSize(width, height);
		m_content.clear(m_id);
	}

	void GUI_InventoryItem::setID(int id) {
		m_id = id;
		m_content.clear(m_id);
	}

	int GUI_InventoryItem::getID() const {
		return m_id;
	}

	bool GUI_InventoryItem::isSolidAtLocation(int x, int y) const {
		return m_content.getElementAtLocation(x, y) != -1;
	}

	void GUI_InventoryItem::setItemLayout(const int* layout) {
		for(int y = 0; y < m_content.getHeight(); y++) {
			for(int x = 0; x < m_content.getWidth(); x++) {
				m_content.setElementAtLocation(x, y, *layout++);
			}
		}
	}

	int GUI_InventoryItem::locationOnReceiverX() const {
		return m_recieverLocationX;
	}

	int GUI_InventoryItem::locationOnReceiverY() const {
		return m_recieverLocationY;
	}

	void GUI_InventoryItem::setLocationOnReceiver(int x, int y) {
		m_recieverLocationX = x;
		m_recieverLocationY = y;
	}

	bool GUI_InventoryItem::isHit(const Vector2f& position, const bool allow_disabled) const {
		if(!DragContainer::isHit(position, allow_disabled)) {
			return false;
		}

		int gx = gridXLocationFromPixelPosition(position.d_x);
		int gy = gridYLocationFromPixelPosition(position.d_y);

		if(gx < 0 || gx >= m_content.getWidth() || gy < 0 || gy >= m_content.getHeight()) {
			return false;
		}

		return m_content.getElementAtLocation(gx, gy) == m_id;
	}

	bool GUI_InventoryItem::currentDropTargetIsValid() const {
		return m_validDropTarget;
	}

	void GUI_InventoryItem::populateGeometryBuffer() {
		if(!isUserStringDefined("BlockImage"))
			return;

		const Image* img = PropertyHelper<Image*>::fromString(getUserString("BlockImage"));

		if(!img)
			return;

		const Sizef square_size(squarePixelSize());

		argb_t colour = 0xFF00FF00;

		if(d_dragging && !currentDropTargetIsValid())
			colour = 0xFFFF0000;

		for(int y = 0; y < m_content.getHeight(); y++) {
			for(int x = 0; x < m_content.getWidth(); x++) {
				if(m_content.getElementAtLocation(x, y) != -1) {
					img->render(*d_geometry,
								Vector2f(x * square_size.d_width + 1, y * square_size.d_height + 1),
								Sizef(square_size.d_width - 2, square_size.d_height - 2),
								0,
								ColourRect(colour));
				}
			}
		}
	}

	Rectf GUI_InventoryItem::gridBasePixelRect() const {
		return getUnclippedOuterRect().get();
	}

	void GUI_InventoryItem::onMoved(ElementEventArgs& e) {
		invalidate();

		DragContainer::onMoved(e);

		GUI_InventoryReceiver* receiver = dynamic_cast<GUI_InventoryReceiver*>(d_dropTarget);

		if(receiver) {
			const Sizef square_size(receiver->squarePixelSize());
			Rectf		area(getUnclippedOuterRect().get());
			area.offset(Vector2f(square_size.d_width / 2, square_size.d_height / 2));
			const int x = receiver->gridXLocationFromPixelPosition(area.left());
			const int y = receiver->gridYLocationFromPixelPosition(area.top());

			m_validDropTarget = receiver->itemWillFitAtLocation(*this, x, y);
			return;
		}

		m_validDropTarget = false;
	}

	void GUI_InventoryItem::onDragDropTargetChanged(DragDropEventArgs& e) {
		DragContainer::onDragDropTargetChanged(e);

		m_validDropTarget = (dynamic_cast<GUI_InventoryReceiver*>(d_dropTarget) != 0);
		invalidate();
	}
} // namespace CEGUI
