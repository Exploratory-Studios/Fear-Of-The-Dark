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
				//if(!tiles[x][y][0]->isSolid()) {
				m_densityFields[x][y] = new DensityField(); // Tile isn't solid, create a fluid DensityField here
															//} else {
				//	m_densityFields[x][y] = nullptr; // Tile is solid, push back a nullptr to both ensure proper
				//									 // system coords and show it's solid to the fluid
				//}
			}
		}
		m_fluidColour = GLEngine::ColourRGBA8(0, 119, 190, 255);
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
			if(!tile->isSolid())
				m_densityFields[tile->getPosition().x][tile->getPosition().y] = new DensityField();
		}
	}

	void FluidDomain::draw(GLEngine::SpriteBatch& sb, glm::vec4& destRect) {
		glm::vec4 snapped =
			glm::vec4(std::floor(destRect.x),
					  std::floor(destRect.y),
					  std::ceil(destRect.z * (float)FLUID_PARTITION_SIZE) / (float)FLUID_PARTITION_SIZE,
					  std::ceil(destRect.w * (float)FLUID_PARTITION_SIZE) / (float)FLUID_PARTITION_SIZE);
		sb.draw(snapped,
				glm::vec4(0.0f,
						  0.0f,
						  (float)m_usedTextureWidth / (float)m_allocatedTextureWidth,
						  (float)m_usedTextureHeight / (float)m_allocatedTextureHeight),
				m_texture.id,
				1.0f,
				m_fluidColour);
	}

	void FluidDomain::updateTexture(glm::vec4& screenDestRect) {
		// Check if texture exists yet lol
		if(m_texture.filePath == "")
			createTexture();

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

			m_lastScale = Singletons::getGameCamera()->getScale();
		}

		// Texture is the correct size, m_texture contains the right size info. Just call glTexImage2d(...)
		// ... Or in our case, use the ResourceManager::setTexture call from GLEngine! Thanks, past Davis!

		updateTextureData(screenDestRect);

		glBindTexture(GL_TEXTURE_2D, m_texture.id);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexSubImage2D(GL_TEXTURE_2D,
						(GLint)0,
						0,
						0,
						m_usedTextureWidth,
						m_usedTextureHeight,
						GL_RED,
						GL_UNSIGNED_BYTE,
						&((*m_textureData)[0]));

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}

	void FluidDomain::createTexture() {
		m_textureData->resize(m_allocatedTextureWidth * m_allocatedTextureHeight,
							  0); // Allocate all the texture we'll ever need.
		m_textureData->shrink_to_fit();
		m_texture = GLEngine::ResourceManager::addTexture("fluidTexture",
														  m_allocatedTextureWidth,
														  m_allocatedTextureHeight,
														  *m_textureData,
														  GL_RED);

		// (Re)bind the texture object
		glBindTexture(GL_TEXTURE_2D, m_texture.id);

		// Set to not wrap
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		// Unbind
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void FluidDomain::resizeTexture(unsigned int width, unsigned int height) {
		// Change the texture object's dimensions for posterity.
		m_usedTextureWidth	= width;
		m_usedTextureHeight = height;

		m_textureData->resize(width * height, 0);
		m_textureData->shrink_to_fit();

		// When we call glTexSubImage2D, it resizes the actual texture.
	}

	void FluidDomain::updateTextureData(glm::vec4& screenDestRect) {
		// Get origin density field:

		int densityFieldX0 = std::floor(screenDestRect.x); // Can be negative/over the world's width
		int densityFieldY0 = std::floor(screenDestRect.y); // Can be negative/over the world's height

		// Get origin cell in density field
		// (portion of densityfield) * partitions in field
		int cellX0 = (screenDestRect.x - (float)densityFieldX0) * FLUID_PARTITION_SIZE;
		while(cellX0 < 0)
			cellX0++;
		int cellY0 = (screenDestRect.y - (float)densityFieldY0) * FLUID_PARTITION_SIZE;
		while(cellY0 < 0)
			cellY0++;
		// cellX0 & cellY0 are both now completely positive. Or should be, anyways

		// Now we have our origin cell at (densityFieldX0, densityFieldY0):(cellX0, cellY0)

		for(unsigned int i = 0; i < m_textureData->size(); i++) {
			(*m_textureData)[i] = 0;
		}

		// Texturedata->size = textureDataWidthFields * FLUID_PARTITION_SIZE * textureDataHeightFields * FLUID_PARTITION_SIZE

		for(unsigned int addedCellY = 0; addedCellY < m_usedTextureHeight; addedCellY++) {
			unsigned int addedFieldY = addedCellY / FLUID_PARTITION_SIZE;
			unsigned int fieldY		 = densityFieldY0 + addedFieldY;
			if(!(0 <= fieldY && fieldY < WORLD_HEIGHT))
				continue; // Out of worldly bounds.

			for(unsigned int addedCellX = 0; addedCellX < m_usedTextureWidth; addedCellX++) {
				unsigned int addedFieldX = addedCellX / FLUID_PARTITION_SIZE;
				unsigned int fieldX = (densityFieldX0 + addedFieldX + m_densityFields.size()) % m_densityFields.size();

				unsigned int index = addedCellX + m_usedTextureWidth * addedCellY; // GL-space index

				DensityField* field = m_densityFields[fieldX][fieldY];

				if(field) {
					unsigned int cellX		= (addedCellX - addedFieldX * FLUID_PARTITION_SIZE);
					unsigned int cellY		= (addedCellY - addedFieldY * FLUID_PARTITION_SIZE);
					(*m_textureData)[index] = field->getDensityCell(cellX, cellY)->density;
				} else {
					(*m_textureData)[index] = 0;
				}
			}
		}

		// There is a discrepancy between the texture's size and the amount of cells we traverse. As the size of the texture approaches zero, so does the difference between the discrepancy's two sides.
	}

	void FluidDomain::update() {
		updateDensityFields(); // Right now everything just sets the equilibrium. Nothing breaks it.
	}

	void FluidDomain::updateDensityFields() {
		// Loop through fields and only update the non-nullptrs who don't have the equilibrium flag set
		for(unsigned int x = 0; x < m_densityFields.size(); x++) {
			for(unsigned int y = 0; y < m_densityFields[x].size(); y++) {
				if(m_densityFields[x][y])
					if(!m_densityFields[x][y]->inEquilibrium)
						updateDensityField(x, y); // Set the delta fields
			}
		}
		for(unsigned int x = 0; x < m_densityFields.size(); x++) {
			for(unsigned int y = 0; y < m_densityFields[x].size(); y++) {
				if(m_densityFields[x][y]) {
					if(!m_densityFields[x][y]->inEquilibrium) {
						DensityField* field = m_densityFields[x][y];
						field->swapForDelta();
					}
				}
			}
		}
	}

	void FluidDomain::updateDensityField(unsigned fieldX, unsigned fieldY) {
		// Loops through all FluidCells in density field, propagating them into their deltafields
		DensityField* field = m_densityFields[fieldX][fieldY];

		float totalTraded = 0.0f;

		for(unsigned int x = 0; x < FLUID_PARTITION_SIZE; x++) {
			for(unsigned int y = 0; y < FLUID_PARTITION_SIZE; y++) {
				FluidCell* cell0 = field->getDensityCell(x, y);

				FluidCell* cellX0	= getRelativeDeltaCell(fieldX, fieldY, x, y, +1, 0);
				float	   tradedX0 = getReceivedDensityFromNeighbour(fieldX, fieldY, x, y, +1, 0);
				cellX0->density += tradedX0;
				totalTraded += tradedX0;

				if(fieldY < WORLD_HEIGHT - 1) {
					FluidCell* cellY0	= getRelativeDeltaCell(fieldX, fieldY, x, y, 0, +1);
					float	   tradedY0 = getReceivedDensityFromNeighbour(fieldX, fieldY, x, y, 0, +1);
					cellY0->density += tradedY0;
					totalTraded += tradedY0;
				}

				FluidCell* cellX1	= getRelativeDeltaCell(fieldX, fieldY, x, y, -1, 0);
				float	   tradedX1 = getReceivedDensityFromNeighbour(fieldX, fieldY, x, y, -1, 0);
				cellX1->density += tradedX1;
				totalTraded += tradedX1;

				if(fieldY > 0) {
					FluidCell* cellY1	= getRelativeDeltaCell(fieldX, fieldY, x, y, 0, -1);
					float	   tradedY1 = getReceivedDensityFromNeighbour(fieldX, fieldY, x, y, 0, -1);
					cellY1->density += tradedY1;
					totalTraded += tradedY1;
				}
			}
		}

		if(totalTraded < 0.1f) {
			// Equilibrium has been reached. Set the flag!
			field->inEquilibrium = true;
		}
	}

	float FluidDomain::getReceivedDensityFromNeighbour(unsigned int fieldX,
													   unsigned int fieldY,
													   unsigned int cellX,
													   unsigned int cellY,
													   int			cellXMod,
													   int			cellYMod) {
		// Calculates how much density is traded between this cell (@x, y) and its neighbour (@xn, yn)
		// Uses the formula: [1 - (neighbour[n].density - (ideal_density +- gravity))/(sum_neighbours_dens)](density - ideal_density)
		float neighbourDensitySum = 0.0f;
		if(fieldY == 0 && cellY == 0) {
			// We're at the bottom
			// Only use three total neighbours. Not below.
			neighbourDensitySum += getRelativeCellDensity(fieldX, fieldY, cellX, cellY, -1, 0); // Left
			neighbourDensitySum += getRelativeCellDensity(fieldX, fieldY, cellX, cellY, +1, 0); // Right
			neighbourDensitySum += getRelativeCellDensity(fieldX, fieldY, cellX, cellY, 0, +1); // Up
		} else if(fieldY == WORLD_HEIGHT - 1 && cellY == FLUID_PARTITION_SIZE - 1) {
			// We're at the tippy-top
			// Only use three total neighbours. Not above.
			neighbourDensitySum += getRelativeCellDensity(fieldX, fieldY, cellX, cellY, -1, 0); // Left
			neighbourDensitySum += getRelativeCellDensity(fieldX, fieldY, cellX, cellY, +1, 0); // Right
			neighbourDensitySum += getRelativeCellDensity(fieldX, fieldY, cellX, cellY, 0, -1); // Down
		} else {
			// We're anywhere else. Use all four neighbours :)
			neighbourDensitySum += getRelativeCellDensity(fieldX, fieldY, cellX, cellY, -1, 0); // Left
			neighbourDensitySum += getRelativeCellDensity(fieldX, fieldY, cellX, cellY, +1, 0); // Right
			neighbourDensitySum += getRelativeCellDensity(fieldX, fieldY, cellX, cellY, 0, +1); // Up
			neighbourDensitySum += getRelativeCellDensity(fieldX, fieldY, cellX, cellY, 0, -1); // Down
		}

		if(std::abs(neighbourDensitySum) < 0.01f) {
			return 0.0f;
		}

		float neighbourDensity = getRelativeCellDensity(fieldX, fieldY, cellX, cellY, cellXMod, cellYMod);
		float idealDens		   = m_idealDensity;
		if(cellYMod == 1) {
			idealDens += m_gravityConstant;
		} else if(cellYMod == -1) {
			idealDens -= m_gravityConstant;
		}

		float tradedDensity;
		float term1, term2;

		// Term 1 = [1 - (neighbour[n].density - (ideal_density +- gravity))/(sum_neighbours_dens)]
		term1 = (1 - (neighbourDensity - idealDens) / (neighbourDensitySum));

		// Term 2 = (density - ideal_density)
		term2 = (m_densityFields[fieldX][fieldY]->getDensityCell(cellX, cellY)->density - idealDens);

		tradedDensity = term1 * term2;

		return tradedDensity;
	}

	FluidCell* FluidDomain::getRelativeCell(unsigned int fieldX0,
											unsigned int fieldY0,
											unsigned int cellX0,
											unsigned int cellY0,
											int			 cellXOffset,
											int			 cellYOffset) {
		// Just assume the offsetted cell is within worldly bounds. (In the y-direction at least)
		int newCellX  = cellX0 + cellXOffset;
		int newCellY  = cellY0 + cellYOffset;
		int newFieldX = fieldX0;
		int newFieldY = fieldY0;

		if(newCellX < 0) {
			newFieldX--;
			newCellX += FLUID_PARTITION_SIZE;
			if(newFieldX < 0) {
				newFieldX += m_densityFields.size();
			}
		} else if(newCellX > FLUID_PARTITION_SIZE - 1) {
			newFieldX++;
			newCellX -= FLUID_PARTITION_SIZE;
			if(newFieldX > m_densityFields.size() - 1) {
				newFieldX -= m_densityFields.size();
			}
		}

		if(newCellY < 0) {
			newFieldY--;
			newCellY += FLUID_PARTITION_SIZE;
		} else if(newCellY > FLUID_PARTITION_SIZE - 1) {
			newFieldY++;
			newCellY -= FLUID_PARTITION_SIZE;
		}

		return m_densityFields[newFieldX][newFieldY]->getDensityCell(newCellX, newCellY);
	}

	float FluidDomain::getRelativeCellDensity(unsigned int fieldX0,
											  unsigned int fieldY0,
											  unsigned int cellX0,
											  unsigned int cellY0,
											  int		   cellXOffset,
											  int		   cellYOffset) {
		getRelativeCell(fieldX0, fieldY0, cellX0, cellY0, cellXOffset, cellYOffset)->density;
	}

	FluidCell* FluidDomain::getRelativeDeltaCell(unsigned int fieldX0,
												 unsigned int fieldY0,
												 unsigned int cellX0,
												 unsigned int cellY0,
												 int		  cellXOffset,
												 int		  cellYOffset) {
		// Just assume the offsetted cell is within worldly bounds. (In the y-direction at least)
		int newCellX  = cellX0 + cellXOffset;
		int newCellY  = cellY0 + cellYOffset;
		int newFieldX = fieldX0;
		int newFieldY = fieldY0;

		if(newCellX < 0) {
			newFieldX--;
			newCellX += FLUID_PARTITION_SIZE;
			if(newFieldX < 0) {
				newFieldX += m_densityFields.size();
			}
		} else if(newCellX > FLUID_PARTITION_SIZE - 1) {
			newFieldX++;
			newCellX -= FLUID_PARTITION_SIZE;
			if(newFieldX > m_densityFields.size() - 1) {
				newFieldX -= m_densityFields.size();
			}
		}

		if(newCellY < 0) {
			newFieldY--;
			newCellY += FLUID_PARTITION_SIZE;
		} else if(newCellY > FLUID_PARTITION_SIZE - 1) {
			newFieldY++;
			newCellY -= FLUID_PARTITION_SIZE;
		}

		return m_densityFields[newFieldX][newFieldY]->getDeltaDensityCell(newCellX, newCellY);
	}

	float FluidDomain::getRelativeDeltaCellDensity(unsigned int fieldX0,
												   unsigned int fieldY0,
												   unsigned int cellX0,
												   unsigned int cellY0,
												   int			cellXOffset,
												   int			cellYOffset) {
		getRelativeDeltaCell(fieldX0, fieldY0, cellX0, cellY0, cellXOffset, cellYOffset)->density;
	}

} // namespace FluidModule
