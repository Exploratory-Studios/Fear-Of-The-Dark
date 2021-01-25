#include "FluidParticleBatch.h"

#include "FluidField.h"
#include "FluidVelocityField.h"

#include "PresetValues.h"

#include <ResourceManager.h>

float weightFunction_regularDistance(float& distance, float kernelSize) {
	if(distance < kernelSize) {
		return 1.0f/std::max(std::abs(distance), 1.0f);
	}
	return 0.0f;
}

FluidParticleBatch::FluidParticleBatch(float gravity, float x, float y, float fieldSize, unsigned int numCellsWide, std::string id) : m_gravity(gravity), m_x(x), m_y(y), m_fieldSize(fieldSize), m_numCells(numCellsWide), m_id(id) {

}

FluidParticleBatch::~FluidParticleBatch() {
	m_textureData.clear();
	m_positions.clear();
	m_velocities.clear();
}

void FluidParticleBatch::setNeighbours(FluidParticleBatch* left, FluidParticleBatch* right, FluidParticleBatch* bottom, FluidParticleBatch* top) {
	m_left = left;
	m_right = right;
	m_bottom = bottom;
	m_top = top;
}

void FluidParticleBatch::givePosition(glm::vec2& pos) {
	m_positions.push_back(pos);
}

void FluidParticleBatch::giveVelocity(glm::vec2& vel) {
	m_velocities.push_back(vel);
}

void FluidParticleBatch::update(float& timeStep, FluidVelocityField* velocityField, std::vector<bool>& obstacles) {
	diffuse(timeStep);
	advect(timeStep, velocityField);
	collideParticles(obstacles);

	removeRemoved();
}

void FluidParticleBatch::diffuse(float& timeStep) {
	/// TODO: Something to do with pressure.
}

void FluidParticleBatch::advect(float& timeStep, FluidVelocityField* velocityField) {
	float tileToCell = m_numCells / m_fieldSize;

	for(unsigned int i = 0; i < m_positions.size(); i++) {
		glm::vec2 pos = m_positions[i];

		int leftX = (int)(pos.x * tileToCell);
		int rightX = leftX+1;

		int bottomY = (int)(pos.y * tileToCell);
		int topY = bottomY+1;

		float closenessRight = (pos.x * tileToCell) - leftX;
		float closenessLeft = 1.0f - closenessRight;

		float closenessTop = (pos.y * tileToCell) - bottomY;
		float closenessBottom = 1.0f - closenessTop;

		float LB_X = velocityField->getXVelocities()->getData(leftX, bottomY);
		float LT_X = velocityField->getXVelocities()->getData(leftX, topY);
		float RB_X = velocityField->getXVelocities()->getData(rightX, bottomY);
		float RT_X = velocityField->getXVelocities()->getData(rightX, topY);

		float LB_Y = velocityField->getYVelocities()->getData(leftX, bottomY);
		float LT_Y = velocityField->getYVelocities()->getData(leftX, topY);
		float RB_Y = velocityField->getYVelocities()->getData(rightX, bottomY);
		float RT_Y = velocityField->getYVelocities()->getData(rightX, topY);

		float interpolatedXVel = closenessRight * (closenessBottom * RB_X + closenessTop * RT_X) +
		                         closenessLeft * (closenessBottom * LB_X + closenessTop * LT_X);

		float interpolatedYVel = closenessRight * (closenessBottom * RB_Y + closenessTop * RT_Y) +
		                         closenessLeft * (closenessBottom * LB_Y + closenessTop * LT_Y);

		m_velocities[i] += glm::vec2(interpolatedXVel, interpolatedYVel + m_gravity * timeStep);
		if(m_positions[i].x < m_x) {
			// left neighbour
			givePositionToNeighbour(m_left, i);
		} else if(m_positions[i].x >= m_x + m_fieldSize) {
			// right
			givePositionToNeighbour(m_right, i);
		} else if(m_positions[i].y < m_y) {
			// bottom
			givePositionToNeighbour(m_bottom, i);
		} else if(m_positions[i].y >= m_y + m_fieldSize) {
			// top
			givePositionToNeighbour(m_top, i);
		}
	}

	for(unsigned int i = 0; i < m_positions.size(); i++) {
		m_positions[i] += m_velocities[i];
		m_velocities[i] *= 0.8f;
	}
}

