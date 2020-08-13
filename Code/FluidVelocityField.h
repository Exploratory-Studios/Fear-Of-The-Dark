#pragma once

/*
 * Velocity fields are global for the game.
 * They act as spatial partitions.
 * They simply hold a 1 dimensional vector of directions (where the fluid should go).
 * They also provide access to this vector, and provide functions to update the velocity field.
 */

#include "FluidField.h"

class FluidVelocityField {
	public:
		FluidVelocityField(unsigned int num_cells_x, unsigned int num_cells_y, float cellSize, float x, float y, std::string id);
		~FluidVelocityField();

		void setNeighbours(FluidVelocityField* left, FluidVelocityField* right, FluidVelocityField* bottom, FluidVelocityField* top);

		void update(float& timeStep, float& viscosity, std::vector<bool>& obstacles);
		glm::vec2& getVelocity_GlobalCoords(float x, float y); // Gets the velocity at a cell given some world coordinates
		glm::vec2& getVelocity_LocalCoords(unsigned int& x, unsigned int& y); /// Gets the velocity at a cell (essentially: return m_velocities[x][y];)

		FluidField* getXVelocities();
		FluidField* getYVelocities();

	private:
		void project(float& timeStep, std::vector<bool>& obstacles);

		float m_x, m_y; // Actual world position of the start (bottom left) of the partition



		FluidField* m_velocities_x = nullptr;
		FluidField* m_velocities_y = nullptr;
};
