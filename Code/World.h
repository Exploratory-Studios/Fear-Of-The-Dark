#pragma once

#include <DebugRenderer.h>
#include <ParticleEngine2D.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>

#include "Categories.h"
#include "PresetValues.h"

#include "XMLDataTypes.h"

class Entity;
class EntityNPC;
class EntityPlayer;
class Tile;
class AudioManager;
class ScriptQueue;

class World {
		friend class WorldIOManager;
		friend class App;

	public:
		World();
		~World();
		void setTile(Tile* tile);
		Tile* getTile(int x, int y, int layer);
		XMLModule::BiomeData getBiome(int x); // Returns the biome data of a certain x position

		void sortEntities();
		void addEntity(Entity* e);
		void removeEntity(unsigned int index);
		void removeEntity(std::string UUID);

		void sortLights();
		void addLight(Tile* t);
		void removeLight(unsigned int index);
		void removeLight(Tile* t);
		void getRenderedLights(glm::vec4 destRect, float lights[MAX_LIGHTS_RENDERED]);

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
		unsigned long int getTime() {
			return m_time;
		}
		unsigned long int getFrame() {
			return m_frame;
		}
		std::string getName() {
			return m_name;
		}
		unsigned int getEra() {
			return m_worldEra;
		}
		unsigned int getNextEra() {
			return m_nextWorldEra;
		}

		void setName(std::string name) {
			m_name = name;
		}
		void setPlayer(EntityPlayer& p);
		void setTime(unsigned int time) {
			m_time = time;
		}

		void setLightsUniform(glm::vec4 destRect, GLEngine::GLSLProgram* textureProgram);

		void drawTiles(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, GLEngine::DebugRenderer& dr, glm::vec4 destRect, GLEngine::GLSLProgram* textureProgram);
		void drawTilesNormal(GLEngine::SpriteBatch& sb, glm::vec4 destRect, GLEngine::GLSLProgram* textureProgram);
		void updateTiles(glm::vec4 destRect);
		void tickTiles(glm::vec4 destRect);

		void drawEntities(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, GLEngine::DebugRenderer& dr, glm::vec4 destRect);
		void drawEntitiesNormal(GLEngine::SpriteBatch& sb, glm::vec4 destRect);
		void updateEntities(AudioManager* audio, float timeStep);
		void tickEntities(AudioManager* audio);

		void drawParticles(GLEngine::SpriteBatch* sb);

		void drawDebug(GLEngine::DebugRenderer& dr, float xOffset);

		void incrementTime() {
			m_frame++;
			if((m_frame % 60 / TICK_RATE) == 0) m_time++;
		}

		void setWorldEra(unsigned int newEraID) {
			m_nextWorldEra = newEraID;
		}

		float getDistance(glm::vec2 point0, glm::vec2 point1); // Gets the distance between two points, accounting for crossover.

	private:
		World(bool quickLoad) {}

		void setTile_noEvent(Tile* tile);

		void specialUpdateTile(Tile* origin);

		void spawnEntities();

		Tile**** m_tiles = nullptr;
		std::vector<Tile*> m_lights; // Vector of tiles (ordered by x pos), that need to be checked for light rendering.
		std::vector<Tile*> m_deadTiles; // Vector of tiles that need to be destroyed and deleted, but may be bound to other systems.

		std::vector<Entity*> m_entities; // entities by x value
		std::map<std::string, Entity*> m_entitiesByUUID; // Organized by UUID (For Lua)

		EntityPlayer* m_player = nullptr;

		unsigned int m_biomesMap[(WORLD_SIZE / CHUNK_SIZE)]; // Simply a 1d vector of biome IDs, which can be mapped onto the world by referencing each "chunk"'s size

		unsigned long int m_time = 0; // Tick time
		unsigned long int m_frame = 0; // frames that have passed
		float m_sunlight = 0.0f;

		unsigned int m_worldEra = 0;
		unsigned int m_nextWorldEra = 0; // If this is different than m_worldEra, trigger WorldIOManager to advance time.

		std::string m_name;
};