void FluidParticleBatch::collideParticles(std::vector<bool>& obstacles) {
	// Conserves momentum, but "bounces" particles off of each other by applying a repulsive force.
	for(unsigned int i = 0; i < m_positions.size(); i++) {
		// Collide particles
		{
			for(unsigned int j = 0; j < m_positions.size(); j++) {
				if(i==j) continue;
				glm::vec2 p1 = m_positions[i];
				glm::vec2 p2 = m_positions[j];
				float squaredDist = std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2);

				if(squaredDist <= std::pow(FLUID_PARTICLE_RADIUS*2, 2)) {
					if(squaredDist == 0.0f) {
						m_positions[i] += glm::vec2(FLUID_PARTICLE_RADIUS*2);
					} else {
						float dist = std::sqrt(squaredDist);
						// This is the factor we need to "multiply" the difference in positions (see: distance) by to get particles to stop colliding.
						float forceOut = (2*FLUID_PARTICLE_RADIUS) / (dist);

						glm::vec2 diff = p1-p2;

						m_positions[i] += diff * glm::vec2(forceOut);
					}
				}

			}
		}
		// Collide obstacles
		{
			// We need to treat obstacles like tiles, because that's what they are.
			// We also detect collision as if the particle were a rectangle. We can do this because it's low resolution
			float L_X = m_positions[i].x - FLUID_PARTICLE_RADIUS;
			float R_X = m_positions[i].x + FLUID_PARTICLE_RADIUS;
			float B_Y = m_positions[i].y - FLUID_PARTICLE_RADIUS;
			float T_Y = m_positions[i].y + FLUID_PARTICLE_RADIUS;

			unsigned int L_X_cell = (int)(L_X-m_x) / FLUID_CELL_SIZE; // Cell coordinate system
			unsigned int R_X_cell = (int)(R_X-m_x) / FLUID_CELL_SIZE;
			unsigned int T_Y_cell = (int)(T_Y-m_x) / FLUID_CELL_SIZE;
			unsigned int B_Y_cell = (int)(B_Y-m_x) / FLUID_CELL_SIZE;

			unsigned int BL_index = (L_X_cell) * FLUID_PARTITION_SIZE + (B_Y_cell);
			unsigned int TL_index = (L_X_cell) * FLUID_PARTITION_SIZE + (T_Y_cell);
			unsigned int BR_index = (R_X_cell) * FLUID_PARTITION_SIZE + (B_Y_cell);
			unsigned int TR_index = (R_X_cell) * FLUID_PARTITION_SIZE + (T_Y_cell);

			if(obstacles[BL_index]) { // this corner happens to be in a solid position. Check and respond if necessary:
				collideParticleWithObstacle(L_X_cell, B_Y_cell, m_positions[i]);
			}
			if(obstacles[TL_index]) { // this corner happens to be in a solid position. Check and respond if necessary:
				collideParticleWithObstacle(L_X_cell, T_Y_cell, m_positions[i]);
			}
			if(obstacles[BR_index]) { // this corner happens to be in a solid position. Check and respond if necessary:
				collideParticleWithObstacle(R_X_cell, B_Y_cell, m_positions[i]);
			}
			if(obstacles[TR_index]) { // this corner happens to be in a solid position. Check and respond if necessary:
				collideParticleWithObstacle(R_X_cell, T_Y_cell, m_positions[i]);
			}
		}
	}
}

