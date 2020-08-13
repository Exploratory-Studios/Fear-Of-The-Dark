#include "FluidVelocityField.h"

FluidVelocityField::FluidVelocityField(unsigned int num_cells_x, unsigned int num_cells_y, float cellSize, float x, float y, std::string id) : m_x(x), m_y(y) {
	m_velocities_x = new FluidField(num_cells_x, num_cells_y, cellSize, id + "xVel");
	m_velocities_y = new FluidField(num_cells_x, num_cells_y, cellSize, id + "yVel");
}

FluidVelocityField::~FluidVelocityField() {
	delete m_velocities_x;
	delete m_velocities_y;
}

void FluidVelocityField::setNeighbours(FluidVelocityField* left, FluidVelocityField* right, FluidVelocityField* bottom, FluidVelocityField* top) {
	m_velocities_x->setNeighbours(left->getXVelocities(), right->getXVelocities(), bottom->getXVelocities(), top->getXVelocities());
	m_velocities_y->setNeighbours(left->getYVelocities(), right->getYVelocities(), bottom->getYVelocities(), top->getYVelocities());
}

void FluidVelocityField::update(float& timeStep, float& viscosity, std::vector<bool>& obstacles) {
	for(int x = 0; x < m_velocities_y->getNumCellsX(); x++)
		for(int y = 0; y < m_velocities_y->getNumCellsY(); y++)
			m_velocities_y->addSource(x, y, -9.8f);

	m_velocities_x->addSources(timeStep);
	m_velocities_y->addSources(timeStep);

	m_velocities_x->swap();
	m_velocities_y->swap();

	m_velocities_x->diffuse(timeStep, viscosity, obstacles);
	m_velocities_y->diffuse(timeStep, viscosity, obstacles);

	project(timeStep, obstacles);

	m_velocities_x->swap();
	m_velocities_y->swap();

	m_velocities_x->advect(timeStep, this, obstacles);
	m_velocities_y->advect(timeStep, this, obstacles);

	project(timeStep, obstacles);
}

glm::vec2& FluidVelocityField::getVelocity_GlobalCoords(float x, float y) {
}

glm::vec2& FluidVelocityField::getVelocity_LocalCoords(unsigned int& x, unsigned int& y) {
}

FluidField* FluidVelocityField::getXVelocities() {
	return m_velocities_x;
}

FluidField* FluidVelocityField::getYVelocities() {
	return m_velocities_y;
}

void FluidVelocityField::project(float& timeStep, std::vector<bool>& obstacles) {
	// Basic Idea is to remove a heightmap (think of vectors pointing down slopes or something) from our data to get a mass-conserving thing.
	// Get heightmap (Poisson equation)
	for(int x = 0; x < m_velocities_x->getNumCellsX(); x++) {
		for(int y = 0; y < m_velocities_x->getNumCellsY(); y++) {
			// Poisson equation
			int rightX = x+1, leftX = x-1, topY = y+1, bottomY = y-1;
			m_velocities_y->getLastData(x, y) = -0.5f * m_velocities_x->getCellSize() *
			                                    (m_velocities_x->getData(rightX, y) -
			                                     m_velocities_x->getData(leftX, y) +
			                                     m_velocities_y->getData(x, topY) -
			                                     m_velocities_y->getData(x, bottomY));
			m_velocities_x->getLastData(x, y) = 0;
		}
	}

	// Gauss-Siedel relaxation
	for(int k = 0; k < 20; k++) {
		for(int x = 0; x < m_velocities_x->getNumCellsX(); x++) {
			for(int y = 0; y < m_velocities_y->getNumCellsY(); y++) {
				// Poisson equation
				int rightX = x+1, leftX = x-1, topY = y+1, bottomY = y-1;

				m_velocities_x->getLastData(x, y) = (m_velocities_y->getLastData(x,y) +
				                                     m_velocities_x->getLastData(leftX,y) +
				                                     m_velocities_x->getLastData(rightX,y) +
				                                     m_velocities_x->getLastData(x,topY) +
				                                     m_velocities_x->getLastData(x,bottomY)) / 4.0f;
			}
		}
	}

	// Now we have our heightmap (stored in m_velocities_y->lastData, with a temp variable in m_velocities_x lastData, too)
	// Simply subtract it to get the mass-conserving map
	for(int x = 0; x < m_velocities_x->getNumCellsX(); x++) {
		for(int y = 0; y < m_velocities_y->getNumCellsY(); y++) {
			// Poisson equation
			int rightX = x+1, leftX = x-1, topY = y+1, bottomY = y-1;

			m_velocities_x->getData(x,y) -= 0.5f * (m_velocities_x->getLastData(rightX, y) - m_velocities_x->getLastData(leftX, y)) / m_velocities_x->getCellSize();
			m_velocities_y->getData(x,y) -= 0.5f * (m_velocities_x->getLastData(x, topY) - m_velocities_x->getLastData(x, bottomY)) / m_velocities_y->getCellSize();
		}
	}
}
