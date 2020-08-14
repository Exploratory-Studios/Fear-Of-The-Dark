#include "FluidField.h"

#include <ResourceManager.h>

#include "Singletons.h"
#include "PresetValues.h"

#include "FluidVelocityField.h"
#include "Fluid.h"

FluidField::FluidField(unsigned int numCells_x, unsigned int numCells_y, float cellSize, float x, float y, std::string id, float fill) : m_cellSize(cellSize), m_x(x), m_y(y), m_id(id) {
	m_numCells_x = numCells_x;
	m_numCells_y = numCells_y;
	constructVector(fill);
}

FluidField::~FluidField() {
	destroyVector();
}

void FluidField::setNeighbours(FluidField* left, FluidField* right, FluidField* bottom, FluidField* top) {
	m_left = left;
	m_right = right;
	m_bottom = bottom;
	m_top = top;
}

void FluidField::diffuse(float& timeStep, float& rateOfDiffusion, std::vector<bool>& obstacles) {
	// All based on a stable, backwards algorithm:
	// 		lastValue[here] = newValue[here] - a*(newValues[left, right, up, down] - 4*newValue[here])
	// This is a linear system for unknowns newValues[here], which we can solve for:
	// We use Gauss-Seidel relaxation to invert the matrix (idk what this means honestly, the paper says it: https://pdfs.semanticscholar.org/847f/819a4ea14bd789aca8bc88e85e906cfc657c.pdf

	float a = timeStep * rateOfDiffusion * m_numCells_x * m_numCells_y;

	//if(a == 0) {
	//	m_data = m_lastData;
	//	return;
	//}

	for(unsigned int k = 0; k < 20; k++) { // Something for the relaxation
		for(int x = 0; x < m_numCells_x; x++) {
			for(int y = 0; y < m_numCells_y; y++) {
				int leftX = x-1;
				int rightX = x+1;
				int bottomY = y-1;
				int topY = y+1;
				getData(x, y) = (getLastData(x, y) + a *
				                 (getData(leftX, y) +
				                  getData(rightX, y) +
				                  getData(x, bottomY) +
				                  getData(x, topY))) / (1 + 4*a);
			}
		}
		//setObstacles(obstacles, true);
		//setObstacles(obstacles, false);
	}
}

void FluidField::advect(float& timeStep, FluidVelocityField* velocities, std::vector<bool>& obstacles) {
	// We need to backtrace, then interpolate

	for(int x = 0; x < m_numCells_x; x++) {
		for(int y = 0; y < m_numCells_y; y++) {
			// Find data, that given this coordinate's last velocities, would have ended up here.
			float previousX = (float)x - timeStep * velocities->getXVelocities()->getLastData(x, y);
			float previousY = (float)y - timeStep * velocities->getYVelocities()->getLastData(x, y);

			// Now we have the coordinates (previousX&Y), we need to linearly interpolate what that value would have been in the previous coordinates and put it here.
			int leftX = (int)previousX;
			int rightX = leftX + 1;
			int bottomY = (int)previousY;
			int topY = bottomY + 1;

			// get "closeness" multiplier for linear interpolation
			float rightCloseness = previousX - leftX;
			float leftCloseness = 1.0f - rightCloseness;
			float topCloseness = previousY - bottomY;
			float bottomCloseness = 1.0f - topCloseness;

			// get different values
			float LB = velocities->getXVelocities()->getLastData(leftX, bottomY);
			float LT = velocities->getXVelocities()->getLastData(leftX, topY);
			float RB = velocities->getXVelocities()->getLastData(rightX, bottomY);
			float RT = velocities->getXVelocities()->getLastData(rightX, topY);

			// Linear interpolation time
			float interpolated = rightCloseness * (bottomCloseness * RB + topCloseness * RT) +
			                     leftCloseness * (bottomCloseness * LB + topCloseness * LT);

			getData(x, y) = interpolated;
		}
	}
}

void FluidField::addSource(int& x, int& y, float source, bool clamp/*=true*/) {
	//getLastData(x, y) += source;
	getData(x, y) += source;
}

void FluidField::draw(GLEngine::SpriteBatch& sb, glm::vec2& pos) {
	if(m_texture.filePath.length() == 0) createTexture();
	constructTextureData(m_textureData);
	GLEngine::ResourceManager::setTexture(m_texture.filePath, &m_textureData);

	sb.draw(glm::vec4(pos.x, pos.y, m_numCells_x * m_cellSize, m_numCells_y * m_cellSize), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_texture.id, 0.0f, GLEngine::ColourRGBA8(0, 119, 190, 255));
}

void FluidField::constructTextureData(std::vector<unsigned char>& data) {
	// Only write to the R value (once every 4 components)
	data.resize(m_numCells_x*m_numCells_y*4, 0);

	// OpenGL indexes the data a little backwards from ours ([y][x]), so we have to adjust

	for(int x = 0; x < m_numCells_x; x++) {
		for(int y = 0; y < m_numCells_y; y++) {
			// We use getData(y, x) instead of (x,y) to compensate for the above note.
			data[(x*m_numCells_y+y)*4] = glm::clamp((int)(getData(y, x) * 255), 0, 255);
		}
	}
}

