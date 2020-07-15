#pragma once

#include "Entity.h"

class EntityProjectile : public Entity {
	public:
		EntityProjectile(glm::vec2 pos, unsigned int layer, unsigned int id, SaveDataTypes::MetaData data, bool loadTex);
		EntityProjectile(glm::vec2 pos, unsigned int layer, EntityIDs id, SaveDataTypes::MetaData data, bool loadTex);
		virtual ~EntityProjectile();

		void init();

		void setDirection(glm::vec2& direction) {
			m_velocity = direction * glm::vec2(m_speed);
		}
		void setOwner(Entity* owner) {
			m_owner = owner;
		}
		Entity* getOwner() {
			return m_owner;
		}

	protected:
		Entity* m_owner = nullptr;

		// Movement
		float m_speed;
		bool m_collideWithBlocks;

		// Combat
		float m_damage;

};
