#pragma once

class Entity;

#include "Animation.h"

namespace CombatModule {

	enum class AttackRange { // For the AI to select which one to use.
		CLOSE,
		MEDIUM,
		FAR
	};

	class Attack;

	Attack* createAttack(unsigned int attackID, ::Entity* owner);

	class Attack {
	  public:
		Attack(unsigned int attackID,
			   ::Entity*	owner); // Uses the ID to fill in various fields (exclusive to each type of attack)
		virtual ~Attack() {};

		virtual void execute(glm::vec2 direction) {
		} // Spawns the projectile or starts the script

		::AnimationModule::SkeletalAnimation getLeadIn() const {
			return m_leadIn;
		}
		::AnimationModule::SkeletalAnimation getLeadOut() const {
			return m_leadOut;
		}

	  protected:
		::Entity* m_owner;

		::AnimationModule::SkeletalAnimation m_leadIn;
		::AnimationModule::SkeletalAnimation m_leadOut;
	};

	class MeleeAttack : public Attack {
	  public:
		MeleeAttack(unsigned int attackID, ::Entity* owner);
		~MeleeAttack() {};

		virtual void execute(glm::vec2 direction) override;

	  protected:
		unsigned int m_projectileID;
	};

	class RangedAttack : public Attack {
	  public:
		RangedAttack(unsigned int attackID, ::Entity* owner);
		~RangedAttack() {};

		virtual void execute(glm::vec2 direction) override;

	  protected:
		unsigned int m_projectileID;
		unsigned int m_numProjectiles = 1;
		float		 m_angleWidth	  = (1.0f / 60.0f) * (2.0f * 3.141f); // 6 degrees default IN RADIANS!
	};

	class MagicAttack : public Attack {
	  public:
		MagicAttack(unsigned int attackID, ::Entity* owner);
		~MagicAttack() {};

		virtual void execute(glm::vec2 direction) override;

	  protected:
		BARE2D::LuaScript m_script;
	};
} // namespace CombatModule
