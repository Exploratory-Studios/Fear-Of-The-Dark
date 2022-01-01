#include "FluidDomain.h"

#include <ResourceManager.h>
#include <GLContextManager.h>

#include "XMLData.h"
#include "Singletons.h"

#include "Tile.h"
#include "DensityField.h"

namespace FluidModule {

	FluidDomain::FluidDomain(std::vector<std::vector<std::vector<Tile*>>>& tiles, unsigned int id) {
		m_id = id;
		init();

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
	}

	FluidDomain::~FluidDomain() {
		// Delete density fields
		for(unsigned int x = 0; x < m_densityFields.size(); x++) {
			for(unsigned int y = 0; y < m_densityFields[x].size(); y++) {
				if(m_densityFields[x][y])
					delete m_densityFields[x][y];
			}
		}
	}
	
	void FluidDomain::init() {
		XMLModule::FluidData data = XMLModule::XMLData::getFluidData(m_id);
		m_fluidColour			  = GLEngine::ColourRGBA8(data.red, data.green, data.blue, data.alpha);
		m_viscosity				  = data.viscosity;
		m_gravityConstant		  = data.gravConstant;
		m_idealDensity			  = data.idealDensity;
		m_trickleConstant 		  = data.trickleConstant;
		
		m_textureData = new std::vector<unsigned char>();
	}
	
	void FluidDomain::init(SaveDataTypes::FluidData& data) {
		m_id = data.id;
		
		init();
		
		m_densityFields.resize(data.xSize);
		for(unsigned int x = 0; x < m_densityFields.size(); x++) {
			m_densityFields[x].resize(data.ySize);
			for(unsigned int y = 0; y < m_densityFields[x].size(); y++) {
				m_densityFields[x][y] = nullptr;
			}
		}
		
		for(unsigned int i = 0; i < data.densityFields.size(); i++) {
			m_densityFields[data.densityFields[i].x][data.densityFields[i].y] = new DensityField(data.densityFields[i]);
		}
	}

	void FluidDomain::updateField(Tile* tile) {
		DensityField* existent = m_densityFields[tile->getPosition().x][tile->getPosition().y];
		if(tile) {
			if(!tile->isSolid()) {
				if(!existent)
					m_densityFields[tile->getPosition().x][tile->getPosition().y] = new DensityField();
			} else if(existent) {
				// Tile is solid and we have a density field here. Delete it!
				delete m_densityFields[tile->getPosition().x][tile->getPosition().y];
				m_densityFields[tile->getPosition().x][tile->getPosition().y] = nullptr;
			}
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

		GLEngine::GLContextManager::getGLContext()->bindTexture(GL_TEXTURE_2D, m_texture.id);
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
		m_texture = GLEngine::ResourceManager::addTexture("fluidTexture" + std::to_string(m_id),
		            m_allocatedTextureWidth,
		            m_allocatedTextureHeight,
		            *m_textureData,
		            GL_RED);

		// (Re)bind the texture object
		GLEngine::GLContextManager::getGLContext()->bindTexture(GL_TEXTURE_2D, m_texture.id);

		// Set to not wrap
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		if(m_smoothFluid) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		} else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}

		// Unbind
		GLEngine::GLContextManager::getGLContext()->bindTexture(GL_TEXTURE_2D, 0);
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
					unsigned int cellX = (addedCellX - addedFieldX * FLUID_PARTITION_SIZE);
					unsigned int cellY = (addedCellY - addedFieldY * FLUID_PARTITION_SIZE);
					(*m_textureData)[index] =
					    std::max(std::min(field->getDensityCell(cellX, cellY)->density * 255, 255.0f), 0.0f);

