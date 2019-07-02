#include "Chunk.h"

#include "EntityManager.h"
#include "Entity.h"
#include "Player.h"

void Chunk::addEntity(Entity* ent) { m_entityManager->addEntity(ent); }

std::vector<Entity*> Chunk::getEntities() {
    return m_entityManager->getEntities();
}

Chunk::Chunk() {
    m_entityManager = new EntityManager(this);
}

Chunk::Chunk(Tile* tileArray[WORLD_HEIGHT][CHUNK_SIZE], Tile* extraTileArray[WORLD_HEIGHT][2], int index, Chunk* surroundingChunks[2], EntityManager* entityManager) {
    init(tileArray, extraTileArray, surroundingChunks, entityManager);
    m_index = index;
}

Chunk::~Chunk() {
    delete m_entityManager;
}

void Chunk::init(Tile* tileArray[WORLD_HEIGHT][CHUNK_SIZE], Tile* extraTileArray[WORLD_HEIGHT][2], Chunk* surroundingChunks[2], EntityManager* entityManager) {
	m_surroundingChunks[0] = surroundingChunks[0];
	m_surroundingChunks[1] = surroundingChunks[1];

	if(entityManager) {
        m_entityManager = entityManager;
    } else {
        m_entityManager = new EntityManager(this);
    }

    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            if(tiles[i][j]) {
                tiles[i][j] = tileArray[i][j]; // make sure that there are no uninitialized tiles
            } else {
                logger->log("UNINITIALIZED BLOCK!", true);
            }
        }
        for(int j = 0; j < 2; j++) {
            if(extraTiles[i][j]) {
                extraTiles[i][j] = extraTileArray[i][j];
            } else {
                logger->log("UNINITIALIZED EXTRA BLOCK!", true);
            }
        }
    }
}

void Chunk::update(float time, float timeStepVariable, Chunk* chunks[WORLD_SIZE]) {
    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            tiles[i][j]->update(time);
        }
        extraTiles[i][0]->update(time);
        extraTiles[i][1]->update(time);
    }
    m_entityManager->update(timeStepVariable, chunks);
}

void Chunk::tick(float tickTime, Player* p) {
    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            tiles[i][j]->tick(tickTime);
        }
        extraTiles[i][0]->tick(tickTime);
        extraTiles[i][1]->tick(tickTime);
    }
    m_entityManager->tick(p);
}

void Chunk::draw(GLEngine::SpriteBatch& sb, int xOffset, float time) {
    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            tiles[i][j]->draw(sb, xOffset);
        }
        extraTiles[i][0]->draw(sb, xOffset);
    }
    m_entityManager->draw(sb, time, xOffset);
}

void Chunk::setPlace(Categories::Places place) {
    m_place = place;
}

void Chunk::setTile(Tile* newTile, const unsigned int& x, const unsigned int& y) {
    delete tiles[y][(x + CHUNK_SIZE) % CHUNK_SIZE];
    tiles[y][(x + CHUNK_SIZE) % CHUNK_SIZE] = newTile;

    if((x + CHUNK_SIZE) % CHUNK_SIZE == 0) m_surroundingChunks[0]->extraTiles[y][1] = newTile;
    if((x + CHUNK_SIZE) % CHUNK_SIZE == CHUNK_SIZE-1) m_surroundingChunks[1]->extraTiles[y][0] = newTile;
}
