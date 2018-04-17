#include "Chunk.h"
#include <iostream>
Chunk::Chunk() {

}

Chunk::Chunk(Tile* tileArray[WORLD_HEIGHT][CHUNK_SIZE], int index) {
    init(tileArray);
    m_index = index;
}

void Chunk::init(Tile* tileArray[WORLD_HEIGHT][CHUNK_SIZE]) {
    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            if(tiles[i][j]) {
                tiles[i][j] = tileArray[i][j]; // make sure that there are no uninitialized tiles
            } else {
                std::cout << "UNINITIALIZED BLOCK!\n";
            }
        }
    }
}

void Chunk::update(float time) {
    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            tiles[i][j]->update(time);
        }
    }
}

void Chunk::tick(int tickTime) {
    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            tiles[i][j]->tick(tickTime);
        }
    }
}

void Chunk::draw(GLEngine::SpriteBatch& sb) {
    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            tiles[i][j]->draw(sb);
        }
    }
}

void Chunk::setPlace(Categories::Places place) {
    m_place = place;
}

void Chunk::setTile(Tile* newTile, const unsigned int& x, const unsigned int& y) {
    tiles[y][x] = newTile;
}
