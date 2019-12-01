#include "World.h"

#include <cstdlib>
#include "Tile.h"
#include "Entities.h"
#include "Player.h"

World::World() {
    m_tiles = (Tile****)malloc(WORLD_HEIGHT * WORLD_SIZE * WORLD_DEPTH * sizeof(Tile));
    for(int y = 0; y < WORLD_HEIGHT; y++) {
        m_tiles[y] = (Tile***)malloc(WORLD_SIZE * WORLD_DEPTH * sizeof(Tile));
        for(int x = 0; x < WORLD_SIZE; x++) {
            m_tiles[y][x] = (Tile**)malloc( WORLD_DEPTH * sizeof(Tile));
            for(int z = 0; z < WORLD_DEPTH; z++) {
                m_tiles[y][x][z] = new BlockAir(glm::vec2(x, y), z, MetaData(), false);
            }
        }
    }

    m_entities.clear();
}

World::~World() {
    free(m_tiles);
}

void World::setTile(Tile* tile) {
    unsigned int x, y, layer;
    x = tile->getPosition().x;
    y = tile->getPosition().y;
    layer = tile->getLayer();

    m_tiles[y][x][layer]->destroy(this); // Make sure everything gets cleaned up nicely.
    tile->setAmbientLight(m_tiles[y][x][layer]->getAmbientLight());

    m_tiles[y][x][layer] = tile;

    for(int i = y-1; i >= 0; i--) {
        Tile* t = m_tiles[i][x][layer];
        t->setNeedsSunCheck();
        if(!t->isTransparent()) {
            break;
        } else {
            Tile* l = getTile(x-1, i, layer);
            if(l) l->setNeedsSunCheck();
            Tile* r = getTile(x+1, i, layer);
            if(r) r->setNeedsSunCheck();
            Tile* b = getTile(x, i, layer+1);
            if(b) b->setNeedsSunCheck();
            Tile* f = getTile(x, i, layer-1);
            if(f) f->setNeedsSunCheck();
        }
    }
}

Tile* World::getTile(int x, int y, int layer) {
    if(y < WORLD_HEIGHT && y >= 0) {
        if(layer >= 0 && layer < WORLD_DEPTH) {
            return m_tiles[y][(x + WORLD_SIZE) % WORLD_SIZE][layer];
        }
    }
    return nullptr;
}

Categories::Places& World::getPlace(int x) {
    return m_placesMap[(unsigned int)(x / CHUNK_SIZE)];
}

void World::sortEntities() {
    bool changed = true;
    while(changed) {
        changed = false;
        for(unsigned int i = 0; i < m_entities.size()-1; i++) {
            if(m_entities[i]->getPosition().x > m_entities[i+1]->getPosition().x) {
                Entity* temp = m_entities[i];
                m_entities[i] = m_entities[i+1];
                m_entities[i+1] = temp;
                changed = true;
            }
        }
    }
}

void World::addEntity(Entity* e) {
    /** ADDS ENTITY TO VECTOR, MAKES SURE IT'S SORTED */

    /*
        Push back the entity to the end.
        Check entity with index 1 less. If their x pos is more than e, swap 'em!
        Repeat until the entity with a lower index's x pos is less than e's
    */

    m_entities.push_back(e);
    for(int i = m_entities.size()-1; i > 0; i--) {
        if(m_entities[i]->getPosition().x > e->getPosition().x) {
            Entity* temp = m_entities[i];
            m_entities[i] = e;
            m_entities[i+1] = temp;
        } else {
            return;
        }
    }
}

void World::removeEntity(unsigned int index) {
    /** REMOVES AND DELETES ENTITY, COMPLETELY HANDLING MEMORY MANAGEMENT
    */

    delete m_entities[index];

    for(unsigned int i = index; i < m_entities.size()-1; i++) {
        m_entities[i] = m_entities[i+1];
    }

    m_entities.pop_back();
}

void World::setPlayer(Player& p) {
    /// Copies p, creates/updates a pointer to the world-stored player.
    if(!m_player) {
        m_player = new Player(p);
        m_entities.push_back(m_player);
    } else {
        *m_player = p;
    }
}

