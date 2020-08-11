#include "FluidField.h"

#include <ResourceManager.h>

#include "FluidVelocityField.h"

FluidField::FluidField(unsigned int numCells_x, unsigned int numCells_y, float cellSize, GLEngine::GLTexture texture) : m_cellSize(cellSize), m_texture(texture) {
	m_numCells_x = numCells_x;
	m_numCells_y = numCells_y;
	constructVector();
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

	for(unsigned int k = 0; k < 1; k++) { // Something for the relaxation
		for(int x = 0; x < m_numCells_x; x++) {
			for(int y = 0; y < m_numCells_y; y++) {
				int leftX = x-1;
				int rightX = x+1;
				int bottomY = y-1;
				int topY = y+1;
				getData(x, y) = (getLastData(x, y) + a * (getData(leftX, y) + getData(rightX, y) + getData(x, bottomY) + getData(x, topY))) / (1 + 4*a);
			}
		}
		// setObstacles()?
	}
}

void FluidField::advect(float& timeStep, FluidVelocityField* velocities, std::vector<bool>& obstacles) {
	// We need to backtrace, then interpolate

	for(int x = 0; x < m_numCells_x; x++) {
		for(int y = 0; y < m_numCells_y; y++) {
			// get backtraced x and y
			float backtracedX = x - timeStep * velocities->getXVelocities()->getData(x, y);
			float backtracedY = y - timeStep * velocities->getYVelocities()->getData(x, y);

			// Get closest neighbours
			// X's
			int neighbourXPos0 = (int)backtracedX; // Lower bound
			int neighbourXPos1 = neighbourXPos0 + 1;

			// Y's
			int neighbourYPos0 = (int)backtracedY; // Lower bound
			int neighbourYPos1 = neighbourYPos0 + 1;

			// Now we need to get how "far" each neighbour is, to linearly interpolate
			float xClosenessRight = backtracedX - neighbourXPos0;
			float xClosenessLeft = 1.0f - xClosenessRight;
			float yClosenessTop = backtracedY - neighbourYPos0;
			float yClosenessBottom = 1.0f - yClosenessTop;

			// Actually interpolate the densities now (XPos0 == left, yPos0 == bottom)
			float value = xClosenessLeft  * (yClosenessBottom * getLastData(neighbourXPos0, neighbourYPos0) +
			                                 yClosenessTop    * getLastData(neighbourXPos0, neighbourYPos1)) +
			              xClosenessRight * (yClosenessBottom * getLastData(neighbourXPos1, neighbourYPos0) +
			                                 yClosenessTop    * getLastData(neighbourXPos1, neighbourYPos1));
			m_data[x * m_numCells_y + y] = value;
		}
	}
}

void FluidField::project(float& timeStep, std::vector<bool>& obstacles) {
	// We don't even care about this rn
}

void FluidField::addSource(int& x, int& y, float source) {
	getData(x, y) = source;
}

void FluidField::draw(GLEngine::SpriteBatch& sb) {
	std::vector<unsigned char> data;
	data.resize(m_numCells_x*m_numCells_y*4, 0);
	data = constructTextureData();
	GLEngine::ResourceManager::setTexture(m_texture.filePath, &data);

	sb.draw(glm::vec4(0.0f, 0.0f, 100.0f, 100.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_texture.id, 0.0f, GLEngine::ColourRGBA8(255, 255, 255, 255));
}

std::vector<unsigned char> FluidField::constructTextureData() {
	// Only write to the R value (once every 4 components I assume)
	std::vector<unsigned char> data;
	data.resize(m_numCells_x*m_numCells_y*4, 0);

	for(int x = 0; x < m_numCells_x; x++)
		for(int y = 0; y < m_numCells_y; y++) {
			data[(x*m_numCells_y+y)*4] = getData(x, y);
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
					bool top = obstacles[x * m_numCells_y + y+1];
					if(top) {
						getData(x, y) = 0.0f;
						continue;
					}
					bool bottom = obstacles[x * m_numCells_y + y-1];
					if(top) {
						getData(x, y) = 0.0f;
						continue;
					}
				} else {
					// Look for horizontal walls
					bool left = obstacles[(x-1) * m_numCells_y + y];
					if(left) {
						getData(x, y) = 0.0f;
						continue;
					}
					bool right = obstacles[(x+1) * m_numCells_y + y];
					if(right) {
						getData(x, y) = 0.0f;
						continue;
					}
				}
			}
		}
	}
}

void FluidField::setOld() {
	m_lastData = m_data;
}

float& FluidField::getData(int& x, int& y) {
	// Account for edges. Use neighbour pointers
	if(x >= m_numCells_x) {
		int newX = x-m_numCells_x;
		return m_right->getData(newX, y);
	}
	if(x < 0) {
		int newX = x+m_numCells_x;
		return m_left->getData(newX, y);
	}
	if(y >= m_numCells_y) {
		int newY = y-m_numCells_y;
		return m_top->getData(x, newY);
	}
	if(y < 0) {
		int newY = y+m_numCells_y;
		return m_bottom->getData(x, newY);
	}
	return m_data[x * m_numCells_y + y];
}

float& FluidField::getLastData(int& x, int& y) {
	// Account for edges. Use neighbour pointers
	if(x >= m_numCells_x) {
		int newX = x-m_numCells_x;
		return m_right->getLastData(newX, y);
	}
	if(x < 0) {
		int newX = x+m_numCells_x;
		return m_left->getLastData(newX, y);
	}
	if(y >= m_numCells_y) {
		int newY = y-m_numCells_y;
		return m_top->getLastData(x, newY);
	}
	if(y < 0) {
		int newY = y+m_numCells_y;
		return m_bottom->getLastData(x, newY);
	}
	return m_lastData[x * m_numCells_y + y];
}

void FluidField::setTexture(GLEngine::GLTexture& tex) {
	m_texture = tex;
}

void FluidField::constructVector() {
	destroyVector();

	m_data.resize(m_numCells_x * m_numCells_y, 0.0f);
	m_lastData.resize(m_numCells_x * m_numCells_y, 0.0f);
}

void FluidField::destroyVector() {
	m_data.clear();
	m_lastData.clear();
}
