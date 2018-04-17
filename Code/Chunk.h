#pragma once

#include <vector>
#include <SpriteBatch.h>

#include "PresetValues.h"
#include "Tile.h"

class Chunk
{
    public:
        Chunk();
        Chunk(Tile* tileArray[WORLD_HEIGHT][CHUNK_SIZE], int index);

        void init(Tile* tileArray[WORLD_HEIGHT][CHUNK_SIZE]);

        void update(float time);
        void tick(int tickTime);
        void draw(GLEngine::SpriteBatch& sb);

        void setPlace(Categories::Places place);
        void setTile(Tile* newTile, const unsigned int& x, const unsigned int& y);
        void setIndex(int index) { m_index = index; }

        Categories::Places      getPlace()                                                  { return m_place;       }
        int                     getIndex()                                                  { return m_index;       }

        Tile* tiles[WORLD_HEIGHT][CHUNK_SIZE] = { { nullptr } };

    private:
        Categories::Places m_place;
        int m_index;
};
