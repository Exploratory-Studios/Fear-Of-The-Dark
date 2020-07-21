#include "GUI_BoolArray2D.h"

#include <stdexcept>

namespace CEGUI {

	GUI_ItemArray2D::GUI_ItemArray2D() : m_width(0), m_height(0), m_content(0) {

	}

	GUI_ItemArray2D::GUI_ItemArray2D(unsigned int width, unsigned int height) : m_content(0) {
		resetSize(width, height);
	}

	GUI_ItemArray2D::~GUI_ItemArray2D() {
		delete[] m_content;
	}

	bool GUI_ItemArray2D::getElementAtLocation(unsigned int x, unsigned int y) {
		if(x < 0 || x >= m_width || y < 0 || y >= m_height) {
			throw std::out_of_range("ItemArray2D::getElementAtLocation: Location out of range");
		}

		return m_content[y * m_width + x];
	}

	void GUI_ItemArray2D::setElementAtLocation(unsigned int x, unsigned int y, bool value) {
		if(x < 0 || x >= m_width || y < 0 || y >= m_height) {
			throw std::out_of_range("ItemArray2D::setElementAtLocation: Location out of range");
		}

		m_content[y * m_width + x] = value;
	}

	void GUI_ItemArray2D::clear(bool value/*= false*/) {
		if(!m_content)
			return;

		const unsigned int size = m_width * m_height;

		for(unsigned int i = 0; i < size; i++) {
			m_content[i] = value;
		}
	}

	void GUI_ItemArray2D::resetSize(unsigned int width, unsigned int height) {
		if(m_width != width || m_height || height) {
			delete[] m_content;
			m_width = width;
			m_height = height;
			m_content = new bool[width * height];
		}

		clear();
	}

}
