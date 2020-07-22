#include "GUI_BoolArray2D.h"

#include <stdexcept>

namespace CEGUI {

	GUI_BoolArray2D::GUI_BoolArray2D() : m_width(0), m_height(0), m_content(0) {

	}

	GUI_BoolArray2D::GUI_BoolArray2D(int width, int height) : m_content(0) {
		resetSize(width, height);
	}

	GUI_BoolArray2D::~GUI_BoolArray2D() {
		delete[] m_content;
	}

	bool GUI_BoolArray2D::getElementAtLocation(int x, int y) const {
		if(x < 0 || x >= m_width || y < 0 || y >= m_height) {
			throw std::out_of_range("ItemArray2D::getElementAtLocation: Location out of range");
		}

		return m_content[y * m_width + x];
	}

	void GUI_BoolArray2D::setElementAtLocation(int x, int y, bool value) {
		if(x < 0 || x >= m_width || y < 0 || y >= m_height) {
			throw std::out_of_range("ItemArray2D::setElementAtLocation: Location out of range");
		}

		m_content[y * m_width + x] = value;
	}

	void GUI_BoolArray2D::clear(bool value/*= false*/) {
		if(!m_content)
			return;

		const int size = m_width * m_height;

		for(int i = 0; i < size; i++) {
			m_content[i] = value;
		}
	}

	void GUI_BoolArray2D::resetSize(int width, int height) {
		if(m_width != width || m_height || height) {
			delete[] m_content;
			m_width = width;
			m_height = height;
			m_content = new bool[width * height];
		}

		clear();
	}

}
