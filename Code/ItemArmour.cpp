#include "ItemArmour.h"

#include <XMLDataManager.hpp>

#include "Animation.h"
#include "EntityNPC.h"

ItemArmour::ItemArmour(short unsigned int quantity, unsigned int armourID) :
	Item(quantity, armourID) {
	init();
}

ItemArmour::~ItemArmour() {
}

void ItemArmour::init() {
	XMLModule::ItemArmourData d = getItemArmourData(m_id);

	m_animationIDs = d.animationIDs;
	m_limbIndices  = d.limbIndices;
	m_resistance   = d.resistance;
	m_threshold	   = d.threshold;
	m_tickScript = d.tickScript;
}

void ItemArmour::onTick(EntityNPC* owner) {
	// Runs on-tick script
	if(m_tickScript.inited)
		BARE2D::LuaScriptQueue::getInstance()->addLuaScript(m_tickScript);
}

void ItemArmour::setLimbAnimations(AnimationModule::Body& body) {
	// Only sets the limbs with indices in m_limbIndices to have animations with IDs in m_animationIDs
	for(unsigned int i = 0; i < m_limbIndices.size(); i++) {
		AnimationModule::Limb* limb = body.getLimb(m_limbIndices[i]);
		if(limb) {
			// Make sure it exists, lol
			AnimationModule::Animation limbAnim(m_animationIDs[i]);
			limb->setAnimation(limbAnim);
		}
	}
}
