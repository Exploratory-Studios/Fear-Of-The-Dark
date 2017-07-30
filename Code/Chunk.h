#pragma once

#define WORLD_HEIGHT 512
#define CHUNK_SIZE 16

#include <vector>
#include <SpriteBatch.h>

#include "Tile.h"
#include "Categories.h"

class Chunk
{
    public:
        Chunk();
        Chunk(Tile tileArray[WORLD_HEIGHT][CHUNK_SIZE]);

        void init(Tile tileArray[WORLD_HEIGHT][CHUNK_SIZE]);

        void update();
        void draw(GLEngine::SpriteBatch& sb);

        void setPlace(Categories::Places place);
        Categories::Places getPlace() { return m_place; }

        Tile tiles[WORLD_HEIGHT][CHUNK_SIZE];

        Categories::Places m_place;
};
