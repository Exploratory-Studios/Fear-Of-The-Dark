#pragma once

#include "GUI_InventoryBase.h"
#include <CEGUI/widgets/DragContainer.h>

namespace CEGUI {

	class GUI_InventoryItem : public DragContainer, public GUI_InventoryBase {
		public:
			static const String WidgetTypeName;
			static const String EventNamespace;

			GUI_InventoryItem(const String& type, const String& name);

			// return if the location is solid (has an item)
			bool isSolidAtLocation(int x, int y) const;

			// Set the content based on the array. Input array must hold enough data.
			void setItemLayout(const bool* layout); // Should we use Item*s? And fully merge the systems? Check the rendering method, really look at the sample first.

			// return current X grid location on InventoryReceiver.  -1 means
			// invalid / not set.  This is typically set and invalidated by the
			// InventoryReciever::addItemAtLocation and InventoryReciever::removeItem
			// calls respectively.
			int locationOnReceiverX() const;

			// return current Y grid location on InventoryReceiver.  -1 means
			// invalid / not set.  This is typically set and invalidated by the
			// InventoryReciever::addItemAtLocation and InventoryReciever::removeItem
			// calls respectively.
			int locationOnReceiverY() const;

			// set the current grid location.  Note this does not update anything as
			// far as the receiver goes.  You will not normally call this directly, see
			// InventoryReciever::addItemAtLocation instead.
			void setLocationOnReceiver(int x, int y);

			// returns whether the current drag/drop target is a valid drop location for
			// this InventoryItem.  Only meaningful if isBeingDragged returns true.
			bool currentDropTargetIsValid() const;

			// base class overrides
			bool isHit(const Vector2f& position, const bool allow_disabled = false) const;
			void setContentSize(int width, int height);

		protected:
			// base class override
			void onMoved(ElementEventArgs& e);
			void onDragDropTargetChanged(DragDropEventArgs& e);
			void populateGeometryBuffer();
			Rectf gridBasePixelRect() const;

			// Is th ecurrent drag/drop target valid for this item
			bool m_validDropTarget;
			// Where it is on the reciever's map
			int m_recieverLocationX, m_recieverLocationY;

	};

}
