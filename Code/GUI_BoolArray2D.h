#pragma once

class Item;

namespace CEGUI {

	class GUI_BoolArray2D { // Just holds if there's an item at a location
		public:
			GUI_BoolArray2D();
			GUI_BoolArray2D(unsigned int width, unsigned int height);
			~GUI_BoolArray2D();

			// Getters/setters
			unsigned int getWidth() const {
				return m_width;
			}
			unsigned int getHeight() const {
				return m_height;
			}

			bool getElementAtLocation(unsigned int x, unsigned int y);
			void setElementAtLocation(unsigned int x, unsigned int y, bool value);

			// Sets all elements to value
			void clear(bool value = false);

			// Sets the size, and clear()s
			void resetSize(unsigned int width, unsigned int height);

		private:
			unsigned int m_width, m_height;
			bool* m_content;

	};

}
