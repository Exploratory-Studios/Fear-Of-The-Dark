#include "DensityField.h"

#include "SaveDataTypes.h"

namespace FluidModule {

	DensityField::DensityField(SaveDataTypes::FluidDensityData& data) {
		init();
		
		for(unsigned int i = 0; i < FLUID_PARTITION_SIZE*FLUID_PARTITION_SIZE; i++) {
			densities[i].density = data.densities[i];
			densities[i].avgDensity_3Steps = data.densities[i];
			deltaDensities[i].density = data.deltaDensities[i];
			deltaDensities[i].avgDensity_3Steps = data.deltaDensities[i];
		}
		
		inEquilibrium = data.inEquilibrium;
	}

}
