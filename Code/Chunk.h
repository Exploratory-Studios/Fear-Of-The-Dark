#pragma once

#include <vector>
#include <SpriteBatch.h>

#include "PresetValues.h"

#include "Tile.h"

class Chunk
{
    public:
        Chunk();
        Chunk(Tile tileArray[WORLD_HEIGHT][CHUNK_SIZE]);

        void init(Tile tileArray[WORLD_HEIGHT][CHUNK_SIZE]);

        void update();
        void draw(GLEngine::SpriteBatch& sb);

        void setPlace(Categories::Places place);
        void setTile(const Tile& newTile, const unsigned int& x, const unsigned int& y);

        Categories::Places      getPlace()                                                  { return m_place;       }

        Tile tiles[WORLD_HEIGHT][CHUNK_SIZE];

    private:
        Categories::Places m_place;
};
