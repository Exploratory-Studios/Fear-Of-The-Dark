#include "Chunk.h"
#include <iostream>
Chunk::Chunk() {

}

Chunk::Chunk(Tile* tileArray[WORLD_HEIGHT][CHUNK_SIZE], Tile* extraTileArray[WORLD_HEIGHT][2], int index) {
    init(tileArray, extraTileArray);
    m_index = index;
}

void Chunk::init(Tile* tileArray[WORLD_HEIGHT][CHUNK_SIZE], Tile* extraTileArray[WORLD_HEIGHT][2]) {
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

void Chunk::update(float time) {
    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            tiles[i][j]->update(time);
        }
        extraTiles[i][0]->update(time);
        extraTiles[i][1]->update(time);
    }
}

void Chunk::tick(int tickTime) {
    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            tiles[i][j]->tick(tickTime);
        }
        extraTiles[i][0]->tick(tickTime);
        extraTiles[i][1]->tick(tickTime);
    }
}

void Chunk::draw(GLEngine::SpriteBatch& sb, int xOffset) {
    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            tiles[i][j]->draw(sb, xOffset);
        }
        extraTiles[i][0]->draw(sb, xOffset);
    }
}

void Chunk::setPlace(Categories::Places place) {
    m_place = place;
}

void Chunk::setTile(Tile* newTile, const unsigned int& x, const unsigned int& y) {
    tiles[y][x] = newTile;
}
