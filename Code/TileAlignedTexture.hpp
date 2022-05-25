#pragma once

#include <glm/glm.hpp>
#include <MutableTexture.hpp>
#include <BasicRenderer.hpp>

/**
 * @class TileAlignedTexture
 * @brief Simply allows a mutable texture to be drawn easily aligned to the world, depending on the camera.
 */
class TileAlignedTexture {
  public:
	/**
   * @brief Constructs a Tile Aligned Texture. Something that renders perfectly in-line with tiles. Useful for uploading information about tiles in texture format. 
   * @param name The name of the texture. Must be unique.
   * @param allctWidth The width (in tiles) that should be allocated. This is effectively the max size.
   * @param allctHeight The height (in tiles) that should be allocated. This is effectively the max size.
   * @param xPartsPerTile The number of pixels per tile (X-direction)
   * @param yPartsPerTile The number of pixels per tile (Y-direction)
   * @param minMagFilter The filter to use when minimizing and magnifying. Generally GL_LINEAR or GL_NEAREST (GL_NEAREST)
   * @param channels The number of colour channels each pixel should have. Ranges from 1-4 and should align with format (4)
   * @param format The enum describing the internal colour format. GL_RED for one channel, GL_RG for 2, GL_RGB for 3, GL_RGBA for 4. (GL_RGBA)
   */
	TileAlignedTexture(std::string	name,
					   unsigned int allctWidth,
					   unsigned int allctHeight,
					   unsigned int xPartsPerTile,
					   unsigned int yPartsPerTile,
					   GLenum		minMagFilter = GL_NEAREST,
					   unsigned int channels	 = 4,
					   GLenum		format		 = GL_RGBA);
	~TileAlignedTexture();

	/**
	 * @brief Renders the texture aligned with the tiles.
	 * @param renderer The renderer to render with
	 * @param screenRect The screen position and size. Obtained from gameplayScreen's getScreenBox()
	 */
	void render(BARE2D::BasicRenderer* renderer, glm::vec4& screenRect, BARE2D::Colour& colour);
	/**
	 * @brief Updates the size of the texture based on the camera scale. Can be called every frame with maximum efficiency.
	 * @param cameraScale The current scale of the camera.
	 */
	void updateSize(float cameraScale, glm::vec4& screenRect);
	/**
	 * @brief Updates the data in the texture
	 * @param data The data to overwrite with. Size should be getWidth()*getHeight().
	 */
	void updateData(unsigned char* data);

	/**
	 * @return The current width of the texture being used. 
	 */
	unsigned int getWidth() const;
	/**
	 * @return The current height of the texture being used. 
	 */
	unsigned int getHeight() const;

  private:
	BARE2D::MutableTexture* m_texture = nullptr;
	unsigned int			m_width, m_height;
	unsigned int			m_allocatedWidth, m_allocatedHeight;
	unsigned int			m_xParts, m_yParts;
	float					m_lastScale = 1.0f;
};
