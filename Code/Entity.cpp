#include "Entity.h"

#include <DebugRenderer.h>
#include <math.h>

Entity::Entity()
{
    //ctor
}

Entity::Entity(glm::vec2 position, Categories::Entity_Type type, unsigned int id) {
    init(position, type, id);
}

Entity::~Entity()
{
    //dtor
}

void Entity::init(glm::vec2 position, Categories::Entity_Type type, unsigned int id) {
    m_position = position;
    switch(type) {
        case Categories::Entity_Type::MOB: /// Temporary, change to abstract class
        {
            if(Category_Data::mobData[id].id == id) {
                m_texture = GLEngine::ResourceManager::getTexture(Category_Data::mobData[id].texturePath);
                m_size = Category_Data::mobData[id].size;
                m_faction = Category_Data::mobData[id].faction;
            } else {
                return;
            }
        }
    }
}

void Entity::update(Chunk* chunks[WORLD_SIZE], float timeStep) {
    setParentChunk(chunks);
    updateLightLevel(m_parentChunk);
}

void Entity::draw(GLEngine::SpriteBatch& sb) {
    glm::vec4 destRect = glm::vec4(m_position.x, m_position.y, m_size.x * TILE_SIZE, m_size.y * TILE_SIZE);
    glm::vec4 uvRect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    GLEngine::ColourRGBA8 colour(255 * m_light, 255 * m_light, 255 * m_light, 255);

    sb.draw(destRect, uvRect, m_texture.id, 0.0f, colour);
}

void Entity::move(float timeStepVariable) {
    if(m_velocity.x > MAX_SPEED) m_velocity.x = MAX_SPEED;
    if(m_velocity.x < -MAX_SPEED) m_velocity.x = -MAX_SPEED;
    if(m_velocity.y > MAX_SPEED) m_velocity.y = MAX_SPEED;
    if(m_velocity.y < -MAX_SPEED) m_velocity.y = -MAX_SPEED;

    if(!m_onGround) {
        m_velocity.y -= 0.098f;
    } else if(m_velocity.y < 0.0f) {
        m_velocity.y = 0.0f;
    }
    m_position += m_velocity;
    m_onGround = false;
}

void Entity::collide(std::vector<Entity*> entities, int chunkI) {

    if(chunkI == m_parentChunkIndex) {
        /// ENTITY COLLISION STARTS HERE
        for(auto e : entities) {
            if(e != this) {
                if(m_position.x < e->getPosition().x + e->getSize().x * TILE_SIZE && m_position.x + m_size.x * TILE_SIZE > e->getPosition().x) {
                    if(m_position.y < e->getPosition().y + e->getSize().y * TILE_SIZE && m_position.y + m_size.y * TILE_SIZE > e->getPosition().y) {
                        int dist = (m_position.x - e->getPosition().x);

                        if(dist != 0) {
                            m_position.x += (dist ^ 2) / 128.0f;
                        }
                    }
                }
            }
        }
        /// ENTITY COLLISION ENDS HERE

        /// TILE COLLISION STARTS HERE
        {
            /// Many thanks to Ben Arnold. He taught me almost everything I know about programming through his Youtube channel, "Makinggameswithben"
            /// This is just a small piece of code that handles and reacts to dynamic rectangle and tile collisions
            std::vector<glm::vec2> collideTilePositions;
            std::vector<glm::vec2> groundTilePositions;

            float x = m_position.x, y = m_position.y, width = m_size.x * TILE_SIZE, height = m_size.y * TILE_SIZE;

            x += m_velocity.x;
            y += m_velocity.y;

            glm::vec2 posBL(x, y);
            glm::vec2 posBR(x + width, y);
            glm::vec2 posTL(x, y + height);
            glm::vec2 posTR(x + width, y + height);

            // Check for ground/ceiling
            checkTilePosition(m_parentChunk->tiles,
                              chunkI,
                              groundTilePositions,
                              posBR.x - 1.0f,
                              posBR.y);

            checkTilePosition(m_parentChunk->tiles,
                              chunkI,
                              groundTilePositions,
                              posBL.x + 1.0f,
                              posBL.y);

            checkTilePosition(m_parentChunk->tiles,
                              chunkI,
                              groundTilePositions,
                              posTR.x - 1.0f,
                              posTR.y);

            checkTilePosition(m_parentChunk->tiles,
                              chunkI,
                              groundTilePositions,
                              posTL.x + 1.0f,
                              posTL.y);


            // Check the corners
            checkTilePosition(m_parentChunk->tiles,
                              chunkI,
                              collideTilePositions,
                              posBR.x,
                              posBR.y + 1.0f);

            checkTilePosition(m_parentChunk->tiles,
                              chunkI,
                              collideTilePositions,
                              posBL.x,
                              posBL.y + 1.0f);

            for(int i = 1; i <= m_size.y - 1; i++) {
                checkTilePosition(m_parentChunk->tiles,
                                  chunkI,
                                  collideTilePositions,
                                  posBL.x,
                                  posBL.y + TILE_SIZE * i);

                checkTilePosition(m_parentChunk->tiles,
                                  chunkI,
                                  collideTilePositions,
                                  posBR.x,
                                  posBR.y + TILE_SIZE * i);
            }

            for(int i = 1; i <= m_size.x - 1; i++) {
                checkTilePosition(m_parentChunk->tiles,
                                  chunkI,
                                  collideTilePositions,
                                  posBL.x + TILE_SIZE * i,
                                  posBL.y);

                checkTilePosition(m_parentChunk->tiles,
                                  chunkI,
                                  collideTilePositions,
                                  posBR.x + TILE_SIZE * i,
                                  posBR.y);
            }

            checkTilePosition(m_parentChunk->tiles,
                              chunkI,
                              collideTilePositions,
                              posTL.x,
                              posTL.y - 1.0f);

            checkTilePosition(m_parentChunk->tiles,
                              chunkI,
                              collideTilePositions,
                              posTR.x,
                              posTR.y - 1.0f);

            for (int i = 0; i < collideTilePositions.size(); i++) {
                collideWithTile(collideTilePositions[i], false);
            }

            for (int i = 0; i < groundTilePositions.size(); i++) {
                collideWithTile(groundTilePositions[i], true);
            }
        }
        /// TILE COLLISION ENDS HERE
    }
}



