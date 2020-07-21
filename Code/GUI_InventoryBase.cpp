#include "GUI_InventoryBase.h"

namespace CEGUI {

	Sizef GUI_InventoryBase::squarePixelSize() const {
		// Returns pixel size of a square in the grid
		const Rectf area(gridBasePixelRect());
		return Sizef(area.getWidth() / m_content.getWidth(),
		             area.getHeight() / m_content.getHeight());
	}

	void GUI_InventoryBase::setContentSize(unsigned int width, unsigned int height) {
		// Sets the size of the grid and inits it.
		m_content.resetSize(width, height);
	}

	unsigned int GUI_InventoryBase::getContentWidth() const {
		return m_content.getWidth();
	}

	unsigned int GUI_InventoryBase::getContentHeight() const {
		return m_content.getHeight();
	}

	unsigned int GUI_InventoryBase::gridXLocationFromPixelPosition(float pixelX) const {
		// Returns the grid position from a given screen pixel coord
		const Rectf area(gridBasePixelRect());

		if(pixelX < static_cast<unsigned int>(area.left()) ||
		        pixelX >= static_cast<unsigned int>(area.right()))
			return -1;

		return static_cast<unsigned int>((pixelX - area.left()) / (area.getWidth() / m_content.getWidth()));
	}
	unsigned int GUI_InventoryBase::gridYLocationFromPixelPosition(float pixelY) const {
		// Returns the grid position from a given screen pixel coord
		const Rectf area(gridBasePixelRect());

		if(pixelY < static_cast<unsigned int>(area.top()) ||
		        pixelY >= static_cast<unsigned int>(area.bottom()))
			return -1;

		return static_cast<unsigned int>((pixelY - area.top()) / (area.getHeight() / m_content.getHeight()));

	}
}
