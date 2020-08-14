#include "Fluid.h"

#include <ResourceManager.h>

#include "PresetValues.h"
#include "Singletons.h"

#include "FluidVelocityField.h"

#include "Tile.h"

Fluid::Fluid(float& viscosity, bool gravity) : m_viscosity(viscosity), m_hasGravity(gravity) {
	createVectors();
}

Fluid::~Fluid() {
	destroyVectors();
}

void Fluid::init() {

	for(float x = 0.0f; x < 3.0f; x += 1.0f) {
		for(float y = 0.0f; y < 3.0f; y += 1.0f) {
			glm::vec2 sourcePos(5.0f+x, 5.0f+y);
			m_particles[0]->addParticle(sourcePos);
		}
	}


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

	m_velocityFields.reserve(numPartitionsX * numPartitionsY);
	m_particles.reserve(numPartitionsX * numPartitionsY);

	for(unsigned int partitionX = 0; partitionX < numPartitionsX; partitionX++) {
		for(unsigned int partitionY = 0; partitionY < numPartitionsY; partitionY++) {
			m_particles.push_back(
			    new FluidParticleBatch(-9.8f,
			                           partitionX * partitionSize,
			                           partitionY * partitionSize,
			                           partitionSize,
			                           FLUID_PARTITION_SIZE,
			                           std::to_string(partitionX * numPartitionsY + partitionY)
			                          ));

			m_velocityFields.push_back(
			    new FluidVelocityField(FLUID_PARTITION_SIZE,
			                           FLUID_PARTITION_SIZE,
			                           FLUID_CELL_SIZE,
			                           partitionX * partitionSize,
			                           partitionY * partitionSize,
			                           std::to_string(partitionX * numPartitionsY + partitionY)
			                          ));
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

			m_particles[partitionX * numPartitionsY + partitionY]->setNeighbours(
			    m_particles[leftNeighbourIndex],
			    m_particles[rightNeighbourIndex],
			    m_particles[bottomNeighbourIndex],
			    m_particles[topNeighbourIndex]);

			/// TODO: We need to deal with the very very top of the world and the very very bottom of the world.
		}
	}



}

void Fluid::destroyVectors() {
	m_occupied.clear();
	m_velocityFields.clear(); // Should be deleting.
	m_particles.clear();
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

	//glm::vec2 sourcePos(5.0f, 10.0f);
	//m_particles[0]->addParticle(sourcePos);
	int x = 13, y = 13;
	m_velocityFields[0]->getXVelocities()->addSource(x, y, 40.f/10.0f);

	for(unsigned int& i : indices) {
		m_velocityFields[i]->update(timeStep, m_viscosity, m_occupied[i]);
		m_particles[i]->update(timeStep, m_velocityFields[i]);
	}
}

void Fluid::draw(GLEngine::SpriteBatch& sb, glm::vec4& screenRect) {
	std::vector<unsigned int> indices;
	std::vector<glm::vec2> positions;

	getIndicesInBox(screenRect, indices, positions);

	glm::vec2 pos = glm::vec2(0.0f);
	m_particles[0]->draw(sb, pos);

	//for(unsigned int& i : indices) {
	//m_densities[i]->draw(sb, positions[i]);
	//}
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
