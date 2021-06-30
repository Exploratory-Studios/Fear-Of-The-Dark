#include "FluidDomain.h"

#include <ResourceManager.h>

#include "Singletons.h"

#include "Tile.h"

namespace FluidModule {

	FluidDomain::FluidDomain(std::vector<std::vector<std::vector<Tile*>>>& tiles) {
		m_textureData = new std::vector<unsigned char>();
		m_densityFields.resize(tiles.size());
		for(unsigned int x = 0; x < m_densityFields.size(); x++) {
			m_densityFields[x].resize(tiles[x].size());
			for(unsigned int y = 0; y < m_densityFields[x].size(); y++) {
				if(!tiles[x][y][0]->isSolid()) {
					m_densityFields[x][y] = new DensityField(); // Tile isn't solid, create a fluid DensityField here
				} else {
					m_densityFields[x][y] = nullptr; // Tile is solid, push back a nullptr to both ensure proper
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

	void FluidDomain::updateField(Tile* tile) {
		DensityField* existent = m_densityFields[tile->getPosition().x][tile->getPosition().y];

		if(tile && !existent) {
			//if(!tile->isSolid())
			m_densityFields[tile->getPosition().x][tile->getPosition().y] = new DensityField();
		}
	}

	void FluidDomain::draw(GLEngine::SpriteBatch& sb, glm::vec4& destRect) {
		GLEngine::ColourRGBA8 fullColour = GLEngine::ColourRGBA8(255, 255, 255, 255);
		sb.draw(destRect, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_texture.id, 1.0f, fullColour);
	}

	void FluidDomain::updateTexture(glm::vec4& screenDestRect) {
		// Check if texture needs to be resized:
		//if(m_lastScale != Singletons::getGameCamera()->getScale()) {
		// New scale is different, now to resize the texture:
		// Find out how many blocks are in the x and y dimensions of the screen: (floats to conserve partial blocks - multiple cells to each block)
		float blocksInScreenX = screenDestRect.z;
		float blocksInScreenY = screenDestRect.w;

		// Find how many cells are in the x and y dimensions of the texture (1 cell = 1 pixel)
		unsigned int cellsInScreenX = std::ceil(blocksInScreenX * FLUID_PARTITION_SIZE);
		unsigned int cellsInScreenY = std::ceil(blocksInScreenY * FLUID_PARTITION_SIZE);

		// Now we have the size, actually resize the texture
		resizeTexture(cellsInScreenX, cellsInScreenY);

		m_lastScale = Singletons::getGameCamera()->getScale();
		//}

		// Texture is the correct size, m_texture contains the right size info. Just call glTexImage2d(...)
		// ... Or in our case, use the ResourceManager::setTexture call from GLEngine! Thanks, past Davis!

		updateTextureData(screenDestRect);
		glBindTexture(GL_TEXTURE_2D, m_texture.id);
		glTexSubImage2D(GL_TEXTURE_2D,
						(GLint)0,
						10, // Little experimental shift 10
						10,
						(GLsizei)m_texture.width - 10,
						(GLsizei)m_texture.height - 10,
						GL_RED,
						GL_UNSIGNED_BYTE,
						&((*m_textureData)[0]));
	}

	void FluidDomain::createTexture() {
		m_texture = GLEngine::ResourceManager::addTexture("fluidTexture",
														  m_densityFields.size(),
														  m_densityFields[0].size(),
														  *m_textureData,
														  GL_RED);

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

		m_textureData->resize(m_texture.width * m_texture.height, 0);

		glBindTexture(GL_TEXTURE_2D, m_texture.id);
		// Just allocates space, then later we can download sub textures.
		glTexImage2D(GL_TEXTURE_2D,
					 (GLint)0,
					 GL_RED,
					 m_texture.width,
					 m_texture.height,
					 (GLint)0,
					 GL_RED,
					 GL_UNSIGNED_BYTE,
					 nullptr);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void FluidDomain::updateTextureData(glm::vec4& screenDestRect) {
		// Get origin density field:

		int densityFieldX0 = std::floor(screenDestRect.x); // Can be negative/over the world's width
		int densityFieldY0 = std::floor(screenDestRect.y); // Can be negative/over the world's height

		// Get origin cell in density field
		// (portion of densityfield) * partitions in field
		int cellX0 = (screenDestRect.x - (float)densityFieldX0) * FLUID_PARTITION_SIZE;
		if(cellX0 < 0)
			cellX0++;
		int cellY0 = (screenDestRect.y - (float)densityFieldY0) * FLUID_PARTITION_SIZE;
		if(cellY0 < 0)
			cellY0++;
		// cellX0 & cellY0 are both now completely positive. Or should be, anyways

		unsigned int worldWidth = Singletons::getWorld()->getSize() * CHUNK_SIZE * FLUID_PARTITION_SIZE;

		// Now we have our origin cell at (densityFieldX0, densityFieldY0):(cellX0, cellY0)
		// We need to crawl (columns, THEN rows - OpenGL is weird) every cell now.

		for(unsigned int addedXFields = 0; addedXFields < std::ceil(screenDestRect.z); addedXFields++) {
			int densityIndexX = (densityFieldX0 + addedXFields + m_densityFields.size()) % m_densityFields.size();
			for(unsigned int addedYFields = 0;
				addedYFields < std::ceil(screenDestRect.w) && 0 < (addedYFields + densityFieldY0) <= WORLD_HEIGHT;
				addedYFields++) {
				int densityIndexY = (densityFieldY0 + addedYFields);

				if(densityIndexY >= WORLD_HEIGHT || densityIndexY < 0)
					continue; // This stops any truly out of bounds stuff from happening. Top of the world isn't stitched to the bottom!

				for(unsigned int addedXCells = 0; addedXCells < FLUID_PARTITION_SIZE; addedXCells++) {
					for(unsigned int addedYCells = 0; addedYCells < FLUID_PARTITION_SIZE; addedYCells++) {
						unsigned int cursoryCellX = addedXFields * FLUID_PARTITION_SIZE + addedXCells;
						unsigned int cursoryCellY = addedYFields * FLUID_PARTITION_SIZE + addedYCells;

						unsigned int textureCellIndex =
							(cursoryCellX * FLUID_PARTITION_SIZE * std::ceil(screenDestRect.w)) + cursoryCellY;

						DensityField* field = (m_densityFields[densityIndexX][densityIndexY]);

						if(field) {
							(*m_textureData)[textureCellIndex] =
								255; //field->densities[addedXCells + addedYCells * FLUID_PARTITION_SIZE].density;
						} else {
							(*m_textureData)[textureCellIndex] = 255;
						}
					}
				}
			}
		}

		for(unsigned int i = 0; i < m_textureData->size(); i++) {
			(*m_textureData)[i] = 128;
		}

		// There is a discrepancy between the texture's size and the amount of cells we traverse. As the size of the texture approaches zero, so does the difference between the discrepancy's two sides.
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
