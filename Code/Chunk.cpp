#include "Chunk.h"

#include "EntityManager.h"
#include "Entity.h"
#include "Player.h"

void Chunk::addEntity(Entity ent) { m_entityManager->addEntity(ent); }

std::vector<Entity>* Chunk::getEntities() {
    return m_entityManager->getEntities();
}
#include <iostream>
Chunk::Chunk() {
    m_entityManager = new EntityManager(this, nullptr);
    std::cout << "HELLO";
}

Chunk::Chunk(AudioManager* audio) {
    m_entityManager = new EntityManager(this, audio);
}

Chunk::Chunk(Tile* tileArray[WORLD_HEIGHT][CHUNK_SIZE], Tile* extraTileArray[WORLD_HEIGHT][2], int index, Chunk* surroundingChunks[2], EntityManager* entityManager, AudioManager* audio) {
    init(tileArray, extraTileArray, surroundingChunks, entityManager, audio);
    m_index = index;
    m_entityManager = new EntityManager(this, audio);
}

Chunk::~Chunk() {
    delete m_entityManager;
}

void Chunk::init(Tile* tileArray[WORLD_HEIGHT][CHUNK_SIZE], Tile* extraTileArray[WORLD_HEIGHT][2], Chunk* surroundingChunks[2], EntityManager* entityManager, AudioManager* audio) {
	m_surroundingChunks[0] = surroundingChunks[0];
	m_surroundingChunks[1] = surroundingChunks[1];

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

void Chunk::update(float time, float timeStepVariable, Chunk* chunks[WORLD_SIZE], bool updateEntities/* = true*/) {
    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            tiles[i][j]->update(time);
        }
        extraTiles[i][0]->update(time);
        extraTiles[i][1]->update(time);
    }
    if(updateEntities)
        m_entityManager->update(timeStepVariable, chunks);
}

void Chunk::tick(float tickTime, Player* p, WorldEra& era, bool updateEntities/* = true*/) {
    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            tiles[i][j]->tick(tickTime);
        }
        extraTiles[i][0]->tick(tickTime);
        extraTiles[i][1]->tick(tickTime);
    }
    if(updateEntities)
        m_entityManager->tick(p, tickTime, era);
}

void Chunk::draw(GLEngine::SpriteBatch& sb, int xOffset, float time, GLEngine::Camera2D camera) {
    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            if(camera.isBoxInView(glm::vec2(j + CHUNK_SIZE * m_index + xOffset * CHUNK_SIZE, i), glm::vec2(1.0f, 1.0f))) {
                tiles[i][j]->draw(sb, xOffset);
                int backdrop = -1;
                while(i+backdrop > 0) {
                    if(tiles[i+backdrop][j]->isTransparent()) {
                        tiles[i][j]->drawBackdrop(sb, xOffset, backdrop, tiles[i+backdrop][j]->getLight());
                    } else {
                        break;
                    }
                    backdrop--;
                }
            }
        }
        extraTiles[i][0]->draw(sb, xOffset);
    }
    m_entityManager->draw(sb, time, xOffset);
}

void Chunk::setPlace(Categories::Places place) {
    m_place = place;
}

void Chunk::setTile(Tile* newTile, const unsigned int& x, const unsigned int& y) {
    unsigned int xPos = (x >= CHUNK_SIZE) ? (int)newTile->getPosition().x % CHUNK_SIZE : x;
    unsigned int yPos = (y >= WORLD_SIZE) ? (int)newTile->getPosition().y : x;

    delete tiles[yPos][(xPos + CHUNK_SIZE) % CHUNK_SIZE];
    tiles[yPos][(xPos + CHUNK_SIZE) % CHUNK_SIZE] = newTile;

    for(int i = y; i > 0; i--) {
        tiles[i][(xPos + CHUNK_SIZE) % CHUNK_SIZE]->setNeedsSunCheck();
        if(!tiles[i][(xPos + CHUNK_SIZE) % CHUNK_SIZE]->isTransparent()) {
            break;
        }
    }

    if((xPos % CHUNK_SIZE) <= 0) {
        m_surroundingChunks[0]->extraTiles[yPos][1] = newTile;
    }
    if((xPos % CHUNK_SIZE) >= CHUNK_SIZE-1) {
        m_surroundingChunks[1]->extraTiles[yPos][0] = newTile;
    }
}

void Chunk::setAudioManager(AudioManager* audio) {
    m_audioManager = audio;
    for(int i = 0; i < m_entityManager->getEntities()->size(); i++) {
        m_entityManager->getEntity(i)->setAudioManager(audio);
    }
}

Entity* Chunk::getEntity(unsigned int index) {
    return m_entityManager->getEntity(index);
}