void World::drawTiles(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, GLEngine::DebugRenderer& dr, glm::vec4 destRect) {
    /**
        Draws an area of tiles at position destRect.xy, with width and height of destRect.z and destRect.w respectively.
        Negative coordinates are mapped to accomodate for 'crossover'
        eg. destRect = (-10, 10, 20, 10) will draw tiles from x=(WORLD_SIZE - 10) to x=(-10 + 20) and y=(10) to y=(10 + 10)
    */

    int playerLayer = m_player->getLayer();
    int diff[WORLD_DEPTH];
    for(int i = 0; i < WORLD_DEPTH; i++) {
        diff[i] = playerLayer - i;
    }

    for(int y = destRect.y; y < destRect.w + destRect.y; y++) {
        for(int x = destRect.x; x < destRect.z + destRect.x; x++) {
            int columnIndex = (int)((x/*+destRect.x*/) + (WORLD_SIZE)) % WORLD_SIZE;
            int offset = (/*destRect.x + */x) - columnIndex;
            for(unsigned int layer = 0; layer < WORLD_DEPTH; layer++) {
                if(y >= 0) {
                    if(y < WORLD_HEIGHT) {
                        m_tiles[y][columnIndex][layer]->draw(sb, sf, offset, diff[layer]);
                        if(diff[layer] <= 0 && m_tiles[y][columnIndex][layer]->doDraw() && !m_tiles[y][columnIndex][layer]->isTransparent()) {
                            break;
                        }
                    }
                }
            }
        }
    }

    sb.renderBatch();
}

void World::updateTiles(glm::vec4 destRect) {
    /**
        Simply updates an area of tiles at position destRect.xy, with width and height of destRect.z and destRect.w respectively.
        Negative coordinates are mapped to accomodate for 'crossover'
        eg. destRect = (-10, 10, 20, 10) will update tiles from x=(WORLD_SIZE - 10) to x=(-10 + 20) and y=(10) to y=(10 + 10)
    */

    /*for(int i = 0; i < m_deadTiles.size(); i++) {
        m_deadTiles[i]->destroy(this);
        delete m_deadTiles[i];
    }
    m_deadTiles.clear();*/ // This breaks stuff apparently

    for(int y = destRect.y; y < destRect.w + destRect.y; y++) {
        for(int x = destRect.x; x < destRect.z + destRect.x; x++) {
            for(unsigned int layer = 0; layer < WORLD_DEPTH; layer++) {
                if(y >= 0 && y < WORLD_HEIGHT) {
                    (m_tiles[y][(x + WORLD_SIZE) % WORLD_SIZE][layer])->update(this, m_time, true);
                }
            }
        }
    }
}

void World::tickTiles(glm::vec4 destRect) {
    /**
        Simply ticks an area of tiles at position destRect.xy, with width and height of destRect.z and destRect.w respectively.
        Negative coordinates are mapped to accomodate for 'crossover'
        eg. destRect = (-10, 10, 20, 10) will tick tiles from x=(WORLD_SIZE - 10) to x=(-10 + 20) and y=(10) to y=(10 + 10)
    */

    const float sunlight = std::cos(m_time / (DAY_LENGTH / 6.28318f)) / 2.0f + 0.5f;

    for(int y = destRect.y; y < destRect.w + destRect.y; y++) {
        for(int x = destRect.x; x < destRect.z + destRect.x; x++) {
            for(unsigned int layer = 0; layer < WORLD_DEPTH; layer++) {
                if(y >= 0 && y < WORLD_HEIGHT) {
                    (m_tiles[y][(x + WORLD_SIZE) % WORLD_SIZE][layer])->tick(this, m_time, sunlight);
                }
            }
        }
    }
}


void World::drawEntities(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, GLEngine::DebugRenderer& dr, glm::vec4 destRect) {
    /**
        Draws an area of tiles at position destRect.xy, with width and height of destRect.z and destRect.w respectively.
        Negative coordinates are mapped to accomodate for 'crossover'
        eg. destRect = (-10, 10, 20, 10) will draw tiles from x=(WORLD_SIZE - 10) to x=(-10 + 20) and y=(10) to y=(10 + 10)
    */

    int playerLayer = m_player->getLayer();
    int diff[WORLD_DEPTH];
    for(int i = 0; i < WORLD_DEPTH; i++) {
        diff[i] = playerLayer - i;
    }

    for(unsigned int i = 0; i < m_entities.size(); i++) {
        m_entities[i]->draw(sb, m_time, diff[m_entities[i]->getLayer()], 0.0f); /// TODO: Finish these entity functions up and improve!
    }

    sb.renderBatch();
}

void World::drawDebug(GLEngine::DebugRenderer& dr, float xOffset) {
    for(unsigned int i = 0; i < m_entities.size(); i++) {
        m_entities[i]->debugDraw(dr, 0.0f); /// TODO: Finish these entity functions up and improve!
    }
    dr.end();
}

