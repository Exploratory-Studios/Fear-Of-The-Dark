#include "FluidDomain.h"

#include <ResourceManager.h>

#include "Singletons.h"

#include "Tile.h"

namespace FluidModule {

	FluidDomain::FluidDomain(std::vector<std::vector<std::vector<Tile*>>>& tiles) {
		m_textureData = new std::vector<unsigned char>();
		m_densityFields.reserve(tiles.size());
		for(unsigned int x = 0; x < m_densityFields.size(); x++) {
			m_densityFields[x].reserve(tiles[x].size());
			for(unsigned int y = 0; y < m_densityFields[x].size(); y++) {
				if(!tiles[x][y][0]->isSolid()) {
					m_densityFields[x].push_back(
						new DensityField()); // Tile isn't solid, create a fluid DensityField here
				} else {
					m_densityFields[x].push_back(nullptr); // Tile is solid, push back a nullptr to both ensure proper
														   // system coords and show it's solid to the fluid
				}
			}
		}

		createTexture();
	}

	FluidDomain::~FluidDomain() {
		// Delete density fields
		for(unsigned int x = 0; x < m_densityFields.size(); x++) {
			for(unsigned int y = 0; y < m_densityFields[x].size(); y++) {
				delete m_densityFields[x][y];
			}
		}
	}

	void FluidDomain::draw(GLEngine::SpriteBatch& sb, glm::vec4& destRect) {
		// Makes sure texture is the right size according to the game's camera, as well as having the right data inside
		updateTexture(destRect);
	}

	void FluidDomain::updateTexture(glm::vec4& screenDestRect) {
		// Check if texture needs to be resized:
		if(m_lastScale != Singletons::getGameCamera()->getScale()) {
			// New scale is different, now to resize the texture:
			// Find out how many blocks are in the x and y dimensions of the screen: (floats to conserve partial blocks - multiple cells to each block)
			float blocksInScreenX = screenDestRect.z;
			float blocksInScreenY = screenDestRect.w;

			// Find how many cells are in the x and y dimensions of the texture (1 cell = 1 pixel)
			unsigned int cellsInScreenX = std::ceil(blocksInScreenX * FLUID_PARTITION_SIZE);
			unsigned int cellsInScreenY = std::ceil(blocksInScreenY * FLUID_PARTITION_SIZE);

			// Now we have the size, actually resize the texture
			resizeTexture(cellsInScreenX, cellsInScreenY);
		}

		// Texture is the correct size, m_texture contains the right size info. Just call glTexImage2d(...)
		// ... Or in our case, use the ResourceManager::setTexture call from GLEngine! Thanks, past Davis!

		GLEngine::ResourceManager::setTexture(m_texture, getTextureData(screenDestRect));
	}

	void FluidDomain::createTexture() {
		unsigned int size = 1; // Just create an arbitrarily-sized texture so that we have one at least.

		std::vector<unsigned char> data;
		data.resize(size * size, (unsigned char)0);
		m_texture = GLEngine::ResourceManager::addTexture("fluidTexture", size, size, data);

		// (Re)bind the texture object
		glBindTexture(GL_TEXTURE_2D, m_texture.id);

		// Set to not wrap
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Unbind
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void FluidDomain::resizeTexture(unsigned int width, unsigned int height) {
		// Change the texture object's dimensions for posterity.
		m_texture.width	 = width;
		m_texture.height = height;

		// When we glTexImage2D, we actually input the texture's width/height, so we don't need to bind/unbind and all that here.
	}

	std::vector<unsigned char>* FluidDomain::getTextureData(glm::vec4& screenDestRect) {
		m_textureData->reserve(m_texture.width * m_texture.height);

		// Get origin density field:
		unsigned int densityFieldX0 = std::floor(screenDestRect.x);
		unsigned int densityFieldY0 = std::floor(screenDestRect.y);

		// Get origin cell in density field
		// (portion of densityfield) * partitions in field
		unsigned int cellX0 = (screenDestRect.x - (float)densityFieldX0) * FLUID_PARTITION_SIZE;
		unsigned int cellY0 = (screenDestRect.y - (float)densityFieldY0) * FLUID_PARTITION_SIZE;

		// Now we have our origin cell at (densityFieldX0, densityFieldY0):(cellX0, cellY0)
		// We need to crawl (columns, THEN rows - OpenGL is weird) every cell now.
		for(unsigned int x = cellX0; x < screenDestRect.z * FLUID_PARTITION_SIZE; x++) {
			for(unsigned int y = densityFieldY0; y < screenDestRect.w * FLUID_PARTITION_SIZE; y++) {
				unsigned int indexGL  = x * screenDestRect.z * FLUID_PARTITION_SIZE + y;
				unsigned int indexReg = y * screenDestRect.w * FLUID_PARTITION_SIZE + x;

				(*m_textureData)[indexGL] =
					m_densityFields[x % FLUID_PARTITION_SIZE][y % FLUID_PARTITION_SIZE]->densities[indexReg].density;
			}
		}

		return m_textureData;
	}

	void FluidDomain::update() {
	}

	void updateDensityFields() {
		// Loop through fields and only update the non-nullptrs who don't have the equilibrium flag set
	}

	void updateDensityField(unsigned fieldX, unsigned fieldY) {
		// Loops through all FluidCells in density field, propagating them into their deltafields
	}

	void getReceivedDensityFromNeighbour(unsigned int fieldX,
										 unsigned int fieldY,
										 unsigned int cellX,
										 unsigned int cellY,
										 int		  cellXN,
										 int		  cellYN) {
		// Calculates how much density is traded between this cell (@x, y) and its neighbour (@xn, yn)
	}

} // namespace FluidModule
