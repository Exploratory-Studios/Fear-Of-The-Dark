#include "Chunk.h"

Chunk::Chunk() {

}

Chunk::Chunk(Tile tileArray[WORLD_HEIGHT][CHUNK_SIZE]) {
    init(tileArray);
}

void Chunk::init(Tile tileArray[WORLD_HEIGHT][CHUNK_SIZE]) {
    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            tiles[i][j] = tileArray[i][j];
        }
    }
}

void Chunk::update() {
    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            tiles[i][j].update();
        }
    }
}

void Chunk::draw(GLEngine::SpriteBatch& sb) {
    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            tiles[i][j].draw(sb);
        }
    }
}

void Chunk::setPlace(Categories::Places place) {
    m_place = place;
}

void Chunk::setTile(const Tile& newTile, const unsigned int& x, const unsigned int& y) {
    tiles[y][x] = newTile;
}
