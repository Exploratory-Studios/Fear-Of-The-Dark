#include "TileAlignedTexture.hpp"

#include <BAREErrors.hpp>
#include <ResourceManager.hpp>

#include <math.h>

TileAlignedTexture::TileAlignedTexture(std::string	name,
									   unsigned int allctWidth,
									   unsigned int allctHeight,
									   unsigned int xPartsPerTile,
									   unsigned int yPartsPerTile,
									   GLenum		minMagFilter,
									   unsigned int channels,
									   GLenum		format) {
	m_texture = BARE2D::ResourceManager::createMutableTexture(name,
															  allctWidth * xPartsPerTile,
															  allctHeight * yPartsPerTile,
															  minMagFilter,
															  channels,
															  format);

	m_xParts		  = xPartsPerTile;
	m_yParts		  = yPartsPerTile;
	m_allocatedWidth  = allctWidth * m_xParts;
	m_allocatedHeight = allctHeight * m_yParts;
	m_width			  = m_allocatedWidth;
	m_height		  = m_allocatedHeight;
}

TileAlignedTexture::~TileAlignedTexture() {
}

void TileAlignedTexture::render(BARE2D::BasicRenderer* renderer, glm::vec4& screenRect, BARE2D::Colour& colour) {
	if(!m_texture) {
		BARE2D::throwError(BARE2D::BAREError::NULL_PTR_ACCESS, "In TileAlignedTexture::render()");
		return;
	}
	glm::vec4 snapped = glm::vec4(std::floor(screenRect.x),
								  std::floor(screenRect.y),
								  std::ceil(screenRect.z * (float)m_xParts) / (float)m_xParts,
								  std::ceil(screenRect.w * (float)m_yParts) / (float)m_yParts);

	glm::vec4 uv =
		glm::vec4(0.0f, 0.0f, (float)m_width / (float)m_allocatedWidth, (float)m_height / (float)m_allocatedHeight);

	renderer->draw(snapped, uv, m_texture->id, 0.0f, colour);
}
void TileAlignedTexture::updateSize(float cameraScale, glm::vec4& screenRect) {
	// Check if texture needs to be resized:
	if(m_lastScale != cameraScale) {
		// New scale is different, now to resize the texture:
		// Find out how many blocks are in the x and y dimensions of the screen: (floats to conserve partial blocks - multiple cells to each block)
		float blocksInScreenX = screenRect.z;
		float blocksInScreenY = screenRect.w;

		// Find how many cells are in the x and y dimensions of the texture (1 cell = 1 pixel)
		unsigned int cellsInScreenX = std::ceil(blocksInScreenX * m_xParts);
		unsigned int cellsInScreenY = std::ceil(blocksInScreenY * m_yParts);

		// Now we have the size, actually resize the texture
		m_width	 = cellsInScreenX;
		m_height = cellsInScreenY;

		m_lastScale = cameraScale;

		if(m_width > m_allocatedWidth || m_height > m_allocatedHeight) {
			m_width	 = m_allocatedWidth;
			m_height = m_allocatedHeight;
			BARE2D::throwError(BARE2D::BAREError::OTHER_ERROR,
							   "Size of texture has exceeded allocated size. Check your program, dipshit.");
		}
	}
}

void TileAlignedTexture::updateData(unsigned char* data) {
	// Texture is the correct size, m_texture contains the right size info. Just call glTexImage2d(...)
	// ... Or in our case, use the ResourceManager::MutableTexture::setData()! Thanks, past Davis!
	m_texture->setData(data, 0, 0, m_width, m_height);
}
unsigned int TileAlignedTexture::getWidth() const {
	return m_width;
}
unsigned int TileAlignedTexture::getHeight() const {
	return m_height;
}
