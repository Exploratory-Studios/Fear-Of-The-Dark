#pragma once

#include <DebugRenderer.h>
#include <ParticleEngine2D.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>

#include "Categories.h"
#include "PresetValues.h"

class Player;
class Entity;
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
    Categories::Places& getPlace(int x);

    void sortEntities();
    void addEntity(Entity* e);
    void removeEntity(unsigned int index);

    void sortLights();
    void addLight(Tile* t);
    void removeLight(unsigned int index);
    void removeLight(Tile* t);
    void getRenderedLights(glm::vec4 destRect, float lights[MAX_LIGHTS_RENDERED]);

    std::vector<Entity*> getEntities() { return m_entities; }
    Player* getPlayer() { return m_player; }
    unsigned int getTime() { return m_time; }
    std::string getName() { return m_name; }
    WorldEra getEra() { return m_worldEra; }
    WorldEra getNextEra() { return m_nextWorldEra; }

    void setName(std::string name) { m_name = name; }
    void setPlayer(Player& p);
    void setTime(unsigned int time) { m_time = time; }

    void setLightsUniform(glm::vec4 destRect, GLEngine::GLSLProgram* textureProgram);

    void drawTiles(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, GLEngine::DebugRenderer& dr, glm::vec4 destRect);
    void updateTiles(glm::vec4 destRect);
    void tickTiles(glm::vec4 destRect);

    void drawEntities(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, GLEngine::DebugRenderer& dr, glm::vec4 destRect);
    void updateEntities(AudioManager* audio, float timeStep, ScriptQueue* sq);
    void tickEntities(AudioManager* audio);

    void drawParticles(GLEngine::SpriteBatch* sb);

    void drawDebug(GLEngine::DebugRenderer& dr, float xOffset);

    void incrementTime() { m_time++; }

    void setWorldEra(WorldEra newEra) { m_nextWorldEra = newEra; }

    float getDistance(glm::vec2 point0, glm::vec2 point1); // Gets the distance between two points, accounting for crossover.

private:
    World(bool quickLoad) {}

    void spawnEntities();

    Tile**** m_tiles = nullptr;
    std::vector<Tile*> m_lights; // Vector of tiles (ordered by x pos), that need to be checked for light rendering.
    std::vector<Tile*> m_deadTiles; // Vector of tiles that need to be destroyed and deleted, but may be bound to other systems.

    std::vector<Entity*> m_entities;

    Player* m_player = nullptr;

    GLEngine::ParticleEngine2D m_particle2d;

    Categories::Places m_placesMap[(WORLD_SIZE / CHUNK_SIZE)]; // Simply a 1d vector of biome IDs, which can be mapped onto the world by referencing each "chunk"'s size

    float m_time = 0.0f; // Tick time, really
    float m_sunlight = 0.0f;

    WorldEra m_worldEra = WorldEra::NEOLITHIC_ERA;
    WorldEra m_nextWorldEra = WorldEra::NEOLITHIC_ERA; // If this is different than m_worldEra, trigger WorldIOManager to advance time.

    std::string m_name;
};
