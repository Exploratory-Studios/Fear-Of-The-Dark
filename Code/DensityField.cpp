#include "DensityField.h"

namespace FluidModule
{

	DensityField::DensityField(unsigned int w, unsigned int h, float idealDensity /* = 1.0f*/) : m_width(w), m_height(h), m_idealDensity(idealDensity)
	{
		m_densities = new float[w*h];
		m_deltaBuffer = new float[w*h];
	}

	DensityField::~DensityField()
	{
		delete m_densities;
		delete m_deltaBuffer;
	}

	void DensityField::update()
	{
		propagate(m_deltaBuffer);

		for(unsigned int i = 0; i < m_width*m_height; i++) {
			m_densities = m_deltaBuffer[i];
		}
	}

	void DensityField::propagate(float* buf)
	{
		// Calls propagateCell for entire field.
		for(unsigned int x = 0; x < m_width; x++) {
			for(unsigned int y = 0; y < m_height; y++) {
				propagateCell(buf, x, y);
			}
		}
	}

	void DensityField::propagateCell(float* buf, unsigned int x, unsigned int y)
	{
		// Propagates density of a single cell to neighbours, based on *that* formula.
		// Traded Pressure = [1-(neighbour[n].density - [ideal_density +- gravity])/sum(neighbour[n].dens)](dens - ideal_density)
		// ideal_density -> liquid_specific
		// gravity -> global gravitational constant (add if trading downwards, subtract if trading upwards). Makes liquid flow down. Incentivises higher densities lower in space and lower densities higher in space.

	}

}
