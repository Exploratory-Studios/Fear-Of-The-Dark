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
    friend class WorldIOManager;

    public:
        Chunk();
        Chunk(AudioManager* audio);
        Chunk(std::vector<Tile*> tileArray[WORLD_HEIGHT][CHUNK_SIZE], std::vector<Tile*> extraTileArray[WORLD_HEIGHT][2], int index, Chunk* surroundingChunks[2], EntityManager* entityManager, AudioManager* audio);

        ~Chunk();

        void init(std::vector<Tile*> tileArray[WORLD_HEIGHT][CHUNK_SIZE], std::vector<Tile*> extraTileArray[WORLD_HEIGHT][2], Chunk* surroundingChunks[2], EntityManager* entityManager, AudioManager* audio);

        void update(float time, float timeStepVariable, Chunk** chunks, Player* p, bool updateEntities = true); // updateEntities is used for script pausing
        void tick(float tickTime, Player* p, WorldEra& era, bool updateEntities = true);
        void draw(GLEngine::SpriteBatch& sb, int xOffset, float time, GLEngine::Camera2D camera); // xOffset is in chunks

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

        void setAudioManager(AudioManager* audio);

        Tile* getTile(int x, int y, unsigned int layer) { // Takes x between (0) and (CHUNK_SIZE-1), inclusive
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
        //std::vector<Tile*>** m_extraTiles = new std::vector<Tile*>*[WORLD_HEIGHT]; // On each side, so that we don't have to activate 3 chunks at a time instead of one

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
