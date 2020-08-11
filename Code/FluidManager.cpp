#include "FluidManager.h"

#include "Tile.h"

FluidManager::FluidManager() {
	float waterViscosity = 0.5f;
	addFluid(new Fluid(waterViscosity));
}

FluidManager::~FluidManager() {
	for(auto* f : m_fluids) delete f;
}

void FluidManager::update(float& timeStep, glm::vec4& screenRect) {
	for(auto* f : m_fluids) f->update(timeStep, screenRect);
}

void FluidManager::draw(GLEngine::SpriteBatch& sb, glm::vec4& screenRect) {
	for(auto* f : m_fluids) f->draw(sb, screenRect);
}

void FluidManager::addFluid(Fluid* fluid) {
	m_fluids.push_back(fluid);
}

void FluidManager::addObstacle(Tile* tile) {
	float worldX = tile->getPosition().x, worldY = tile->getPosition().y;
}
