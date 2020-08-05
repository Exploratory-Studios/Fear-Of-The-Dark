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

		void sortEntities(); // Sorts entity vector, based on the horizontal position. Uses swap sort (Best for 1 step changes, which are the most likely)

		void queueEntityToRemove(Entity* entity); // Adds entity to a remove/add queue, to be processed at the end of each update cycle
		void queueEntityToRemove(std::string UUID);
		void queueEntityToAdd(Entity* entity);

		void drawEntities(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, GLEngine::DebugRenderer& dr, glm::vec4 destRect);
		void drawEntitiesNormal(GLEngine::SpriteBatch& sb, glm::vec4 destRect);
		void updateEntities(float timeStep); // Removes/adds necessary entities (determined by queues)
		void tickEntities();

		std::vector<Entity*> getEntities() {
			return m_entities;
		}
		Entity* getEntityByUUID(std::string UUID) {
			auto i = m_entitiesByUUID.find(UUID);
			return i->second;
		}
		unsigned int getEntityIndex(Entity* entity);
		EntityPlayer* getPlayer() {
			return m_player;
		}
		void setPlayer(EntityPlayer* p);

	private:
		void addEntity(Entity* e);
		void removeEntity(unsigned int index);
		void removeEntity(std::string UUID);
		void removeEntity(Entity* entity);

		void spawnEntities();

		EntityPlayer* m_player = nullptr;
		std::vector<Entity*> m_entities; // entities by x value

		std::vector<Entity*> m_entitiesToRemove; // All will be removed/added at once at the end of each update, to prevent weird processing errors.
		std::vector<Entity*> m_entitiesToAdd;

		World* m_world = nullptr;

		std::map<std::string, Entity*> m_entitiesByUUID; // Organized by UUID (For Lua)

		bool m_inited = false;
};
