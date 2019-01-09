#include "Chunk.h"
#include <iostream>

#include "EntityManager.h"

Chunk::Chunk() {

}

Chunk::Chunk(Tile* tileArray[WORLD_HEIGHT][CHUNK_SIZE], Tile* extraTileArray[WORLD_HEIGHT][2], int index, Chunk* surroundingChunks[2]) {
    init(tileArray, extraTileArray, surroundingChunks);
    m_index = index;
}

void Chunk::init(Tile* tileArray[WORLD_HEIGHT][CHUNK_SIZE], Tile* extraTileArray[WORLD_HEIGHT][2], Chunk* surroundingChunks[2]) {
	m_surroundingChunks[0] = surroundingChunks[0];
	m_surroundingChunks[1] = surroundingChunks[1];

    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            if(tiles[i][j]) {
                tiles[i][j] = tileArray[i][j]; // make sure that there are no uninitialized tiles
            } else {
                std::cout << "UNINITIALIZED BLOCK!\n";
            }
        }
        for(int j = 0; j < 2; j++) {
            if(extraTiles[i][j]) {
                extraTiles[i][j] = extraTileArray[i][j];
            } else {
                std::cout << "UNINITIALIZED EXTRA BLOCK!\n";
            }
        }
    }
}

void Chunk::update(float time, float timeStepVariable) {
    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            tiles[i][j]->update(time);
        }
        extraTiles[i][0]->update(time);
        extraTiles[i][1]->update(time);
    }
    m_entityManager->update(timeStepVariable);
}

void Chunk::tick(float* tickTime) {
    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            tiles[i][j]->tick(tickTime);
        }
        extraTiles[i][0]->tick(tickTime);
        extraTiles[i][1]->tick(tickTime);
    }
    m_entityManager->tick();
}

void Chunk::draw(GLEngine::SpriteBatch& sb, int xOffset, float time) {
    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            tiles[i][j]->draw(sb, xOffset);
        }
        extraTiles[i][0]->draw(sb, xOffset);
    }
    m_entityManager->draw(sb, time);
}

void Chunk::setPlace(Categories::Places place) {
    m_place = place;
}

void Chunk::setTile(Tile* newTile, const unsigned int& x, const unsigned int& y) {
    tiles[y][x] = newTile;
}
