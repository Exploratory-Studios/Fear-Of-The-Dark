#pragma once

#include <vector>
#include <SpriteBatch.h>
#include <Camera2D.h>
#include <DebugRenderer.h>

#include "Scripting/ScriptQueue.h"

#include "PresetValues.h"
#include "Tile.h"
#include "Logging.h"

class EntityManager;
class Entity;
class Player;

class Chunk
{
    friend class WorldIOManager;

    public:
        Chunk(int index);

        ~Chunk();

        void init(int index);

        void update(float time, float timeStepVariable, Chunk** chunks, Player* p, bool updateEntities = true); // updateEntities is used for script pausing
        void tick(float tickTime, Player* p, WorldEra& era, bool updateEntities = true);
        void draw(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, int xOffset, float time, GLEngine::Camera2D camera, Player* p); // xOffset is in chunks

        #ifdef DEBUG
        void drawDebug(GLEngine::DebugRenderer& dr, int xOffset);
        #endif // DEBUG

        void setPlace(Categories::Places place);
        void setTile(Tile* newTile);
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
        ScriptQueue*                getScriptQueue()                                            { return m_sq; }

        Tile* getTile(int x, int y, unsigned int layer) {
            if(y < WORLD_HEIGHT && y >= 0) {
                return ySafe_getTile(x, y, layer);
            }
            return nullptr;
        }

    private:
        /*Tile* getExtraTile(unsigned int x, unsigned int y, unsigned int layer) {
            if(x < (m_index-1) * CHUNK_SIZE) {
                if(m_extraTiles[y][0].size() > layer) {
                    return m_extraTiles[y][x][layer];
                }
            } else {
                if(m_extraTiles[y][0].size() > layer) {
                    return m_extraTiles[y][x][layer];
                }
            }
            return nullptr;
        }*/
        Tile* ySafe_getTile(int x, int y, unsigned int layer) {
            if(x < m_index * CHUNK_SIZE || x >= (m_index + 1) * CHUNK_SIZE) {
                return getSurroundingTile(x, y, layer);
            }
            return m_tiles[y][(x + CHUNK_SIZE) % CHUNK_SIZE][layer];
        }
        Tile* getSurroundingTile(int x, int y, unsigned int layer) {
            if((x + CHUNK_SIZE) % CHUNK_SIZE < CHUNK_SIZE / 2) { return m_surroundingChunks[1]->ySafe_getTile((x + CHUNK_SIZE * WORLD_SIZE) % (CHUNK_SIZE * WORLD_SIZE), y, layer); }
            return m_surroundingChunks[0]->ySafe_getTile((x + CHUNK_SIZE * WORLD_SIZE) % (CHUNK_SIZE * WORLD_SIZE), y, layer);
        }

        std::vector<Tile*>** m_tiles = new std::vector<Tile*>*[WORLD_HEIGHT];

        Logger* logger = Logger::getInstance();

        EntityManager* m_entityManager = nullptr;
        ScriptQueue* m_sq = nullptr;

		Chunk* m_surroundingChunks[2] = { nullptr };

		std::vector<Tile*> m_deadTiles; // Tiles that need to be deleted, but still might be attached to something. My solution: to keep them for a full frame cycle and then delete

        Categories::Places m_place;
        int m_index;

        bool m_activated = false; // Should this update?
        bool m_draw = false; // Should this be drawn?

        bool m_dialogueStarted = false;
        bool m_dialogueActive = false;
};
