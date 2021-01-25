#pragma once

#include <algorithm>
#include <SpriteBatch.h>
#include <GLTexture.h>

class FluidField;
class FluidVelocityField;

/**
 * @class FluidParticleBatch
 * @author Davis-Dev
 * @date 13/08/20
 * @file FluidParticleBatch.h
 * @brief Basically the same as FluidField, but has a vector of particle positions, velocities, and other traits, such as density, pressure, etc.
 */

class FluidParticleBatch {
	public:
		FluidParticleBatch(float gravity, float x, float y, float fieldSize, unsigned int numCellsWide, std::string id);
		~FluidParticleBatch();

		void setNeighbours(FluidParticleBatch* left, FluidParticleBatch* right, FluidParticleBatch* bottom, FluidParticleBatch* top);

		void givePosition(glm::vec2& pos);
		void giveVelocity(glm::vec2& vel);

		void update(float& timeStep, FluidVelocityField* velocityField, std::vector<bool>& obstacles);

		void diffuse(float& timeStep);
		void advect(float& timeStep, FluidVelocityField* velocityField);
		void collideParticles(std::vector<bool>& obstacles);

		void draw(GLEngine::SpriteBatch& sb, glm::vec2& pos);
		void constructTextureData(std::vector<unsigned char>& data);

		void addParticle(glm::vec2& position, glm::vec2 velocity = glm::vec2(0.0f));

	protected:
		void createTexture();
		void givePositionToNeighbour(FluidParticleBatch* neighbour, unsigned int index);
		void removeRemoved();
		void collideParticleWithObstacle(unsigned int obstacleX, unsigned int obstacleY, glm::vec2& particle);

		float getDensityAtPoint(glm::vec2& point, std::function<float(float&, float)>& weightFunction_distToValue);

		std::vector<glm::vec2> m_positions;
		std::vector<glm::vec2> m_velocities;
		std::vector<unsigned int> m_removalQueue;

		std::vector<unsigned char> m_textureData;
		GLEngine::GLTexture m_texture;

		float m_x, m_y;
		float m_gravity;
		float m_fieldSize;
		unsigned int m_numCells;

		std::string m_id;

		FluidParticleBatch* m_left = nullptr, *m_right = nullptr, *m_bottom = nullptr, *m_top = nullptr; // neighbouring fields


};
