#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <SpriteBatch.h>
#include <GLTexture.h>

class FluidVelocityField;

class FluidField {
	public:
		FluidField(unsigned int numCells_x, unsigned int numCells_y, float cellSize, float x, float y, std::string id, float fill = 0.0f);
		~FluidField();

		void setNeighbours(FluidField* left, FluidField* right, FluidField* bottom, FluidField* top);

		void diffuse(float& timeStep, float& rateOfDiffusion, std::vector<bool>& obstacles);
		void advect(float& timeStep, FluidVelocityField* velocities, std::vector<bool>& obstacles);
		void project(float& timeStep, std::vector<bool>& obstacles); // Makes the velocity field mass-conserving (see paper)
		void addSource(int& x, int& y, float source, bool clamp = true);

		void draw(GLEngine::SpriteBatch& sb, glm::vec2& pos);
		void constructTextureData(std::vector<unsigned char>& data);

		void setObstacles(std::vector<bool>& obstacles, bool horizontal);

		void swap();
		void addSources(float& timeStep);

		float& getData(int& x, int& y);
		float& getLastData(int& x, int& y);
		std::vector<float> getDataVec();

		float getCellSize() {
			return m_cellSize;
		}
		int getNumCellsX() {
			return m_numCells_x;
		}
		int getNumCellsY() {
			return m_numCells_y;
		}

	protected:
		void constructVector(float fill);
		void destroyVector();

		void createTexture();

		std::vector<float> m_data;
		std::vector<float> m_lastData;
		std::vector<unsigned char> m_textureData;

		GLEngine::GLTexture m_texture;

		int m_numCells_x, m_numCells_y; // How many cells horizontally/vertically
		float m_cellSize; // width/height of cell, in terms of the width/height of a tile being 1.0f
		float m_x, m_y;

		std::string m_id;

		FluidField* m_left = nullptr, *m_right = nullptr, *m_bottom = nullptr, *m_top = nullptr; // neighbouring fields

};
