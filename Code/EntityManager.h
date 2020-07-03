#pragma once

#include <map>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <DebugRenderer.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

class Entity;
class EntityPlayer;
class AudioManager;
class World;

class EntityManager {
	public:
		EntityManager();
		~EntityManager(); // Deletes all created entities. RAII Rocks.

		void init(World* world);
		void dispose();

		void sortEntities();
		void addEntity(Entity* e);
		void removeEntity(unsigned int index);
		void removeEntity(std::string UUID);

		void drawEntities(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, GLEngine::DebugRenderer& dr, glm::vec4 destRect);
		void drawEntitiesNormal(GLEngine::SpriteBatch& sb, glm::vec4 destRect);
		void updateEntities(float timeStep);
		void tickEntities();

		std::vector<Entity*> getEntities() {
			return m_entities;
		}
		Entity* getEntityByUUID(std::string UUID) {
			auto i = m_entitiesByUUID.find(UUID);
			return i->second;
		}
		EntityPlayer* getPlayer() {
			return m_player;
		}

	private:
		void spawnEntities();

		EntityPlayer* m_player = nullptr;
		std::vector<Entity*> m_entities; // entities by x value

		World* m_world = nullptr;

		std::map<std::string, Entity*> m_entitiesByUUID; // Organized by UUID (For Lua)

		bool m_inited = false;
};
