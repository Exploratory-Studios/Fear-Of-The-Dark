#pragma once

#include "FluidField.h"

#include <glm/glm.hpp>
#include <SpriteBatch.h>
#include <GLTexture.h>
#include <vector>

class Tile;

class Fluid {
	public:
		Fluid(float& viscosity, bool gravity);
		~Fluid();

		void init();

		void addObstacle(Tile* t); // Adds a tile's coordinates to the appropriate m_occupied vector.

		void update(float& timeStep, glm::vec4& screenRect);
		void draw(GLEngine::SpriteBatch& sb, glm::vec4& screenRect);

	private:
		void addObstacles();
		void createVectors();
		void destroyVectors();

		void getIndicesInBox(glm::vec4& destRect, std::vector<unsigned int>& indices, std::vector<glm::vec2>& positions);

		float m_viscosity; // Rate of diffusion
		bool m_hasGravity;

		std::vector<std::vector<bool>> m_occupied;
		std::vector<FluidField*> m_densities;
		std::vector<FluidVelocityField*> m_velocityFields;

};