void World::updateEntities(AudioManager* audio, float timeStep) {
    /**
        Simply updates an area of tiles at position destRect.xy, with width and height of destRect.z and destRect.w respectively.
        Negative coordinates are mapped to accomodate for 'crossover'
        eg. destRect = (-10, 10, 20, 10) will update tiles from x=(WORLD_SIZE - 10) to x=(-10 + 20) and y=(10) to y=(10 + 10)
    */

    for(unsigned int i = 0; i < m_entities.size(); i++) {
        m_entities[i]->update(this, audio, timeStep);
        m_entities[i]->collide(this, i);
        m_entities[i]->setAITarget(this, i);
    }
}

void World::tickEntities(AudioManager* audio) {
    /**
        Simply ticks an area of tiles at position destRect.xy, with width and height of destRect.z and destRect.w respectively.
        Negative coordinates are mapped to accomodate for 'crossover'
        eg. destRect = (-10, 10, 20, 10) will tick tiles from x=(WORLD_SIZE - 10) to x=(-10 + 20) and y=(10) to y=(10 + 10)
    */

    //spawnEntities();

    for(unsigned int i = 0; i < m_entities.size(); i++) {
        m_entities[i]->tick(this, audio); /// TODO: Improve it.
    }
}

float World::getDistance(glm::vec2 point0, glm::vec2 point1) {
    float xDist;

    if(point0.x < point1.x) {
        float xDist0 = point1.x - point0.x; // Regular dist not accounting for crossover.
        float xDist1 = (point0.x) + (WORLD_SIZE - point1.x); // Distance accounting for crossover.

        xDist = xDist0 < xDist1 ? xDist0 : xDist1; // Choose the shorter of the two.
    } else if(point0.x > point1.x) {
        float xDist0 = point0.x - point1.x; // Regular dist not accounting for crossover.
        float xDist1 = (point1.x) + (WORLD_SIZE - point0.x); // Distance accounting for crossover.

        xDist = xDist0 < xDist1 ? xDist0 : xDist1; // Choose the shorter of the two.
    }

    float yDist = std::abs(point0.y - point1.y);

    return std::sqrt(xDist*xDist + yDist*yDist);
}


/// PRIVATE FUNCTIONS BELOW

void World::spawnEntities() {
    /**
        Should, for an arbritrary amount,
        1. Select an x value near the player
        2. Select a random y and z value that allows for 3*2 space (at least) and a floor
        3. Select an entity that would fit all the requirements (light level, weather, etc.)
        4. Spawn it!
    */

    /// 1.
    std::vector<glm::vec3> positions;

    for(int x = 0; x < 1; x++) {
        positions.push_back(glm::vec3(m_player->getPosition().x + (std::rand() % ENTITY_SPAWN_RANGE), 0.0f, -1.0f));
    }

    /// 2.
    for(int i = 0; i < positions.size(); i++) {
        bool foundPosition = false;
        for(int y = 0; !foundPosition && y < WORLD_HEIGHT-2; y++) {
            bool checkNext = false;
            for(int z = 0; !checkNext && z < WORLD_DEPTH; z++) {
                unsigned int x = ((int)positions[i].x + WORLD_SIZE) % WORLD_SIZE;
                for(unsigned int y1 = 0; !checkNext && y1 < 2; y1++) {
                    for(int x1 = -1; !checkNext && x1 < 2; x1++) {
                        if(!getTile((x + x1 + WORLD_SIZE) % WORLD_SIZE, y + y1, z)) {
                            checkNext = true;
                        } else if(getTile((x + x1 + WORLD_SIZE) % WORLD_SIZE, y + y1, z)->isSolid()) {
                            checkNext = true;
                        }
                    }
                }
                for(int x1 = -1; !checkNext && x1 < 2; x1++) {
                    if(!getTile((x + x1 + WORLD_SIZE) % WORLD_SIZE, y-1, z)) {
                        checkNext = true;
                    } else if(!getTile((x + x1 + WORLD_SIZE) % WORLD_SIZE, y-1, z)->isSolid()) {
                        checkNext = true;
                    }
                }
                if(!checkNext) {
                    positions[i].y = y;
                    positions[i].z = z;
                    foundPosition = true;
                }
            }
        }
        if(!foundPosition) {
            if(positions.size() > 1) {
                for(int k = i; k < positions.size()-1; k++) {
                    positions[k] = positions[k+1];
                }
            }
            positions.pop_back();
        }

    }

    for(int i = 0; i < positions.size(); i++) {
        addEntity(createEntity((unsigned int)Categories::EntityIDs::MOB_NEUTRAL_COMPANIONCUBE, glm::vec2(positions[i].x, positions[i].y), positions[i].z));
    }
}









