#include "Attack.h"

#include "Entity.h"

namespace CombatModule {

	Attack::Attack(unsigned int attackID, ::Entity* owner) : m_owner(owner) {
		XMLModule::AttackData d = XMLModule::XMLData::getAttackData(attackID);

		m_leadIn.init(d.leadInAnimation);
		m_leadOut.init(d.leadOutAnimation);
	}

};