void FluidParticleBatch::collideParticleWithObstacle(unsigned int obstacleX, unsigned int obstacleY, glm::vec2& particle) {
	// NOTE: This ain't a perfect algorithm, but it works okay for small particle sizes and large tile sizes.
	// NOTE: Assumes that the obstacle at X and Y is solid

	// Check distance to all four sides from centre of particle
	glm::vec2 pos = particle - glm::vec2(m_x, m_y);

	float testX = pos.x, testY = pos.y; // coordinates to test with (either left/right bound or top/bottom bound)
	if(pos.x < (float)(obstacleX * FLUID_CELL_SIZE)) testX = (float)(obstacleX * FLUID_CELL_SIZE);
	else if(pos.x > (float)((obstacleX + 1) * FLUID_CELL_SIZE)) testX = (float)(obstacleX + 1) * FLUID_CELL_SIZE;
	if(pos.y < (float)(obstacleY * FLUID_CELL_SIZE)) testY = (float)(obstacleY * FLUID_CELL_SIZE);
	else if(pos.y > (float)((obstacleY + 1) * FLUID_CELL_SIZE)) testY = (float)(obstacleY + 1) * FLUID_CELL_SIZE;

	float xDist = pos.x-testX; // In tile coordinate system
	float yDist = pos.y-testY;
	float distSquared = std::pow(xDist, 2) + std::pow(yDist, 2);
	if(distSquared <= std::pow(FLUID_PARTICLE_RADIUS, 2)) {
		if(xDist < yDist) {
			// Move horizontally, we're deepest into the side bound
			particle.x += xDist;
		} else {
			// Move vertically, we're deepest into the top/bottom bound
			particle.y += yDist;
		}
	}
}

void FluidParticleBatch::draw(GLEngine::SpriteBatch& sb, glm::vec2& pos) {
	constructTextureData(m_textureData);

	GLEngine::ResourceManager::setTexture(m_texture.filePath, &m_textureData);

	sb.draw(glm::vec4(m_x, m_y, m_fieldSize, m_fieldSize), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_texture.id, 0.0f, GLEngine::ColourRGBA8(0, 119, 190, 255));
}

void FluidParticleBatch::constructTextureData(std::vector<unsigned char>& data) {
	if(m_texture.filePath.length() == 0) createTexture();
	for(unsigned int i = 0; i < m_textureData.size(); i++) {
		m_textureData[i] = 0;
	}
	m_textureData.resize(m_numCells*m_numCells*4, 0.0f);

	float cellSize = m_fieldSize / m_numCells;

	for(int i = 0; i < m_positions.size(); i++) {
		if(m_positions[i].x > 0.0f && m_positions[i].y > 0.0f) {
			unsigned int index = ((int)(m_positions[i].y / cellSize) * m_numCells + (int)(m_positions[i].x / cellSize))*4;

			if(index > m_textureData.size()) continue;
			m_textureData[index] = std::min(m_textureData[index] + 25, 255);

		}
	}
}

void FluidParticleBatch::addParticle(glm::vec2& position, glm::vec2 velocity) {
	m_positions.push_back(position);
	m_velocities.push_back(velocity);
}

void FluidParticleBatch::createTexture() {
	unsigned int size = m_numCells;

	std::vector<unsigned char> data;
	data.resize(size*size, (unsigned char)0);
	m_texture = GLEngine::ResourceManager::addTexture("fluidParticleTexture" + m_id, size, size, data);

	// (Re)bind the texture object
	glBindTexture(GL_TEXTURE_2D, m_texture.id);

	// Set to not wrap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Unbind
	glBindTexture(GL_TEXTURE_2D, 0);
}

void FluidParticleBatch::givePositionToNeighbour(FluidParticleBatch* neighbour, unsigned int index) {
	// Adds a position's index to the remove queue, and adds it to the neighbour
	m_removalQueue.push_back(index);
	neighbour->givePosition(m_positions[index]);
	neighbour->giveVelocity(m_velocities[index]);
}

void FluidParticleBatch::removeRemoved() {
	// removes all queued particles. I know it's a stupid name
	for(unsigned int i = 0; i < m_removalQueue.size(); i++) {
		// Just swaps this element with the one at the back. Speedier. O(1) instead of O(n)
		m_positions[m_removalQueue[i]] = m_positions[m_positions.size()-(1+i)];
		m_velocities[m_removalQueue[i]] = m_velocities[m_velocities.size()-(1+i)];
	}
	for(unsigned int i = 0; i < m_removalQueue.size(); i++) {
		m_positions.pop_back();
		m_velocities.pop_back();
	}
	m_removalQueue.clear();
}

float FluidParticleBatch::getDensityAtPoint(glm::vec2& point, std::function<float(float &, float)>& weightFunction_distToValue) {

}
