#include "FluidVelocityField.h"

FluidVelocityField::FluidVelocityField(unsigned int num_cells_x, unsigned int num_cells_y, float cellSize, float x, float y) : m_x(x), m_y(y) {
	m_velocities_x = new FluidField(num_cells_x, num_cells_y, cellSize, GLEngine::GLTexture());
	m_velocities_y = new FluidField(num_cells_x, num_cells_y, cellSize, GLEngine::GLTexture());
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
	m_velocities_x->diffuse(timeStep, viscosity, obstacles);
	m_velocities_y->diffuse(timeStep, viscosity, obstacles);

	m_velocities_x->project(timeStep, obstacles);
	m_velocities_y->project(timeStep, obstacles);


	m_velocities_x->advect(timeStep, this, obstacles);
	m_velocities_y->advect(timeStep, this, obstacles);

	m_velocities_x->setObstacles(obstacles, true);
	m_velocities_y->setObstacles(obstacles, false);

	m_velocities_x->project(timeStep, obstacles);
	m_velocities_y->project(timeStep, obstacles);
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
