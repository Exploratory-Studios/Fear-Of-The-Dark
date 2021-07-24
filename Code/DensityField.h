#pragma once

#include "PresetValues.h"

namespace FluidModule {
	struct FluidCell {
		FluidCell() {
		}
		~FluidCell() {
		}
		void updateAvgDensity() {
			avgDensity_3Steps = ((2.0f) * avgDensity_3Steps + density) / 3.0f;

			// First, the avg density will be 1.0f, then 1.0f, 1.0f, 1.0f...
		}

		float density			= 0.0f;
		float avgDensity_3Steps = 0.0f; // The average density over the past three steps.

		// Avg(n steps) = [(n-1)*avg_n_steps + this->density]/n
	};

	struct DensityField {
		DensityField() {
			densities	   = new FluidCell[FLUID_PARTITION_SIZE * FLUID_PARTITION_SIZE];
			deltaDensities = new FluidCell[FLUID_PARTITION_SIZE * FLUID_PARTITION_SIZE];
		}
		~DensityField() {
			delete[] densities;
			delete[] deltaDensities;
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

		bool checkForEquilibrium() {
			float totalTraded = 0.0f;

			/*for(unsigned int i = 0; i < FLUID_PARTITION_SIZE * FLUID_PARTITION_SIZE; i++)
				totalTraded += std::abs(densities[i].density - deltaDensities[i].density);*/

			for(unsigned int i = 0; i < FLUID_PARTITION_SIZE * FLUID_PARTITION_SIZE; i++) {
				totalTraded += std::abs(densities[i].density - densities[i].avgDensity_3Steps);
			}

			if(totalTraded >= FLUID_AVG_CELL_EQUILIBRIUM_THRESHOLD * FLUID_PARTITION_SIZE * FLUID_PARTITION_SIZE) {
				inEquilibrium = false;
			} else {
				inEquilibrium = true;
			}
			return inEquilibrium;
		}

		void swapForDelta() {
			for(unsigned int i = 0; i < FLUID_PARTITION_SIZE * FLUID_PARTITION_SIZE; i++) {
				densities[i].density	  = deltaDensities[i].density;
				deltaDensities[i].density = 0.0f;
			}
		}
	};
} // namespace FluidModule
