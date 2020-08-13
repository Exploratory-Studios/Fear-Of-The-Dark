#include "Fluid.h"

#include <ResourceManager.h>

#include "PresetValues.h"
#include "Singletons.h"

#include "FluidVelocityField.h"

#include "Tile.h"

// 16*16 tiles
#define FLUID_PARTITION_SIZE 32
#define FLUID_CELL_SIZE 0.5f

Fluid::Fluid(float& viscosity, bool gravity) : m_viscosity(viscosity), m_hasGravity(gravity) {
	createVectors();
}

Fluid::~Fluid() {
	destroyVectors();
}

void Fluid::init() {

	int x = 10, y = 22;
	//m_densities[0]->addSource(x, y, 50.0f); // 0.5f every second

	addObstacles();
}

void Fluid::addObstacles() {
	float partSize = (FLUID_PARTITION_SIZE * FLUID_CELL_SIZE);
	unsigned int cellsX = Singletons::getWorld()->getSize() / partSize;
	unsigned int cellsY = WORLD_HEIGHT / partSize;

	for(int x = 0; x < cellsX; x++) {
		for(int y = 0; y < cellsY; y++) {
			for(int worldX0 = 0; worldX0 < partSize; worldX0++) {
				for(int worldY0 = 0; worldY0 < partSize; worldY0++) {
					Tile* t = Singletons::getWorld()->getTile(x*partSize + worldX0, y*partSize + worldY0, 0);
					if(t->isSolid()) {
						addObstacle(t);
					}
				}
			}
		}
	}
}

void Fluid::createVectors() {
	destroyVectors();

	float partitionSize = FLUID_PARTITION_SIZE * FLUID_CELL_SIZE;
	unsigned int numPartitionsX = Singletons::getWorld()->getSize() / partitionSize;
	unsigned int numPartitionsY = WORLD_HEIGHT / partitionSize;

	std::vector<bool> temp;
	temp.resize(FLUID_PARTITION_SIZE * FLUID_PARTITION_SIZE, false);
	m_occupied.resize(numPartitionsX * numPartitionsY, temp);

	m_densities.reserve(numPartitionsX * numPartitionsY);
	m_velocityFields.reserve(numPartitionsX * numPartitionsY);

	for(unsigned int partitionX = 0; partitionX < numPartitionsX; partitionX++) {
		for(unsigned int partitionY = 0; partitionY < numPartitionsY; partitionY++) {
			m_densities.push_back(new FluidField(FLUID_PARTITION_SIZE, FLUID_PARTITION_SIZE, FLUID_CELL_SIZE, std::to_string(partitionX * numPartitionsY + partitionY)));

			m_velocityFields.push_back(new FluidVelocityField(FLUID_PARTITION_SIZE,
			                           FLUID_PARTITION_SIZE,
			                           FLUID_CELL_SIZE,
			                           partitionX * partitionSize,
			                           partitionY * partitionSize,
			                           std::to_string(partitionX * numPartitionsY + partitionY)));
		}
	}

	for(unsigned int partitionX = 0; partitionX < numPartitionsX; partitionX++) {
		for(unsigned int partitionY = 0; partitionY < numPartitionsY; partitionY++) {
			unsigned int leftNeighbourIndex = (((partitionX+numPartitionsX-1)%numPartitionsX) * numPartitionsY + partitionY);
			unsigned int rightNeighbourIndex = (((partitionX+1)%numPartitionsX) * numPartitionsY + partitionY);
			unsigned int bottomNeighbourIndex = (partitionX * numPartitionsY + (partitionY+numPartitionsY-1)%numPartitionsY);
			unsigned int topNeighbourIndex = (partitionX * numPartitionsY + (partitionY+1)%numPartitionsY);

			m_velocityFields[partitionX * numPartitionsY + partitionY]->setNeighbours(m_velocityFields[leftNeighbourIndex],
			        m_velocityFields[rightNeighbourIndex],
			        m_velocityFields[bottomNeighbourIndex],
			        m_velocityFields[topNeighbourIndex]);

			m_densities[partitionX * numPartitionsY + partitionY]->setNeighbours(
			    m_densities[leftNeighbourIndex],
			    m_densities[rightNeighbourIndex],
			    m_densities[bottomNeighbourIndex],
			    m_densities[topNeighbourIndex]);

			/// TODO: We need to deal with the very very top of the world and the very very bottom of the world.
		}
	}



}

