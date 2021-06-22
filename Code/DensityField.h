#pragma once

namespace FluidModule
{

	class DensityField
	{
		public:
			DensityField(unsigned int w, unsigned int h, float idealDensity = 1.0f);
			~DensityField();

			float& getDensity(unsigned int x, unsigned int y)
			{
				return m_densities[y * m_height + x];
			}

			void update();

		private:
			float& getDensity_Delta(unsigned int x, unsigned int y)
			{
				return m_deltaBuffer[y * m_height + x];
			}
		
			unsigned int m_width, m_height; // width and height of densityfield (in # of cells)
			float* m_densities = nullptr; // Holds all densities
			float* m_deltaBuffer = nullptr; // Is the 'swap' buffer to update fields.

			void propagate(float** buf); // Calls propagateCell for entire field.
			void propagateCell(float** buf, unsigned int x, unsigned int y); // Propagates density of a cell, based on *that* formula.


	};

}