/// PRIVATE FUNCTIONS
void Entity::setParentChunk(Chunk* worldChunks[WORLD_SIZE]) {
    int index = std::floor(m_position.x / TILE_SIZE / CHUNK_SIZE);

    if(index != m_parentChunkIndex) {
        if(index >= 0) {
            m_parentChunk = worldChunks[index];
            m_parentChunkIndex = index;
        }
    }

}

bool Entity::checkTilePosition(Tile* tiles[WORLD_HEIGHT][CHUNK_SIZE], int chunkI, std::vector<glm::vec2>& collideTilePositions, float x, float y) {
    // Get the position of this corner in grid-space
    glm::vec2 gridPos = glm::vec2(floor(x / TILE_SIZE),
                                      floor(y / TILE_SIZE)); // grid-space coords


    // If we are outside the world, just return
    if (gridPos.x < CHUNK_SIZE * m_parentChunkIndex || gridPos.x >= CHUNK_SIZE + CHUNK_SIZE * m_parentChunkIndex ||
        gridPos.y < 0 || gridPos.y >= WORLD_HEIGHT) {
        return false;
    }

    // If this is not an air tile, we should collide with it
    if (tiles[(int)gridPos.y - m_parentChunkIndex][(int)gridPos.x]->getID() == 1) { //  - m_parentChunkIndex
        collideTilePositions.push_back(glm::vec2((float)gridPos.x + 0.500f, (float)gridPos.y + 0.500f)); // CollideTilePositions are put in as gridspace coords
        return true;
    }
    return false;
}

void Entity::collideWithTile(glm::vec2 tilePos, bool ground) {
    float x = m_position.x, y = m_position.y;

    //x += m_velocity.x / m_size.x; // To account for collision prediction,
    if(!m_onGround) y += m_velocity.y / m_size.y;

    glm::vec2 centrePos = glm::vec2(x + (m_size.x * TILE_SIZE) / 2.0f, y + (m_size.y * TILE_SIZE) / 2.0f);

    glm::vec2 distVec = centrePos - (tilePos * glm::vec2(TILE_SIZE));

    glm::vec2 depthVec = abs(abs(distVec) - (glm::vec2(TILE_SIZE / 2.0f) + m_size * glm::vec2(TILE_SIZE) / glm::vec2(2.0f)));

    // Determine if it's shorter to go in the X or Y direction
    //if(depthVec.x > 0 && depthVec.y > 0) {
        if(abs(depthVec.x / m_size.x) < abs(depthVec.y / m_size.y)) { // X direction is shorter // Maybe change these variables from depthvec to distvec and reverse the comparison to >
            if(!ground) {
                if(distVec.x < 0.000f) {
                    m_position.x -= depthVec.x - abs(m_velocity.x * m_size.x); // TILE ON RIGHT
                } else if(distVec.x >= 0.000f) {
                    m_position.x += depthVec.x - abs(m_velocity.x * m_size.x); // TILE ON LEFT
                }
                m_velocity.x = 0.0f;
            }
        } else {
            if(ground) {
                if(distVec.y < 0.000f && m_velocity.y >= 0.0f) {
                    m_position.y -= depthVec.y - abs(m_velocity.y / m_size.y); // TILE ON TOP
                    m_velocity.y = 0.0f;
                } else if(distVec.y >= 0.000f && m_velocity.y < 0.000000f) {
                    m_position.y += depthVec.y / m_size.y - abs(m_velocity.y / m_size.y); // TILE ON BOTTOM
                    m_onGround = true;
                    m_velocity.y = 0.0000000f;
                }
            }
        }
    //}
}

void Entity::updateLightLevel(Chunk* currentChunk) {
    if(currentChunk) {
        int entityChunkX, entityChunkY; // The entity's coords in the chunk

        entityChunkX = (int)((m_position.x + 0.5f) / TILE_SIZE) % CHUNK_SIZE;
        entityChunkY = (m_position.y + 0.5f) / TILE_SIZE;

        if(entityChunkX >= 0 && entityChunkX < CHUNK_SIZE) {
            if(entityChunkY >= 0 && entityChunkY < WORLD_HEIGHT) {
                m_light = currentChunk->tiles[entityChunkY][entityChunkX]->getLight();
            }
        }
    }
}
