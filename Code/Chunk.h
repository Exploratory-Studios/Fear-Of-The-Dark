#pragma once

#include <vector>
#include <SpriteBatch.h>

#include "PresetValues.h"
#include "Tile.h"
#include "Logging.h"

class EntityManager;
class Entity;
class TalkingNPC;
class Player;

class Chunk
{
    public:
        Chunk();
        Chunk(Tile* tileArray[WORLD_HEIGHT][CHUNK_SIZE], Tile* extraTileArray[WORLD_HEIGHT][2], int index, Chunk* surroundingChunks[2], EntityManager* entityManager);

        ~Chunk();

        void init(Tile* tileArray[WORLD_HEIGHT][CHUNK_SIZE], Tile* extraTileArray[WORLD_HEIGHT][2], Chunk* surroundingChunks[2], EntityManager* entityManager);

        void update(float time, float timeStepVariable, Chunk* chunks[WORLD_SIZE]);
        void tick(float* tickTime, Player& p);
        void draw(GLEngine::SpriteBatch& sb, int xOffset, float time); // xOffset is in chunks

        void setPlace(Categories::Places place);
        void setTile(Tile* newTile, const unsigned int& x, const unsigned int& y);
        void setIndex(int index) { m_index = index; }
        void setSurroundingChunk(Chunk* chunk, int i) { m_surroundingChunks[i] = chunk; }

        void activateChunk() { m_activated = true; }
        void deactivateChunk() { m_activated = false; }

        void addEntity(Entity ent);
        void addTalkingEntity(TalkingNPC ent);

        Categories::Places          getPlace()                                                  { return m_place;       }
        int                         getIndex()                                                  { return m_index;       }
        Chunk**					    getSurroundingChunks()										{ return m_surroundingChunks; }
        bool                        isDialogueActive()                                          { return m_dialogueActive; }
        bool                        isDialogueStarted()                                         { return m_dialogueStarted; }
        std::vector<TalkingNPC>*    getTalkingEntities();
        std::vector<Entity>*        getEntities();
        std::vector<Entity>*        getAllEntities();

        Tile* tiles[WORLD_HEIGHT][CHUNK_SIZE] = { { nullptr } };
        Tile* extraTiles[WORLD_HEIGHT][2] = { { nullptr } }; // On each side, so that we don't have to activate 3 chunks at a time instead of one

    private:
        Logger* logger = Logger::getInstance();

        EntityManager* m_entityManager = nullptr;

		Chunk* m_surroundingChunks[2] = { nullptr };

        Categories::Places m_place;
        int m_index;

        bool m_activated = true;

        bool m_dialogueStarted = false;
        bool m_dialogueActive = false;
};
