#include "Chunk.h"

#include "EntityManager.h"
#include "Entity.h"
#include "Player.h"

void Chunk::addEntity(Entity* ent) { m_entityManager->addEntity(ent); }

void Chunk::deleteDeadTiles() {
    for(int i = 0; i < m_deadTiles.size(); i++) {
        if(m_deadTiles[i]) delete m_deadTiles[i];
    }
    m_deadTiles.clear();
}

std::vector<Entity*> Chunk::getEntities() {
    return m_entityManager->getEntities();
}

Chunk::Chunk() {
    m_entityManager = new EntityManager(this, nullptr);

    for(unsigned int i = 0; i < WORLD_HEIGHT; i++) {
        m_tiles[i] = new std::vector<Tile*>[CHUNK_SIZE];
        for(int j = 0; j < CHUNK_SIZE; j++) {
            m_tiles[i][j].push_back(nullptr);
        }
    }

    /*for(unsigned int i = 0; i < WORLD_HEIGHT; i++) {
        m_extraTiles[i] = new std::vector<Tile*>[2];
        for(int j = 0; j < 2; j++) {
            m_extraTiles[i][j].push_back(nullptr);
        }
    }*/
}

Chunk::Chunk(AudioManager* audio) {
    m_entityManager = new EntityManager(this, audio);

    for(unsigned int i = 0; i < WORLD_HEIGHT; i++) {
        m_tiles[i] = new std::vector<Tile*>[CHUNK_SIZE];
        for(int j = 0; j < CHUNK_SIZE; j++) {
            m_tiles[i][j].push_back(nullptr);
        }
    }

    /*for(unsigned int i = 0; i < WORLD_HEIGHT; i++) {
        m_extraTiles[i] = new std::vector<Tile*>[2];
        for(int j = 0; j < 2; j++) {
            m_extraTiles[i][j].push_back(nullptr);
        }
    }*/
}

Chunk::Chunk(std::vector<Tile*> tileArray[WORLD_HEIGHT][CHUNK_SIZE], std::vector<Tile*> extraTileArray[WORLD_HEIGHT][2], int index, Chunk* surroundingChunks[2], EntityManager* entityManager, AudioManager* audio) {
    init(tileArray, extraTileArray, surroundingChunks, entityManager, audio);
    m_index = index;
    m_entityManager = new EntityManager(this, audio);
}

Chunk::~Chunk() {
    delete m_entityManager;

    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            for(int k = 0; k < m_tiles[i][j].size(); k++) {
                if(m_tiles[i][j][k]) delete m_tiles[i][j][k];
            }
        }
        delete[] m_tiles[i];
    }
    delete[] m_tiles;
}

void Chunk::init(std::vector<Tile*> tileArray[WORLD_HEIGHT][CHUNK_SIZE], std::vector<Tile*> extraTileArray[WORLD_HEIGHT][2], Chunk* surroundingChunks[2], EntityManager* entityManager, AudioManager* audio) {
    for(unsigned int i = 0; i < WORLD_HEIGHT; i++) {
        m_tiles[i] = new std::vector<Tile*>[CHUNK_SIZE];
        for(int j = 0; j < CHUNK_SIZE; j++) {
            m_tiles[i][j].push_back(nullptr);
        }
    }

    /*for(unsigned int i = 0; i < WORLD_HEIGHT; i++) {
        m_extraTiles[i] = new std::vector<Tile*>[2];
        for(int j = 0; j < 2; j++) {
            m_extraTiles[i][j].push_back(nullptr);
        }
    }*/

	m_surroundingChunks[0] = surroundingChunks[0];
	m_surroundingChunks[1] = surroundingChunks[1];

    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            m_tiles[i][j].reserve(tileArray[i][j].size());
            m_tiles[i][j] = tileArray[i][j]; // make sure that there are no uninitialized tiles
        }
        /*for(int j = 0; j < 2; j++) {
            m_extraTiles[i][j].reserve(extraTileArray[i][j].size());
            m_extraTiles[i][j] = extraTileArray[i][j];
        }*/
    }
}

void Chunk::update(float time, float timeStepVariable, Chunk** chunks, Player* p, bool updateEntities/* = true*/) {
    deleteDeadTiles();

    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            for(int k = 0; k < m_tiles[i][j].size(); k++) {
                m_tiles[i][j][k]->update(time);
            }
        }
        /*for(int k = 0; k < m_extraTiles[i][0].size(); k++) {
            m_extraTiles[i][0][k]->update(time);
        }
        for(int k = 0; k < m_extraTiles[i][1].size(); k++) {
            m_extraTiles[i][1][k]->update(time);
        }*/
    }
    if(updateEntities)
        m_entityManager->update(timeStepVariable, chunks, p);
}

