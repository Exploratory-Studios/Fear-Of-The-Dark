#pragma once

#include <DebugRenderer.h>
#include <ParticleEngine2D.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <Camera2D.h>

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
		friend class Singletons;

	public:
		World();
		~World();
		void setTile(Tile* tile);
		Tile* getTile(int x, int y, int layer);
		XMLModule::BiomeData getBiome(int x); // Returns the biome data of a certain x position

		void sortLights();
		void addLight(Tile* t);
		void removeLight(unsigned int index);
		void removeLight(Tile* t);
		void getRenderedLights(glm::vec4 destRect, float lights[MAX_LIGHTS_RENDERED]);

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
		void drawTilesGUI(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, glm::vec4 destRect);
		void updateTiles(glm::vec4 destRect);
		void tickTiles(glm::vec4 destRect);

		void drawParticles(GLEngine::SpriteBatch* sb);
		void drawSunlight(GLEngine::SpriteBatch& sb, glm::vec4 destRect); // Just draws the sunlight value into the rgb components.

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

		//Tile**** m_tiles = nullptr;
		std::vector<std::vector<std::vector<Tile*>>> m_tiles; // [x][y][z]

		std::vector<Tile*> m_lights; // Vector of tiles (ordered by x pos), that need to be checked for light rendering.
		std::vector<Tile*> m_deadTiles; // Vector of tiles that need to be destroyed and deleted, but may be bound to other systems.

		unsigned int m_biomesMap[(WORLD_SIZE / CHUNK_SIZE)]; // Simply a 1d vector of biome IDs, which can be mapped onto the world by referencing each "chunk"'s size

		unsigned long int m_time = 0; // Tick time
		unsigned long int m_frame = 0; // frames that have passed
		float m_sunlight = 0.0f;

		unsigned int m_worldEra = 0;
		unsigned int m_nextWorldEra = 0; // If this is different than m_worldEra, trigger WorldIOManager to advance time.

		std::string m_name;
};
