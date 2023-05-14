#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <BasicRenderer.hpp>
#include <MutableTexture.hpp>

/**
 * @class Map
 * @brief Holds all the data on what the Player has discovered, as well as a method to draw that to a texture.
 */
class Map {
  public:
	Map();
	~Map();

	/**
	 * @brief Marks a rectangle (destrect) in worldspace coordinates as discovered 
	 * @param destRect The rectangle to mark discovered (worldspace coordinates)
	 */
	void discoverRect(glm::vec4 destRect);

	/**
	 * @brief Marks a circle as discovered on the map 
	 * @param pos The position of the center of the circle
	 * @param radius The radius of the circle (# of tiles)
	 */
	void discoverCircle(glm::vec2 pos, float radius);

	/**
	 * @brief Marks a rectangle as undiscovered 
	 * @param destRect The rectangle
	 */
	void undiscoverRect(glm::vec4 destRect);
	/**
	 * @brief Marks a circle as undiscovered 
	 * @param pos The centre position
	 * @param radius The radius
	 */
	void undiscoverCircle(glm::vec2 pos, float radius);

	/**
	 * @brief Literally just draws the entire map out - scrolling and GUI is left to the screen. 
	 * @param renderer The renderer. Duh.
	 */
	void draw(BARE2D::BasicRenderer* renderer);

  private:
	/**
 * @brief Sets the m_discovered vector to all 0 and the right size depending on the world size. 
 */
	void initDiscoveredVector();
	void setRect(glm::vec4 destRect, bool setting);
	void setCircle(glm::vec2 centre, float radius, bool setting);

	BARE2D::MutableTexture* m_texture; // This is the texture for the map's display
	std::vector<std::vector<bool>>
		m_discovered; // This represents every tile - true if discovered/shown on map, false otherwise.
};