#include "FluidDomain.h"

#include "Singletons.h"

#include "Tile.h"

namespace FluidModule {

	FluidDomain::FluidDomain(std::vector<std::vector<std::vector<Tile*>>>& tiles) {
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
		// Check if texture needs to be resized:
		if(lastScale != Singletons::getGameCamera()->getScale()) {
			// New scale is different, now to resize the texture:
		}
	}

	void FluidDomain::createTexture() {
		unsigned int size = m_numCells_x;

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
