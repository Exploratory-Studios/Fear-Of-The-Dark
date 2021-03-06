#include "Attack.h"

#include "Entity.h"
#include "EntityProjectile.h"
#include "SaveDataTypes.h"

#include "Singletons.h"
#include "Factory.h"

#include <iostream>

namespace CombatModule {

	Attack::Attack(unsigned int attackID, ::Entity* owner) : m_owner(owner) {
		XMLModule::AttackData d = XMLModule::XMLData::getAttackData(attackID);

		m_leadIn.init(d.leadInAnimationID);
		m_leadOut.init(d.leadOutAnimationID);
	}

	MeleeAttack::MeleeAttack(unsigned int attackID, ::Entity* owner) : Attack(attackID, owner) {
		XMLModule::MeleeAttackData d = XMLModule::XMLData::getMeleeAttackData(attackID);

		m_projectileID = d.projectileID;
	}

	void MeleeAttack::execute(glm::vec2 direction) {
		glm::vec2	 pos   = m_owner->getPosition();
		unsigned int layer = m_owner->getLayer();

		EntityProjectile* e = static_cast<EntityProjectile*>(
			Factory::createEntity(m_projectileID, pos, layer, SaveDataTypes::MetaData(), true));
		e->setOwner(m_owner);

		e->setPosition(m_owner->getPosition() + m_owner->getSize() / glm::vec2(2.0f) - e->getSize() / glm::vec2(2.0f) +
					   direction * e->getSize());
		e->setDirection(direction);

		Singletons::getEntityManager()->queueEntityToAdd(e);
	}

	RangedAttack::RangedAttack(unsigned int attackID, ::Entity* owner) : Attack(attackID, owner) {
		XMLModule::RangedAttackData d = XMLModule::XMLData::getRangedAttackData(attackID);

		m_projectileID	 = d.projectileID;
		m_numProjectiles = d.numProjectiles;
		m_angleWidth	 = glm::radians(d.angleWidth);
	}

	void RangedAttack::execute(glm::vec2 direction) {
		glm::vec2	 pos   = m_owner->getPosition();
		unsigned int layer = m_owner->getLayer();

		EntityProjectile* e = static_cast<EntityProjectile*>(
			Factory::createEntity(m_projectileID, pos, layer, SaveDataTypes::MetaData(), true));
		e->setOwner(m_owner);

		e->setPosition(m_owner->getPosition() + direction * e->getSize());
		e->setDirection(direction);

		Singletons::getEntityManager()->queueEntityToAdd(e);
	}

	MagicAttack::MagicAttack(unsigned int attackID, ::Entity* owner) : Attack(attackID, owner) {
		XMLModule::MagicAttackData d = XMLModule::XMLData::getMagicAttackData(attackID);

		m_scriptID = d.script.getID();
	}

	void MagicAttack::execute(glm::vec2 direction) {
		ScriptingModule::ScriptQueue::activateScript(m_scriptID, m_owner->generateLuaValues());
	}

}; // namespace CombatModule
