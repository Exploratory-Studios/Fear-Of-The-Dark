#pragma once

#include "FluidField.h"

#include <glm/glm.hpp>
#include <SpriteBatch.h>
#include <GLTexture.h>
#include <vector>

class Tile;

// The size of a 1/4 chunk length (8*8)
#define FLUID_PARTITION_SIZE 40
#define FLUID_CELL_SIZE 0.2f

class Fluid {
	public:
		Fluid(float& viscosity);
		~Fluid();

		void addObstacle(Tile* t); // Adds a tile's coordinates to the appropriate m_occupied vector.

		void update(float& timeStep, glm::vec4& screenRect);
		void draw(GLEngine::SpriteBatch& sb, glm::vec4& screenRect);

	private:
		void createVectors();
		void destroyVectors();

		void createTexture();

		float m_viscosity; // Rate of diffusion
		GLEngine::GLTexture m_texture; // To store lots and lots of data about cell densities

		std::vector<std::vector<bool>> m_occupied;
		std::vector<FluidField*> m_densities;
		std::vector<FluidVelocityField*> m_velocityFields;

};
