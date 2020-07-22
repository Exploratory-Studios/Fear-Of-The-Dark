#pragma once

namespace CEGUI {

	class GUI_BoolArray2D { // Just holds if there's an item at a location
		public:
			GUI_BoolArray2D();
			GUI_BoolArray2D(int width, int height);
			~GUI_BoolArray2D();

			// Getters/setters
			int getWidth() const {
				return m_width;
			}
			int getHeight() const {
				return m_height;
			}

			bool getElementAtLocation(int x, int y) const;
			void setElementAtLocation(int x, int y, bool value);

			// Sets all elements to value
			void clear(bool value = false);

			// Sets the size, and clear()s
			void resetSize(int width, int height);

		private:
			int m_width, m_height;
			bool* m_content;

	};

}
