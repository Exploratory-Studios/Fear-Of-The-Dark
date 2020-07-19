#pragma once

#include "Item.h"

namespace AnimationModule {
	class Body;
}

class Armour : public Item {
		/**
		 * Responsibilities:
		 *  - Hold animation IDs for limb(s)
		 *  - Hold limb indices for limb(s)
		 *  - Hold defence value
		 *  - Hold an on-tick script
		 *  - Have functions for onTick, and setLimbAnimations (which takes a reference to a Body)
		 *  - No draw function needed, the limb objects handle that
		 **/
	public:
		Armour(short unsigned int quantity, unsigned int armourID, bool loadTex);
		~Armour();

		void init();

		void onTick(EntityNPC* owner); // Runs on-tick script
		void setLimbAnimations(AnimationModule::Body& body); // Only sets the limbs with indices in m_limbIndices to have animations with IDs in m_animationIDs

	private:
		std::vector<unsigned int> m_animationIDs;
		std::vector<unsigned int> m_limbIndices;
		unsigned int m_defence = 0;
		unsigned int m_tickScriptID = (unsigned int) - 1;
};
