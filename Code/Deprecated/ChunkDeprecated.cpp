#include "Chunk.h"

#include "EntityManager.h"
#include "Entity.h"
#include "Player.h"

void Chunk::addEntity(Entity* ent) { m_entityManager->addEntity(ent); }

void Chunk::deleteDeadTiles() {
    for(int i = 0; i < m_deadTiles.size(); i++) {
        if(m_deadTiles[i]) {
            m_deadTiles[i]->destroy();
            delete m_deadTiles[i];
        }
    }
    m_deadTiles.clear();
}

std::vector<Entity*> Chunk::getEntities() {
    return m_entityManager->getEntities();
}

Chunk::Chunk(int index) {
    init(index);
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

void Chunk::init(int index) {
    m_index = index;
}

void Chunk::update(float time, float timeStepVariable, Chunk** chunks, Player* p, bool updateEntities/* = true*/) {
    deleteDeadTiles();

    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            for(int k = 0; k < m_tiles[i][j].size(); k++) {
                m_tiles[i][j][k]->update(time, true);
            }
        }
    }
    /*for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            for(int k = 0; k < m_tiles[i][j].size(); k++) {
                m_tiles[i][j][k]->finishFrame();
            }
        }
    }*/
    if(updateEntities)
        m_entityManager->update(timeStepVariable, chunks, p);
}

void Chunk::tick(float tickTime, Player* p, WorldEra& era, bool updateEntities/* = true*/) {
    float sunLight = std::cos(tickTime / (DAY_LENGTH / 6.28318f)) / 2.0f + 0.5f;
    for(int i = WORLD_HEIGHT-1; i >= 0; i--) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            for(int k = 0; k < m_tiles[i][j].size(); k++) {
                m_tiles[i][j][k]->tick(tickTime, sunLight);
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

void Chunk::draw(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, int xOffset, float time, GLEngine::Camera2D camera, Player* p) {

    int playerLayer = p->getLayer();
    int diff[WORLD_DEPTH];
    for(int i = 0; i < WORLD_DEPTH; i++) {
        diff[i] = playerLayer - i;
    }

    int total = 0;

    for(int i = 0; i < WORLD_HEIGHT; i++) {
        for(int j = 0; j < CHUNK_SIZE; j++) {
            for(int k = 0; k < m_tiles[i][j].size(); k++) {
                if(camera.isBoxInView(glm::vec2(j + CHUNK_SIZE * m_index + xOffset * CHUNK_SIZE, i), glm::vec2(1.0f, 1.0f))) {
                    m_tiles[i][j][k]->draw(sb, sf, xOffset, diff[k]);
                    total++;
                    if(diff[k] <= 0 && m_tiles[i][j][k]->doDraw() && !m_tiles[i][j][k]->isTransparent()) {
                        break;
                    }
                    /*if(k == WORLD_DEPTH-1) {
                        int backdrop = -1;
                        while(i+backdrop > 0) {
                            if(m_tiles[i+backdrop][j][k]->isTransparent()) {
                                m_tiles[i][j][k]->drawBackdrop(sb, xOffset, backdrop, m_tiles[i+backdrop][j][k]->getLight());
                            } else {
                                break;
                            }
                            backdrop--;
                        }
                    }*/
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

void Chunk::setTile(Tile* newTile) {
    unsigned int xPos = (int)newTile->getPosition().x % CHUNK_SIZE;
    unsigned int yPos = (int)newTile->getPosition().y;

    Tile* oldTile = ySafe_getTile(newTile->getPosition().x, yPos, newTile->getLayer());

    if(oldTile) m_deadTiles.push_back(oldTile);

    m_tiles[yPos][xPos][newTile->getLayer()] = newTile;

    if(!newTile->isTransparent()) {
        for(int i = yPos-1; i >= 0; i--) {
            Tile* t = ySafe_getTile(newTile->getPosition().x, i, newTile->getLayer());
            if(!t) {
                break;
            }
            t->setNeedsSunCheck();
            if(!t->isTransparent()) {
                break;
            }
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