void FluidField::setObstacles(std::vector<bool>& obstacles, bool horizontal) {
	// If horizontal: all x components should be set to 0 on vertical walls.
	// If vertical: all y components should be set to 0 on horizontal walls.
	for(int x = 0; x < m_numCells_x; x++) {
		for(int y = 0; y < m_numCells_y; y++) {
			bool solid = obstacles[x * m_numCells_y + y];
			if(solid) {
				if(horizontal) {
					// Look for vertical walls
					if(y < m_numCells_y-1) {
						bool top = obstacles[x * m_numCells_y + y+1];
						if(top) {
							getData(x, y) = 0.0f;
						}
					}
					if(y > 0) {
						bool bottom = obstacles[x * m_numCells_y + y-1];
						if(bottom) {
							getData(x, y) = 0.0f;
						}
					}
				} else {
					// Look for horizontal walls
					if(x > 0) {
						bool left = obstacles[(x-1) * m_numCells_y + y];
						if(left) {
							getData(x, y) = 0.0f;
						}
					}
					if(x < m_numCells_x-1) {
						bool right = obstacles[(x+1) * m_numCells_y + y];
						if(right) {
							getData(x, y) = 0.0f;
						}
					}
				}
			}
		}
	}
}

void FluidField::swap() {
	std::vector<float> tmp = m_data;
	m_data = m_lastData;
	m_lastData = tmp;
}

void FluidField::addSources(float& timeStep) {
	for(int x = 0; x < m_data.size(); x++) {
		//m_data[x] += m_lastData[x] * timeStep;
		//m_lastData[x] = 0.0f;
	}
}

float& FluidField::getData(int& x, int& y) {
	// Account for edges. Use neighbour pointers
	if(x < 0 || y < 0) {
		if(x < 0 && m_x < FLUID_PARTITION_SIZE) {
			x += (float)Singletons::getWorld()->getSize() / FLUID_CELL_SIZE;
			return m_left->getData(x, y);
		}
		if(y < 0 && m_y < FLUID_PARTITION_SIZE) {
			y += (float)WORLD_HEIGHT / FLUID_CELL_SIZE;
			return m_bottom->getData(x, y);
		}
	}
	if(x >= m_numCells_x+m_x/FLUID_CELL_SIZE) {
		return m_right->getData(x, y);
	}
	if(x < m_x/FLUID_CELL_SIZE) {
		return m_left->getData(x, y);
	}
	if(y >= m_numCells_y+m_y/FLUID_CELL_SIZE) {
		return m_top->getData(x, y);
	}
	if(y < m_y/FLUID_CELL_SIZE) {
		return m_bottom->getData(x, y);
	}
	return m_data[(x-m_x) * m_numCells_y + (y-m_y)];
}

float& FluidField::getLastData(int& x, int& y) {
	// Account for edges. Use neighbour pointers
	if(x < 0 || y < 0) {
		if(x < 0 && m_x < FLUID_PARTITION_SIZE) {
			x += (float)Singletons::getWorld()->getSize() / FLUID_CELL_SIZE;
			return m_left->getLastData(x, y);
		}
		if(y < 0 && m_y < FLUID_PARTITION_SIZE) {
			y += (float)WORLD_HEIGHT / FLUID_CELL_SIZE;
			return m_bottom->getLastData(x, y);
		}
	}
	if(x >= m_numCells_x+m_x/FLUID_CELL_SIZE) {
		return m_right->getLastData(x, y);
	}
	if(x < m_x/FLUID_CELL_SIZE) {
		return m_left->getLastData(x, y);
	}
	if(y >= m_numCells_y+m_y/FLUID_CELL_SIZE) {
		return m_top->getLastData(x, y);
	}
	if(y < m_y/FLUID_CELL_SIZE) {
		return m_bottom->getLastData(x, y);
	}
	return m_lastData[(x-m_x) * m_numCells_y + (y-m_y)];
}

void FluidField::constructVector(float fill) {
	destroyVector();

	m_data.resize(m_numCells_x * m_numCells_y, fill);
	m_lastData.resize(m_numCells_x * m_numCells_y, fill);
}

void FluidField::destroyVector() {
	m_data.clear();
	m_lastData.clear();
}

void FluidField::createTexture() {
	unsigned int size = m_numCells_x;

	std::vector<unsigned char> data;
	data.resize(size*size, (unsigned char)0);
	m_texture = GLEngine::ResourceManager::addTexture("fluidTexture" + m_id, size, size, data);

	// (Re)bind the texture object
	glBindTexture(GL_TEXTURE_2D, m_texture.id);

	// Set to not wrap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Unbind
	glBindTexture(GL_TEXTURE_2D, 0);
}
