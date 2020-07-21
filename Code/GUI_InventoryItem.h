#pragma once

#include "InventoryBase.h"
#include <CEGUI/widget/DragContainer.h>

namespace CEGUI {

	class GUI_InventoryItem : public DragContainer, public InventoryBase {
		public:
			static const String WidgetTypeName;
			static const String EventNamespace;

			GUI_InventoryItem(const String& type, const String& name);

			// return if the location is solid (has an item)
			bool isSolidAtLocation(unsigned int x, unsigned int y) const;

			// Set the content based on the array. Input array must hold enough data.
			void setItemLayout(const bool* layout); // Should we use Item*s? And fully merge the systems? Check the rendering method, really look at the sample first.

	};

}
