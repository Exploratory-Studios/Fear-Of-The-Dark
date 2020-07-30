#pragma once

#include "GUI_InventoryBase.h"
#include <CEGUI/Window.h>

#include "GUI_InventoryItem.h"

namespace CEGUI {

	class GUI_InventoryReceiver : public Window, public GUI_InventoryBase {
		public:
			static const String WidgetTypeName;
			static const String EventNamespace;

			GUI_InventoryReceiver(const String& type, const String& name);

			// returns whether the item will fit at the given grid coords
			bool itemWillFitAtLocation(const GUI_InventoryItem& item, int x, int y);

			// Adds the given item at the given grid coords to the content. it'll only be added if the item will fit.
			// Returns true if the item was added correctly
			bool addItemAtLocation(GUI_InventoryItem& item, int x, int y);

			// removes an item
			void removeItem(GUI_InventoryItem& item);

			// Sets the verification function
			void setVerificationFunction(std::function<bool(GUI_InventoryItem&)> func) {
				m_verificationFunction = func;
			}

			void setReceiving(bool& setting) {
				m_receiving = setting;
			}

		protected:
			// write the item's layoout data into the content map.
			void writeItemToContentMap(const GUI_InventoryItem& item);
			// erases.
			void eraseItemFromContentMap(const GUI_InventoryItem& item);

			// base class overrides
			void onDragDropItemDropped(DragDropEventArgs& e);
			void populateGeometryBuffer();
			Rectf gridBasePixelRect() const;

			// Function to verify if an item can be placed
			std::function<bool(GUI_InventoryItem&)> m_verificationFunction;

			// Can this receiver accept new items?
			bool m_receiving = true;
	};

}
