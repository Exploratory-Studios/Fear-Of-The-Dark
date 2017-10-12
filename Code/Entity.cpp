#include "Entity.h"

#include <DebugRenderer.h>

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
        case Categories::Entity_Type::MOB: /// Temporary
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

void Entity::update(Chunk* chunks[WORLD_SIZE]) {
    setParentChunk(chunks);

    m_wasOnGround = m_onGround;
}

void Entity::draw(GLEngine::SpriteBatch& sb, GLEngine::DebugRenderer& dr) {
    glm::vec4 destRect = glm::vec4(m_position.x, m_position.y, m_size.x * TILE_SIZE, m_size.y * TILE_SIZE);
    glm::vec4 uvRect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    GLEngine::ColourRGBA8 colour(255, 255, 255, 255);

    sb.draw(destRect, uvRect, m_texture.id, 0.0f, colour);

    dr.drawCircle(m_testPos, GLEngine::ColourRGBA8(255, 255, 255, 255), 4);
}

void Entity::move(float timeStepVariable) {
    if(!m_onGround) {
        m_velocity.y -= 0.07 * timeStepVariable;
    } else {
        if(m_velocity.y < 0.0f) {
            m_velocity.y = 0.0f;
        }
    }
    m_onGround = false;
    m_position += m_velocity * glm::vec2(timeStepVariable);
}

void Entity::collide(std::vector<Entity*> entities, int chunkI) {

    if(chunkI == m_parentChunkIndex) {
        /// ENTITY COLLISION STARTS HERE
        for(auto e : entities) {
            if(e != this) {
                if(e->getPosition().x < m_position.x + m_size.x || e->getPosition().x + e->getSize().x > m_position.x) {
                    if(e->getPosition().y < m_position.y - m_size.y && e->getPosition().y - e->getSize().y > m_position.y) {
                        m_position += 0.01f;
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

            float x = m_position.x, y = m_position.y, width = m_size.x * TILE_SIZE, height = m_size.y * TILE_SIZE;

            x += m_velocity.x;
            y += m_velocity.y;

            glm::vec2 posBL(x, y);
            glm::vec2 posBR(x + width, y);
            glm::vec2 posTL(x, y + height);
            glm::vec2 posTR(x + width, y + height);

            /// Check the corners
            checkTilePosition(m_parentChunk->tiles,
                              chunkI,
                              collideTilePositions,
                              posBR.x,
                              posBR.y);

            checkTilePosition(m_parentChunk->tiles,
                              chunkI,
                              collideTilePositions,
                              posBL.x,
                              posBL.y);

            checkTilePosition(m_parentChunk->tiles,
                              chunkI,
                              collideTilePositions,
                              posTL.x,
                              posTL.y);

            checkTilePosition(m_parentChunk->tiles,
                              chunkI,
                              collideTilePositions,
                              posTR.x,
                              posTR.y);

            for (int i = 0; i < collideTilePositions.size(); i++) {
                collideWithTile(collideTilePositions[i]);
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

void Entity::checkTilePosition(Tile tiles[WORLD_HEIGHT][CHUNK_SIZE], int chunkI, std::vector<glm::vec2>& collideTilePositions, float x, float y) {
    // Get the position of this corner in grid-space
    glm::vec2 gridPos = glm::vec2(floor(x / TILE_SIZE),
                                      floor(y / TILE_SIZE)); // grid-space coords

    // If we are outside the world, just return
    if (gridPos.x < CHUNK_SIZE * m_parentChunkIndex || gridPos.x >= CHUNK_SIZE + CHUNK_SIZE * m_parentChunkIndex ||
        gridPos.y < 0 || gridPos.y >= WORLD_HEIGHT) {
        return;
    }

    // If this is not an air tile, we should collide with it
    if (tiles[(int)gridPos.y][(int)gridPos.x].isSolid()) {
        collideTilePositions.push_back(glm::vec2((int)gridPos.x + 0.5, (int)gridPos.y + 0.5)); // CollideTilePositions are put in as gridspace coords
    }
}

void Entity::collideWithTile(glm::vec2 tilePos) {
    float x = m_position.x, y = m_position.y;
    x += m_velocity.x; // To account for collision prediction,
    y += m_velocity.y; // we check if the player is still colliding in real space, not predicted

    glm::vec2 centrePos = glm::vec2(x + (m_size.x * TILE_SIZE) / 2.0f, y + (m_size.y * TILE_SIZE) / 2.0f);

    glm::vec2 distVec = centrePos - (tilePos * glm::vec2(TILE_SIZE));

    glm::vec2 depthVec = abs(abs(distVec) - (glm::vec2(TILE_SIZE / 2.0f) + m_size * glm::vec2(TILE_SIZE) / glm::vec2(2.0f)));

    // Determine if it's shorter to go in the X or Y direction
    if(abs(depthVec.x * m_size.x) < abs(depthVec.y * m_size.y)) { // X direction is shorter // Maybe change these variables from depthvec to distvec and reverse the comparison to >
        if(distVec.x < 0.0f) {
            m_position.x -= depthVec.x - abs(m_velocity.x); // TILE ON RIGHT
        } else if(distVec.x > 0.0f) {
            m_position.x += depthVec.x - abs(m_velocity.x); // TILE ON LEFT
        }
        m_velocity.x = 0.0f;
    } else {
        m_onGround = false;
        if(distVec.y < 0.0f && m_velocity.y >= 0.0f) {
            m_position.y -= depthVec.y - abs(m_velocity.y); // TILE ON TOP
        } else if(distVec.y > 0.0f && m_velocity.y <= 0.0f) {
            m_position.y += depthVec.y - abs(m_velocity.y); // TILE ON BOTTOM
            m_onGround = true;
        }
    }

}