void Fluid::destroyVectors() {
	m_occupied.clear();
	m_densities.clear();
	m_velocityFields.clear();
}

void Fluid::addObstacle(Tile* t) {
	glm::vec2 pos = t->getPosition();
	glm::vec2 size = t->getSize();

	float partitionSize = (FLUID_PARTITION_SIZE * FLUID_CELL_SIZE);
	unsigned int partitionX = pos.x / partitionSize;
	unsigned int partitionY = pos.y / partitionSize;

	unsigned int localX, localY;
	localX = (int)(pos.x) % (int)partitionSize;
	localY = (int)(pos.y) % (int)partitionSize;

	unsigned int index = partitionX * (WORLD_HEIGHT/partitionSize) + partitionY;

	for(unsigned int x = localX / FLUID_CELL_SIZE; x < (localX + size.x) / FLUID_CELL_SIZE; x++) {
		for(unsigned int y = localY / FLUID_CELL_SIZE; y < (localY + size.y) / FLUID_CELL_SIZE; y++) {
			m_occupied[index][x * (FLUID_PARTITION_SIZE) + y] = true;
		}
	}
}

void Fluid::update(float& timeStep, glm::vec4& screenRect) {
	std::vector<unsigned int> indices;
	std::vector<glm::vec2> positions;

	getIndicesInBox(screenRect, indices, positions);

	int x = 18, y = 28;
	m_densities[0]->addSource(x, y, 5.0f); // 0.5f every second

	for(unsigned int& i : indices) {
		float waterDiffusion = 0.01f;
		m_velocityFields[i]->update(timeStep, m_viscosity, m_occupied[i]);
		m_densities[i]->addSources(timeStep); // Add sources from lastDensities to this density
		m_densities[i]->swap(); // swap: now lastDensities and density both have new sources
		m_densities[i]->diffuse(timeStep, waterDiffusion, m_occupied[i]); // "lastDensities" has diffused values, using those from "density"
		m_densities[i]->swap(); // swap: now density has the diffused values
		m_densities[i]->advect(timeStep, m_velocityFields[i], m_occupied[i]); // change density using values at lastDensities
	}
}

void Fluid::draw(GLEngine::SpriteBatch& sb, glm::vec4& screenRect) {
	std::vector<unsigned int> indices;
	std::vector<glm::vec2> positions;

	getIndicesInBox(screenRect, indices, positions);

	for(unsigned int& i : indices) {
		m_densities[i]->draw(sb, positions[i]);
	}
}

void Fluid::getIndicesInBox(glm::vec4& destRect, std::vector<unsigned int>& indices, std::vector<glm::vec2>& positions) {
	float partSize = FLUID_PARTITION_SIZE * FLUID_CELL_SIZE;
	unsigned int partsX = Singletons::getWorld()->getSize() / partSize;
	unsigned int partsY = WORLD_HEIGHT / partSize;

	for(float x = destRect.x - partSize/2.0f; x < destRect.x + destRect.z + partSize/2.0f; x += partSize) {
		int xFloor = (int)(x);
		int xIndex = std::floor(xFloor / partSize);
		if(xIndex < 0 || xIndex >= partsX) continue;

		for(float y = destRect.y - partSize/2.0f; y < destRect.y + destRect.w + partSize/2.0f; y += partSize) {
			unsigned int yIndex = std::floor((int)y / partSize);
			if(yIndex < 0 || yIndex >= partsY) continue;

			unsigned int index = xIndex * partsY + yIndex;

			positions.push_back(glm::vec2(xIndex * partSize, yIndex * partSize));
			indices.push_back(index);
		}
	}

	//indices.push_back(0);
	//positions.push_back(glm::vec2(0.0f));
}
