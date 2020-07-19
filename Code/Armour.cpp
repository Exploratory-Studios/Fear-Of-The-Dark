#include "Armour.h"

#include "Animation.h"
#include "EntityNPC.h"

Armour::Armour(short unsigned int quantity, unsigned int armourID, bool loadTex) : Item(quantity, armourID, loadTex) {
	init();
}

Armour::~Armour() {

}

void Armour::init() {
	XMLModule::ItemArmourData d = XMLModule::XMLData::getItemArmourData(m_id);

	m_animationIDs = d.animationIDs;
	m_limbIndices = d.limbIndices;
	m_defence = d.defence;
	m_tickScriptID = d.tickScriptID;
}

void Armour::onTick(EntityNPC* owner) {
	// Runs on-tick script
	if(m_tickScriptID != (unsigned int) - 1) ScriptingModule::ScriptQueue::activateScript(m_tickScriptID, owner->generateLuaValues());
}

void Armour::setLimbAnimations(AnimationModule::Body& body) {
	// Only sets the limbs with indices in m_limbIndices to have animations with IDs in m_animationIDs
	for(unsigned int i = 0; i < m_limbIndices.size(); i++) {
		AnimationModule::Limb* limb = body.getLimb(m_limbIndices[i]);
		if(limb) {
			// Make sure it exists, lol
			AnimationModule::Animation limbAnim(i);
			limb->setAnimation(limbAnim);
		}
	}
}
