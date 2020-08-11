#include "Fluid.h"

#include <ResourceManager.h>

#include "PresetValues.h"
#include "Singletons.h"

#include "FluidVelocityField.h"

#include "Tile.h"

Fluid::Fluid(float& viscosity) : m_viscosity(viscosity) {
	m_texture.id = (GLuint)-1;

	createVectors();
}

Fluid::~Fluid() {
	destroyVectors();
}

void Fluid::createTexture() {
	unsigned int size = FLUID_PARTITION_SIZE;

	std::vector<unsigned char> data;
	data.resize(size*size, (unsigned char)0);
	m_texture = GLEngine::ResourceManager::addTexture("fluidTexture", size, size, data);

	for(auto* densityMap : m_densities) {
		densityMap->setTexture(m_texture);
	}
}

void Fluid::createVectors() {
	destroyVectors();

	float partitionSize = FLUID_PARTITION_SIZE * FLUID_CELL_SIZE;
	unsigned int numPartitionsX = Singletons::getWorld()->getSize() / partitionSize;
	unsigned int numPartitionsY = WORLD_HEIGHT / partitionSize;

	std::vector<bool> temp;
	temp.resize(FLUID_PARTITION_SIZE, false);
	m_occupied.resize(numPartitionsX * numPartitionsY, temp);

	m_densities.reserve(numPartitionsX * numPartitionsY);
	m_velocityFields.reserve(numPartitionsX * numPartitionsY);

	for(unsigned int partitionX = 0; partitionX < numPartitionsX; partitionX++) {
		for(unsigned int partitionY = 0; partitionY < numPartitionsY; partitionY++) {
			m_densities.push_back(new FluidField(FLUID_PARTITION_SIZE, FLUID_PARTITION_SIZE, FLUID_CELL_SIZE, m_texture));

			m_velocityFields.push_back(new FluidVelocityField(FLUID_PARTITION_SIZE,
			                           FLUID_PARTITION_SIZE,
			                           FLUID_CELL_SIZE,
			                           partitionX * partitionSize,
			                           partitionY * partitionSize));
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

	unsigned int index = partitionX * WORLD_HEIGHT + partitionY;

	for(unsigned int x = localX / FLUID_CELL_SIZE; x < size.x / FLUID_CELL_SIZE; x++) {
		for(unsigned int y = localY / FLUID_CELL_SIZE; y < size.y / FLUID_CELL_SIZE; y++) {
			m_occupied[index][x * (size.y / FLUID_CELL_SIZE) + y] = true;
		}
	}
}

void Fluid::update(float& timeStep, glm::vec4& screenRect) {
	std::vector<unsigned int> indices;

	float partSize = FLUID_PARTITION_SIZE * FLUID_CELL_SIZE;

	for(float x = screenRect.x - partSize/2.0f; x < screenRect.z + partSize/2.0f; x += partSize) {
		unsigned int xIndex = ((((int)x + Singletons::getWorld()->getSize()) % Singletons::getWorld()->getSize()) / partSize);
		for(float y = screenRect.y - partSize/2.0f; y < screenRect.w + partSize/2.0f; y += partSize) {
			if(y < -(screenRect.w + partSize/2.0f)) break;
			if(y < 0 || y > WORLD_HEIGHT) continue;
			unsigned int yIndex = (y / partSize);
			unsigned int index = xIndex * (WORLD_HEIGHT / partSize) + yIndex;

			if(index > m_densities.size()) continue;

			Logger::getInstance()->log("index" + std::to_string(index));

			indices.push_back(index);

		}
	}

	for(unsigned int& i : indices) {
		m_densities[i]->diffuse(timeStep, m_viscosity, m_occupied[i]);
		m_densities[i]->advect(timeStep, m_velocityFields[i], m_occupied[i]);
		m_velocityFields[i]->update(timeStep, m_viscosity, m_occupied[i]);
	}
}

void Fluid::draw(GLEngine::SpriteBatch& sb, glm::vec4& screenRect) {
	if(m_texture.id == (GLuint)-1) createTexture();

	std::vector<unsigned int> indices;

	float partSize = FLUID_PARTITION_SIZE * FLUID_CELL_SIZE;

	for(float x = screenRect.x - partSize/2.0f; x < screenRect.z + partSize/2.0f; x += partSize) {
		unsigned int xIndex = ((((int)x + Singletons::getWorld()->getSize()) % Singletons::getWorld()->getSize()) / partSize);
		for(float y = screenRect.y - partSize/2.0f; y < screenRect.w + partSize/2.0f; y += partSize) {
			if(y < -(screenRect.w + partSize/2.0f)) break;
			if(y < 0 || y > WORLD_HEIGHT) continue;
			unsigned int yIndex = (y / partSize);
			unsigned int index = xIndex * (WORLD_HEIGHT / partSize) + yIndex;

			indices.push_back(index);

		}
	}

	for(unsigned int& i : indices) {
		m_densities[i]->draw(sb);
	}
}
