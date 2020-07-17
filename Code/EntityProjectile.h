#pragma once

#include "Entity.h"

class EntityProjectile : public Entity {
	public:
		EntityProjectile(glm::vec2 pos, unsigned int layer, unsigned int id, SaveDataTypes::MetaData data, bool loadTex);
		EntityProjectile(glm::vec2 pos, unsigned int layer, EntityIDs id, SaveDataTypes::MetaData data, bool loadTex);
		virtual ~EntityProjectile();

		virtual void draw(GLEngine::SpriteBatch& sb, float time, int layerDifference, float xOffset) override;
		virtual void drawNormal(GLEngine::SpriteBatch& sb, float time, int layerDifference, float xOffset) override;

		virtual void collideWithTiles(World* world) override;
		virtual bool collideWithOther(Entity* other) override;

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
		float getDamage() {
			return m_damage;
		}
		float getKnockback() {
			return m_knockback;
		}
		std::vector<unsigned int> getBuffs() {
			return m_buffIDs;
		}
		bool isActive() {
			return m_active;
		}
		void setActive(bool setting) {
			m_active = setting;
		}

	protected:
		virtual void onUpdate(World* world, float timeStep, unsigned int selfIndex) override;
		virtual void onTick(World* world) override;

		Entity* m_owner = nullptr;
		AnimationModule::Animation m_anim; // This is the animation that plays from creation onwards.

		// Movement
		float m_speed;
		bool m_collideWithBlocks;

		// Combat
		float m_damage;
		float m_knockback = 0.0f;
		float m_lifeTime = -1.0f;
		std::vector<unsigned int> m_buffIDs;

		bool m_active = true;

};
