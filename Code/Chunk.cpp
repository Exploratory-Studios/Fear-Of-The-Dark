#include "Chunk.h"
#include <iostream>

#include "EntityManager.h"
#include "Entity.h"
#include "Entities/TalkingNPC.h"
#include "Player.h"

void Chunk::addEntity(Entity* ent) {
    if(m_entityManager) m_entityManager->addEntity(ent);
}
void Chunk::addTalkingEntity(TalkingNPC* ent) {
    if(m_entityManager) m_entityManager->addTalkingEntity(ent);
}

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

void Chunk::tick(float* tickTime, Player* p) {
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
    tiles[y][x] = newTile;
}
