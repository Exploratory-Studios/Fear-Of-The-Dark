#include "GUI_InventoryItemRenderer.h"

#include "GUI_InventoryItem.h"

#include <CEGUI/falagard/WidgetLookFeel.h>

namespace CEGUI {

	const String GUI_InventoryItemRenderer::TypeName("InventoryItemRenderer");

	GUI_InventoryItemRenderer::GUI_InventoryItemRenderer(const String& type) :
		WindowRenderer(type) {

	}

	void GUI_InventoryItemRenderer::render() {
		const WidgetLookFeel& wlf = getLookNFeel();

		GUI_InventoryItem* item = dynamic_cast<GUI_InventoryItem*>(d_window);

		if(!item) {
			wlf.getStateImagery(d_window->isDisabled() ? "Disabled" : "Enabled").render(*d_window);
		}
		if(item->isBeingDragged()) {
			wlf.getStateImagery(item->currentDropTargetIsValid() ? "DraggingValidTarget" : "DraggingInvalidTarget").render(*item);
		} else {
			wlf.getStateImagery("Normal").render(*item);
		}
	}

}
