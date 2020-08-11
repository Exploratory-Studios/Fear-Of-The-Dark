#pragma once

/* There must only be one fluid manager per game. Fluid managers have the ability to add Fluids, which
 * are all-encompassing types, holding their own partitions, update functions, and all necessary data.
 */

#include <SpriteBatch.h>
#include <glm/glm.hpp>

#include "Fluid.h"

class FluidManager {
	public:
		FluidManager();
		~FluidManager(); // Destroys all fluids

		void update(float& timeStep, glm::vec4& screenRect);
		void draw(GLEngine::SpriteBatch& sb, glm::vec4& screenRect);

		void addFluid(Fluid* fluid);
		void addObstacle(Tile* tile);
		//void addObstacle(Entity* e); TODO?

	private:
		std::vector<Fluid*> m_fluids;

};
