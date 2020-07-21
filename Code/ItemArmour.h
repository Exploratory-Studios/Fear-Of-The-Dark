#pragma once

#include "Item.h"

namespace AnimationModule {
	class Body;
}

class ItemArmour : public Item {
		/**
		 * Responsibilities:
		 *  - Hold animation IDs for limb(s)
		 *  - Hold limb indices for limb(s)
		 *  - Hold damage threshold value
		 *  - Hold damage resistance value
		 *  - Hold an on-tick script
		 *  - Have functions for onTick, and setLimbAnimations (which takes a reference to a Body)
		 *  - No draw function needed, the limb objects handle that
		 **/
	public:
		ItemArmour(short unsigned int quantity, unsigned int armourID, bool loadTex);
		~ItemArmour();

		void init();

		void onTick(EntityNPC* owner); // Runs on-tick script
		void setLimbAnimations(AnimationModule::Body& body); // Only sets the limbs with indices in m_limbIndices to have animations with IDs in m_animationIDs

		float getResistance() {
			return m_resistance;
		}
		float getThreshold() {
			return m_threshold;
		}

	private:
		std::vector<unsigned int> m_animationIDs;
		std::vector<unsigned int> m_limbIndices;
		unsigned int m_threshold = 0;
		float m_resistance = 0.0f;
		unsigned int m_tickScriptID = (unsigned int) - 1;
};
