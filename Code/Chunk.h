#pragma once

#include <vector>
#include <SpriteBatch.h>
#include <Camera2D.h>
#include <DebugRenderer.h>

#include "PresetValues.h"
#include "Tile.h"
#include "Logging.h"

class EntityManager;
class Entity;
class Player;

class Chunk
{
    public:
        Chunk();
        Chunk(AudioManager* audio);
        Chunk(Tile* tileArray[WORLD_HEIGHT][CHUNK_SIZE], Tile* extraTileArray[WORLD_HEIGHT][2], int index, Chunk* surroundingChunks[2], EntityManager* entityManager, AudioManager* audio);

        ~Chunk();

        void init(Tile* tileArray[WORLD_HEIGHT][CHUNK_SIZE], Tile* extraTileArray[WORLD_HEIGHT][2], Chunk* surroundingChunks[2], EntityManager* entityManager, AudioManager* audio);

        void update(float time, float timeStepVariable, Chunk** chunks, Player* p, bool updateEntities = true); // updateEntities is used for script pausing
        void tick(float tickTime, Player* p, WorldEra& era, bool updateEntities = true);
        void draw(GLEngine::SpriteBatch& sb, int xOffset, float time, GLEngine::Camera2D camera); // xOffset is in chunks

        #ifdef DEBUG
        void drawDebug(GLEngine::DebugRenderer& dr, int xOffset);
        #endif // DEBUG

        void setPlace(Categories::Places place);
        void setTile(Tile* newTile, const unsigned int& x = CHUNK_SIZE, const unsigned int& y = WORLD_SIZE);
        void setIndex(int index) { m_index = index; }
        void setSurroundingChunk(Chunk* chunk, int i) { m_surroundingChunks[i] = chunk; }

        void activateChunk() { m_activated = true; }
        void deactivateChunk() { m_activated = false; }

        void drawChunk() { m_draw = true; }
        void stopdrawingChunk() { m_draw = false; }

        void deleteDeadTiles();

        void addEntity(Entity* ent);
        void removeEntity(unsigned int index);
        Entity* getEntity(unsigned int index);

        Categories::Places          getPlace()                                                  { return m_place;       }
        int                         getIndex()                                                  { return m_index;       }
        Chunk**					    getSurroundingChunks()										{ return m_surroundingChunks; }
        bool                        isDialogueActive()                                          { return m_dialogueActive; }
        bool                        isDialogueStarted()                                         { return m_dialogueStarted; }
        std::vector<Entity*>        getEntities();

        void setAudioManager(AudioManager* audio);

        Tile*** tiles = new Tile**[WORLD_HEIGHT];
        Tile*** extraTiles = new Tile**[WORLD_HEIGHT]; // On each side, so that we don't have to activate 3 chunks at a time instead of one

    private:
        Logger* logger = Logger::getInstance();

        EntityManager* m_entityManager = nullptr;

		Chunk* m_surroundingChunks[2] = { nullptr };

		std::vector<Tile*> m_deadTiles; // Tiles that need to be deleted, but still might be attached to something. My solution: to keep them for a full frame cycle and then delete

        Categories::Places m_place;
        int m_index;

        bool m_activated = false; // Should this update?
        bool m_draw = false; // Should this be drawn?

        AudioManager* m_audioManager = nullptr;

        bool m_dialogueStarted = false;
        bool m_dialogueActive = false;
};
