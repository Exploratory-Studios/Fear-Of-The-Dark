#pragma once

#include <glm/glm.hpp>

#include <SpriteBatch.h>

namespace FluidModule
{

	class FluidManager
	{
		public:
			FluidManager();
			~FluidManager();

			void init();

			void draw(GLEngine::SpriteBatch& sb, glm::vec4& destRect);
			void update(float timeStep, glm::vec4& destRect);

	};

}
