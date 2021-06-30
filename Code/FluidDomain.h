#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <GLTexture.h>
#include <SpriteBatch.h>

#include "DensityField.h"

class Tile;

namespace FluidModule {
	class FluidDomain {
	  public:
		FluidDomain(std::vector<std::vector<std::vector<Tile*>>>& tiles);
		~FluidDomain();

		void update();
		void draw(GLEngine::SpriteBatch& sb, glm::vec4& destRect);   // Does the actual drawing. We *must* updateTexture beforehand. Don't do it with draw, because it can unbind the texture from the FBO.
		void updateTexture(glm::vec4& screenDestRect);				 // Updates the texture. Handles everything!

		void updateField(Tile* tile);

	  private:
		void createTexture();										 // Creates the texture. Is only for the constructor
		void resizeTexture(unsigned int width, unsigned int height); // Resizes the texture. For use with shifting scale
		void updateTextureData(glm::vec4& screenDestRect);			 // Creates the array for texture assignment

		GLEngine::GLTexture			m_texture;
		std::vector<unsigned char>* m_textureData;

		float m_lastScale = 0; // The last camera scale. Used to adjust texture size.

		std::vector<std::vector<DensityField*>> m_densityFields;
		// A 2D vector of density field pointers,
		// where a nullptr is equivalent to a solid block (AKA, no density should be traded there)

		void updateDensityFields(); // Loop through fields and only update the non-nullptrs who don't have the equilibrium flag set
		void updateDensityField(
			unsigned fieldX,
			unsigned fieldY); // Loops through all FluidCells in density field, propagating them into their deltafields
		void getReceivedDensityFromNeighbour(
			unsigned int fieldX,
			unsigned int fieldY,
			unsigned int cellX,
			unsigned int cellY,
			int			 cellXN,
			int cellYN); // Calculates how much density is traded between this cell (@x, y) and its neighbour (@xn, yn)
						 /// NOTE: The domain spans the entire world so no out-of-bounds should ever take place.
	};
} // namespace FluidModule
