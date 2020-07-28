#include "GUI_IntArray2D.h"

#include <stdexcept>

namespace CEGUI {

	GUI_IntArray2D::GUI_IntArray2D() : m_width(0), m_height(0), m_content(0) {

	}

	GUI_IntArray2D::GUI_IntArray2D(int width, int height) : m_content(0) {
		resetSize(width, height);
	}

	GUI_IntArray2D::~GUI_IntArray2D() {
		delete[] m_content;
	}

	int GUI_IntArray2D::getElementAtLocation(int x, int y) const {
		if(x < 0 || x >= m_width || y < 0 || y >= m_height) {
			throw std::out_of_range("ItemArray2D::getElementAtLocation: Location out of range");
		}

		return m_content[y * m_width + x];
	}

	void GUI_IntArray2D::setElementAtLocation(int x, int y, int value) {
		if(x < 0 || x >= m_width || y < 0 || y >= m_height) {
			throw std::out_of_range("ItemArray2D::setElementAtLocation: Location out of range");
		}

		m_content[y * m_width + x] = value;
	}

	void GUI_IntArray2D::clear(int value/*= -1*/) {
		if(!m_content)
			return;

		const int size = m_width * m_height;

		for(int i = 0; i < size; i++) {
			m_content[i] = value;
		}
	}

	void GUI_IntArray2D::resetSize(int width, int height) {
		if(m_width != width || m_height || height) {
			delete[] m_content;
			m_width = width;
			m_height = height;
			m_content = new int[width * height];
		}

		clear();
	}

}
