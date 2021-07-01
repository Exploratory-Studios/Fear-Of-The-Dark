#pragma once

#include "PresetValues.h"

namespace FluidModule {
	struct FluidCell {
		FluidCell() {
		}
		~FluidCell() {
		}
		float density = 0.0f;
	};

	struct DensityField {
		DensityField() {
			densities	   = new FluidCell[FLUID_PARTITION_SIZE * FLUID_PARTITION_SIZE];
			deltaDensities = new FluidCell[FLUID_PARTITION_SIZE * FLUID_PARTITION_SIZE];
		}
		~DensityField() {
			delete densities;
			delete deltaDensities;
		}

		FluidCell* getDensityCell(unsigned int x, unsigned int y) {
			return &densities[x + FLUID_PARTITION_SIZE * y];
		}
		FluidCell* getDeltaDensityCell(unsigned int x, unsigned int y) {
			return &deltaDensities[x + FLUID_PARTITION_SIZE * y];
		}

		FluidCell* densities	  = nullptr;
		FluidCell* deltaDensities = nullptr;

		bool inEquilibrium =
			false; // If any interaction between cells trades more than some arbitrary amount of pressure, equilibrium is not achieved
		bool brokeEquilibrium =
			false; // Has equilibrium been broken this past update cycle (Should this be true, break neighbours' equilibriums)

		void swapForDelta() {
			densities = deltaDensities;
		}
	};
} // namespace FluidModule
