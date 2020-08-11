#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <SpriteBatch.h>
#include <GLTexture.h>

class FluidVelocityField;

class FluidField {
	public:
		FluidField(unsigned int numCells_x, unsigned int numCells_y, float cellSize, GLEngine::GLTexture texture);
		~FluidField();

		void setNeighbours(FluidField* left, FluidField* right, FluidField* bottom, FluidField* top);

		void diffuse(float& timeStep, float& rateOfDiffusion, std::vector<bool>& obstacles);
		void advect(float& timeStep, FluidVelocityField* velocities, std::vector<bool>& obstacles);
		void project(float& timeStep, std::vector<bool>& obstacles); // Makes the velocity field mass-conserving (see paper)
		void addSource(int& x, int& y, float source);

		void draw(GLEngine::SpriteBatch& sb);
		std::vector<unsigned char> constructTextureData();

		void setObstacles(std::vector<bool>& obstacles, bool horizontal);

		void setOld();

		float& getData(int& x, int& y);
		float& getLastData(int& x, int& y);
		std::vector<float> getDataVec();

		void setTexture(GLEngine::GLTexture& tex);

	protected:
		void constructVector();
		void destroyVector();

		std::vector<float> m_data;
		std::vector<float> m_lastData;

		GLEngine::GLTexture m_texture;

		int m_numCells_x, m_numCells_y; // How many cells horizontally/vertically
		float m_cellSize; // width/height of cell, in terms of the width/height of a tile being 1.0f

		FluidField* m_left = nullptr, *m_right = nullptr, *m_bottom = nullptr, *m_top = nullptr; // neighbouring fields

};
