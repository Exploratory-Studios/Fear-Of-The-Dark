#pragma once

#include <CEGUI/Rect.h>

#include "GUI_BoolArray2D.h"

namespace CEGUI {

	class GUI_InventoryBase {
		public:
			virtual Sizef squarePixelSize() const; // Returns pixel size of a square in the grid

			virtual void setContentSize(unsigned int width, unsigned int height); // Sets the size of the grid and inits it.

			unsigned int getContentWidth() const;
			unsigned int getContentHeight() const;

			unsigned int gridXLocationFromPixelPosition(float pixelX) const; // Returns the grid position from a given screen pixel coord
			unsigned int gridYLocationFromPixelPosition(float pixelY) const; // Returns the grid position from a given screen pixel coord

		protected:
			virtual Rectf gridBasePixelRect() const = 0; // Return the screen rect where the content is rendered.

			GUI_ItemArray2D m_content; // Holds data.
	};

}