					if(field->inEquilibrium == false) {
						//(*m_textureData)[index] = 128;
					}
				} else {
					(*m_textureData)[index] = 0;
				}
			}
		}

		// There is a discrepancy between the texture's size and the amount of cells we traverse. As the size of the texture approaches zero, so does the difference between the discrepancy's two sides.
	}

	void FluidDomain::update() {
		updateDensityFields();
	}

	void FluidDomain::addFluid(unsigned int fieldX,
	                           unsigned int fieldY,
	                           unsigned int cellX,
	                           unsigned int cellY,
	                           float		amount) {
		DensityField* field = m_densityFields[fieldX][fieldY];

		field->getDensityCell(cellX, cellY)->density += amount;

		m_brokenEquilibriums.push_back(field);
	}

	void FluidDomain::setFluid(unsigned int fieldX,
	                           unsigned int fieldY,
	                           unsigned int cellX,
	                           unsigned int cellY,
	                           float		amount) {
		DensityField* field = m_densityFields[fieldX][fieldY];

		field->getDensityCell(cellX, cellY)->density		   = amount;
		field->getDensityCell(cellX, cellY)->avgDensity_3Steps = amount;

		m_brokenEquilibriums.push_back(field);
	}

	bool FluidDomain::displaceFluidArea(float x0, float y0, float x1, float y1) {
		unsigned int x0_cs = std::floor(x0 * FLUID_PARTITION_SIZE), // x0 in fluid cellspace
		             y0_cs = std::floor(y0 * FLUID_PARTITION_SIZE), //  fluid cellspace
		             x1_cs = std::ceil(x1 * FLUID_PARTITION_SIZE), //  fluid cellspace
		             y1_cs = std::ceil(y1 * FLUID_PARTITION_SIZE); //  fluid cellspace

		std::vector<glm::vec2> availableFieldCoords;

		// Check for available surrounding fields above/below:
		for(unsigned int fieldX = std::floor(x0); fieldX < std::ceil(x1); fieldX++) {
			// Check above
			if(getRelativeField(fieldX, std::floor(y0), 0, -1)) {
				availableFieldCoords.push_back(glm::vec2(fieldX + 0.5f, std::floor(y0)-1 + 0.5f));
			}
			// Check below
			if(getRelativeField(fieldX, std::ceil(y1), 0, 1)) {
				availableFieldCoords.push_back(glm::vec2(fieldX + 0.5f, std::ceil(y1)+1 + 0.5f));
			}
		}

		// Check for available surrounding fields on left/right:
		for(unsigned int fieldY = std::floor(y0); fieldY < std::ceil(y1); fieldY++) {
			// Check to the left
			if(getRelativeField(std::floor(y0), fieldY, -1, 0)) {
				availableFieldCoords.push_back(glm::vec2(std::floor(y0)-1 + 0.5f, fieldY + 0.5f));
			}
			// Check below
			if(getRelativeField(std::ceil(y1), fieldY, 1, 0)) {
				availableFieldCoords.push_back(glm::vec2(std::ceil(y1)+1 + 0.5f, fieldY + 0.5f));
			}
		}

		// If there are no available fields at this point, we can't continue. Return false, as there's simply no place to displace water.
		if(availableFieldCoords.size() == 0) {
			return false;
		}

		for(unsigned int cellX = x0_cs; cellX < x1_cs; cellX++) {
			for(unsigned int cellY = y0_cs; cellY < y1_cs; cellY++) {
				// At this point in the loop, we're running this for every cell in the area

				float x = float(cellX - x0_cs) * float(FLUID_PARTITION_SIZE) + x0; // find the "real" coordinates of this cell
				float y = float(cellY - y0_cs) * float(FLUID_PARTITION_SIZE) + y0;

				// Check for closest field:
				unsigned int closestIndex = 0;
				float closest = 99999.99f;

				for(unsigned int i = 0; i < availableFieldCoords.size(); i++) {
					float dist = glm::distance(availableFieldCoords[i], glm::vec2(x, y));
					if(closest > dist) {
						closest = dist;
						closestIndex = i;
					}
				}

				// Move the cell's density to the closest field:
				float endX, endY;

				// Find the final x value for this density.
				if(x < availableFieldCoords[closest].x - 0.5f) {
					// move it to the left side of the field
					endX = availableFieldCoords[closest].x - 0.5f;
				} else if(x > availableFieldCoords[closest].x + 0.5f) {
					// move it to the right side of the field
					endX = availableFieldCoords[closest].x + 0.5f;
				} else {
					// no need to move the x value
					endX = x;
				}

				// Find the final y value for this density
				if(y < availableFieldCoords[closest].y - 0.5f) {
					// move it to the bottom side of the field
					endY = availableFieldCoords[closest].y - 0.5f;
				} else if(x > availableFieldCoords[closest].y + 0.5f) {
					// move it to the top side of the field
					endY = availableFieldCoords[closest].y + 0.5f;
				} else {
					// no need to move the y value
					endY = y;
				}

				// Finally, add the density to that field at that position and remove it from currPosition
				unsigned int endX_field = std::floor(endX);
				unsigned int endY_field = std::floor(endY);

				unsigned int endX_cell = (endX - endX_field) / FLUID_PARTITION_SIZE;
				unsigned int endY_cell = (endY - endY_field) / FLUID_PARTITION_SIZE;

				unsigned int currX_field = std::floor(x);
				unsigned int currY_field = std::floor(y);

				unsigned int currX_cell = (x - currX_field) / FLUID_PARTITION_SIZE;
				unsigned int currY_cell = (y - currY_field) / FLUID_PARTITION_SIZE;


				addFluid(endX_field, endY_field, endX_cell, endY_cell, getRelativeCell(currX_field, currY_field, currX_cell, currY_cell, 0, 0)->density);
				setFluid(currX_field, currY_field, currX_cell, currY_cell, 0.0f);

			}
		}

		return true;
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
						if(!field->checkForEquilibrium()) {
							// Equilibrium has been broken, set the neighbours!
							m_brokenEquilibriums.push_back(getRelativeField(x, y, -1, 0));
							m_brokenEquilibriums.push_back(getRelativeField(x, y, 1, 0));

							if(y > 0) {
								m_brokenEquilibriums.push_back(m_densityFields[x][y - 1]);
							}
							if(y < m_densityFields[x].size() - 1) {
								m_brokenEquilibriums.push_back(m_densityFields[x][y + 1]);
							}
						}
						field->swapForDelta();
					}
				}
			}
		}

		for(unsigned int i = 0; i < m_madeEquilibriums.size(); i++) {
			if(m_madeEquilibriums[i])
				m_madeEquilibriums[i]->inEquilibrium = true;
		}
		m_madeEquilibriums.clear();

		for(unsigned int i = 0; i < m_brokenEquilibriums.size(); i++) {
			if(m_brokenEquilibriums[i])
				m_brokenEquilibriums[i]->inEquilibrium = false;
		}
		m_brokenEquilibriums.clear();
	}

	void FluidDomain::updateDensityField(unsigned fieldX, unsigned fieldY) {
		// Loops through all FluidCells in density field, propagating them into their deltafields
		DensityField* field = m_densityFields[fieldX][fieldY];

		float totalTraded = 0.0f;

		for(unsigned int x = 0; x < FLUID_PARTITION_SIZE; x++) {
			for(unsigned int y = 0; y < FLUID_PARTITION_SIZE; y++) {
				FluidCell* cell0	   = field->getDensityCell(x, y);
				float	   cell0_needs = m_idealDensity - cell0->density;
				FluidCell* cell0_delta = field->getDeltaDensityCell(x, y);

				// Calculate total neighbourly needs (excluding self)
				float neighbourlyNeeds = 0.0f;

				FluidCell* cellX0		= getRelativeCell(fieldX, fieldY, x, y, -1, 0);
				float	   cellX0_needs = 0.0f;
				FluidCell* cellX1		= getRelativeCell(fieldX, fieldY, x, y, 1, 0);
				float	   cellX1_needs = 0.0f;
				FluidCell* cellX0_d		= getRelativeDeltaCell(fieldX, fieldY, x, y, -1, 0);
				FluidCell* cellX1_d		= getRelativeDeltaCell(fieldX, fieldY, x, y, 1, 0);

				float trickle = ((std::rand() % 200) - 100)/200.0f * m_trickleConstant * m_idealDensity;

				if(cellX0) {
					cellX0_needs = std::max((m_idealDensity + trickle) - cellX0->density, 0.0f);
					neighbourlyNeeds += cellX0_needs;
				}
				if(cellX1) {
					cellX1_needs = std::max((m_idealDensity - trickle) - cellX1->density, 0.0f);
					neighbourlyNeeds += cellX1_needs;
				}

				FluidCell* cellY0		= nullptr;
				float	   cellY0_needs = 0.0f;
				FluidCell* cellY0_d		= nullptr;
				if(fieldY > 0) {
					cellY0	 = getRelativeCell(fieldX, fieldY, x, y, 0, -1);
					cellY0_d = getRelativeDeltaCell(fieldX, fieldY, x, y, 0, -1);
					if(cellY0) {
						cellY0_needs = std::max((m_idealDensity + m_gravityConstant) - cellY0->density, 0.0f);
						neighbourlyNeeds += cellY0_needs;
					}
				}

				FluidCell* cellY1		= nullptr;
				float	   cellY1_needs = 0.0f;
				FluidCell* cellY1_d		= nullptr;
				if(fieldY < WORLD_HEIGHT - 1) {
					cellY1	 = getRelativeCell(fieldX, fieldY, x, y, 0, 1);
					cellY1_d = getRelativeDeltaCell(fieldX, fieldY, x, y, 0, 1);
					if(cellY1) {
						cellY1_needs = std::max((m_idealDensity - m_gravityConstant) - cellY1->density, 0.0f);
						neighbourlyNeeds += cellY1_needs;
					}
				}

				// Now we have neighbourly need, find out their individual weights;
				float cellX0_w = 0.0f, cellX1_w = 0.0f, cellY0_w = 0.0f, cellY1_w = 0.0f;

				if(neighbourlyNeeds > 0.0f) {
					if(cellX0) cellX0_w = cellX0_needs / neighbourlyNeeds;
					if(cellX1) cellX1_w = cellX1_needs / neighbourlyNeeds;
					if(cellY0) cellY0_w = cellY0_needs / neighbourlyNeeds;
					if(cellY1) cellY1_w = cellY1_needs / neighbourlyNeeds;
				} else {
					// Neighbours are pretty dang full up there chief. Don't wanna divide by 0. Just set all the weights to 0.
					cellX0_w = cellX1_w = cellY0_w = cellY1_w = 0.0f;
					neighbourlyNeeds = 0.0f;
				}

				// Now we have their weights (great!) we just need to multiply that by the donated density (which decreases as viscosity increases to 1)
				float 		 selfDensity = cell0->density;
				// Ensure we aren't transferring more than we need to (greater than needs) or more than we can (density * viscosity reduction)
				float		 donatedDensity = std::min(neighbourlyNeeds, selfDensity * (1.0f - m_viscosity));

				float dX0 = 0.0f, dX1 = 0.0f, dY0 = 0.0f, dY1 = 0.0f;

				if(cellX0) {
					dX0 = donatedDensity * cellX0_w;
					cellX0_d->density += dX0;
				}

				if(cellX1) {
					dX1 = donatedDensity * cellX1_w;
					cellX1_d->density += dX1;
				}

				if(cellY0) {
					dY0 = donatedDensity * cellY0_w;
					cellY0_d->density += dY0;
				}
				if(cellY1) {
					dY1 = donatedDensity * cellY1_w;
					cellY1_d->density += dY1;
				}

				cell0_delta->density += selfDensity - donatedDensity;
				cell0->updateAvgDensity(); // A good a place as any for this.
			}
		}
	}

	float FluidDomain::getReceivedDensityFromNeighbour(unsigned int fieldX,
	        unsigned int fieldY,
	        unsigned int cellX,
	        unsigned int cellY,
	        int			cellXMod,
	        int			cellYMod,
	        float		neighbourSum) {
		// Calculates how much density is traded between this cell (@x, y) and its neighbour (@xn, yn)
		/*float idealDens = m_idealDensity;
		if(cellYMod == -1) {
			idealDens -= m_gravityConstant;
		} else if(cellYMod == 1) {
			idealDens += m_gravityConstant;
		}

		float selfDens = m_densityFields[fieldX][fieldY]->getDensityCell(cellX, cellY)->density;

		float densityNeedSum += idealDens - selfDens;

		float neighbourDensityNeed =
			idealDens - getRelativeCellDensity(fieldX, fieldY, cellX, cellY, cellXMod, cellYMod);

		// (Roughly) (neighbour_density_weight) * (self_density). Pretty simple, really.
		float tradedDensity = (neighbourDensityNeed / densityNeedSum) * (selfDens);

		return std::max(tradedDensity, 0.0f);*/
	}

	DensityField* FluidDomain::getRelativeField(unsigned int fieldX0,
	        unsigned int fieldY0,
	        int			 fieldXOffset,
	        int			 fieldYOffset) {
		int newFieldX = fieldX0 + fieldXOffset;
		int newFieldY = fieldY0 + fieldYOffset;

		if(newFieldX < 0) {
			newFieldX += m_densityFields.size();
		} else if(newFieldX > m_densityFields.size() - 1) {
			newFieldX -= m_densityFields.size();
		}

		if(newFieldY > WORLD_HEIGHT - 1 || newFieldY < 0) {
			return nullptr;
		}

		DensityField* field = m_densityFields[newFieldX][newFieldY];

		if(field)
			return field;

		return nullptr;
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
			if(newFieldY < 0)
				return nullptr;
			newCellY += FLUID_PARTITION_SIZE;
		} else if(newCellY > FLUID_PARTITION_SIZE - 1) {
			newFieldY++;
			if(newFieldY > WORLD_HEIGHT - 1)
				return nullptr;
			newCellY -= FLUID_PARTITION_SIZE;
		}

		DensityField* field = m_densityFields[newFieldX][newFieldY];

		if(field)
			return field->getDensityCell(newCellX, newCellY);

		return nullptr;
	}

	float FluidDomain::getRelativeCellDensity(unsigned int fieldX0,
	        unsigned int fieldY0,
	        unsigned int cellX0,
	        unsigned int cellY0,
	        int		   cellXOffset,
	        int		   cellYOffset) {
		FluidCell* cell = getRelativeCell(fieldX0, fieldY0, cellX0, cellY0, cellXOffset, cellYOffset);

		if(cell)
			cell->density;

		return m_idealDensity;
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

		DensityField* field = m_densityFields[newFieldX][newFieldY];

		if(field)
			return field->getDeltaDensityCell(newCellX, newCellY);

		return nullptr;
	}

	float FluidDomain::getRelativeDeltaCellDensity(unsigned int fieldX0,
	        unsigned int fieldY0,
	        unsigned int cellX0,
	        unsigned int cellY0,
	        int			cellXOffset,
	        int			cellYOffset) {
		FluidCell* cell = getRelativeDeltaCell(fieldX0, fieldY0, cellX0, cellY0, cellXOffset, cellYOffset);

		if(cell)
			cell->density;

		return m_idealDensity;
	}

} // namespace FluidModule
