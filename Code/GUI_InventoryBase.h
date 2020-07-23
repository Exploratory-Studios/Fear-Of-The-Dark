#pragma once

#include <CEGUI/Rect.h>

#include "GUI_BoolArray2D.h"

namespace CEGUI {

	class GUI_InventoryBase {
		public:
			virtual Sizef squarePixelSize() const; // Returns pixel size of a square in the grid

			virtual void setContentSize(int width, int height); // Sets the size of the grid and inits it.

			int getContentWidth() const;
			int getContentHeight() const;

			int gridXLocationFromPixelPosition(float pixelX) const; // Returns the grid position from a given screen pixel coord
			int gridYLocationFromPixelPosition(float pixelY) const; // Returns the grid position from a given screen pixel coord

			virtual Rectf getGridBasePixelRect() const;

		protected:
			virtual Rectf gridBasePixelRect() const = 0; // Return the screen rect where the content is rendered.

			GUI_BoolArray2D m_content; // Holds data.
	};

}
