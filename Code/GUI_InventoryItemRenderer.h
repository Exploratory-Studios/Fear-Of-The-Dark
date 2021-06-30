#pragma once

#include <CEGUI/WindowRenderer.h>

namespace CEGUI {

	/*
	 * Really, only two states:
	 *  1. Normal
	 *  2. DraggingValidTarget (when the item has space to go into a container)
	 *  3. DraggingInvalidTarget (only when the item is out of a container/there's no space for it)
	 *
	 */

	class GUI_InventoryItemRenderer : public WindowRenderer {
	  public:
		static const String TypeName;
		GUI_InventoryItemRenderer(const String& type);
		void render();
	};

} // namespace CEGUI
