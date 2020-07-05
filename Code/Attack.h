#pragma once

class Entity;

#include "Animation.h"

namespace CombatModule {

	enum class AttackRange { // For the AI to select which one to use.
		CLOSE,
		MEDIUM,
		FAR
	};

	class Attack {
		public:
			Attack(unsigned int attackID, ::Entity* owner); // Uses the ID to fill in various fields (exclusive to each type of attack)
			virtual ~Attack() {};

			virtual void execute() {} // Spawns the projectile or starts the script

			::AnimationModule::Animation getLeadIn() const {
				return m_leadIn;
			}
			::AnimationModule::Animation getLeadOut() const {
				return m_leadOut;
			}

		protected:
			::Entity* m_owner;

			::AnimationModule::Animation m_leadIn;
			::AnimationModule::Animation m_leadOut;
	};

	class MeleeAttack : public Attack {
		public:
			MeleeAttack(unsigned int attackID, ::Entity* owner);
			~MeleeAttack() {};

			virtual void execute() override;

		protected:
			unsigned int m_projectileID;
	};

	class RangedAttack : public Attack {
		public:
			RangedAttack(unsigned int attackID, ::Entity* owner);
			~RangedAttack() {};

			virtual void execute() override;

		protected:
			unsigned int m_projectileID;
			unsigned int m_numProjectiles = 1;
			float m_angleWidth = (1.0f / 60.0f) * (2.0f * 3.141f); // 6 degrees default IN RADIANS!
	};

	class MagicAttack : public Attack {
		public:
			MagicAttack(unsigned int attackID, ::Entity* owner);
			~MagicAttack() {};

			virtual void execute() override;

		protected:
			unsigned int m_scriptID = 0;
	};
}
