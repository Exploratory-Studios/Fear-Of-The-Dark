#pragma once

#include <vector>
#include <SpriteBatch.h>

#include "PresetValues.h"
#include "Tile.h"

class Chunk
{
    public:
        Chunk();
        Chunk(Tile* tileArray[WORLD_HEIGHT][CHUNK_SIZE], Tile* extraTileArray[WORLD_HEIGHT][2], int index);

        void init(Tile* tileArray[WORLD_HEIGHT][CHUNK_SIZE], Tile* extraTileArray[WORLD_HEIGHT][2]);

        void update(float time);
        void tick(int tickTime);
        void draw(GLEngine::SpriteBatch& sb, int xOffset); // xOffset is in chunks

        void setPlace(Categories::Places place);
        void setTile(Tile* newTile, const unsigned int& x, const unsigned int& y);
        void setIndex(int index) { m_index = index; }

        Categories::Places      getPlace()                                                  { return m_place;       }
        int                     getIndex()                                                  { return m_index;       }

        Tile* tiles[WORLD_HEIGHT][CHUNK_SIZE] = { { nullptr } };
        Tile* extraTiles[WORLD_HEIGHT][2] = { { nullptr } }; // On each side, so that we don't have to activate 3 chunks at a time instead of one

    private:
        Categories::Places m_place;
        int m_index;
};
