#pragma once

namespace CEGUI {

	class GUI_IntArray2D { // Just holds if there's an item at a location
		public:
			GUI_IntArray2D();
			GUI_IntArray2D(int width, int height);
			~GUI_IntArray2D();

			// Getters/setters
			int getWidth() const {
				return m_width;
			}
			int getHeight() const {
				return m_height;
			}

			int getElementAtLocation(int x, int y) const;
			void setElementAtLocation(int x, int y, int value);

			// Sets all elements to value
			void clear(int value = -1);

			// Sets the size, and clear()s
			void resetSize(int width, int height);

		private:
			int m_width, m_height;
			int* m_content;

	};

}
