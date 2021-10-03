#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <GLTexture.h>
#include <SpriteBatch.h>

#include "DensityField.h"

class Tile;

namespace FluidModule {

	enum class FluidIDs { WATER };

	class FluidDomain {
	  public:
		FluidDomain(std::vector<std::vector<std::vector<Tile*>>>& tiles, unsigned int id);
		~FluidDomain();

		void update();
		void draw(
			GLEngine::SpriteBatch& sb,
			glm::vec4&
				destRect); // Does the actual drawing. We *must* updateTexture beforehand. Don't do it with draw, because it can unbind the texture from the FBO.
		void updateTexture(glm::vec4& screenDestRect); // Updates the texture. Handles everything!

		void updateField(Tile* tile);

		void  addFluid(unsigned int fieldX,
					   unsigned int fieldY,
					   unsigned int cellX,
					   unsigned int cellY,
					   float		amount); // Adds fluid, breaks equilibrium
		float removeFluid();		  // Returns the amount of fluid removed.
		void  setFluid(unsigned int fieldX,
					   unsigned int fieldY,
					   unsigned int cellX,
					   unsigned int cellY,
					   float		amount); // Sets the level of fluid in a cell

		bool displaceFluidArea(
			float x0,
			float y0,
			float x1,
			float y1); // Displaces an area of fluid from (x0, y0) to (x1, y1). If successful, returns true. Else, returns false

	  private:
		void createTexture();										 // Creates the texture. Is only for the constructor
		void resizeTexture(unsigned int width, unsigned int height); // Resizes the texture. For use with shifting scale
		void updateTextureData(glm::vec4& screenDestRect);			 // Creates the array for texture assignment

		unsigned int				m_id;
		GLEngine::GLTexture			m_texture;
		GLEngine::ColourRGBA8		m_fluidColour;
		std::vector<unsigned char>* m_textureData;
		unsigned int				m_usedTextureWidth, m_usedTextureHeight;
		unsigned int				m_allocatedTextureWidth = 60 * FLUID_PARTITION_SIZE,
					 m_allocatedTextureHeight				= 40 * FLUID_PARTITION_SIZE;
		float m_idealDensity								= 1.0f;
		float m_gravityConstant								= 0.7f;
		float m_trickleConstant								= 0.4f; // Controls the random L/R movement of fluid
		float m_viscosity									= 0.5f; // Higher numbers are equal to slower movement.

		float m_lastScale = 0; // The last camera scale. Used to adjust texture size.

		std::vector<std::vector<DensityField*>> m_densityFields;
		std::vector<DensityField*>				m_brokenEquilibriums;
		std::vector<DensityField*>				m_madeEquilibriums;
		// A 2D vector of density field pointers,
		// where a nullptr is equivalent to a solid block (AKA, no density should be traded there)

		void updateDensityFields(); // Loop through fields and only update the non-nullptrs who don't have the equilibrium flag set
		void updateDensityField(unsigned fieldX, unsigned fieldY);
		// Loops through all FluidCells in density field, propagating them into their deltafields
		float getReceivedDensityFromNeighbour(unsigned int fieldX,
											  unsigned int fieldY,
											  unsigned int cellX,
											  unsigned int cellY,
											  int		   cellXMod,
											  int		   cellYMod,
											  float		   neighbourSum);
		// Calculates how much density is traded to this cell (@x, y) from its neighbour (@x+xMod, y+ymod)
		
		DensityField* getRelativeField(unsigned int fieldX0, unsigned int fieldY0, int fieldXOffset, int fieldYOffset);
		// Returns a DensityField address to some densityfield relative to field0.

		FluidCell* getRelativeCell(unsigned int fieldX0,
								   unsigned int fieldY0,
								   unsigned int cellX0,
								   unsigned int cellY0,
								   int			cellXOffset,
								   int			cellYOffset);
		// Returns a FluidCell address to some cell relative to cell0 in field0

		float getRelativeCellDensity(unsigned int fieldX0,
									 unsigned int fieldY0,
									 unsigned int cellX0,
									 unsigned int cellY0,
									 int		  cellXOffset,
									 int		  cellYOffset);
		// Returns a cell's density at some cell relative to cell0 in field0.

		FluidCell* getRelativeDeltaCell(unsigned int fieldX0,
										unsigned int fieldY0,
										unsigned int cellX0,
										unsigned int cellY0,
										int			 cellXOffset,
										int			 cellYOffset);
		// Returns a FluidCell address to some cell relative to cell0 in field0

		float getRelativeDeltaCellDensity(unsigned int fieldX0,
										  unsigned int fieldY0,
										  unsigned int cellX0,
										  unsigned int cellY0,
										  int		   cellXOffset,
										  int		   cellYOffset);
		// Returns a cell's density at some cell relative to cell0 in field0.

		bool m_smoothFluid = false;
	};
} // namespace FluidModule
