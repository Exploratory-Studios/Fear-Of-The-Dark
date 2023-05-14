#include "Map.hpp"

#include <math.h>

#include "Singletons.h"

Map::Map() {
	initDiscoveredVector();
}

Map::~Map() {
}

void Map::discoverRect(glm::vec4 destRect) {
	setRect(destRect, true);
}
void Map::discoverCircle(glm::vec2 pos, float radius) {
	setCircle(pos, radius, true);
}
void Map::undiscoverRect(glm::vec4 destRect) {
	setRect(destRect, false);
}
void Map::undiscoverCircle(glm::vec2 pos, float radius) {
	setCircle(pos, radius, false);
}
void Map::draw(BARE2D::BasicRenderer* renderer) {
}

void Map::initDiscoveredVector() {
	// Get the required size
	unsigned int xSize = Singletons::getWorld()->getSize();
	unsigned int ySize = WORLD_HEIGHT;

	// Now allocate the vectors
	for(unsigned int x = 0; x < xSize; x++) {
		std::vector<bool> column;
		for(unsigned int y = 0; y < ySize; y++) {
			column.push_back(false);
		}

		m_discovered.push_back(column);
	}

	// Now m_discovered is a 2d vector of size {xSize, ySize} and is accessed as m_discovered[x][y].
}

void Map::setRect(glm::vec4 destRect, bool setting) {
	// Set those values!
	for(unsigned int x = 0; x < (unsigned int)destRect.z; x++) {
		for(unsigned int y = 0; y < (unsigned int)destRect.w; y++) {
			unsigned int adjustedX = ((unsigned int)destRect.x + x + m_discovered.size()) % m_discovered.size();
			unsigned int adjustedY =
				std::min(std::max((unsigned int)destRect.y + y, WORLD_HEIGHT - 1), (unsigned int)0);

			m_discovered[adjustedX][adjustedY] = setting;
		}
	}
}

void Map::setCircle(glm::vec2 centre, float radius, bool setting) {
	for(int x = -(int)radius; x < (int)radius; x++) {
		for(int y = -(int)radius; y < (int)radius; y++) {
			float sqDist = x * x + y * y;
			if(sqDist > radius * radius)
				continue;

			// We are within the circle
			unsigned int adjustedX = ((unsigned int)centre.x + x + m_discovered.size()) % m_discovered.size();
			unsigned int adjustedY = std::min(std::max((unsigned int)centre.y + y, WORLD_HEIGHT - 1), (unsigned int)0);

			m_discovered[adjustedX][adjustedY] = setting;
		}
	}
}