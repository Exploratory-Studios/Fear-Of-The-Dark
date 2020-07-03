#pragma once

class Entity;

namespace CombatModule {

	enum class AttackRange { // For the AI to select which one to use.
		CLOSE,
		MEDIUM,
		FAR
	};

	class Attack {
		public:
			Attack(unsigned int attackID, ::Entity* owner); // Uses the ID to fill in various fields (exclusive to each type of attack)

			virtual void execute() {} // Spawns the projectile or starts the script

			Animation getLeadIn() const {
				return m_leadIn;
			}
			Animation getLeadOut() const {
				return m_leadOut;
			}

		private:
			::Entity* m_owner;

			::Animation m_leadIn;
			::Animation m_leadOut;
	};

	class MeleeAttack : public Attack {
		public:
			MeleeAttack(unsigned int attackID, ::Entity* owner);

			virtual void execute() override;

		private:
			float m_range = 1.0f; // Blocks
			float m_damage = 0.0f;
			float m_angleWidth = (1.0f / 4.0f) * (2.0f * 3.141f); // 90 degrees default IN RADIANS!
	};

	class RangedAttack : public Attack {
		public:
			RangedAttack(unsigned int attackID, ::Entity* owner);

			virtual void execute() override;

		private:
			unsigned int m_projectileID = 0;
			unsigned int m_numProjectiles = 1;
			float m_angleWidth = (1.0f / 60.0f) * (2.0f * 3.141f); // 6 degrees default IN RADIANS!
	};

	class MagicAttack : public Attack {
		public:
			MagicAttack(unsigned int attackID, ::Entity* owner);

			virtual void execute() override;

		private:
			unsigned int m_scriptID = 0;
	};
}