void Chunk::tick(float tickTime, Player* p, WorldEra& era, bool updateEntities/* = true*/) {
    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            for(int k = 0; k < m_tiles[i][j].size(); k++) {
                m_tiles[i][j][k]->tick(tickTime);
            }
        }
        /*for(int k = 0; k < m_extraTiles[i][0].size(); k++) {
            m_extraTiles[i][0][k]->tick(tickTime);
        }
        for(int k = 0; k < m_extraTiles[i][0].size(); k++) {
            m_extraTiles[i][1][k]->tick(tickTime);
        }*/
    }
    if(updateEntities)
        m_entityManager->tick(p, tickTime, era);
}

void Chunk::draw(GLEngine::SpriteBatch& sb, int xOffset, float time, GLEngine::Camera2D camera) {
    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            for(int k = 0; k < m_tiles[i][j].size(); k++) {
                if(camera.isBoxInView(glm::vec2(j + CHUNK_SIZE * m_index + xOffset * CHUNK_SIZE, i), glm::vec2(1.0f, 1.0f))) {
                    m_tiles[i][j][k]->draw(sb, xOffset);
                    int backdrop = -1;
                    while(i+backdrop > 0) {
                        if(m_tiles[i+backdrop][j][k]->isTransparent()) {
                            m_tiles[i][j][k]->drawBackdrop(sb, xOffset, backdrop, m_tiles[i+backdrop][j][k]->getLight());
                        } else {
                            break;
                        }
                        backdrop--;
                    }
                }
            }
        }
        /*for(int k = 0; k < m_extraTiles[i][0].size(); k++) {
            m_extraTiles[i][0][k]->draw(sb, xOffset);
        }
        for(int k = 0; k < m_extraTiles[i][1].size(); k++) {
            m_extraTiles[i][1][k]->draw(sb, xOffset);
        }*/
    }
    m_entityManager->draw(sb, time, xOffset);
}

#ifdef DEBUG
void Chunk::drawDebug(GLEngine::DebugRenderer& dr, int xOffset) {
    m_entityManager->drawDebug(dr, xOffset);
}
#endif // DEBUG

void Chunk::setPlace(Categories::Places place) {
    m_place = place;
}

void Chunk::setTile(Tile* newTile, unsigned int layer) {
    unsigned int xPos = (int)newTile->getPosition().x % CHUNK_SIZE;
    unsigned int yPos = (int)newTile->getPosition().y;

    m_deadTiles.push_back(getTile(xPos, yPos, layer));

    if(m_tiles[yPos][xPos].size() > layer) {
        m_tiles[yPos][xPos][layer] = newTile;
    } else {
        for(int i = m_tiles[yPos][xPos].size(); i < layer; i++) {
            m_tiles[yPos][xPos].push_back(nullptr);
        }
        m_tiles[yPos][xPos][layer] = newTile;
    }

    for(int i = yPos; i > 0; i--) {
        if(!getTile(xPos, i, layer)) break;
        getTile(xPos, i, layer)->setNeedsSunCheck();
        if(!getTile(xPos, i, layer)->isTransparent()) {
            break;
        }
    }

    /*if((xPos % CHUNK_SIZE) <= 0) {
        if(m_surroundingChunks[0]->m_extraTiles[yPos][1].size() > layer) {
            m_surroundingChunks[0]->m_extraTiles[yPos][1][layer] = newTile;
        } else {
            for(int i = m_surroundingChunks[0]->m_extraTiles[yPos][1].size(); i < layer; i++) {
                m_surroundingChunks[0]->m_extraTiles[yPos][1].push_back(nullptr);
            }
            m_surroundingChunks[0]->m_extraTiles[yPos][1][layer] = newTile;
        }
    }
    if((xPos % CHUNK_SIZE) >= CHUNK_SIZE-1) {
        if(m_surroundingChunks[1]->m_extraTiles[yPos][0].size() > layer) {
            m_surroundingChunks[1]->m_extraTiles[yPos][0][layer] = newTile;
        } else {
            for(int i = m_surroundingChunks[1]->m_extraTiles[yPos][0].size(); i < layer; i++) {
                m_surroundingChunks[1]->m_extraTiles[yPos][0].push_back(nullptr);
            }
            m_surroundingChunks[1]->m_extraTiles[yPos][0][layer] = newTile;
        }
    }*/
}

void Chunk::setAudioManager(AudioManager* audio) {
    m_audioManager = audio;
    for(int i = 0; i < m_entityManager->getEntities().size(); i++) {
        m_entityManager->getEntities()[i]->setAudioManager(audio);
    }
}

void Chunk::removeEntity(unsigned int index) {
    m_entityManager->removeEntity(index);
}

Entity* Chunk::getEntity(unsigned int index) {
    return m_entityManager->getEntities()[index];
}
